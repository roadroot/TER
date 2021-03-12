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

#ifndef SAT_SOLVER_H
#define SAT_SOLVER_H

#include <stdint.h>
#include <vector>

class SATSolver {

public:

	uint32_t n_args; // number of interesting vars
	std::vector<uint8_t> assignment;
	std::vector<int> conflict;

	SATSolver() {}
	virtual ~SATSolver() {}
	virtual void add_clause(const std::vector<int> & clause) = 0;
	virtual void add_clauses(const std::vector<std::vector<int>> & clauses) = 0;
	virtual bool solve() = 0;
	virtual bool solve(const std::vector<int> & assumptions) = 0;
	virtual bool propagate() = 0;
	virtual bool propagate(const std::vector<int> & assumptions) = 0;

};

#endif