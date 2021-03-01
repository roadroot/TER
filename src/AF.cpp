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

#include "AF.h"
#include "IAF.h"

using namespace std;

AF::AF() : args(0), count(0) {}
AF::AF(ter::IAF &iaf) : args(0), count(0)
{
	// TODO Implemented by us
	for (string arg : iaf.get_args())
		add_argument(arg);
	initialize_att_containers();
	for (ter::Attack atk : iaf.get_attacks())
		add_attack(make_pair(atk.source, atk.target));
}

AF::AF(vector<string> args, vector<ter::Attack> attacks) : args(0), count(0)
{
	// TODO Implemented by us
	for (string arg : args)
		add_argument(arg);
	initialize_att_containers();
	for (ter::Attack atk : attacks)
		add_attack(make_pair(atk.source, atk.target));
}

void AF::add_argument(string arg)
{
	int_to_arg.push_back(arg);
	arg_to_int[arg] = args++;
}

void AF::add_attack(pair<string, string> att)
{
	// TODO SAVED attacks with indefined args are ignored
	if (arg_to_int.count(att.first) == 0 || arg_to_int.count(att.second) == 0)
	{
		return;
	}
	uint32_t source = arg_to_int[att.first];
	uint32_t target = arg_to_int[att.second];
	attackers[target].push_back(source);
	if (source == target)
	{
		self_attack[source] = true;
	}
	att_exists[make_pair(source, target)] = true;
	if (att_exists.count(make_pair(target, source)) > 0)
	{
		symmetric_attack[make_pair(source, target)] = true;
		symmetric_attack[make_pair(target, source)] = true;
	}
	else
	{
		symmetric_attack[make_pair(source, target)] = false;
	}
}

void AF::add_dyn_attack(pair<string, string> att)
{
	dyn_atts.push_back(make_pair(arg_to_int[att.first], arg_to_int[att.second]));
	dyn_attackers[arg_to_int[att.second]].push_back(arg_to_int[att.first]);
}

void AF::add_change(pair<string, string> att, bool is_addition)
{
	pair<uint32_t, uint32_t> att_to_int = make_pair(arg_to_int[att.first], arg_to_int[att.second]);
	changes.push_back(make_pair(att_to_int, is_addition));
	att_encountered_last[att_to_int] = changes.size();
}

void AF::initialize_att_containers()
{
	attackers.resize(args);
	dyn_attackers.resize(args);
	self_attack.resize(args);
}

void AF::initialize_vars()
{
	arg_var.resize(args);
	for (uint32_t i = 0; i < args; i++)
	{
		arg_var[i] = ++count;
	}

	for (uint32_t i = 0; i < dyn_atts.size(); i++)
	{
		att_var[dyn_atts[i]] = ++count;
	}

	if (sem == STG || sem == SST)
	{
		range_var.resize(args);
		for (uint32_t i = 0; i < args; i++)
		{
			range_var[i] = ++count;
		}
	}

	if (sem != STG)
	{
		attacked_by_accepted_var.resize(args);
		for (uint32_t i = 0; i < args; i++)
		{
			attacked_by_accepted_var[i] = ++count;
		}
	}

	if (dyn_atts.size() > 0)
	{
		for (uint32_t i = 0; i < dyn_atts.size(); i++)
		{
			source_accepted_var[dyn_atts[i]] = ++count;
		}

		for (uint32_t i = 0; i < dyn_atts.size(); i++)
		{
			source_attacked_by_accepted_var[dyn_atts[i]] = ++count;
		}

		selector_var.resize(changes.size() + 1);
		for (uint32_t i = 0; i <= changes.size(); i++)
		{
			selector_var[i] = count + i + 1;
		}
	}
}