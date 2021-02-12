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
#include "EnumerateExtensions.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

namespace EnumerateExtensions {

bool complete(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_complete(af, solver);
	print_begin(true);
	while (true) {
		bool sat = solver.solve();
		if (!sat) break;
		vector<uint32_t> extension;
		vector<int> blocking_clause(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
				blocking_clause[i] = -af.arg_var[i];
			} else {
				blocking_clause[i] = af.arg_var[i];
			}
		}
		print_single_extension(af, extension, 1);
		solver.add_clause(blocking_clause);
	}
	print_end(true);
	return true;
}

bool preferred(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_complete(af, solver);
	print_begin(true);
	while (true) {
		bool sat = solver.solve();
		if (!sat) break;
		vector<int> complement_clause;
		complement_clause.reserve(af.args);
		vector<int> assumptions;
		assumptions.reserve(af.args);
		vector<uint8_t> visited;
		visited.resize(af.args);
		while (true) {
			complement_clause.clear();
			for (uint32_t i = 0; i < af.args; i++) {
				if (solver.assignment[af.arg_var[i]-1]) {
					if (!visited[i]) {
						assumptions.push_back(af.arg_var[i]);
						visited[i] = true;
					}
				} else {
					complement_clause.push_back(af.arg_var[i]);
				}
			}
			solver.add_clause(complement_clause);
			bool superset = solver.solve(assumptions);
			if (!superset) break;
		}
		vector<uint32_t> extension;
		for (uint32_t i = 0; i < af.args; i++) {
			if (visited[i]) {
				extension.push_back(i);
			}
		}
		print_single_extension(af, extension, 1);
	}
	print_end(true);
	return true;
}

bool stable(const AF & af)
{
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_stable(af, solver);
#if defined(GR_IN_ST)
	vector<int> assumptions = grounded_assumptions(af);
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		vector<int> clause = { assumptions[i] };
		solver.add_clause(clause);
	}
#endif
	bool extension_found = false;
	int jj=0;
	while (true) {
		jj++;
		bool sat = solver.solve();
		if (!sat) break;
		if (!extension_found) {
			print_begin(true);
			extension_found = true;
		}
		vector<uint32_t> extension;
		vector<int> blocking_clause(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
				blocking_clause[i] = -af.arg_var[i];
			} else{
				blocking_clause[i] = af.arg_var[i];
			}
		}
		std::cout << "hehe boi " << jj << " :" ;
		print_single_extension(af, extension, 1);
		solver.add_clause(blocking_clause);
	}
	if (!extension_found) print_begin(false);
	print_end(true);
	return extension_found;
}

vector<vector<std::string>> stable(const AF & af,bool usless)
{
	vector<vector<std::string>> *results = new vector<vector<std::string>>();
	SAT_Solver solver = SAT_Solver(af.count, af.args);
	Encodings::add_stable(af, solver);
#if defined(GR_IN_ST)
	vector<int> assumptions = grounded_assumptions(af);
	for (uint32_t i = 0; i < assumptions.size(); i++) {
		vector<int> clause = { assumptions[i] };
		solver.add_clause(clause);
	}
#endif
	bool extension_found = false;
	int jj=0;
	while (true) {
		jj++;
		bool sat = solver.solve();
		if (!sat) break;
		if (!extension_found) {
			//print_begin(true);
			extension_found = true;
		}

		vector<uint32_t> extension;
		vector<int> blocking_clause(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
				blocking_clause[i] = -af.arg_var[i];
			} else{
				blocking_clause[i] = af.arg_var[i];
			}
		}
		//std::cout << "hehe boi " << jj << " :" ;
		results->push_back(get_single_extension(af,extension,1));
		//print_single_extension(af, extension, 1);
		solver.add_clause(blocking_clause);
	}
	//if (!extension_found) print_begin(false);
	//print_end(true);
	return *results;
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
	bool stb_exists = false;
	while (solver.solve(range_assumption)) {
		if (!stb_exists) {
			print_begin(true);
			stb_exists = true;
		}
		vector<uint32_t> extension;
		vector<int> blocking_clause(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
				blocking_clause[i] = -af.arg_var[i];
			} else{
				blocking_clause[i] = af.arg_var[i];
			}
		}
		print_single_extension(af, extension, 1);
		solver.add_clause(blocking_clause);
	}
	if (stb_exists) {
		print_end(true);
		return true;
	}
#endif

	print_begin(true);

	while (true) {
		bool sat = solver.solve();
		if (!sat) break;

		vector<int> complement_clause;
		complement_clause.reserve(af.args);

		vector<int> assumptions;
		assumptions.reserve(af.args);

		vector<uint8_t> visited;
		visited.resize(af.args);

		vector<uint8_t> arg_accepted;
		arg_accepted.resize(af.args);

		while (true) {
			complement_clause.clear();
			for (uint32_t i = 0; i < af.args; i++) {
				if (solver.assignment[af.range_var[i]-1]) {
					if (!visited[i]) {
						assumptions.push_back(af.range_var[i]);
						visited[i] = true;
					}
				} else {
					complement_clause.push_back(af.range_var[i]);
				}
				arg_accepted[i] = solver.assignment[af.arg_var[i]-1];
			}
			solver.add_clause(complement_clause);
			bool superset = solver.solve(assumptions);
			if (!superset) break;
		}
		
		assumptions.resize(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (visited[i]) {
				assumptions[i] = af.range_var[i];
			} else {
				assumptions[i] = -af.range_var[i];
			}
		}
		
		SAT_Solver new_solver(af.count, 3*af.args);

		Encodings::add_complete(af, new_solver);
		Encodings::add_range(af, new_solver);

		while (true) {
			bool sat = new_solver.solve(assumptions);
			if (!sat) break;
			vector<uint32_t> extension;
			vector<int> blocking_clause(af.args);
			for (uint32_t i = 0; i < af.args; i++) {
				if (new_solver.assignment[af.arg_var[i]-1]) {
					extension.push_back(i);
					blocking_clause[i] = -af.arg_var[i];
				} else {
					blocking_clause[i] = af.arg_var[i];
				}
			}
			print_single_extension(af, extension, 1);
			new_solver.add_clause(blocking_clause);
		}
	}
	print_end(true);
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
	bool stb_exists = false;
	while (solver.solve(range_assumption)) {
		if (!stb_exists) {
			print_begin(true);
			stb_exists = true;
		}
		vector<uint32_t> extension;
		vector<int> blocking_clause(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (solver.assignment[af.arg_var[i]-1]) {
				extension.push_back(i);
				blocking_clause[i] = -af.arg_var[i];
			} else{
				blocking_clause[i] = af.arg_var[i];
			}
		}
		print_single_extension(af, extension, 1);
		solver.add_clause(blocking_clause);
	}
	if (stb_exists) {
		print_end(true);
		return true;
	}
#endif

	print_begin(true);

	while (true) {
		bool sat = solver.solve();
		if (!sat) break;

		vector<int> complement_clause;
		complement_clause.reserve(af.args);

		vector<int> assumptions;
		assumptions.reserve(af.args);

		vector<uint8_t> visited;
		visited.resize(af.args);

		vector<uint8_t> arg_accepted;
		arg_accepted.resize(af.args);

		while (true) {
			complement_clause.clear();
			for (uint32_t i = 0; i < af.args; i++) {
				if (solver.assignment[af.range_var[i]-1]) {
					if (!visited[i]) {
						assumptions.push_back(af.range_var[i]);
						visited[i] = true;
					}
				} else {
					complement_clause.push_back(af.range_var[i]);
				}
				arg_accepted[i] = solver.assignment[af.arg_var[i]-1];
			}
			solver.add_clause(complement_clause);
			bool superset = solver.solve(assumptions);
			if (!superset) break;
		}
		
		assumptions.resize(af.args);
		for (uint32_t i = 0; i < af.args; i++) {
			if (visited[i]) {
				assumptions[i] = af.range_var[i];
			} else {
				assumptions[i] = -af.range_var[i];
			}
		}
		
		SAT_Solver new_solver(af.count, 3*af.args);

		Encodings::add_conflict_free(af, new_solver);
		Encodings::add_range(af, new_solver);

		while (true) {
			bool sat = new_solver.solve(assumptions);
			if (!sat) break;
			vector<uint32_t> extension;
			vector<int> blocking_clause(af.args);
			for (uint32_t i = 0; i < af.args; i++) {
				if (new_solver.assignment[af.arg_var[i]-1]) {
					extension.push_back(i);
					blocking_clause[i] = -af.arg_var[i];
				} else {
					blocking_clause[i] = af.arg_var[i];
				}
			}
			print_single_extension(af, extension, 1);
			new_solver.add_clause(blocking_clause);
		}
	}
	print_end(true);
	return true;
}

}