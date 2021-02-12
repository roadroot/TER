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

#include "Encodings.h"
#include "Util.h"
#include "SkeptAcceptance.h"

using namespace std;

namespace SkeptAcceptance {

bool complete(const AF & af, string const & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, 0);
	vector<int> arg_rejected_clause = { -af.arg_var[af.arg_to_int.at(arg)] };
	solver.add_clause(arg_rejected_clause);
	Encodings::add_complete(af, solver);
	bool sat = solver.solve();
	return !sat;
}

bool preferred(const AF & af, string const & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	solver.set_polarity(true);
	Encodings::add_complete(af, solver);

	vector<int> assumptions = { -af.arg_var[af.arg_to_int.at(arg)] };

	while (true) {
		bool sat = solver.solve(assumptions);
		if (!sat) break;

		vector<int> complement_clause;
		complement_clause.reserve(af.args);
		vector<uint8_t> visited(af.args);
		vector<int> new_assumptions = assumptions;
		new_assumptions.reserve(af.args);

		while (true) {
			complement_clause.clear();
			for (uint32_t i = 0; i < af.args; i++) {
				if (solver.assignment[af.arg_var[i]-1]) {
					if (!visited[i]) {
						new_assumptions.push_back(af.arg_var[i]);
						visited[i] = 1;
					}
				} else {
					complement_clause.push_back(af.arg_var[i]);
				}
			}
			solver.add_clause(complement_clause);
			bool superset_exists = solver.solve(new_assumptions);
			if (!superset_exists) break;
		}

		new_assumptions[0] = -new_assumptions[0];

		if (!solver.solve(new_assumptions)) {
			return false;
		}
	}
	return true;
}

bool stable(const AF & af, string const & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, 0);
#if defined(GR_IN_ST)
	vector<int> assumptions = grounded_assumptions(af);
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		vector<int> clause = { assumptions[i] };
		solver.add_clause(clause);
	}
#endif
	vector<int> arg_rejected_clause = { -af.arg_var[af.arg_to_int.at(arg)] };
	solver.add_clause(arg_rejected_clause);
	Encodings::add_stable(af, solver);
	bool sat = solver.solve();
	return !sat;
}

bool semi_stable(const AF & af, string const & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, 2*af.args);
	solver.set_polarity(true);
	Encodings::add_complete(af, solver);
	Encodings::add_range(af, solver);

#if defined(ST_EXISTS_SST)
	vector<int> range_assumption(af.args);
	for (uint32_t i = 0; i < af.args; i++) {
		range_assumption[i] = af.range_var[i];
	}
	if (solver.solve(range_assumption)) {
		range_assumption.push_back(-af.arg_var[af.arg_to_int.at(arg)]);
		return !solver.solve(range_assumption);
	}
#endif

	vector<int> assumptions = { -af.arg_var[af.arg_to_int.at(arg)] };
	
	while (true) {
		bool sat = solver.solve(assumptions);
		if (!sat) break;

		vector<int> complement_clause;
		complement_clause.reserve(af.args);
		vector<uint8_t> visited(af.args);
		vector<int> new_assumptions = assumptions;
		new_assumptions.reserve(af.args);

		while (true) {
			complement_clause.clear();
			for (uint32_t i = 0; i < af.args; i++) {
				if (solver.assignment[af.range_var[i]-1]) {
					if (!visited[i]) {
						new_assumptions.push_back(af.range_var[i]);
						visited[i] = 1;
					}
				} else {
					complement_clause.push_back(af.range_var[i]);
				}
			}
			solver.add_clause(complement_clause);
			bool superset_exists = solver.solve(new_assumptions);
			if (!superset_exists) break;
		}

		new_assumptions[0] = -new_assumptions[0];

		if (!solver.solve(new_assumptions)) {
			return false;
		}
	}
	return true;
}

bool stage(const AF & af, string const & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, 2*af.args);
	solver.set_polarity(true);
	Encodings::add_conflict_free(af, solver);
	Encodings::add_range(af, solver);

#if defined(ST_EXISTS_STG)
	vector<int> range_assumption(af.args);
	for (uint32_t i = 0; i < af.args; i++) {
		range_assumption[i] = af.range_var[i];
	}
	if (solver.solve(range_assumption)) {
		range_assumption.push_back(-af.arg_var[af.arg_to_int.at(arg)]);
		return !solver.solve(range_assumption);
	}
#endif

	vector<int> assumptions = { -af.arg_var[af.arg_to_int.at(arg)] };
	
	while (true) {
		bool sat = solver.solve(assumptions);
		if (!sat) break;

		vector<int> complement_clause;
		complement_clause.reserve(af.args);
		vector<uint8_t> visited(af.args);
		vector<int> new_assumptions = assumptions;
		new_assumptions.reserve(af.args);

		while (true) {
			complement_clause.clear();
			for (uint32_t i = 0; i < af.args; i++) {
				if (solver.assignment[af.range_var[i]-1]) {
					if (!visited[i]) {
						new_assumptions.push_back(af.range_var[i]);
						visited[i] = 1;
					}
				} else {
					complement_clause.push_back(af.range_var[i]);
				}
			}
			solver.add_clause(complement_clause);
			bool superset_exists = solver.solve(new_assumptions);
			if (!superset_exists) break;
		}

		new_assumptions[0] = -new_assumptions[0];

		if (!solver.solve(new_assumptions)) {
			return false;
		}
	}
	return true;
}

}