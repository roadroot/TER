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
#include "DynEnumerateExtensions.h"

using namespace std;

namespace DynEnumerateExtensions {

void complete(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count + af.changes.size() + 1, af.args);

#if defined(INCREMENTAL)
	solver.set_initial_vars(af.count);
#endif
	Encodings::add_complete(af, solver);

	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	print_begin(true);

	vector<vector<uint32_t>> last_extensions;
	vector<vector<uint32_t>> last_complements;

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0) {
			vector<vector<uint32_t>> current_extensions;
			current_extensions.reserve(last_extensions.size());
			vector<vector<uint32_t>> current_complements;
			current_complements.reserve(last_complements.size());
			for (uint32_t j = 0; j < last_extensions.size(); j++) {
				vector<int> new_assumptions = assumptions;
				new_assumptions.reserve(assumptions.size()+af.args);
				for (uint32_t k = 0; k < last_extensions[j].size(); k++) {
					new_assumptions.push_back(af.arg_var[last_extensions[j][k]]);
				}
				for (uint32_t k = 0; k < last_complements[j].size(); k++) {
					new_assumptions.push_back(-af.arg_var[last_complements[j][k]]);
				}
				bool sat = solver.solve(new_assumptions);
				if (sat) {
					current_extensions.push_back(last_extensions[j]);
					current_complements.push_back(last_complements[j]);
					vector<int> blocking_clause;
					blocking_clause.reserve(af.args+1);
					blocking_clause.push_back(af.selector_var[i]);
					for (uint32_t k = 0; k < af.args; k++) {
						if (solver.assignment[af.arg_var[k]-1]) {
							blocking_clause.push_back(-af.arg_var[k]);
						} else {
							blocking_clause.push_back(af.arg_var[k]);
						}
					}
					solver.add_clause(blocking_clause);
				}
			}
			last_extensions = current_extensions;
			last_complements = current_complements;
		}
#endif
		vector<int> assumptions_and_selector = assumptions;
		assumptions_and_selector.push_back(-af.selector_var[i]);
		while (true) {
			bool sat = solver.solve(assumptions_and_selector);
			if (!sat) break;
			vector<uint32_t> extension;
			extension.reserve(af.args);
			vector<uint32_t> complement;
			complement.reserve(af.args);
			vector<int> blocking_clause;
			blocking_clause.reserve(af.args+1);
			blocking_clause.push_back(af.selector_var[i]);
			for (uint32_t j = 0; j < af.args; j++) {
				if (solver.assignment[af.arg_var[j]-1]) {
					extension.push_back(j);
					blocking_clause.push_back(-af.arg_var[j]);
				} else {
					complement.push_back(j);
					blocking_clause.push_back(af.arg_var[j]);
				}
			}
			last_extensions.push_back(move(extension));
			last_complements.push_back(move(complement));
			solver.add_clause(blocking_clause);
		}
		print_extensions(af, last_extensions);
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

	vector<vector<uint32_t>> last_extensions;
	vector<vector<uint32_t>> last_complements;

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0) {
			vector<vector<uint32_t>> current_extensions;
			current_extensions.reserve(last_extensions.size());
			vector<vector<uint32_t>> current_complements;
			current_complements.reserve(last_complements.size());
			for (uint32_t j = 0; j < last_extensions.size(); j++) {
				vector<int> new_assumptions = assumptions;
				new_assumptions.reserve(assumptions.size()+af.args);
				for (uint32_t k = 0; k < last_extensions[j].size(); k++) {
					new_assumptions.push_back(af.arg_var[last_extensions[j][k]]);
				}
				for (uint32_t k = 0; k < last_complements[j].size(); k++) {
					new_assumptions.push_back(-af.arg_var[last_complements[j][k]]);
				}
				bool sat = solver.solve(new_assumptions);
				if (sat) {
					vector<int> complement_clause;
					complement_clause.reserve(af.args);
					complement_clause.push_back(af.selector_var[i]);
					new_assumptions = assumptions;
					new_assumptions.push_back(-af.selector_var[i]);
					new_assumptions.reserve(assumptions.size()+af.args);
					for (uint32_t k = 0; k < af.args; k++) {
						if (solver.assignment[af.arg_var[k]-1]) {
							new_assumptions.push_back(af.arg_var[k]);
						} else {
							complement_clause.push_back(af.arg_var[k]);
						}
					}
					solver.add_clause(complement_clause);
					if (!solver.solve(new_assumptions)) {
						current_extensions.push_back(last_extensions[j]);
						current_complements.push_back(last_complements[j]);
					}
				}
			}
			last_extensions = current_extensions;
			last_complements = current_complements;
		}
#endif
		vector<int> assumptions_and_selector = assumptions;
		assumptions_and_selector.push_back(-af.selector_var[i]);
		while (true) {
			bool sat = solver.solve(assumptions_and_selector);
			if (!sat) break;

			vector<uint32_t> extension;
			extension.reserve(af.args);
			
			vector<int> complement_clause;
			complement_clause.reserve(af.args);

			vector<int> new_assumptions = assumptions_and_selector;
			
			vector<uint8_t> visited(af.args);
			
			while (true) {
				complement_clause.clear();
				complement_clause.push_back(af.selector_var[i]);
				for (uint32_t j = 0; j < af.args; j++) {
					if (solver.assignment[af.arg_var[j]-1]) {
						if (!visited[j]) {
							new_assumptions.push_back(af.arg_var[j]);
							extension.push_back(j);
							visited[j] = 1;
						}
					} else {
						complement_clause.push_back(af.arg_var[j]);
					}
				}
				solver.add_clause(complement_clause);
				bool superset = solver.solve(new_assumptions);
				if (!superset) break;
			}
			vector<uint32_t> complement;
			complement.reserve(af.args);
			for (uint32_t j = 0; j < af.args; j++) {
				if (!visited[j]) {
					complement.push_back(j);
				}
			}

			last_extensions.push_back(move(extension));
			last_complements.push_back(move(complement));
		}
		print_extensions(af, last_extensions);
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
	SAT_Solver solver = SAT_Solver(af.count + af.changes.size() + 1, af.args);

#if defined(INCREMENTAL)
	solver.set_initial_vars(af.count);
#endif
	Encodings::add_stable(af, solver);

	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	print_begin(true);

	vector<vector<uint32_t>> last_extensions;
	vector<vector<uint32_t>> last_complements;

	bool last_call_unsat = false;

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0 && last_extensions.size() != 0) {
			vector<vector<uint32_t>> current_extensions;
			current_extensions.reserve(last_extensions.size());
			vector<vector<uint32_t>> current_complements;
			current_complements.reserve(last_complements.size());
			for (uint32_t j = 0; j < last_extensions.size(); j++) {
				vector<int> new_assumptions = assumptions;
				new_assumptions.reserve(assumptions.size()+af.args);
				for (uint32_t k = 0; k < last_extensions[j].size(); k++) {
					new_assumptions.push_back(af.arg_var[last_extensions[j][k]]);
				}
				for (uint32_t k = 0; k < last_complements[j].size(); k++) {
					new_assumptions.push_back(-af.arg_var[last_complements[j][k]]);
				}
				bool sat = solver.solve(new_assumptions);
				if (sat) {
					current_extensions.push_back(last_extensions[j]);
					current_complements.push_back(last_complements[j]);
					vector<int> blocking_clause;
					blocking_clause.reserve(af.args+1);
					blocking_clause.push_back(af.selector_var[i]);
					for (uint32_t k = 0; k < af.args; k++) {
						if (solver.assignment[af.arg_var[k]-1]) {
							blocking_clause.push_back(-af.arg_var[k]);
						} else {
							blocking_clause.push_back(af.arg_var[k]);
						}
					}
					solver.add_clause(blocking_clause);
				}
			}
			last_extensions = current_extensions;
			last_complements = current_complements;
		}
#endif
#if defined(CHECK_NEG)
		if (i > 0 && last_call_unsat) {
			last_call_unsat = false;
			bool unchanged = conflict_clause_valid(af, solver.conflict, assumptions, i);
			if (unchanged) {
				print_extensions(af, last_extensions);
				last_call_unsat = true;
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
		vector<int> assumptions_and_selector = assumptions;
		assumptions_and_selector.push_back(-af.selector_var[i]);
		while (true) {
			bool sat = solver.solve(assumptions_and_selector);
			if (!sat) break;
			vector<uint32_t> extension;
			extension.reserve(af.args);
			vector<uint32_t> complement;
			complement.reserve(af.args);
			vector<int> blocking_clause;
			blocking_clause.reserve(af.args+1);
			blocking_clause.push_back(af.selector_var[i]);
			for (uint32_t j = 0; j < af.args; j++) {
				if (solver.assignment[af.arg_var[j]-1]) {
					extension.push_back(j);
					blocking_clause.push_back(-af.arg_var[j]);
				} else {
					complement.push_back(j);
					blocking_clause.push_back(af.arg_var[j]);
				}
			}
			last_extensions.push_back(move(extension));
			last_complements.push_back(move(complement));
			solver.add_clause(blocking_clause);
		}
		print_extensions(af, last_extensions);
		last_call_unsat = (last_extensions.size() == 0);
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

}