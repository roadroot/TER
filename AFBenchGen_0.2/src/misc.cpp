/**
 * @file 		misc.cpp
 * @brief 		Miscellanea functions
 * @author 		Federico Cerutti <federico.cerutti@acm.org>
 * @copyright	MIT
 */

#include "generator.h"

/**
 * @brief 				Function checking the existence of a file
 * @param[in] filename	Pointer to a `const char` containing the name of the file
 * @retval bool
 */
bool fexists(const char *filename) {
	if (ifstream(filename))
		return true;
	else
		return false;
}

/**
 * @brief				Function for parsing the parameters
 * @details				This function does not return any value. It fills global variables according to
 * 						the received parameters.
 * @param[in] argc		`int` containing the numbers of parameters
 * @param[in] argv		List of parameters
 * @retval bool
 */
bool parseParams(int argc, char *argv[]) {
	for (int k = 1; k < argc; k++) {
		if (string("-SCCnumMEAN").compare(argv[k]) == 0) {
			SCCnumMEAN = atof(string(argv[++k]).c_str());
		} else if (string("-SCCnumSTD").compare(argv[k]) == 0) {
			SCCnumSTD = atof(string(argv[++k]).c_str());
		} else if (string("-SCCdimMEAN").compare(argv[k]) == 0) {
			SCCdimMEAN = atof(string(argv[++k]).c_str());
		} else if (string("-SCCdimSTD").compare(argv[k]) == 0) {
			SCCdimSTD = atof(string(argv[++k]).c_str());
		} else if (string("-SCCpercC").compare(argv[k]) == 0) {
			SCCpercC = atoi(string(argv[++k]).c_str());
		} else if (string("-SCCpercAffected").compare(argv[k]) == 0) {
			SCCpercAffected = atoi(string(argv[++k]).c_str());
		} else if (string("-SCCpercEach").compare(argv[k]) == 0) {
			SCCpercEach = atoi(string(argv[++k]).c_str());
		} else if (string("-SCCpercAffectAnotherSCC").compare(argv[k]) == 0) {
			SCCpercAffectAnotherSCC = atoi(string(argv[++k]).c_str());
		} else if (string("-SCCdir").compare(argv[k]) == 0) {
			SCCdir = atoi(string(argv[++k]).c_str());
		} else if (string("-stats").compare(argv[k]) == 0) {
			filestat = string(argv[++k]);
		} else {
			cout << "Unrecognised parameter: " << argv[k] << ": " << argv[k + 1]
					<< endl;
			return false;
		}
	}
	return true;
}

/**
 * @brief			Function for printing on screen a disclaimer and a brief help
 * @param[in] rev	The version of this software
 * @retval	void
 */
void showHelp(const char *rev) {
	cout << rev << endl;

	cout
			<< "AFBenchGen Copyright (C) 2014 Federico Cerutti <federico.cerutti@acm.org>, Mauro Vallati <m.vallati@hud.ac.uk>"
			<< endl;
	cout << "This program comes with ABSOLUTELY NO WARRANTY" << endl;
	cout << "This program is distributed under the MIT license" << endl;

	cout << "#### Running" << endl;
	cout << "./AFBenchGen <param1> ... <paramN>" << endl;
	cout << "-stats \t <file> \t generate stats about an existing AF" << endl;
	cout << "-SCCnumMEAN \t <double> \t mean for the number of SCCs" << endl;
	cout << "-SCCnumSTD \t <double> \t sigma for the number of SCCs" << endl;
	cout << "-SCCdimMEAN \t <double> \t mean for the dimension of SCCs" << endl;
	cout << "-SCCdimSTD \t <double> \t sigma for the dimension of SCCs" << endl;
	cout
			<< "-SCCpercC \t <int> \t value between 1 and 100 representing the percentage of the set of influence between SCCs, -1 (default) for random selection"
			<< endl;
	cout
			<< "-SCCpercAffected \t <int> \t value between 1 and 100 representing the percentage of the set influenced between SCCs, -1 (default) for random selection"
			<< endl;
	cout
			<< "-SCCpercEach \t <int> \t value between 1 and 100 representing the percentage of attackers in each SCC, -1 (default) for random selection"
			<< endl;
	cout
			<< "-SCCpercAffectAnotherSCC \t <int> \t value between 1 and 100 representing the percentage of next SCC that will be affected by previous SCCs, -1 (default) for random selection"
			<< endl;
	cout
			<< "-SCCdir {0=False,1=True}, if true (default) this ensures that the SCCs are not connected in a way which can produce bigger SCCs"
			<< endl;
	return;
}
