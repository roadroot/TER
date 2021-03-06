#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <AF.h>
#include <IAF.h>
#include "EnumerateExtensions.h"
#include "SingleExtension.h"
#include <math.h>
#include <map>
#include <algorithm>
#include <time.h>
#include <bits/stdc++.h>
#include "Utilities.h"
#include "Result.h"
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>

#define DEBUG_ENABLED false
#define TIMEOUT 1s

using namespace std;

bool is_number(const std::string &s) {
  return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

// parse user input
semantics string_to_sem(string problem)
{
    if (problem == "CO")
        return CO;
    if (problem == "PR")
        return PR;
    if (problem == "ST")
        return ST;
    if (problem == "GR")
        return GR;
    return UNKNOWN_SEM;
}

// parse user input
task string_to_task(string problem)
{
    if (problem == "DC")
        return DC;
    if (problem == "DS")
        return DS;
    return UNKNOWN_TASK;
}

// calculate the number of comletions in an IAF
long num_completion(ter::IAF &iaf)
{
    return pow(2, iaf.get_iargs().size()) * pow(2, iaf.get_iattacks().size());
}

string to_string(vector<vector<string>> &result)
{
    string equ_str = "\t{\n";
    for (vector<string> v2 : result)
    {
        equ_str += "\t\t[";
        for (string s : v2)
            equ_str += s + ",";
        equ_str = equ_str.substr(0, equ_str.length() - 1) + "],\n";
    }
    return equ_str.substr(0, equ_str.length() - 2) + "\n\t}";
}

string to_string(vector<vector<vector<string>>> &completions)
{
    string json = "{\n";
    for (vector<vector<string>> v1 : completions)
        json += to_string(v1) + ",\n";
    return json.substr(0, json.length() - 2) + "\n}\n";
}

string to_string(map<string, float> &scores)
{
    string json;
    json += "{\n";
    for (auto const &x : scores)
        json += "\t" + x.first + ": " + to_string(x.second) + ",\n";
    json += "}\n";
    return json;
}

// long to vector of bytes (for the completion generation (look at compute_completions_*))
vector<bool> encode(long value)
{
    vector<bool> boolvalue = vector<bool>();
    do
        boolvalue.insert(boolvalue.begin(), value % 2);
    while (value /= 2);
    return boolvalue;
}

// calculate the completions, their extention and arguments score with skept acceptance
void compute_completions_skeptical(ter::IAF &iaf, ter::Result &result, semantics sem = ST, int grad_meth = 1)
{
    map<string, float> scores_2 = map<string, float>();
    long num_comp = 0;
    for (string arg : iaf.args_) {
        result.scores [arg] = 0;
        scores_2[arg] = 0;
    }
    for (string arg : iaf.iargs_) {
        result.scores[arg] = 0;
        scores_2[arg] = 0;
    }
    long n_poss_arg = pow(2, iaf.iargs_.size());
    long n_poss_attacks = pow(2, iaf.iattacks_.size());
    for (long i = 0; i < n_poss_arg; i++)
    {
        vector<string> cargs = iaf.get_args();
        vector<bool> valid_args = encode(i);
        for (int j = 0; j < valid_args.size(); j++)
            if (valid_args[j])
                cargs.push_back(iaf.get_iarg(j));
        valid_args.clear();

        for (long l = 0; l < n_poss_attacks; l++)
        {
            vector<ter::Attack> cattacks = iaf.get_attacks();
            vector<bool> valid_attacks = encode(l);
            AF af;
            for (int m = 0; m < valid_attacks.size(); m++)
                if (valid_attacks[m]) {
                    ter::Attack atk = iaf.get_iattack(m);
                    if(!exists(cargs, atk.source) || !exists(cargs, atk.target))
                        goto END_FOR_SKEPTICAL;
                    cattacks.push_back(atk);
                }
            valid_attacks.clear();

            af = AF(cargs, cattacks);
            for(string arg : cargs) scores_2[arg]++;
            num_comp++;
            af.initialize_vars();
            std::vector<std::vector<std::string>> *extentions;
            switch (sem)
            {
            case ST:
                extentions = EnumerateExtensions::stable(af);
                break;

            case CO:
                extentions = EnumerateExtensions::complete(af);
                break;

            case PR:
                extentions = EnumerateExtensions::preferred(af);
                break;

            case GR:
                extentions = SingleExtension::grounded(af,0);
                break;

            default:
                break;
            }

            for (string arg : af.int_to_arg)
            {
                bool accepted = true;
                for (std::vector<std::string> ext : *extentions)
                    if (!exists(ext, arg))
                    {
                        accepted = false;
                        break;
                    }
                if (accepted)
                    result.scores[arg] += 1;
            }
            cattacks.clear();
            for(std::vector<std::string> ext : *extentions)
                ext.clear();
            extentions->clear();
            delete extentions;
            END_FOR_SKEPTICAL:;
        }
        cargs.clear();
    }
    for(const auto pair : scores_2) {
        if(grad_meth == 1)
            result.scores[pair.first] /= num_comp;
        else
            result.scores[pair.first] /= pair.second;
    }
    result.completion_number = num_comp;
}

// calculate the completions, their extention and arguments score with cred acceptance
void compute_completions_credulous(ter::IAF &iaf, ter::Result &result, semantics sem = ST, int grad_meth = 1)
{
    map<string, float> scores_2 = map<string, float>();
    double num_comp = 0;
    for (string arg : iaf.args_) {
        result.scores[arg] = 0;
        scores_2[arg] = 0;
    }
    for (string arg : iaf.iargs_) {
        result.scores[arg] = 0;
        scores_2[arg] = 0;
    }
    long n_poss_arg = pow(2, iaf.get_iargs().size());
    long n_poss_attacks = pow(2, iaf.get_iattacks().size());
    for (long i = 0; i < n_poss_arg; i++)
    {
        vector<string> cargs = iaf.get_args();
        vector<bool> valid_args = encode(i);
        for (int j = 0; j < valid_args.size(); j++)
            if (valid_args[j])
                cargs.push_back(iaf.get_iarg(j));
        valid_args.clear();

        for (long l = 0; l < n_poss_attacks; l++)
        {
            vector<ter::Attack> cattacks = iaf.get_attacks();
            vector<bool> valid_attacks = encode(l);
            AF af;
            for (int m = 0; m < valid_attacks.size(); m++)
                if (valid_attacks[m]) {
                    ter::Attack atk = iaf.get_iattack(m);
                    if(!exists(cargs, atk.source) || !exists(cargs, atk.target))
                        goto END_FOR_CREDULOUS;
                    cattacks.push_back(atk);
                }
            valid_attacks.clear();

            af = AF(cargs, cattacks);
            for(string arg : cargs) scores_2[arg]++;
            num_comp++;
            af.initialize_vars();
            std::vector<std::vector<std::string>> *extentions;
            switch (sem)
            {
            case ST:
                extentions = EnumerateExtensions::stable(af);
                break;

            case CO:
                extentions = EnumerateExtensions::complete(af);
                break;

            case PR:
                extentions = EnumerateExtensions::preferred(af);
                break;

            case GR:
                extentions = SingleExtension::grounded(af,0);
                break;

            default:
                break;
            }
            for (string arg : af.int_to_arg)
            {
                for (std::vector<std::string> ext : *extentions)
                    if (exists(ext, arg)) {
                        result.scores[arg] += 1;
                        break;
                    }
            }
            cattacks.clear();
            for(std::vector<std::string> ext : *extentions)
                ext.clear();
            extentions->clear();
            delete extentions;
            END_FOR_CREDULOUS:;
        }
        cargs.clear();
    }
    for(const auto pair : scores_2) {
        //cout << pair.first << " " << pair.second << " " << scores[pair.first] << endl;
        if(grad_meth == 1)
            result.scores[pair.first] /= num_comp;
        else
            result.scores[pair.first] /= pair.second;
    }
    result.completion_number = num_comp;
}

void compute_completions_skeptical_wrapper(ter::IAF &iaf, ter::Result &result, semantics sem = ST, int grad_meth = 1, std::chrono::seconds timeout=10s)
{
    std::mutex m;
    std::condition_variable cv;

    std::thread t([&cv, &iaf, &result, &sem, &grad_meth]() 
    {
        compute_completions_skeptical(iaf, result, sem, grad_meth);
        cv.notify_one();
    });

    t.detach();

    {
        std::unique_lock<std::mutex> l(m);
        if(cv.wait_for(l, timeout) == std::cv_status::timeout)  {
            cout << "timeout" << endl;
            exit(1);
        }
    }
}

void compute_completions_credulous_wrapper(ter::IAF &iaf, ter::Result &result, semantics sem = ST, int grad_meth = 1, std::chrono::seconds timeout=10s)
{
    std::mutex m;
    std::condition_variable cv;

    std::thread t([&cv, &iaf, &result, &sem, &grad_meth]() 
    {
        compute_completions_credulous(iaf, result, sem, grad_meth);
        cv.notify_one();
    });

    t.detach();

    {
        std::unique_lock<std::mutex> l(m);
        if(cv.wait_for(l, timeout) == std::cv_status::timeout)  {
            cout << "timeout" << endl;
            exit(1);
        }
    }
}

int main(const int argc, const char *argv[])
{
    try
    {
        semantics sem = ST;
        task tsk = DC;
        ter::IAF iaf;
        int grad = 1;
        bool testing = false;
        auto timeout = 3600s;
#if DEBUG_ENABLED
        ifstream input;
        if (argc == 1)
            input.open("Test20.tgf");
        else
            input.open(argv[1]);

        iaf.parse_from_tgf(input);
        input.close();
#else
        if (!(argc == 6 || argc == 7 && is_number(argv[6])))
        {
            cout << "Usage: " << argv[0] << " <file> <format> <SM> <task> <GS>" << endl
                << "<file>:\t\tFile containing the AF" << endl
                << "<format>:\tFile format, one of {tgf, apx}" << endl
                << "<SM>:\t\t\tSemantics used one of {CO, PR, ST, GR}" << endl
                << "<task>:\t\t\tCredulous or skeptical acceptancy, one of {DC, DS}" << endl
                << "<GS>:\t\t\tGraduation system, 1 or 2" << endl;
            return 0;
        }

        if(argc == 7) {
            testing = true;
            timeout = std::chrono::seconds(stoi(argv[6]));
        }

        // read the file and check its readability
        ifstream input = ifstream(argv[1]);
        if (!input.good())
        {
            cout << "An error occured while openning \"" << argv[1] << "\"" << endl;
            return -1;
        }


        // parse the file according to its format
        if (string(argv[2]) == "tgf")
            iaf.parse_from_tgf(input);
        else if (string(argv[2]) == "apx")
            iaf.parse_from_apx(input);
        else
        {
            cout << "Unsupported file format \"" << argv[2] << "\"" << endl;
            return -1;
        }
        input.close();

        // parse the semantics
        sem = string_to_sem(argv[3]);
        if (sem == UNKNOWN_SEM)
        {
            cout << "Unknown semantics \"" << argv[3] << "\"" << endl;
            return -1;
        }

        // parse the task
        tsk = string_to_task(argv[4]);
        if (tsk == UNKNOWN_TASK)
        {
            cout << "Unknown task \"" << argv[4] << "\"" << endl;
            return -1;
        }

        grad = string(argv[5]) == "1" ? 1 : string(argv[5]) == "2" ? 2
                                                                : 0;
        if (!grad)
        {
            cout << "Unknown graduation method \"" << argv[5] << "\"" << endl;
            return -1;
        }

#endif
        ter::Result result = ter::Result(tsk, 0, grad);
        auto start = chrono::high_resolution_clock::now();
        if (tsk == DC)
            compute_completions_credulous_wrapper(iaf, result, sem, grad, timeout);
        else
            compute_completions_skeptical_wrapper(iaf, result, sem, grad, timeout);
        result.time = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();
        if(testing)
            cout << result.to_string();
        else
            cout << to_string(result.scores); 
        return 0;   
    }
    catch (const std::bad_alloc& ex)
    {
        cout << "memory out" << std::endl;
    }
    catch (...) {
        std::cout << "memory out" << std::endl;
    }
}
