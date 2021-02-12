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

#ifndef ARGU_FRAMEWORK_H
#define ARGU_FRAMEWORK_H

#include <vector>
#include <unordered_map>
#include <string>
#include "IAF.h"

enum task { DC, DS, SE, EE, UNKNOWN_TASK };
enum semantics { CO, PR, ST, GR, SST, STG, ID, UNKNOWN_SEM };

/* The following hash_combine function is from the Boost software library
 * and is subject to the following licence.
 *
 * Boost Software License - Version 1.0 - August 17th, 2003
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
  std::hash<T> hasher;
  seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
  template<typename S, typename T> struct hash<pair<S, T>>
  {
	inline size_t operator()(const pair<S, T> & v) const
	{
	  size_t seed = 0;
	  ::hash_combine(seed, v.first);
	  ::hash_combine(seed, v.second);
	  return seed;
	}
  };
}

class AF {
public:

AF();
AF(ter::IAF*);
AF(vector<string>, vector<ter::Attack>);

semantics sem;

uint32_t args;
uint32_t count;

std::vector<std::string> int_to_arg;
std::unordered_map<std::string,uint32_t> arg_to_int;

std::vector<std::vector<uint32_t>> attackers;
std::vector<uint8_t> self_attack;
std::unordered_map<std::pair<uint32_t,uint32_t>,bool> att_exists;
std::unordered_map<std::pair<uint32_t,uint32_t>,bool> symmetric_attack;

std::vector<std::pair<std::pair<uint32_t,uint32_t>,bool>> changes;
std::vector<std::pair<uint32_t,uint32_t>> dyn_atts;
std::vector<std::vector<uint32_t>> dyn_attackers;
std::unordered_map<std::pair<uint32_t,uint32_t>,bool> dyn_att_state;
std::unordered_map<std::pair<uint32_t,uint32_t>,uint32_t> att_encountered_last;

std::vector<int> arg_var;
std::vector<int> range_var;
std::vector<int> attacked_by_accepted_var;

std::unordered_map<std::pair<uint32_t,uint32_t>,int> att_var;
std::unordered_map<std::pair<uint32_t,uint32_t>,int> source_accepted_var;
std::unordered_map<std::pair<uint32_t,uint32_t>,int> source_attacked_by_accepted_var;

std::vector<int> selector_var;

void add_argument(std::string arg);
void add_attack(std::pair<std::string,std::string> att);

void add_change(std::pair<std::string,std::string> att, bool is_addition);
void add_dyn_attack(std::pair<std::string,std::string> att);

void initialize_att_containers();
void initialize_vars();

};

#endif
