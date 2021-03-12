/**
 * @file 		SCC.h
 * @author 		Federico Cerutti <federico.cerutti@acm.org>
 * @copyright	MIT
 */

#ifndef SCC_H_
#define SCC_H_

#include "SetArguments.h"
#include <deque>
#include <string>
#include <iostream>

using namespace std;

class SCC
{
	SetArguments *gamma;
	deque<SetArguments *> *sccs;
	map<int, Argument *> *topological_original;
	deque<string> *roots;
	void SCCSEQ();
	int time;
	void DFS();
	void DFS_visit(Argument *);
	void DFS_visit_reverse(Argument *, SetArguments *);
	void DF_tree(Argument *, SetArguments *);
public:
	typedef deque<SetArguments *>::iterator iterator;
	typedef deque<SetArguments *>::const_iterator const_iterator;
	iterator begin() const { return this->sccs->begin(); }
	iterator end() const { return this->sccs->end(); }
	deque<SetArguments *> *get_sccs();
	SCC(SetArguments *);
	unsigned int size();
	virtual ~SCC();
};

ostream& operator<<(ostream& , const SCC& );

#endif /* SCC_H_ */
