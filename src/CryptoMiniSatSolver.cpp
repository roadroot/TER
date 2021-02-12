/*!
 * Copyright (c) <2020> <Andreas Niskanen, University of Helsinki>
 * 
 * 
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * 
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "CryptoMiniSatSolver.h"

using namespace std;
using namespace CMSat;

CryptoMiniSatSolver::CryptoMiniSatSolver(uint32_t number_of_vars, uint32_t number_of_arg_vars)
{
	solver.set_num_threads(1);
	n_args = number_of_arg_vars;
	assignment = vector<uint8_t>(n_args);
	solver.new_vars(number_of_vars);
}

void CryptoMiniSatSolver::set_polarity(bool polarity)
{
	solver.set_default_polarity(polarity);
}

void CryptoMiniSatSolver::add_clause(const vector<int> & clause)
{
	vector<Lit> lits(clause.size());
	for (int i = 0; i < clause.size(); i++) {
		int var = abs(clause[i])-1;
		lits[i] = Lit(var, (clause[i] > 0) ? false : true);
	}
	solver.add_clause(lits);
}

void CryptoMiniSatSolver::add_clauses(const vector<vector<int>> & clauses)
{
	for (int i = 0; i < clauses.size(); i++) {
		add_clause(clauses[i]);
	}
}

bool CryptoMiniSatSolver::solve()
{
	bool sat = (solver.solve() == l_True);
	if (sat) {
		for (int i = 0; i < n_args; i++) {
			assignment[i] = (solver.get_model()[i] == l_True) ? 1 : 0;
		}
	} else {
		conflict.clear();
		conflict.reserve(solver.get_conflict().size());
		for (int i = 0; i < solver.get_conflict().size(); i++) {
			conflict.push_back(solver.get_conflict()[i].sign() ? (solver.get_conflict()[i].var()+1) : -(solver.get_conflict()[i].var()+1));
		}
	}
	return sat;
}

bool CryptoMiniSatSolver::solve(const vector<int> & assumptions)
{
	vector<Lit> lits(assumptions.size());
	for (int i = 0; i < assumptions.size(); i++) {
		int var = abs(assumptions[i])-1;
		lits[i] = Lit(var, (assumptions[i] > 0) ? false : true);
	}
	bool sat = (solver.solve(&lits) == l_True);
	if (sat) {
		for (int i = 0; i < n_args; i++) {
			assignment[i] = (solver.get_model()[i] == l_True) ? 1 : 0;
		}
	} else {
		conflict.clear();
		conflict.reserve(solver.get_conflict().size());
		for (int i = 0; i < solver.get_conflict().size(); i++) {
			conflict.push_back(solver.get_conflict()[i].sign() ? (solver.get_conflict()[i].var()+1) : -(solver.get_conflict()[i].var()+1));
		}
	}
	return sat;
}

bool CryptoMiniSatSolver::propagate()
{
	solver.set_no_bva();
	for (uint32_t i = 0; i < n_args; i++) {
		assignment[i] = 0;
	}
	vector<Lit> zero_assigned_lits = solver.get_zero_assigned_lits();
	for (uint32_t i = 0; i < zero_assigned_lits.size(); i++) {
		if (zero_assigned_lits[i].var() < n_args) {
			assignment[zero_assigned_lits[i].var()] = (zero_assigned_lits[i].sign() ? 0 : 1);
		}
	}
	return solver.okay();
}

bool CryptoMiniSatSolver::propagate(const vector<int> & assumptions)
{
	for (uint32_t i = 0; i < n_args; i++) {
		assignment[i] = 0;
	}
	vector<Lit> lits(assumptions.size());
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		int var = abs(assumptions[i])-1;
		lits[i] = Lit(var, (assumptions[i] > 0) ? false : true);
	}
	vector<Lit> out_implied;
	bool ret = solver.implied_by(lits, out_implied);
	for (uint32_t i = 0; i < out_implied.size(); i++) {
		if (out_implied[i].var() < n_args) {
			assignment[out_implied[i].var()] = (out_implied[i].sign() ? 0 : 1);
		}
	}
	return ret;
}