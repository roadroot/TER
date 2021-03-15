/**
 * @file 		AF.cpp
 * @class 		AF
 * @brief 		Class encompassing an Argumentation Framework
 * @author 		Mauro Vallati <m.vallati@hud.ac.uk>
 * @copyright	MIT
 */

#include "AF.h"

/**
 * @brief 	Simple constructor
 */
AF::AF()
{
	this->arguments = new SetArguments();
	//this->raw_attacks = map<int, int >();
	this->attacks = new map<string, SetArguments *>();
	this->attackers = new map<string, SetArguments *>();
	this->attacks_hashed = new map<string, SetArguments *>();
	this->attackers_hashed = new map<string, SetArguments *>();
	this->emptyset = new SetArguments();
}

AF::~AF()
{
}

string AF::get_hash_attacks(Argument *index, SetArguments *restriction)
{
	stringstream ss;
	ss << index->getName() << (*restriction);
	return ss.str().c_str();
}

string AF::get_name_from_hash(string hash)
{
	char *p = strtok((char *) hash.c_str(), "{");
	return string(p);
}

/**
 * @brief 		Method for parsing a ASPARTIX compliant file
 * @details		It uses part of the code from Samer Nofal's, University of Liverpool
 * @param[in] file A string representing the (relative/absolute) path to the ASPARTIX compliant file
 * @retval bool It returns `True` if it can correctly parse the file, `False` otherwise
 */
bool AF::readFile(string file)
{
	string inLine;
	ifstream infile;
	infile.open(file.c_str());
	if (!infile.good())
		return false;
	while (true)
	{
		infile >> inLine;
		if (inLine.find("arg") != string::npos && !infile.eof())
		{
			this->arguments->add_Argument(
					new Argument(inLine.substr(4, inLine.find_last_of(")") - 4),
							this->numArgs(), this));
		}
		else
			break;
	}

	while (true)
	{
		if (inLine.find("att") != string::npos)
		{
			Argument *source = this->getArgumentByName(
					(inLine.substr(4, inLine.find_last_of(",") - 4)));
			Argument *target = this->getArgumentByName(
					(inLine.substr(inLine.find_last_of(",") + 1,
							inLine.find_last_of(")") - inLine.find_last_of(",")
									- 1)));

			source->add_attacks(target);
			target->add_attackers(source);
//			this->raw_attacks.push_back(
//					pair<int, int>(
//							this->argNameToCode(
//									(inLine.substr(4,
//											inLine.find_last_of(",") - 4))),
//							this->argNameToCode(
//									(inLine.substr(inLine.find_last_of(",") + 1,
//											inLine.find_last_of(")")
//													- inLine.find_last_of(",")
//													- 1)))));
		}
		if (!infile.eof())
			infile >> inLine;
		else
			break;
	}
	infile.close();

	return true;
}

/**
 * @brief		This method returns the pointer to the Argument whose name is given as parameter
 * @param[in] name	 The name of the argument
 * @retval Argument* The pointer to the Argument object, or NULL if not found
 */
Argument *AF::getArgumentByName(string name)
{
	return this->arguments->getArgumentByName(name);
}

/**
 * @brief 	Returns the number of arguments
 * @retval int
 */
int AF::numArgs()
{
	return this->arguments->cardinality();
}

/**
 * @brief		This method returns the pointer to the Argument whose identification number is given as parameter
 * @param[in] num	 The name of the argument
 * @retval Argument* The pointer to the Argument object, or NULL if not found
 */
Argument *AF::getArgumentByNumber(int num)
{
	return this->arguments->getArgumentByNumber(num);
}

/**
 * @brief 	Begin of the iterator for the set of arguments
 * @retval SetArgumentsIterator An iterator pointing at the first of the elements of the set of arguments
 */
SetArgumentsIterator AF::begin()
{
	return this->arguments->begin();
}

/**
 * @brief 	End of the iterator for the set of arguments
 * @retval SetArgumentsIterator An iterator pointing at the last of the elements of the set of arguments
 */
SetArgumentsIterator AF::end()
{
	return this->arguments->end();
}

/**
 * @brief	Returns a pointer to the set of arguments
 * @retval SetArguments*
 */
SetArguments *AF::get_arguments() const
{
	return this->arguments;
}

void AF::add_attacks(Argument *source, Argument *target)
{
	map<string, SetArguments *>::iterator it = this->attacks->find(
			source->getName());
	if (it == this->attacks->end())
	{
		SetArguments *new_targets = new SetArguments();
		new_targets->add_Argument(target);
		this->attacks->insert(
				pair<string, SetArguments *>(source->getName(), new_targets));
	}
	else
	{
		(*it).second->add_Argument(target);
	}
}

void AF::add_attackers(Argument *target, Argument *source)
{
	map<string, SetArguments *>::iterator it = this->attackers->find(
			target->getName());
	if (it == this->attackers->end())
	{
		SetArguments *new_sources = new SetArguments();
		new_sources->add_Argument(source);
		this->attackers->insert(
				pair<string, SetArguments *>(target->getName(), new_sources));
	}
	else
	{
		(*it).second->add_Argument(source);
	}
}

SetArguments *AF::get_attacks(Argument *index, SetArguments *restriction)
{
//	cerr << "Search for: " << index->getName() << endl;
//	cerr << "Given this restriction: " << *restriction << endl;
	SetArguments *ret = new SetArguments();
	map<string, SetArguments *>::iterator it = this->attacks_hashed->find(
			this->get_hash_attacks(index, restriction));

	if (it != this->attacks_hashed->end())
	{
		return (*it).second;
	}
	else
	{
		map<string, SetArguments *>::iterator it2 = this->attacks->find(
				index->getName());

		if (it2 == this->attacks->end())
		{
			return this->emptyset;
		}

		for (SetArgumentsIterator stay = (*it2).second->begin();
				stay != (*it2).second->end(); stay++)
		{
			if (restriction->exists((*stay)))
			{
				ret->add_Argument((*stay));
			}
		}

		this->attacks_hashed->insert(
				pair<string, SetArguments *>(
						this->get_hash_attacks(index, restriction), ret));

		return (*(this->attacks_hashed->find(
				this->get_hash_attacks(index, restriction)))).second;
	}
}

SetArguments *AF::get_attackers(Argument *index, SetArguments *restriction)
{
	SetArguments *ret = new SetArguments();
	map<string, SetArguments *>::iterator it = this->attackers_hashed->find(
			this->get_hash_attacks(index, restriction));

	if (it != this->attackers_hashed->end())
	{
		return (*it).second;
	}
	else
	{
		map<string, SetArguments *>::iterator it2 = this->attackers->find(
				index->getName());

		if (it2 == this->attackers->end())
		{
			return this->emptyset;
		}

		for (SetArgumentsIterator stay = (*it2).second->begin();
				stay != (*it2).second->end(); stay++)
		{
			if (restriction->exists((*stay)))
			{
				ret->add_Argument((*stay));
			}
		}

		this->attackers_hashed->insert(
				pair<string, SetArguments *>(
						this->get_hash_attacks(index, restriction), ret));

		return (*(this->attackers_hashed->find(
				this->get_hash_attacks(index, restriction)))).second;
	}

}

void AF::add_argument(Argument *arg)
{
	this->arguments->add_Argument(arg);
}

void AF::union_to(SetArguments *other)
{
	for (SetArgumentsIterator arg = other->begin(); arg != other->end(); arg++)
	{
		this->add_argument(
				new Argument((*arg)->getName(), (*arg)->getNumber(), this));
	}

	for (SetArgumentsIterator source = other->begin(); source != other->end();
			source++)
	{
		for (SetArgumentsIterator target = (*source)->get_all_attacks()->begin(); target != (*source)->get_all_attacks()->end(); target++)
		{
			this->getArgumentByName((*source)->getName())->add_attacks(this->getArgumentByName((*target)->getName()));
		}
	}
}

void AF::clear_cache()
{
	this->attackers_hashed->clear();
	this->attacks_hashed->clear();
}
