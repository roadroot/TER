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

#include "GlucoseSolver.h"

using namespace std;
using namespace Glucose;

GlucoseSolver::GlucoseSolver(uint32_t number_of_vars, uint32_t number_of_arg_vars)
{
	n_args = number_of_arg_vars;
	assignment = vector<uint8_t>(n_args);
	solver = new Solver();
#if defined(INCREMENTAL)
	solver->setIncrementalMode();
#endif
	for (uint32_t i = 0; i < number_of_vars; i++) {
		solver->newVar();
	}
}

#if defined(INCREMENTAL)
void GlucoseSolver::set_initial_vars(uint32_t number_of_initial_vars)
{
	solver->initNbInitialVars(number_of_initial_vars);
}
#endif

void GlucoseSolver::set_polarity(bool polarity)
{
	for (uint32_t i = 0; i < n_args; i++) {
		solver->setPolarity(i, false);
	}
}

void GlucoseSolver::add_clause(const vector<int> & clause)
{
	vec<Lit> lits(clause.size());
	for (uint32_t i = 0; i < clause.size(); i++) {
		int var = abs(clause[i])-1;
		while (var >= solver->nVars()) {
			solver->newVar();
		}
		lits[i] = (clause[i] > 0) ? mkLit(var) : ~mkLit(var);
	}
	solver->addClause_(lits);
}

void GlucoseSolver::add_clauses(const vector<vector<int>> & clauses)
{
	for (uint32_t i = 0; i < clauses.size(); i++) {
		add_clause(clauses[i]);
	}
}

bool GlucoseSolver::solve()
{
	bool sat = solver->solve();
	if (sat) {
		for (uint32_t i = 0; i < n_args; i++) {
			assignment[i] = (solver->model[i] == l_True) ? 1 : 0;
		}
	} else {
		conflict.clear();
		conflict.reserve(solver->conflict.size());
		for (int i = 0; i < solver->conflict.size(); i++) {
			conflict.push_back(sign(solver->conflict[i]) ? var(solver->conflict[i]) : -var(solver->conflict[i]));
		}
	}
	return sat;
}

bool GlucoseSolver::solve(const vector<int> & assumptions)
{
	vec<Lit> lits(assumptions.size());
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		int var = abs(assumptions[i])-1;
		while (var >= solver->nVars()) {
			solver->newVar();
		}
		lits[i] = (assumptions[i] > 0) ? mkLit(var) : ~mkLit(var);
	}
	bool sat = solver->solve(lits);
	if (sat) {
		for (uint32_t i = 0; i < n_args; i++) {
			assignment[i] = (solver->modelValue(i) == l_True) ? 1 : 0;
		}
	} else {
		conflict.clear();
		conflict.reserve(solver->conflict.size());
		for (int i = 0; i < solver->conflict.size(); i++) {
			conflict.push_back(sign(solver->conflict[i]) ? (var(solver->conflict[i])+1) : -(var(solver->conflict[i])+1));
		}
	}
	return sat;
}

bool GlucoseSolver::propagate()
{
	CRef conflict = solver->propagate();
	for (uint32_t i = 0; i < n_args; i++) {
		assignment[i] = (solver->value(i) == l_True) ? 1 : 0;
	}
	return conflict == CRef_Undef;
}

bool GlucoseSolver::propagate(const vector<int> & assumptions)
{
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		int var = abs(assumptions[i])-1;
		while (var >= solver->nVars()) {
			solver->newVar();
		}
		solver->newDecisionLevel();
		solver->uncheckedEnqueue(assumptions[i] > 0 ? mkLit(var) : ~mkLit(var));
	}
	CRef conflict = solver->propagate();
	for (uint32_t i = 0; i < n_args; i++) {
		assignment[i] = (solver->value(i) == l_True) ? 1 : 0;
	}
	solver->cancelUntil(0);
	return conflict == CRef_Undef;
}