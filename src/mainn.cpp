#include <iostream>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <IAF.h>
#include <AF.h>
#include "EnumerateExtensions.h"
#include <math.h>
#include <map>
#include <algorithm>
#define DEBUG_ENABLED true

using namespace std;

long num_completion(ter::IAF &iaf) {
    return pow(2,iaf.get_iargs().size())*pow(2,iaf.get_iattacks().size());
}

map<string,float> sckeptical_acceptance(ter::IAF &iaf, vector<vector<vector<string>>>  & results){
    map<string,float> *scores= new map<string,float>();
    for(string arg: iaf.get_args()){
        int score = 0;
        for(vector<vector<string>>v1: results){
            bool accepted=true;
            for(vector<string> v2: v1){    
                if(std::find(v2.begin(), v2.end(), arg) == v2.end()){
                    accepted=false;
                }
            }
            if(accepted){
                score=score+1;
            }
            
        }
        scores->insert(pair<string,float>(arg,((float)score/num_completion(iaf))));
    }
    for(pair<string,float> p:*scores){
        cout << p.first << ": " << p.second << endl;
    }
    return *scores;
}

map<string,float> credule_acceptance(ter::IAF & iaf, vector<vector<vector<string>>>  & results){
    map<string,float> *scores2= new map<string,float>();
    long nb_completions= pow(2,iaf.get_iargs().size())*pow(2,iaf.get_iattacks().size());
    cout << endl << "Nombre de complétions : " << nb_completions << endl;
    for(string arg: iaf.get_args()){
        int score = 0;
        for(vector<vector<string>>v1: results){
            bool accepted=false;
            for(vector<string> v2: v1){    
                if(std::find(v2.begin(), v2.end(), arg) == v2.end()){
                    accepted=true;
                }
            }
            if(accepted){
                score=score+1;
            }
            
        }
        scores2->insert(pair<string,float>(arg,((float)score/nb_completions)));
    }
    for(pair<string,float> p:*scores2){
        cout << p.first << ": " << p.second << endl;
    }
    return *scores2;
}

string to_string(vector<vector<string>> result)  {
    string equ_str = "\t{\n";
    for (vector<string> v2 : result)
    {
        equ_str += "\t\t[";
        for (string s : v2)
            equ_str += s + ",";
        equ_str = equ_str.substr(0, equ_str.length()-1) + "],\n";
    }
    return equ_str.substr(0, equ_str.length()-2) + "\n\t}";
}

string to_string(vector<vector<vector<string>>> *completions)  {
    string json = "{\n";
    for (vector<vector<string>> v1 : *completions)
        json += to_string(v1) + ",\n";
    return json.substr(0, json.length()-2) + "\n}\n";
}

vector<bool> encode(long value)
{
    vector<bool> *boolvalue = new vector<bool>();
    do
        boolvalue->insert(boolvalue->begin(), value % 2);
    while (value /= 2);
    return *boolvalue;
}

vector<vector<vector<string>>> *compute_completions(ter::IAF &iaf)
{
    vector<vector<vector<string>>> *results = new vector<vector<vector<string>>>();
    long n_poss_arg = pow(2, iaf.get_iargs().size());
    long n_poss_attacks = pow(2, iaf.get_iattacks().size());
    for (long i = 0; i < n_poss_arg; i++)
    {
        vector<string> cargs = iaf.get_args();
        vector<bool> valid_args = encode(i);
        for (int j = 0; j < valid_args.size(); j++)
            if (valid_args[j])
                cargs.push_back(iaf.get_iarg(j));

        for (long l = 0; l < n_poss_attacks; l++)
        {
            vector<ter::Attack> cattacks = iaf.get_attacks();
            vector<bool> valid_attacks = encode(l);
            for (int m = 0; m < valid_attacks.size(); m++)
                if (valid_attacks[m])
                    cattacks.push_back(iaf.get_iattack(m));

            AF af = AF(cargs, cattacks);
            af.initialize_vars();
            results->push_back(EnumerateExtensions::stable(af, NULL));
            // TODO eliminte one vector layer
            /* - A vector of strings (arguments) is an extension
            *  - A vector of vectors of strings is all stable extensions of a completion
            *  - The vector of vectors of vectors of strings is the stable extentions of each completion
            *  If we remove a vector layer we will the list of stable extentions without knowing
            *  to which one they belong
            */ 
        }
    }
    return results;
}

int main(int argc, char* argv[])
{
    #if DEBUG_ENABLED
        ifstream input = ifstream("Test20.tgf");
    #else
        if(argc == 0) {
            cout << "Usage: solver <file>" << endl;
            return 0;
        }
        ifstream input = ifstream(argv[0]);    
    #endif
    ter::IAF iaf = ter::IAF();
    iaf.parse_from_tfg(input);
    //AF af = AF(iaf);
    //af.initialize_vars();
    std::vector<std::vector<std::vector<std::string>>> *compStable = compute_completions(iaf);
    cout << to_string(compStable);
    sckeptical_acceptance(iaf, *compStable);
    //EnumerateExtensions::stable(af);

    return 0;
}
