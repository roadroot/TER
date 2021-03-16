#!/bin/bash

generator="./Release/AFBenchGen"
dest="AFs/"

stdnumscc="0"
stddimscc="5"
perceach="90"


for numscc in $(seq 50 5 80)
do
	for dimscc in $(seq 20 5 40)
	do
		for percC in $(seq 50 25 75)
		do
			for percaffected in $(seq 25 25 50)
			do
				for percothersccs in $(seq 25 25 50)
				do
					for i in $(seq 10)
					do
						file=$dest/scc-$numscc-$stdnumscc-$dimscc-$stddimscc-$percC-$percaffected-$perceach-$percothersccs-$i
						nice -n 10 $generator -SCCnumMEAN $numscc -SCCnumSTD $stdnumscc -SCCdimMEAN $dimscc -SCCdimSTD $stddimscc -SCCpercC $percC -SCCpercAffected $percaffected -SCCpercEach $perceach -SCCpercAffectAnotherSCC $percothersccs > $file
					done	
				done
			done
		done
	done

done
