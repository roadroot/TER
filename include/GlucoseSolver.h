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

#ifndef GLUCOSE_SOLVER_H
#define GLUCOSE_SOLVER_H

#include "SATSolver.h"
#if defined(SAT_GLUCOSE)
#include "core/Solver.h"
#endif

class GlucoseSolver : public SATSolver {

private:
#if defined(SAT_GLUCOSE)
	Glucose::Solver * solver;
#endif

public:
	GlucoseSolver(uint32_t number_of_vars, uint32_t number_of_arg_vars);
	~GlucoseSolver() { delete solver; }
#if defined(INCREMENTAL)
	void set_initial_vars(uint32_t number_of_initial_vars);
#endif
	void set_polarity(bool polarity);
	void add_clause(const std::vector<int> & clause);
	void add_clauses(const std::vector<std::vector<int>> & clauses);
	bool solve();
	bool solve(const std::vector<int> & assumptions);
	bool propagate();
	bool propagate(const std::vector<int> & assumptions);
};

#endif