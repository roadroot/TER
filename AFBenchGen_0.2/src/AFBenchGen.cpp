/**
 * @file 		AFBenchGen.cpp
 * @brief 		Main file
 * @author 		Federico Cerutti <federico.cerutti@acm.org> and
 * 				Mauro Vallati <m.vallati@hud.ac.uk>
 * @copyright	MIT
 */

#include "generator.h"
#include <boost/random/normal_distribution.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>

using namespace std;

/**
 * @brief Configuration variables
 */
bool debug = false;
double SCCnumMEAN = 0.0;
double SCCnumSTD = 0.0;
double SCCdimMEAN = 0.0;
double SCCdimSTD = 0.0;
int SCCpercC = -1;
int SCCpercAffected = -1;
int SCCpercEach = -1;
int SCCpercAffectAnotherSCC = -1;
int SCCdir = 1;

string filestat = "";


boost::variate_generator<boost::mt19937, boost::normal_distribution<> > generator(
			boost::mt19937(time(0)), boost::normal_distribution<>());

#ifndef UNIT_TEST

int rand_int(int max)
{
	return rand() % max;
}

template<class T>
T gen_normal(T generator, double *res)
{
	*res = generator();
	return generator;
}

int get_random(double mean, double std)
{
	double res;
	generator = gen_normal(generator, &res);
	return (int) (mean + std * res);
}

void random_set_arguments(int num, int perc, bool scc, AF *ret)
{
	AF *temp = NULL;
	if (num <= 1)
		num = 5;

	if (perc <= 1)
		perc = 25;

	while (true)
	{
		temp = new AF();
		for (int i = ret->numArgs() + 1; i <= ret->numArgs() + num; i++)
		{
			stringstream ss;
			ss << "a" << i;
			temp->add_argument(new Argument(ss.str().c_str(), i, temp));
		}

		for (SetArgumentsIterator source = temp->begin(); source != temp->end();
				source++)
		{
			for (SetArgumentsIterator target = temp->begin();
					target != temp->end(); target++)
			{
				if (rand_int(100) <= perc)
				{
					(*source)->add_attacks((*target));
					(*target)->add_attackers((*source));
				}
			}
		}

//		for (SetArgumentsIterator source = temp->begin(); source != temp->end();
//				source++)
//		{
//			cout << (*source)->getName() << "-> "
//					<< *((*source)->get_all_attacks()) << endl;
//		}

		SCC *test = new SCC(temp->get_arguments());
//		for (SCC::iterator sc = test->begin(); sc != test->end(); sc++)
//		{
//			cout << *(*sc) << endl;
//		}
//		cerr << "Size: " << test->size() << endl;
		if (test->size() == 1)
		{
			ret->union_to(temp->get_arguments());
//			cerr << "SCC created" << endl;
			break;
		}
		else
		{
			delete temp;
		}
	}
}





/**
 * @brief 	Main
 * @retval int	The return value can be:
 * 				* `-127`: Missing parameters
 * 				* `0`: SUCCESS!!!
 *
 */
int main(int argc, char *argv[])
{
	srand(time(NULL));
	if (argc <= 1)
	{
		showHelp(hgrev);
		return -127;
	}

	if (!parseParams(argc, argv))
	{
		showHelp(hgrev);
		return -127;
	}

	if (filestat != "")
	{
		AF *af = new AF();
		if (!af->readFile(filestat))
		{
			cerr << "Error opening the file" << endl;
			return -125;
		}
		SCC *sccs = new SCC(af->get_arguments());
		cout << "dim:" << af->get_arguments()->cardinality() << endl;
		cout << "scc:" << sccs->size() << endl;
		int sccindex = 1;
		for (SCC::iterator ascc = sccs->begin(); ascc != sccs->end(); ascc++)
		{
			cout << "scc-num" << sccindex++ << ":" << (*ascc)->cardinality()
					<< endl;
		}
	}
	else
	{
		if (SCCnumMEAN == 0.0 || SCCdimMEAN == 0.0)
		{
			cerr
					<< "mean and standard deviations both for number of SCCs and for their dimension is mandatory"
					<< endl;
			return -127;
		}

		AF *overall = new AF();

		int SCCnum = get_random(SCCnumMEAN, SCCnumSTD);

		if (SCCpercC == -1)
		{
			SCCpercC = rand_int(100);
		}
		if (SCCpercAffected == -1)
		{
			SCCpercAffected = rand_int(100);
		}
		if(SCCpercEach == -1)
		{
			SCCpercEach = rand_int(100);
		}
		if(SCCpercAffectAnotherSCC == -1)
		{
			SCCpercAffectAnotherSCC = rand_int(100);
		}


		for (int i = 0; i < SCCnum; i++)
		{
			random_set_arguments(get_random(SCCdimMEAN, SCCdimSTD), SCCpercEach, true,
					overall);
		}

		SCC *res = new SCC(overall->get_arguments());
		for (SCC::iterator scc = res->begin(); scc != res->end(); scc++)
		{
			for (SCC::iterator sccdest = (SCCdir == 1? scc + 1: res->begin()); sccdest != res->end();
					sccdest++)
			{
				if (rand_int(100) <= SCCpercAffectAnotherSCC)
				{
					for (SetArgumentsIterator source = (*scc)->begin();
							source != (*scc)->end(); source++)
					{
						if (rand_int(100) <= SCCpercC)
						{
							for (SetArgumentsIterator target =
									(*sccdest)->begin();
									target != (*sccdest)->end(); target++)
							{
								if (rand_int(100) <= SCCpercAffected)
								{
									(*source)->add_attacks((*target));
									(*target)->add_attackers((*source));
									//cerr << (*source)->getName() << "->" << (*target)->getName() << endl;
								}
							}
						}
					}
				}
			}
		}


		overall->clear_cache();

		for (SetArgumentsIterator arg = overall->begin(); arg != overall->end();
				arg++)
		{
			cout << "arg(" << (*arg)->getName() << ")." << endl;
		}
		for (SetArgumentsIterator arg = overall->begin(); arg != overall->end();
				arg++)
		{
			for (SetArgumentsIterator dest = (*arg)->get_all_attacks()->begin();
					dest != (*arg)->get_all_attacks()->end(); dest++)
			{
				cout << "att(" << (*arg)->getName() << "," << (*dest)->getName()
						<< ")." << endl;
			}
		}

	}

	return 0;
}
#endif
