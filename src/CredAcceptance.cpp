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
#include "CredAcceptance.h"

using namespace std;

namespace CredAcceptance {

bool complete(const AF & af, const string & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, 0);
	vector<int> arg_accepted_clause = { af.arg_var[af.arg_to_int.at(arg)] };
	solver.add_clause(arg_accepted_clause);
#if defined(DCCO_VIA_DCAD)
	Encodings::add_admissible(af, solver);
#else
	Encodings::add_complete(af, solver);
#endif
	bool sat = solver.solve();
	return sat;
}

bool preferred(const AF & af, const string & arg)
{
	return complete(af, arg);
}

bool stable(const AF & af, const string & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, 0);
#if defined(GR_IN_ST)
	vector<int> assumptions = grounded_assumptions(af);
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		vector<int> clause = { assumptions[i] };
		solver.add_clause(clause);
	}
#endif
	vector<int> arg_accepted_clause = { af.arg_var[af.arg_to_int.at(arg)] };
	solver.add_clause(arg_accepted_clause);
	Encodings::add_stable(af, solver);
	bool sat = solver.solve();
	return sat;
}

bool grounded(const AF & af, const string & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_complete(af, solver);
	bool sat = solver.propagate();
	if (sat) {
		if (solver.assignment[af.arg_var[af.arg_to_int.at(arg)]-1]) {
			return true;
		}
	}
	return false;
}

bool semi_stable(const AF & af, const string & arg)
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
		range_assumption.push_back(af.arg_var[af.arg_to_int.at(arg)]);
		return solver.solve(range_assumption);
	}
#endif

	vector<int> assumptions = { af.arg_var[af.arg_to_int.at(arg)] };

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
			return true;
		}
	}
	return false;
}

bool stage(const AF & af, const string & arg)
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
		range_assumption.push_back(af.arg_var[af.arg_to_int.at(arg)]);
		return solver.solve(range_assumption);
	}
#endif

	vector<int> assumptions = { af.arg_var[af.arg_to_int.at(arg)] };

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
			return true;
		}
	}
	return false;
}

bool ideal(const AF & af, const string & arg)
{
	SAT_Solver solver = SAT_Solver(af.count+1, af.args);
	solver.set_polarity(true);
#if defined(INCREMENTAL)
	solver.set_initial_vars(af.count);
#endif
	Encodings::add_complete(af, solver);

	vector<int> blocking_clause;
	blocking_clause.reserve(af.args+1);
	int blocking_var = af.count+1;

	blocking_clause.push_back(blocking_var);
	for (uint32_t i = 0; i < af.args; i++) {
		blocking_clause.push_back(af.arg_var[i]);
	}
	solver.add_clause(blocking_clause);

	vector<uint8_t> union_of_admissible(af.args, 0);
	vector<int> clause_activated = { -blocking_var };

	while (true) {
		bool sat = solver.solve(clause_activated);
		if (!sat) break;
		blocking_clause.clear();
		blocking_clause.push_back(blocking_var);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				union_of_admissible[i] = 1;
			} else if (union_of_admissible[i] == 0) {
				blocking_clause.push_back(af.arg_var[i]);
			}
		}
		solver.add_clause(blocking_clause);
	}

	if (!union_of_admissible[af.arg_to_int.at(arg)]) {
		return false;
	}

	vector<uint8_t> union_of_admissible_not_attacked = union_of_admissible;
	for (uint32_t i = 0; i < af.args; i++) {
		if (union_of_admissible[i]) {
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				if (union_of_admissible[af.attackers[i][j]]) {
					union_of_admissible_not_attacked[i] = 0;
					break;
				}
			}
		}
	}

	if (!union_of_admissible_not_attacked[af.arg_to_int.at(arg)]) {
		return false;
	}

	for (uint32_t i = 0; i < af.args; i++) {
		if (!union_of_admissible_not_attacked[i]) {
			vector<int> arg_rejected_clause = { -af.arg_var[i] };
			solver.add_clause(arg_rejected_clause);
		}
	}

	vector<int> assumption = { blocking_var };
	solver.add_clause(assumption);

	while (true) {
		bool sat = solver.solve();
		if (!sat) break;
		blocking_clause.clear();
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				if (i == af.arg_to_int.at(arg)) {
					return true;
				}
				vector<int> arg_accepted_clause = { af.arg_var[i] };
				solver.add_clause(arg_accepted_clause);
			} else {
				blocking_clause.push_back(af.arg_var[i]);
			}
		}
		solver.add_clause(blocking_clause);
	}

	return false;
}

}