/**
 * @file 		Argument.cpp
 * @class 		Argument
 * @brief 		Class representing an Argument
 * @author 		Federico Cerutti <federico.cerutti@acm.org>
 * @copyright	MIT
 */

#include "Argument.h"

/**
 *	@brief 				Constructor
 *	@param[in] name		The name of the argument. It **must** be unique
 *	@param[in] num		A number identifying this argument. It **must** be unique
 *	@param[in] theaf	The pointer to the Argumentation Framework to which this argument belongs
 */
Argument::Argument(string name, int num, AF *theaf)
{
	this->name = name;
	this->number = num;
	this->af = theaf;
//	this->attackers = new SetArguments();
//	this->attacks = new SetArguments();
	this->color_scc = NILSCC;
	this->pi_scc = NULL;
}

Argument::~Argument()
{
	// TODO Auto-generated destructor stub
}

/**
 * @brief 			Method returning the name of the argument
 * @retval string
 */
string Argument::getName() const
{
	return this->name;
}

/**
 * @brief			Method returning the unique integer identifier of this argument
 * @retval int
 */
int Argument::getNumber() const
{
	return this->number;
}

/**
 * @brief			Method for determining a propositional variable representing that
 * 					this argument should be labelled as `IN`
 * @retval Variable
 */
Variable Argument::InVar()
{
	return this->number + 1;
}

/**
 * @brief			Method for determining a propositional variable representing that
 * 					this argument should be labelled as `OUT`
 * @retval Variable
 */
Variable Argument::OutVar()
{
	return this->number + 1 + this->af->numArgs();
}

/**
 * @brief			Method for determining a propositional variable representing that
 * 					this argument should be labelled as `UNDEC`
 * @retval Variable
 */
Variable Argument::UndecVar()
{
	return this->number + 1 + 2 * this->af->numArgs();
}

/**
 * @brief			Method for determining **the negation** of the propositional variable representing that
 * 					this argument should be labelled as `IN`
 * @retval Variable
 */
Variable Argument::NotInVar()
{
	return NOT(this->InVar());
}

/**
 * @brief			Method for determining **the negation** of the propositional variable representing that
 * 					this argument should be labelled as `OUT`
 * @retval Variable
 */
Variable Argument::NotOutVar()
{
	return NOT(this->OutVar());
}

/**
 * @brief			Method for determining **the negation** of the propositional variable representing that
 * 					this argument should be labelled as `UNDEC`
 * @retval Variable
 */
Variable Argument::NotUndecVar()
{
	return NOT(this->UndecVar());
}

/**
 * @brief 		Overloading of the == operator
 * @param[in] 	other   The other term of the comparison
 * @retval bool
 */
bool Argument::operator==(const Argument &other) const
{
	if (this->name == other.getName() &&
			this->number == other.getNumber())
		return true;
	else
		return false;
}

/**
 * @brief 		Overloading of the != operator
 * @param[in] 	other   The other term of the comparison
 * @retval bool
 */
bool Argument::operator!=(const Argument &other) const
{
	if (this->name == other.getName() &&
			this->number == other.getNumber())
		return false;
	else
		return true;
}

/**
 * @brief  Method for adding an attacks originated from this argument
 * @param[in] arg
 * @retval void
 */
void Argument::add_attacks(Argument *arg)
{
	this->af->add_attacks(this, arg);
//	this->attacks->add_Argument(arg);
}

/**
 * @brief  Method for adding an attacker
 * @param[in] arg
 * @retval void
 */
void Argument::add_attackers(Argument *arg)
{
	this->af->add_attackers(this, arg);
	//this->attackers->add_Argument(arg);
}

/**
 * @brief  Method returning the set of attackers
 * @retval SetArguments*
 */
SetArguments *Argument::get_attacks(SetArguments *restriction)
{
	//return this->attacks;
	return this->af->get_attacks(this, restriction);
}

/**
 * @brief  Method returning the set of attacked arguments
 * @retval SetArguments*
 */
SetArguments *Argument::get_attackers(SetArguments *restriction)
{
	//return this->attackers;
	return this->af->get_attackers(this, restriction);
}


SetArguments *Argument::get_all_attacks()
{
	//return this->attacks;
	return this->get_attacks(this->af->get_arguments());
}

/**
 * @brief  Method returning the set of attacked arguments
 * @retval SetArguments*
 */
SetArguments *Argument::get_all_attackers()
{
	//return this->attackers;
	return this->get_attackers(this->af->get_arguments());
}

/**
 * @brief  Method returning the AF to which this argument belongs
 * @retval AF*
 */
AF *Argument::get_af()
{
	return this->af;
}
