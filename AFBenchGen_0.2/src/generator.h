/**
 * @file 		generator.h
 * @author 		Federico Cerutti <federico.cerutti@acm.org>
 * @copyright	MIT
 */

#ifndef _SEMOPT_H
#define	_SEMOPT_H

#include <cmath>
#include <cstdio>
#include <fstream>
#include <cstdlib>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <time.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <cstring>
#include <string>
#include <cassert>
#include <set>

#define DIMBUFFER 100000

using namespace std;

#define HG(a) static const char *hgrev = a;

#define _PUBLIC_RELEASE "0.2"
#ifndef _PUBLIC_RELEASE
#include "hgversion.h"
#else
#define HG(a) static const char *hgrev = a;
HG("Version: 0.2");
#endif

#include "AF.h"
#include "SetArguments.h"
#include "SCC.h"

extern bool debug;
extern double SCCnumMEAN;
extern double SCCnumSTD;
extern double SCCdimMEAN;
extern double SCCdimSTD;
extern int SCCpercC;
extern int SCCpercAffected;
extern int SCCpercEach;
extern int SCCpercAffectAnotherSCC;
extern int SCCdir;

extern string filestat;


bool
parseParams(int argc, char *argv[]);

void showHelp(const char *);


#endif	/* _SEMOPT_H */
