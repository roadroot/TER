/**
 * @file 		SetArguments.h
 * @author 		Federico Cerutti <federico.cerutti@acm.org>
 * @copyright	MIT
 */

#ifndef SETARGUMENTS_H_
#define SETARGUMENTS_H_

#include <map>
#include <string>
#include <iterator>
#include <algorithm>
using namespace std;

#include "Argument.h"

/**
 * @brief Class defining a const iterator for the elements of a set of arguments
 */
class SetArgumentsIterator: public map<string, Argument *>::const_iterator
{
public:
	SetArgumentsIterator() :
			map<string, Argument *>::const_iterator()
	{
	}
	;
	SetArgumentsIterator(map<string, Argument *>::const_iterator it) :
			map<string, Argument *>::const_iterator(it)
	{
	}
	;
	Argument ** operator->()
	{
		return (Argument ** const ) &(map<string, Argument *>::const_iterator::operator->()->second);
	}
	;
	Argument* operator*()
	{
		return map<string, Argument *>::const_iterator::operator*().second;
	}
	SetArgumentsIterator &operator=(const SetArgumentsIterator &param)
	{
		map<string, Argument *>::const_iterator::operator =(param);
		return *this;
	}
};

class SetArguments
{
	map<string, Argument *> arguments;
	map<int, string> key_assoc;
	SetArguments *range_p;
public:
	SetArguments();
	void add_Argument(Argument *);
	virtual ~SetArguments();
	Argument *getArgumentByName(string);
	Argument *getArgumentByNumber(int num);
	int cardinality() const;
	bool empty();
	SetArgumentsIterator begin() const;
	SetArgumentsIterator end() const;
	bool exists(Argument *);
	bool is_subset_equal(SetArguments *);
	bool is_subset(SetArguments *);
	void intersect(SetArguments *, SetArguments *);
	void setminus(SetArguments *, SetArguments *);
	void remove(Argument *);
	void clone(SetArguments *);
	bool operator==(const SetArguments &other) const;
	void union_to(SetArguments *);
	SetArguments *get_attackers(SetArguments *);
	SetArguments *get_attacks(SetArguments *);
	SetArguments *get_all_attackers();
	SetArguments *get_all_attacks();
	SetArguments *range_plus();
	string hash();
};

ostream& operator<<(ostream&, const SetArguments&);

#endif /* SETARGUMENTS_H_ */
