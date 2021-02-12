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
#include "DynCredAcceptance.h"

using namespace std;

namespace DynCredAcceptance {

void complete(const AF & af, const string & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);

	vector<int> arg_accepted_clause = { af.arg_var[af.arg_to_int.at(arg)] };
	solver.add_clause(arg_accepted_clause);

#if defined(DCCO_VIA_DCAD)
	Encodings::add_admissible(af, solver);
#else
	Encodings::add_complete(af, solver);
#endif

	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	vector<uint8_t> accepted(af.changes.size()+1);
	print_begin(false);

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0 && accepted[i-1]) {
			vector<int> new_assumptions = is_extension_assumptions(af, solver.assignment, assumptions);
			bool sat = solver.solve(new_assumptions);
			if (sat) {
				accepted[i] = 1;
				print_yes(i != af.changes.size());
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
		if (i > 0 && !accepted[i-1]) {
			bool unchanged = conflict_clause_valid(af, solver.conflict, assumptions, i);
			if (unchanged) {
				accepted[i] = 0;
				print_no(i != af.changes.size());
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
			accepted[i] = 1;
			print_yes(i != af.changes.size());
		} else {
			accepted[i] = 0;
			print_no(i != af.changes.size());
		}
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

void preferred(const AF & af, const string & arg)
{
	return complete(af, arg);
}


void stable(const AF & af, const string & arg)
{   
	SAT_Solver solver = SAT_Solver(af.count, af.args);

	vector<int> arg_accepted_clause = { af.arg_var[af.arg_to_int.at(arg)] };
	solver.add_clause(arg_accepted_clause);
	Encodings::add_stable(af, solver);
	
	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	print_begin(false);
	vector<uint8_t> accepted(af.changes.size()+1);

	for (uint32_t i = 0; i <= af.changes.size(); i++) {
#if defined(CHECK_POS)
		if (i > 0 && accepted[i-1]) {
			vector<int> new_assumptions = is_extension_assumptions(af, solver.assignment, assumptions);
			bool sat = solver.solve(new_assumptions);
			if (sat) {
				accepted[i] = 1;
				print_yes(i != af.changes.size());
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
		if (i > 0 && !accepted[i-1]) {
			bool unchanged = conflict_clause_valid(af, solver.conflict, assumptions, i);
			if (unchanged) {
				accepted[i] = 0;
				print_no(i != af.changes.size());
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
			accepted[i] = 1;
			print_yes(i != af.changes.size());
		} else {
			accepted[i] = 0;
			print_no(i != af.changes.size());
		}
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

void grounded(const AF & af, const string & arg)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);

	Encodings::add_complete(af, solver);
	uint32_t q = af.arg_to_int.at(arg);

	solver.propagate();
	
	if (solver.assignment[af.arg_var[q]-1]) {
		print_begin(false);
		for (uint32_t i = 0; i <= af.changes.size(); i++) {
			print_yes(i != af.changes.size());
		}
		print_end(true);
		return;
	}

	vector<int> assumptions(af.dyn_atts.size());
	initialize_assumptions(af, assumptions);

	print_begin(false);
	vector<uint8_t> accepted(af.changes.size()+1);
	
	for (uint32_t i = 0; i <= af.changes.size(); i++) {
		bool sat = solver.propagate(assumptions);
		if (sat && solver.assignment[af.arg_var[q]-1]) {
			accepted[i] = 1;
			print_yes(i != af.changes.size());
		} else {
			accepted[i] = 0;
			print_no(i != af.changes.size());
		}
		if (i == af.changes.size()) break;
#if defined(ADD_UNITS)
		if (af.att_encountered_last.at(af.changes[i].first) == i+1) {
			bool is_addition = af.changes[i].second;
			vector<int> att_clause = { is_addition ? af.att_var.at(af.changes[i].first) : -af.att_var.at(af.changes[i].first) };
			solver.add_clause(att_clause);
		}
		solver.propagate();
		if (solver.assignment[af.arg_var[q]-1]) {
			for (uint32_t j = i+1; j <= af.changes.size(); j++) {
				print_yes(j != af.changes.size());
			}
			print_end(true);
			return;
		}
#endif
		update_assumptions(af, assumptions, i);
	}
	print_end(true);
}

}