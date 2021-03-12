/**
 * @file 		AF.cpp
 * @author 		Mauro Vallati <m.vallati@hud.ac.uk>
 * @copyright	MIT
 */

#ifndef GRAPH_H_
#define GRAPH_H_

#include <map>
#include <fstream>

#include "Argument.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>


class SetArguments;
class SetArgumentsIterator;

using namespace std;

class AF
{
	SetArguments *arguments;
	//vector<pair<int, int> > raw_attacks;
	map<string, SetArguments *> *attacks;
	map<string, SetArguments *> *attackers;
	map<string, SetArguments *> *attacks_hashed;
	map<string, SetArguments *> *attackers_hashed;
	SetArguments *emptyset;
	string get_hash_attacks(Argument *, SetArguments *);
	string get_name_from_hash(string);
public:
	AF();
	virtual ~AF();
	bool readFile(string file);
	int numArgs();
	Argument *getArgumentByName(string);
	Argument *getArgumentByNumber(int position);
	SetArguments *get_arguments() const;
	SetArgumentsIterator begin();
	SetArgumentsIterator end();
	void add_attacks(Argument *, Argument *);
	void add_attackers(Argument *, Argument *);
	SetArguments *get_attacks(Argument *, SetArguments *);
	SetArguments *get_attackers(Argument *, SetArguments *);
	void add_argument(Argument *);
	void union_to(SetArguments *);
	void clear_cache();
};

#include "SetArguments.h"

#endif /* GRAPH_H_ */
