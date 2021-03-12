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
#include "SingleExtension.h"

using namespace std;

namespace SingleExtension {

bool complete(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_complete(af, solver);
	bool sat = solver.solve();
	if (sat) {
		vector<uint32_t> extension;
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		print_single_extension(af, extension, 0);
	}
	return true;
}

bool preferred(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_complete(af, solver);
	vector<uint32_t> extension;
	while (true) {
		bool sat = solver.solve();
		if (!sat) break;
		extension.clear();
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		vector<int> complement_clause;
		complement_clause.reserve(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				vector<int> unit_clause = { af.arg_var[i] };
				solver.add_clause(unit_clause);
			} else {
				complement_clause.push_back(af.arg_var[i]);
			}
		}
		solver.add_clause(complement_clause);
	}
	print_single_extension(af, extension, 0);
	return true;
}

bool stable(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
#if defined(GR_IN_ST)
	vector<int> assumptions = grounded_assumptions(af);
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		vector<int> clause = { assumptions[i] };
		solver.add_clause(clause);
	}
#endif
	Encodings::add_stable(af, solver);
	bool sat = solver.solve();
	if (sat) {
		vector<uint32_t> extension;
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		print_single_extension(af, extension, 0);
		return true;
	} else {
		print_no(false);
		return false;
	}
}

vector<vector<std::string>> *grounded(const AF & af,int usless)
{
	vector<vector<std::string>> *results = new vector<vector<std::string>>();
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_complete(af, solver);
	bool sat = solver.propagate();
	if (sat) {
		vector<uint32_t> extension;
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		//print_single_extension(af, extension, 0);
		results->push_back(get_single_extension(af,extension,0));
	}
	return results;
}

bool grounded(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_complete(af, solver);
	bool sat = solver.propagate();
	if (sat) {
		vector<uint32_t> extension;
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		print_single_extension(af, extension, 0);
	}
	return true;
}

bool semi_stable(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, 3*af.args);

	Encodings::add_complete(af, solver);
	Encodings::add_range(af, solver);

#if defined(ST_EXISTS_SST)
	vector<int> range_assumption(af.args);
	for (uint32_t i = 0; i < af.args; i++) {
		range_assumption[i] = af.range_var[i];
	}
#if defined(GR_IN_ST)
	vector<int> grd_assumption = grounded_assumptions(af);
	range_assumption.insert(range_assumption.end(), make_move_iterator(grd_assumption.begin()), make_move_iterator(grd_assumption.end()));
#endif
	if (solver.solve(range_assumption)) {
		vector<uint32_t> extension;
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		print_single_extension(af, extension, 0);
		return true;
	}
#endif

	vector<uint32_t> extension;
	while (true) {
		bool sat = solver.solve();
		if (!sat) break;
		extension.clear();
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		vector<int> complement_clause;
		complement_clause.reserve(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.range_var[i]-1]) {
				vector<int> unit_clause = { af.range_var[i] };
				solver.add_clause(unit_clause);
			} else {
				complement_clause.push_back(af.range_var[i]);
			}
		}
		solver.add_clause(complement_clause);
	}
	print_single_extension(af, extension, 0);
	return true;
}

bool stage(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, 3*af.args);

	Encodings::add_conflict_free(af, solver);
	Encodings::add_range(af, solver);

#if defined(ST_EXISTS_STG)
	vector<int> range_assumption(af.args);
	for (uint32_t i = 0; i < af.args; i++) {
		range_assumption[i] = af.range_var[i];
	}
#if defined(GR_IN_ST)
	vector<int> grd_assumption = grounded_assumptions(af);
	range_assumption.insert(range_assumption.end(), make_move_iterator(grd_assumption.begin()), make_move_iterator(grd_assumption.end()));
#endif
	if (solver.solve(range_assumption)) {
		vector<uint32_t> extension;
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		print_single_extension(af, extension, 0);
		return true;
	}
#endif

	vector<uint32_t> extension;
	while (true) {
		bool sat = solver.solve();
		if (!sat) break;
		extension.clear();
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
			}
		}
		vector<int> complement_clause;
		complement_clause.reserve(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.range_var[i]-1]) {
				vector<int> unit_clause = { af.range_var[i] };
				solver.add_clause(unit_clause);
			} else {
				complement_clause.push_back(af.range_var[i]);
			}
		}
		solver.add_clause(complement_clause);
	}
	print_single_extension(af, extension, 0);
	return true;
}

bool ideal(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count+1, af.args);
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

	for (uint32_t i = 0; i < af.args; i++) {
		if (!union_of_admissible_not_attacked[i]) {
			vector<int> arg_rejected_clause = { -af.arg_var[i] };
			solver.add_clause(arg_rejected_clause);
		}
	}

	vector<int> assumption = { blocking_var };
	solver.add_clause(assumption);

	vector<uint32_t> extension;
	while (true) {
		bool sat = solver.solve();
		if (!sat) break;
		extension.clear();
		blocking_clause.clear();
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				vector<int> arg_accepted_clause = { af.arg_var[i] };
				solver.add_clause(arg_accepted_clause);
				extension.push_back(i);
			} else {
				blocking_clause.push_back(af.arg_var[i]);
			}
		}
		solver.add_clause(blocking_clause);
	}
	print_single_extension(af, extension, 0);
	return true;
}

}
