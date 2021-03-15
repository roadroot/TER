/**
 * @file 		SCC.cpp
 * @class 		SCC
 * @brief 		Class for SCC recursiveness schema
 * @author 		Federico Cerutti <federico.cerutti@acm.org>
 * @copyright	MIT
 */

#include "SCC.h"
#include "generator.h"
#include <stack>

SCC::SCC(SetArguments *_gamma)
{
	this->gamma = _gamma;
	this->sccs = new deque<SetArguments *>();
	this->topological_original = new map<int, Argument*>();
	this->time = 0;
	this->roots = new deque<string>();
	this->SCCSEQ();
}

void SCC::SCCSEQ()
{
	this->DFS();

	for (map<int, Argument*>::reverse_iterator reverse = this->topological_original->rbegin(); reverse != this->topological_original->rend(); reverse++)
	{
		(*reverse).second->color_scc = WHITESCC;
		(*reverse).second->pi_scc = NULL;
		this->roots->push_back((*reverse).second->getName());
	}
	for (map<int, Argument*>::reverse_iterator reverse = this->topological_original->rbegin(); reverse != this->topological_original->rend(); reverse++)
	{
		if ((*reverse).second->color_scc == WHITESCC)
		{
			SetArguments *subset = new SetArguments();
			this->DFS_visit_reverse((*reverse).second, subset);
			this->sccs->push_back(subset);
		}
	}
}

void SCC::DF_tree(Argument *node, SetArguments *subset)
{
	//for (SetArgumentsIterator next = node->get_attackers(this->gamma)->begin(); next != node->get_attackers(this->gamma)->end(); next++)
	for (SetArgumentsIterator next = node->get_attacks(this->gamma)->begin(); next != node->get_attacks(this->gamma)->end(); next++)
	{
		if ((*next)->pi_scc != NULL && !subset->exists((*next)))
		{
			subset->add_Argument((*next));
			this->DF_tree((*next), subset);
		}
	}

}

void SCC::DFS_visit_reverse(Argument *u, SetArguments *subset)
{
	subset->add_Argument(u);
	u->color_scc = GRAYSCC;
	for (SetArgumentsIterator v = u->get_attackers(this->gamma)->begin(); v != u->get_attackers(this->gamma)->end(); v++)
	{
		if ((*v)->color_scc == WHITESCC)
		{
			(*v)->pi_scc = u;

			this->DFS_visit_reverse((*v), subset);
		}
	}
}

/**
 *
 * Cormer p. 604
 */
void SCC::DFS()
{
	for (SetArgumentsIterator it = this->gamma->begin(); it != this->gamma->end(); it++)
	{
		(*it)->color_scc = WHITESCC;
	}
	this->time = 0;


	for (SetArgumentsIterator it = this->gamma->begin(); it != this->gamma->end(); it++)
	{
		if ((*it)->color_scc == WHITESCC)
		{
			this->DFS_visit((*it));
		}
	}
}

void SCC::DFS_visit(Argument *u)
{
	this->time++;
	u->color_scc = GRAYSCC;
	for (SetArgumentsIterator v = u->get_attacks(this->gamma)->begin(); v != u->get_attacks(this->gamma)->end(); v++)
	{
		if ((*v)->color_scc == WHITESCC)
		{
			this->DFS_visit((*v));
		}
	}
	u->color_scc = BLACKSCC;
	this->time++;
	this->topological_original->insert(pair<int, Argument*>(this->time, u));
}


SCC::~SCC()
{
	// TODO Auto-generated destructor stub
}

unsigned int SCC::size()
{
	return this->sccs->size();
}


deque<SetArguments *> *SCC::get_sccs()
{
	return this->sccs;
}

ostream& operator<<(ostream& out, const SCC& r)
{
	int i = 1;
	for (SCC::const_iterator it = r.begin(); it != r.end(); it++)
	{
		out << (*(*it));
	}
	return out;
}
