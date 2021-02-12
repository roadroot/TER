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

using namespace std;

namespace Encodings {

void add_source_accepted_clauses(const AF & af, SAT_Solver & solver)
{
	for (uint32_t i = 0; i < af.dyn_atts.size(); i++) {
		vector<int> clause = { -af.source_accepted_var.at(af.dyn_atts[i]), af.att_var.at(af.dyn_atts[i]) };
		solver.add_clause(clause);
	}
	for (uint32_t i = 0; i < af.dyn_atts.size(); i++) {
		vector<int> clause = { -af.source_accepted_var.at(af.dyn_atts[i]), af.arg_var[af.dyn_atts[i].first] };
		solver.add_clause(clause);
	}
	for (uint32_t i = 0; i < af.dyn_atts.size(); i++) {
		vector<int> clause = { af.source_accepted_var.at(af.dyn_atts[i]), -af.arg_var[af.dyn_atts[i].first], -af.att_var.at(af.dyn_atts[i]) };
		solver.add_clause(clause);
	}
}

void add_attacked_by_accepted_clauses(const AF & af, SAT_Solver & solver)
{
	for (uint32_t i = 0; i < af.args; i++) {
		vector<int> additional_clause = { -af.attacked_by_accepted_var[i], -af.arg_var[i] };
		solver.add_clause(additional_clause);
		for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
			vector<int> clause = { af.attacked_by_accepted_var[i], -af.arg_var[af.attackers[i][j]] };
			solver.add_clause(clause);
		}
		for (uint32_t j = 0; j < af.dyn_attackers[i].size(); j++) {
			vector<int> clause = { af.attacked_by_accepted_var[i], -af.source_accepted_var.at(make_pair(af.dyn_attackers[i][j], i)) };
			solver.add_clause(clause);
		}
		vector<int> clause(af.attackers[i].size() + af.dyn_attackers[i].size() + 1);
		for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
			clause[j] = af.arg_var[af.attackers[i][j]];
		}
		for (uint32_t j = 0; j < af.dyn_attackers[i].size(); j++) {
			clause[j+af.attackers[i].size()] = af.source_accepted_var.at(make_pair(af.dyn_attackers[i][j], i));
		}
		clause[af.attackers[i].size() + af.dyn_attackers[i].size()] = -af.attacked_by_accepted_var[i];
		solver.add_clause(clause);
	}
}

void add_source_attacked_by_accepted_clauses(const AF & af, SAT_Solver & solver)
{
	for (uint32_t i = 0; i < af.dyn_atts.size(); i++) {
		vector<int> clause = { af.source_attacked_by_accepted_var.at(af.dyn_atts[i]), -af.attacked_by_accepted_var[af.dyn_atts[i].first] };
		solver.add_clause(clause);
	}
	for (uint32_t i = 0; i < af.dyn_atts.size(); i++) {
		vector<int> clause = { af.source_attacked_by_accepted_var.at(af.dyn_atts[i]), af.att_var.at(af.dyn_atts[i]) };
		solver.add_clause(clause);
	}
	for (uint32_t i = 0; i < af.dyn_atts.size(); i++) {
		vector<int> clause = { -af.source_attacked_by_accepted_var.at(af.dyn_atts[i]), af.attacked_by_accepted_var[af.dyn_atts[i].first], -af.att_var.at(af.dyn_atts[i]) };
		solver.add_clause(clause);
	}
}

void add_range(const AF & af, SAT_Solver & solver)
{
	if (af.sem == STG) {
		add_source_accepted_clauses(af, solver);
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause;
			clause = { af.range_var[i], -af.arg_var[i] };
			solver.add_clause(clause);
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				clause = { af.range_var[i], -af.arg_var[af.attackers[i][j]] };
				solver.add_clause(clause);
			}
			for (uint32_t j = 0; j < af.dyn_attackers[i].size(); j++) {
				clause = { af.range_var[i], -af.source_accepted_var.at(make_pair(af.dyn_attackers[i][j], i)) };
				solver.add_clause(clause);
			}
			clause.resize(af.attackers[i].size() + af.dyn_attackers[i].size() + 2);
			for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
				clause[j] = af.arg_var[af.attackers[i][j]];
			}
			for (uint32_t j = 0; j < af.dyn_attackers[i].size(); j++) {
				clause[j+af.attackers[i].size()] = af.source_accepted_var.at(make_pair(af.dyn_attackers[i][j], i));
			}
			clause[af.attackers[i].size()+af.dyn_attackers[i].size()] = af.arg_var[i];
			clause[af.attackers[i].size()+af.dyn_attackers[i].size()+1] = -af.range_var[i];
			solver.add_clause(clause);
		}
	} else {
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause = { af.range_var[i], -af.arg_var[i] };
			solver.add_clause(clause);
		}
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause = { af.range_var[i], -af.attacked_by_accepted_var[i] };
			solver.add_clause(clause);
		}
		for (uint32_t i = 0; i < af.args; i++) {
			vector<int> clause = { -af.range_var[i], af.arg_var[i], af.attacked_by_accepted_var[i] };
			solver.add_clause(clause);
		}
	}
}

void add_conflict_free(const AF & af, SAT_Solver & solver)
{
	for (uint32_t i = 0; i < af.args; i++) {
		for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
			vector<int> clause;
			if (i != af.attackers[i][j]) {
				clause = { -af.arg_var[i], -af.arg_var[af.attackers[i][j]] };
			} else {
				clause = { -af.arg_var[i] };
			}
			solver.add_clause(clause);
		}
		for (uint32_t j = 0; j < af.dyn_attackers[i].size(); j++) {
			vector<int> clause;
			if (i != af.dyn_attackers[i][j]) {
				clause = { -af.att_var.at(make_pair(af.dyn_attackers[i][j], i)), -af.arg_var[i], -af.arg_var[af.dyn_attackers[i][j]] };
			} else {
				clause = { -af.att_var.at(make_pair(af.dyn_attackers[i][j], i)), -af.arg_var[i] };
			}
			solver.add_clause(clause);
		}
	}
}

void add_admissible(const AF & af, SAT_Solver & solver)
{
	add_conflict_free(af, solver);
	add_source_accepted_clauses(af, solver);
	add_attacked_by_accepted_clauses(af, solver);
	add_source_attacked_by_accepted_clauses(af, solver);
	for (uint32_t i = 0; i < af.args; i++) {
		if (af.self_attack[i]) continue;
		for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
			if (af.sem != ST && af.sem != STG && af.symmetric_attack.at(make_pair(af.attackers[i][j], i))) continue;
			vector<int> clause = { -af.arg_var[i], af.attacked_by_accepted_var[af.attackers[i][j]] };
			solver.add_clause(clause);
		}
		for (uint32_t j = 0; j < af.dyn_attackers[i].size(); j++) {
			vector<int> clause = { -af.arg_var[i], af.source_attacked_by_accepted_var.at(make_pair(af.dyn_attackers[i][j], i)) };
			solver.add_clause(clause);
		}
	}
}

void add_complete(const AF & af, SAT_Solver & solver)
{
	add_admissible(af, solver);
	for (uint32_t i = 0; i < af.args; i++) {
		vector<int> clause(af.attackers[i].size()+af.dyn_attackers[i].size()+1);
		for (uint32_t j = 0; j < af.attackers[i].size(); j++) {
			clause[j] = -af.attacked_by_accepted_var[af.attackers[i][j]];
		}
		for (uint32_t j = 0; j < af.dyn_attackers[i].size(); j++) {
			clause[j+af.attackers[i].size()] = -af.source_attacked_by_accepted_var.at(make_pair(af.dyn_attackers[i][j], i));
		}
		clause[af.attackers[i].size() + af.dyn_attackers[i].size()] = af.arg_var[i];
		solver.add_clause(clause);
	}
}

void add_stable(const AF & af, SAT_Solver & solver)
{
#if defined(AD_IN_ST)
	add_admissible(af, solver);
#elif defined(CO_IN_ST)
	add_complete(af, solver);
#else
	add_conflict_free(af, solver);
	add_source_accepted_clauses(af, solver);
	add_attacked_by_accepted_clauses(af, solver);
#endif
	for (uint32_t i = 0; i < af.args; i++) {
		vector<int> clause = { af.arg_var[i], af.attacked_by_accepted_var[i] };
		solver.add_clause(clause);
	}
}

}