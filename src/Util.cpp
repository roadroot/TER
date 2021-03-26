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

#include <iostream>

#include "Encodings.h"
#include "Util.h"

using namespace std;

vector<int> grounded_assumptions(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, 3*af.args);
	Encodings::add_complete(af, solver);
	bool sat = solver.propagate();
	vector<int> new_assumptions;
	if (sat) {
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				new_assumptions.push_back(af.arg_var[i]);
			} else if (solver.assignment[af.attacked_by_accepted_var[i]-1]) {
				new_assumptions.push_back(-af.arg_var[i]);
			}
		}
	}
	return new_assumptions;
}

void initialize_assumptions(const AF & af, vector<int> & assumptions)
{
	for (uint32_t i = 0; i < af.dyn_atts.size(); i++) {
		if (af.dyn_att_state.at(af.dyn_atts[i])) {
			assumptions[i] = af.att_var.at(af.dyn_atts[i]);
		} else {
			assumptions[i] = -af.att_var.at(af.dyn_atts[i]);
		}
	}
}

void update_assumptions(const AF & af, vector<int> & assumptions, const uint32_t change_index)
{
	pair<uint32_t,uint32_t> att = af.changes[change_index].first;
	bool is_addition = af.changes[change_index].second;
#if defined(ADD_UNITS)
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		if (af.att_var.at(att) == abs(assumptions[i])) {
			if (af.att_encountered_last.at(att) == change_index+1) {
				assumptions.erase(assumptions.begin()+i);
			} else {
				if (is_addition) {
					assumptions[i] = abs(assumptions[i]);
				} else {
					assumptions[i] = -abs(assumptions[i]);
				}
			}
			break;
		}
	}
#else
	int current = assumptions[af.att_var.at(att)-af.args-1];
	if (is_addition) {
		assumptions[af.att_var.at(att)-af.args-1] = abs(current);
	} else {
		assumptions[af.att_var.at(att)-af.args-1] = -abs(current);
	}
#endif
}

#if defined(CHECK_POS)
vector<int> is_extension_assumptions(const AF & af, const vector<uint8_t> & assignment, const vector<int> & assumptions)
{
	vector<int> new_assumptions = assumptions;
	new_assumptions.reserve(assumptions.size() + af.args);
	for (uint32_t i = 0; i < af.args; i++) {
		if (assignment[i]) {
			new_assumptions.push_back(af.arg_var[i]);
		} else {
			new_assumptions.push_back(-af.arg_var[i]);
		}
	}
	return new_assumptions;
}
#endif

#if defined(CHECK_NEG)
bool conflict_clause_valid(const AF & af, const vector<int> & conflict, const vector<int> & assumptions, const uint32_t change_index)
{
	pair<uint32_t,uint32_t> att = af.changes[change_index-1].first;
#if defined(ADD_UNITS)
	bool is_addition = af.changes[change_index-1].second;
	int new_assumption = is_addition ? af.att_var.at(att) : -af.att_var.at(att);
#else
	int new_assumption = assumptions[af.att_var.at(att)-af.args-1];
#endif
	for (uint32_t i = 0; i < conflict.size(); i++) {
		if (abs(conflict[i]) == abs(new_assumption)) {
			return conflict[i] == new_assumption;
		}
	}
	return true;
}
#endif

void print_begin(bool new_line)
{
	string bracket = "[";
	if (new_line) bracket += "\n";
	cout << bracket << flush;
}

void print_end(bool new_line)
{
	string bracket = "]";
	if (new_line) bracket += "\n";
	cout << bracket << flush;
}

void print_yes(bool comma)
{
	string answer = "YES";
	if (comma) answer += ", ";
	cout << answer << flush;
}

void print_no(bool comma)
{
	string answer = "NO";
	if (comma) answer += ", ";
	cout << answer << flush;
}

void print_single_extension(const AF & af, const std::vector<uint32_t> & extension, bool tab)
{
	if (tab) cout << "\t";
	cout << "[";
	for (uint32_t i = 0; i < extension.size(); i++) {
		cout << af.int_to_arg[extension[i]];
		if (i != extension.size()-1) cout << ",";
	}
	cout << "]" << endl;
}


vector<std::string> get_single_extension(const AF & af, const std::vector<uint32_t> & extension, bool tab)
{
	vector<std::string> result = vector<std::string>();
	//if (tab) cout << "\t";
	//cout << "[";
	for (uint32_t i = 0; i < extension.size(); i++) {
		//cout << af.int_to_arg[extension[i]];
		result.push_back(af.int_to_arg[extension[i]]);
	//	if (i != extension.size()-1) cout << ",";
	}
	//cout << "]" << endl;
	return result;
}

void print_extensions(const AF & af, const std::vector<std::vector<uint32_t>> & extensions)
{
	cout << "\t[\n";
	for (uint32_t i = 0; i < extensions.size(); i++) {
		cout << "\t\t[";
		for (uint32_t j = 0; j < extensions[i].size(); j++) {
			cout << af.int_to_arg[extensions[i][j]];
			if (j != extensions[i].size()-1) cout << ",";
		}
		cout << "]" << "\n";
	}
	cout << "\t]" << endl;
}