## AFBenchGen

### Licence
    AFBenchGen
    Copyright (C) 2014  	Federico Cerutti <federico.cerutti@acm.org>
				Mauro Vallati <m.vallati@hud.ac.uk>

    Author: 	Federico Cerutti <federico.cerutti@acm.org>
		Mauro Vallati <m.vallati@hud.ac.uk>

    AFBenchGen is distributed under MIT license

### Description

AFBenchGen generates Dung's AF

See the following paper for documentation:

Cerutti, F., Giacomin, M. & Vallati, M. (2014). Generating Challenging Benchmark AFs. In S. Parson, N. Oren, C. Reed, & F. Cerutti (Eds.), Computational Models of Arguments --- Proceedings of COMMA 2014 (Vol. 266 of Frontiers in Artificial Intelligence and Applications). IOS Press.


### Quick Start

#### Compiling

cd Release
make

#### Running
./AFBenchGen <param1> ... <paramN>
-stats 	 <file> 	 generate stats about an existing AF
-SCCnumMEAN 	 <double> 	 mean for the number of SCCs
-SCCnumSTD 	 <double> 	 sigma for the number of SCCs
-SCCdimMEAN 	 <double> 	 mean for the dimension of SCCs
-SCCdimSTD 	 <double> 	 sigma for the dimension of SCCs
-SCCpercC 	 <int> 	 value between 1 and 100 representing the percentage of the set of influence between SCCs, -1 (default) for random selection
-SCCpercAffected 	 <int> 	 value between 1 and 100 representing the percentage of the set influenced between SCCs, -1 (default) for random selection
-SCCpercEach 	 <int> 	 value between 1 and 100 representing the percentage of attackers in each SCC, -1 (default) for random selection
-SCCpercAffectAnotherSCC 	 <int> 	 value between 1 and 100 representing the percentage of next SCC that will be affected by previous SCCs, -1 (default) for random selection
-SCCdir {0=False,1=True}, if true (default) this ensures that the SCCs are not connected in a way which can produce bigger SCCs

### Authors
Federico Cerutti <federico.cerutti@acm.org>
Mauro Vallati <m.vallati@hud.ac.uk>
