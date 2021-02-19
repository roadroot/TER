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
        IAF();
        //IAF(vector<string> const &args, vector<Attack> const &attacks, vector<string> const &iargs = {}, vector<Attack> const &iattacks = {});
        //IAF(vector<string> args, vector<Attack> attacks);

        void add_arg(string arg);
        void delete_arg(string arg);
        void delete_arg(int pos);

        void add_iarg(string iarg);
        void delete_iarg(string iarg);
        void delete_iarg(int pos);

        vector<string> get_args();
        vector<string> get_iargs();
        string get_arg(int pos);
        string get_iarg(int pos);
        string get_abs_arg(int pos);

        vector<Attack> get_attacks();
        vector<Attack> get_iattacks();
        void add_attack(Attack &attack);
        void add_iattack(Attack &attack);
        Attack get_attack(int pos);
        Attack get_iattack(int pos);
        Attack get_abs_attack(int pos);
        bool attack_exists(string arg1, string arg2);
        bool attack_exists(Attack attack);
        bool iattack_exists(string arg1, string arg2);
        bool iattack_exists(Attack attack);
        bool attack_abs_exists(string arg1, string arg2);
        bool attack_abs_exists(Attack attack);

        void parse_from_tgf(ifstream &tgf_file);
        void parse_from_apx(ifstream &apx_file);

        void print_af();

        void write_tgf(string name);
        ifstream write_apx();

    private:
        int nb_arg;
        vector<string> args_;
        vector<string> iargs_;
        vector<Attack> attacks_;
        vector<Attack> iattacks_;
    };
} // namespace ter

#endif