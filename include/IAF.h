#ifndef IAF_H
#define IAF_H

#include <vector>
#include <string>
#include <Attack.h>
#include <fstream>
#include <iostream>

using namespace std;
namespace ter
{
	class IAF
	{
	public:
#pragma region constructors
		/**
		 * @brief Default IAF constructor with empty complete/incomplte args/attacks
		 */
		IAF();

		/**
         *  @brief  Generate an incomplete IAF from AF or IAF according to incomplete args ratio and incomplete attacks ratio.
         *  @param  iaf The complete or incomplete AF to generate IAF from.
         *  @param  iargs_ration The ration of incomplete args from all args.
		 *  Must be between 0 and 1, otherwise an exception will be raised.
		 *  The real ration can be greater than iargs_ration if inputed iaf has an incomplete arg ratio greater than iargs_ration.
         *  @param  iattacks_ration The ration of incomplete attacks from all attacks.
		 *  Must be between 0 and 1, otherwise an exception will be raised.
		 *  The real ration can be greater than iattacks_ration if the ratio of attacks containing an incomplete arg with inputed iaf incomplete attacks ratio is greater than iattacks_ration.
         */
		IAF(const IAF &iaf, const float &iargs_ration = 1, const float &iattacks_ration = 1);
#pragma endregion constructors

#pragma region complete_args_manipulation
		/**
        *  @brief  Get the arg at index in complete args.
        *  @param  index The index of the arg in complete args.
        *  @return  The arg in complete args at the index.
        */
		string get_arg(int index);

		/**
		*  @brief Add arg to the end of complete args.
		*  @param arg The complete arg to add.
		*/
		void add_arg(string &arg);
		
		/**
		*  @brief  Remove an argument at an index from complete args.
		*  @param  index The index where to remove.
		*  @return  true if element removed, false else.
		* 
		*  This function won't raise an exception if no element is removed
		*/
		bool remove_arg(int index);

		/**
		*  @brief  Remove a complete argument from complete args.
		*  @param  iarg The complete argument to remove.
		*  @return  true if element removed, false else.
		* 
		*  This function won't raise an exception if no element is removed
		*/
		bool remove_arg(string &arg);

		/**
        *  @brief  Get the list of complete args.
        *  @return The list of complete args.
        */
		vector<string> get_args();
#pragma endregion

#pragma region incomplete_args_manipulation
		/**
        *  @brief  Get the arg at index in incomplete args.
        *  @param  index The index of the arg in incomplete args.
        *  @return The arg in incomplete args at the index.
        */
		string get_iarg(int index);

		/**
		*  @brief Add iarg to the end of incomplete args.
		*  @param iarg The incomplete arg to add.
		*/
		void add_iarg(string &iarg);

		/**
		*  @brief  Remove an incomplete argument at an index from incomplete args.
		*  @param  index The index where to remove.
		*  @return  true if element removed, false else.
		* 
		*  This function won't raise an exception if no element is removed
		*/
		bool remove_iarg(int index);

		/**
		*  @brief  Remove an incomplete argument from incomplete args.
		*  @param  iarg The incomplete argument to remove.
		*  @return  true if element removed, false else.
		* 
		*  This function won't raise an exception if no element is removed
		*/
		bool remove_iarg(string &iarg);

		/**
        *  @brief  Get the list of incomplete args.
        *  @return The list of incomplete args.
        */
		vector<string> get_iargs();
#pragma endregion

#pragma region complete_attacks_manipulation
		/**
        *  @brief  Get the attack at index in complete attacks.
        *  @param  index The index of the attack in complete attacks.
        *  @return  The attack in complete attacks at the index.
        */
		Attack get_attack(int index);

		/**
       *  @brief  Add attack to the end of complete attacks.
       *  @param  attack the attack to add.
       */
		void add_attack(Attack &attack);

		/**
       *  @brief  Remove an attack at an index from complete attacks.
       *  @param  index The index where to remove.
       *  @return  true if element removed, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool remove_attack(int index);

		/**
       *  @brief  Remove first occurance of attack or all from complete attacks.
       *  @param  source The source of the attack that will be removed.
       *  @param  target The target of the attack that will be removed.
       *  @param  all Remove all occurances if true, first else.
       *  @return  true if at least once removed, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool remove_attack(string &source, string &target, bool all = false);

		/**
       *  @brief  Remove first occurance of attack or all from complete attacks.
       *  @param  attack The attack that will be removed.
       *  @param  all Remove all occurances if true, first else.
       *  @return  True if at least once removed, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool remove_attack(Attack &attack, bool all = false);

		/**
       *  @brief  Check if an attack is part from complete attacks.
       *  @param  attack The attack to check.
       *  @return  true if attack exists, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool attack_exists(Attack attack);

		/**
       *  @brief  Check if an attack is part from complete attacks.
       *  @param  source The source of the attack to check.
       *  @param  target The target of the attack to check.
       *  @return  true if attack exists, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool attack_exists(string source, string target);

		/**
        *  @brief  Get the list of complete attacks.
        *  @return The list of complete attacks.
        */
		vector<Attack> get_attacks();
#pragma endregion

#pragma region incomplete_attacks_manipulation
		/**
        *  @brief  Get the attack at index in incomplete attacks.
        *  @param  index The index of the attack in incomplete attacks.
        *  @return The attack in incomplete attacks at the index.
        */
		Attack get_iattack(int index);

		/**
        *  @brief  Add attack to the end of incomplete attacks.
        *  @param  attack The attack to add.
        */
		void add_iattack(Attack &attack);

		/**
       *  @brief  Remove an attack at an index from incomplete attacks.
       *  @param  index The index where to remove.
       *  @return  true if element removed, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool remove_iattack(int index);

		/**
       *  @brief  Remove first occurance of attack or all from incomplete attacks.
       *  @param  source The source of the attack that will be removed.
       *  @param  target The target of the attack that will be removed.
       *  @param  all Remove all occurances if true, first else.
       *  @return true if at least once removed, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool remove_iattack(string &source, string &target, bool all = false);

		/**
       *  @brief  Remove first occurance of attack or all from incomplete attacks.
       *  @param  attack The attack that will be removed.
       *  @param  all Remove all occurances if true, first else.
       *  @return true if at least once removed, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool remove_iattack(Attack &attack, bool all = false);

		/**
       *  @brief  Check if an attack is part from incomplete attacks.
       *  @param  attack The attack to check.
       *  @return  true if attack exists, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool iattack_exists(Attack attack);

		/**
       *  @brief  Check if an attack is part from incomplete attacks.
       *  @param  source The source of the attack to check.
       *  @param  target The target of the attack to check.
       *  @return  true if attack exists, false else.
       * 
       *  This function won't raise an exception if no element is removed
       */
		bool iattack_exists(string arg1, string arg2);

		/**
        *  @brief  Get the list of complete attacks.
        *  @return The list of complete attacks.
        */
		vector<Attack> get_iattacks();
#pragma endregion

#pragma region io
		/**
        *  @brief  Parse input from a tgf formatted stream.
        *  @param  stream The stream to parse into iaf.
        *  
		*  This method may raise an exception if the file is not properly formatted
		*  as it may ignore the line containing the error.
		*  This method doesn't close the stream or open it, you have to do it by yourself.
        */
		void parse_from_tgf(istream &stream);

		/**
        *  @brief  Parse input from an apx formatted stream.
        *  @param  stream The stream to parse into iaf.
        *  
		*  This method may raise an exception if the file is not properly formatted
		*  as it may ignore the line containing the error.
		*  This method doesn't close the stream or open it, you have to do it by yourself.
        */
		void parse_from_apx(istream &stream);

		// TODO redo
		/**
        *  @brief  Print the current iaf (deprecated).
        */
		void print_af();

		/**
        *  @brief  Write the current iaf into a file.
        */
		void write_tgf(string name);
		// TODO ifstream write_apx();
#pragma endregion

#pragma region operators
		/**
        *  @brief  Remove the first occurance of the attack from complete attacks.
        *  @return The current IAF.
        */
		IAF &operator-(Attack &atk)
		{
			remove_attack(atk);
			return *this;
		}

		/**
        *  @brief  Remove the first occurance of the attack from incomplete attacks.
        *  @return The current IAF.
        */
		IAF &operator/(Attack &atk)
		{
			remove_iattack(atk);
			return *this;
		}

		/**
        *  @brief  Add the attack to complete attacks.
        *  @return The current IAF.
        */
		IAF &operator+(Attack &atk)
		{
			add_attack(atk);
			return *this;
		}

		/**
        *  @brief  Add the attack to incomplete attacks.
        *  @return The current IAF.
        */
		IAF &operator*(Attack &atk)
		{
			add_iattack(atk);
			return *this;
		}

#pragma endregion

	private:
		vector<string> args_;
		vector<string> iargs_;
		vector<Attack> attacks_;
		vector<Attack> iattacks_;
	};
} // namespace ter

#endif