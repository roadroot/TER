/*!
 * Copyright (c) <2019> <Andreas Niskanen, University of Helsinki>
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
#include "DynSingleExtension.h"

using namespace std;

namespace DynSingleExtension {

void complete(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);

	Encodings::add_complete(af, solver);

	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);
	
	print_begin(true);
	vector<vector<uint32_t>> extension(af.changes.size()+1);
	
	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0) {
			vector<int> new_assumptions = is_extension_assumptions(af, solver.assignment, assumptions);
			bool sat = solver.solve(new_assumptions);
			if (sat) {
				extension[i] = extension[i-1];
				print_single_extension(af, extension[i], 1);
				if (i == af.changes.size()) break;
#if defined(ADD_UNITS)
				if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
					bool is_addition = af.changes[i].second;
					vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
					solver.add_clause(att_clause);
				}
#endif
				update_assumptions(af, assumptions, i);
				continue;
			}
		}
#endif
		bool sat = solver.solve(assumptions);
		if (sat) {
			for (uint32_t j = 0; j < af.args; j++) {
				if (solver.assignment[af.arg_var[j]-1]) {
					extension[i].push_back(j);
				}
			}
		}
		print_single_extension(af, extension[i], 1);
		if (i == af.changes.size()) break;
#if defined(ADD_UNITS)
		if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
			bool is_addition = af.changes[i].second;
			vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
			solver.add_clause(att_clause);
		}
#endif
		update_assumptions(af, assumptions, i);
	}
	print_end(true);
}

void preferred(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count + af.changes.size() + 1, af.args);

#if defined(INCREMENTAL)
	solver.set_initial_vars(af.count);
#endif
	Encodings::add_complete(af, solver);
	
	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	print_begin(true);
	vector<vector<uint32_t>> extension(af.changes.size()+1);

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0) {
			vector<int> new_assumptions = is_extension_assumptions(af, solver.assignment, assumptions);
			bool sat = solver.solve(new_assumptions);
			if (sat) {
				vector<int> complement_clause;
				complement_clause.reserve(af.args);
				complement_clause.push_back(af.selector_var[i]);
				new_assumptions = assumptions;
				new_assumptions.push_back(-af.selector_var[i]);
				new_assumptions.reserve(assumptions.size()+af.args);
				for (uint32_t j = 0; j < af.args; j++) {
					if (solver.assignment[af.arg_var[j]-1]) {
						new_assumptions.push_back(af.arg_var[j]);
					} else {
						complement_clause.push_back(af.arg_var[j]);
					}
				}
				solver.add_clause(complement_clause);
				if (!solver.solve(new_assumptions)) {
					extension[i] = extension[i-1];
					print_single_extension(af, extension[i], 1);
					if (i == af.changes.size()) break;
					assumptions.push_back(af.selector_var[i]);
#if defined(ADD_UNITS)
					if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
						bool is_addition = af.changes[i].second;
						vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
						solver.add_clause(att_clause);
					}
#endif
					update_assumptions(af, assumptions, i);
					continue;
				}
			}
		}
#endif
		vector<int> assumptions_and_selector = assumptions;
		assumptions_and_selector.push_back(-af.selector_var[i]);
		while (true) {
			bool sat = solver.solve(assumptions_and_selector);
			if (!sat) break;
			vector<int> complement_clause;
			complement_clause.reserve(af.args+1);
			complement_clause.push_back(af.selector_var[i]);
			for (uint32_t j = 0; j < af.args; j++) {
				if (solver.assignment[af.arg_var[j]-1]) {
					vector<int> arg_accepted_clause = {af.selector_var[i], af.arg_var[j]};
					solver.add_clause(arg_accepted_clause);
				} else {
					complement_clause.push_back(af.arg_var[j]);
				}
			}
			solver.add_clause(complement_clause);
		}
		for (uint32_t j = 0; j < af.args; j++) {
			if (solver.assignment[af.arg_var[j]-1]) {
				extension[i].push_back(j);
			}
		}
		print_single_extension(af, extension[i], 1);
		if (i == af.changes.size()) break;
		assumptions.push_back(af.selector_var[i]);
#if defined(ADD_UNITS)
		if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
			bool is_addition = af.changes[i].second;
			vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
			solver.add_clause(att_clause);
		}
#endif
		update_assumptions(af, assumptions, i);
	}
	print_end(true);
}

void stable(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);

	Encodings::add_stable(af, solver);

	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	print_begin(true);
	vector<vector<uint32_t>> extension(af.changes.size()+1);
	bool extension_exists = false;
	
	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0 && extension_exists) {
			vector<int> new_assumptions = is_extension_assumptions(af, solver.assignment, assumptions);
			bool sat = solver.solve(new_assumptions);
			if (sat) {
				extension[i] = extension[i-1];
				print_single_extension(af, extension[i], 1);
				if (i == af.changes.size()) break;
#if defined(ADD_UNITS)
				if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
					bool is_addition = af.changes[i].second;
					vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
					solver.add_clause(att_clause);
				}
#endif
				update_assumptions(af, assumptions, i);
				continue;
			}
		}
#endif
#if defined(CHECK_NEG)
		if (i > 0 && !extension_exists) {
			bool unchanged = conflict_clause_valid(af, solver.conflict, assumptions, i);
			if (unchanged) {
				extension[i] = {};
				print_single_extension(af, extension[i], 1);
				if (i == af.changes.size()) break;
#if defined(ADD_UNITS)
				if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
					bool is_addition = af.changes[i].second;
					vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
					solver.add_clause(att_clause);
				}
#endif
				update_assumptions(af, assumptions, i);
				continue;
			}
		}
#endif
		bool sat = solver.solve(assumptions);
		if (sat) {
			for (uint32_t j = 0; j < af.args; j++) {
				if (solver.assignment[af.arg_var[j]-1]) {
					extension[i].push_back(j);
				}
			}
			extension_exists = true;
		} else {
			extension_exists = false;
			extension[i] = {};
		}
		print_single_extension(af, extension[i], 1);
		if (i == af.changes.size()) break;
#if defined(ADD_UNITS)
		if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
			bool is_addition = af.changes[i].second;
			vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
			solver.add_clause(att_clause);
		}
#endif
		update_assumptions(af, assumptions, i);
	}
	print_end(true);
}

void grounded(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);

	Encodings::add_complete(af, solver);
	solver.propagate();

	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	print_begin(true);
	vector<vector<uint32_t>> extension(af.changes.size()+1);

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
		for (uint32_t j = 0; j < af.args; j++) {
			if (solver.assignment[af.arg_var[j]-1]) {
				extension[i].push_back(j);
			}
		}
	}

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
		bool sat = solver.propagate(assumptions);
		if (sat) {
			for (uint32_t j = 0; j < af.args; j++) {
				if (solver.assignment[af.arg_var[j]-1]) {
					extension[i].push_back(j);
				}
			}
		}
		print_single_extension(af, extension[i], 1);
		if (i == af.changes.size()) break;
#if defined(ADD_UNITS)
		if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
			bool is_addition = af.changes[i].second;
			vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
			solver.add_clause(att_clause);
		}
		solver.propagate();
		for (uint32_t j = i+1; j <= af.changes.size(); j++) {
			extension[j].clear();
			for (uint32_t k = 0; k < af.args; k++) {
				if (solver.assignment[af.arg_var[k]-1]) {
					extension[j].push_back(k);
				}
			}
		}
#endif
		update_assumptions(af, assumptions, i);
	}
	print_end(true);
}

}