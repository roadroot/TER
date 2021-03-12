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

#ifndef UTIL_H
#define UTIL_H

#include "AF.h"

std::vector<int> grounded_assumptions(const AF & af);

void initialize_assumptions(const AF & af, std::vector<int> & assumptions);
void update_assumptions(const AF & af, std::vector<int> & assumptions, const uint32_t change_index);

#if defined(CHECK_POS)
std::vector<int> is_extension_assumptions(const AF & af, const std::vector<uint8_t> & assignment, const std::vector<int> & assumptions);
#endif

#if defined(CHECK_NEG)
bool conflict_clause_valid(const AF & af, const std::vector<int> & conflict, const std::vector<int> & assumptions, const uint32_t change_index);
#endif

void print_begin(bool new_line);
void print_end(bool new_line);

std::vector<std::string> get_single_extension(const AF & af, const std::vector<uint32_t> & extension, bool tab);


void print_yes(bool comma);
void print_no(bool comma);

void print_single_extension(const AF & af, const std::vector<uint32_t> & extension, const bool tab);
void print_extensions(const AF & af, const std::vector<std::vector<uint32_t>> & extensions);

#endif