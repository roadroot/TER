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

using namespace std;

map<string,float> sckeptical_acceptance(ter::IAF & iaf, vector<vector<vector<string>>>  & results){
    map<string,float> *scores= new map<string,float>();
    int nb_completions= pow(2,iaf.get_iargs().size())*pow(2,iaf.get_iattacks().size());
    cout << endl << "Nombre de complétions : " << nb_completions << endl;
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
        scores->insert(pair<string,float>(arg,((float)score/nb_completions)));
    }
    for(pair<string,float> p:*scores){
        cout << p.first << ": " << p.second << endl;
    }
    return *scores;
}

map<string,float> credule_acceptance(ter::IAF & iaf, vector<vector<vector<string>>>  & results){
    map<string,float> *scores2= new map<string,float>();
    int nb_completions= pow(2,iaf.get_iargs().size())*pow(2,iaf.get_iattacks().size());
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

vector<bool> encode(int value)
{
    int mod = 0;
    vector<bool> *boolvalue = new vector<bool>();
    while (value != 0)
    {
        mod = value % 2;
        value = value / 2;
        boolvalue->push_back(mod);
    }
    return *boolvalue;
}

vector<vector<vector<string>>> *compute_completions(ter::IAF *iaf)
{
    vector<vector<vector<string>>> *results = new vector<vector<vector<string>>>();
    int n_poss_arg = pow(2, iaf->get_iargs().size());
    int n_poss_attacks = pow(2, iaf->get_iattacks().size());
    for (int i = 0; i < n_poss_arg; i++)
    {
        vector<string> cargs = iaf->get_args();
        vector<bool> valid_args = encode(i);
        for (int j = 0; j < valid_args.size(); j++)
            if (valid_args[j])
                cargs.push_back(iaf->get_iarg(j));

        for (int l = 0; l < n_poss_attacks; l++)
        {
            vector<ter::Attack> cattacks = iaf->get_attacks();
            vector<bool> valid_attacks = encode(l);
            for (int m = 0; m < valid_attacks.size(); m++)
                if (valid_attacks[m])
                    cattacks.push_back(iaf->get_iattack(m));

            //cout << results->size() << endl;
            AF af = AF(cargs, cattacks);
            af.initialize_vars();
            results->push_back(EnumerateExtensions::stable(af, NULL));
            // TODO eliminte one vector layer
        }
    }
    return results;
}

int main()
{
    ifstream input;
    input.open("Test20.tgf");
    ter::IAF *iaf = new ter::IAF();
    iaf->parse_from_tfg(input);
    //AF af = AF(iaf);
    //af.initialize_vars();
    std::vector<std::vector<std::vector<std::string>>> *compStable = compute_completions(iaf);
    cout << to_string(compStable);
    sckeptical_acceptance(*iaf, *compStable);
    //EnumerateExtensions::stable(af);

    return 0;
}
