import os
import itertools
import random
import re
import pandas as pd
import numpy as np
from os import listdir
from os.path import isfile, join
import matplotlib.pyplot as plt

af_sizes= [10,20,40]
nb_exemples=100
ratio_att_arg =0.5
inc_ratio = [0.05,0.15,0.25,0.5]
ter_path="/home/nassim/Desktop/projects/TER/trafic"

def getFactors(n):
    # Create an empty list for factors
    factors=[]

    # Loop over all factors
    for i in range(1, n + 1):
        if n % i == 0:
            factors.append(i)

    # Return the list of factors
    s=int(len(factors)/2)
    return factors[1:s]

def af_to_iaf(file,ratio):
    af= open(file)
    lines = af.read()
    args = re.findall("arg\(a.+\)\.\n",lines)
    attacks = re.findall("att\(a.+?,a.+?\)\.\n",lines)
    for i in range(len(args)):
        pr=random.random()
        if(pr<=ratio):
            args[i]="?" + args[i]
    for i in range(len(attacks)):
        pr=random.random()
        if(pr<=ratio):
            attacks[i]="?" + attacks[i]
    af= open(file,"w")
    for i in args:
        af.write(i)
    for i in attacks:
        af.write(i)
    af.close()

def af_to_iaf2(file,ratio):
    af= open(file)
    lines = af.read()
    args = re.findall("arg\(a.+\)\.\n",lines)
    attacks = re.findall("att\(a.+?,a.+?\)\.\n",lines)
    for i in range(len(args)):
        pr=random.random()
        if(pr<=ratio):
            args[i]="?" + args[i]
    for i in range(len(attacks)):
        pr=random.random()
        if(pr<=ratio):
            attacks[i]="?" + attacks[i]
    af= open(file,"w")
    for i in args:
        af.write(i)
    for i in attacks:
        af.write(i)
    af.close()


def random_af(size):
    l=getFactors(size)
    v=random.choice(l)
    return "./AFBenchGen -SCCnumMEAN " + str(v) + " -SCCnumSTD 0 -SCCdimMEAN " + str(size/v) +  " -SCCdimSTD 1 -SCCpercEach 15"

def complete_generator(path=os.getcwd()):
    npath= path + "/Experimentation_iafs2"
    try:
        os.stat(npath)
    except:
        os.mkdir(npath)
    for s in af_sizes:
        nnpath= npath + "/exemples_" + str(s)
        os.mkdir(nnpath)
        for r in inc_ratio:
            nnnpath= nnpath + "/exemples_r_" + str(r)
            os.mkdir(nnnpath)
            for i in range(nb_exemples):
                filename=nnnpath + "/exemple_" + str(s) + "_" + str(r) + "_" + str(i) + ".apx"
                os.system(random_af(s) + " > " + filename)
                af_to_iaf(filename,r)

def test_some(path):
    pth = os.getcwd() + "/Results_20_0.1_2/"
    os.mkdir(pth)
    onlyfiles = [f for f in listdir(path) if isfile(join(path, f))]
    my_test_files=onlyfiles[:10]
    for f in my_test_files:
        file_name= path + f
        result_file_name = pth + "t_" + f
        os.system(ter_path + " " + file_name + " apx CO DS 1 > " + result_file_name)


def get_results(path):
    n_list= []
    time_list=[]
    n_completions_list = []
    onlyfiles = [f for f in listdir(path) if isfile(join(path, f))]
    for f in onlyfiles:
        group=re.match('t_exemple_20_0\.1_(.+)\.apx',f)
        n_list.append(int(group.groups()[0]))
        fl = open(path + f)
        lines = fl.read()
        times = re.findall("t:.+",lines)
        time_list.append(float(times[0][2:]))
        n_comps= re.findall("c:.+",lines)
        n_completions_list.append(int(n_comps[0][2:]))
    plt.bar(n_list, time_list) 
    plt.show()



#os.system("./AFBenchGen -SCCnumMEAN " + str(2) + " -SCCnumSTD 0 -SCCdimMEAN " + str(10) +  " -SCCdimSTD 1 > test.txt")
#complete_generator()
#af_to_iaf("test.txt",0.30)
#test_some("Experimentation_iafs2/exemples_20/exemples_r_0.1/")
get_results("Results_20_0.1/")