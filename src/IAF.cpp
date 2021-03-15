#include <string>
#include <IAF.h>
#include <fstream>
#include <iostream>
#include "Utilities.h"

// TODO Implemented by us
namespace ter
{
#pragma region constructors
    IAF::IAF()
    {
    }

    // TODO maybe handle attacks with no existing arg
    IAF::IAF(const IAF &iaf, const float &iargs_ration /*= 1*/, const float &iattacks_ration /*= 1*/)
    {
        // clone the IAF
        args_ = vector<string>(iaf.args_);
        iargs_ = vector<string>(iaf.iargs_);
        // TODO create indexed attacks instead of searching the args vector
        attacks_ = vector<Attack>(attacks_);
        iattacks_ = vector<Attack>(iattacks_);

        // check if input is consistent
        if (iargs_ration < 0 || 1 < iargs_ration || iattacks_ration < 0 || 1 < iattacks_ration)
            throw invalid_argument("Incomplete ratios must be between 0 and 1");

        // generate random incomplete args
        float incoplete_size = iargs_ration * (args_.size() + iargs_.size());
        while (iargs_.size() < incoplete_size)
        {
            int rand_index = random_index(args_.size());
            iargs_.push_back(args_[rand_index]);
            remove_arg(rand_index);
        }

        // add all attacks that have incomplete args (this may make incomplete attacks size greater than the requested one)
        for (string arg : iargs_)
            for (int atk = 0; atk < attacks_.size(); atk++)
                if (attacks_[atk].source == arg || attacks_[atk].target == arg)
                    (*this) * attacks_[atk] - attacks_[atk--];

        // generate random incomplete attacks
        incoplete_size = iattacks_ration * (attacks_.size() + iattacks_.size());
        while (iargs_.size() < incoplete_size)
        {
            int rand_index = random_index(iargs_.size());
            (*this) * attacks_[rand_index] - attacks_[rand_index];
        }
    }

#pragma endregion

    int IAF::get_size(){
        return args_.size() + iargs_.size();
    }

#pragma region complete_args_manipulation
    string IAF::get_arg(int index)
    {
        return args_[index];
    }

    void IAF::add_arg(string &arg)
    {
        args_.push_back(arg);
    }

    bool IAF::remove_arg(int index)
    {
        if (index >= args_.size())
            return false;
        args_.erase(args_.begin() + index);
        return false;
    }

    bool IAF::remove_arg(string &arg)
    {
        for (int i = 0; i < args_.size(); i++)
            if (arg == args_[i])
            {
                args_.erase(args_.begin() + i);
                return true;
            }
        return false;
    }

    vector<string> IAF::get_args()
    {
        return args_;
    }

#pragma endregion

#pragma region incomplete_args_manipulation
    string IAF::get_iarg(int index)
    {
        return iargs_[index];
    }

    void IAF::add_iarg(string &iarg)
    {
        args_.push_back(iarg);
    }

    bool IAF::remove_iarg(int index)
    {
        if (index >= iargs_.size())
            return false;
        iargs_.erase(args_.begin() + index);
        return true;
    }

    bool IAF::remove_iarg(string &iarg)
    {
        for (int i = 0; i < iargs_.size(); i++)
            if (iarg == iargs_[i])
            {
                iargs_.erase(args_.begin() + i);
                return true;
            }
        return false;
    }

    vector<string> IAF::get_iargs()
    {
        return iargs_;
    }

#pragma endregion

#pragma region complete_attacks_manipulation
    Attack IAF::get_attack(int index)
    {
        return attacks_[index];
    }

    void IAF::add_attack(Attack &attack)
    {
        attacks_.push_back(attack);
    }

    bool IAF::remove_attack(int index)
    {
        if (attacks_.size() <= index)
            return false;
        attacks_.erase(attacks_.begin() + index);
        return true;
    }

    bool IAF::remove_attack(string &source, string &target, bool all)
    {
        bool found = false;
        for (std::vector<ter::Attack>::iterator it = attacks_.begin(); it < attacks_.end(); ++it)
            if (source == it->source && target == it->target)
            {
                attacks_.erase(it--);
                found = true;
                if (!all)
                    return found;
            }
        return found;
    }

    bool IAF::remove_attack(Attack &attack, bool all)
    {
        return remove_attack(attack.source, attack.target, all);
    }

    bool IAF::attack_exists(Attack attack)
    {
        return attack_exists(attack.source, attack.target);
    }

    bool IAF::attack_exists(string source, string target)
    {
        for (Attack a : attacks_)
            if (a.source == source && a.target == target)
                return true;

        return false;
    }

    vector<Attack> IAF::get_attacks()
    {
        return attacks_;
    }

#pragma endregion

#pragma region incomplete_attacks_manipulation
    Attack IAF::get_iattack(int index)
    {
        return iattacks_[index];
    }

    void IAF::add_iattack(Attack &attack)
    {
        iattacks_.push_back(attack);
    }

    bool IAF::remove_iattack(int index)
    {
        if (attacks_.size() <= index)
            return false;
        attacks_.erase(attacks_.begin() + index);
        return true;
    }

    bool IAF::remove_iattack(string &source, string &target, bool all)
    {
        bool found = false;
        for (std::vector<ter::Attack>::iterator it = attacks_.begin(); it < attacks_.end(); ++it)
            if (source == it->source && target == it->target)
            {
                attacks_.erase(it--);
                found = true;
                if (!all)
                    return found;
            }
        return found;
    }

    bool IAF::remove_iattack(Attack &attack, bool all)
    {
        return remove_attack(attack.source, attack.target, all);
    }

    bool IAF::iattack_exists(Attack attack)
    {
        return iattack_exists(attack.source, attack.target);
    }

    bool IAF::iattack_exists(string source, string target)
    {
        for (Attack a : iattacks_)
            if (a.source == source && a.target == target)
                return true;
        return false;
    }

    vector<Attack> IAF::get_iattacks()
    {
        return iattacks_;
    }

#pragma endregion

#pragma region io
    void IAF::parse_from_tgf(istream &stream)
    {
        // TODO wrong lines ignored
        // TODO handle exceptions
        string line;
        while (getline(stream, line) && line.compare("#") != 0)
            if (line[0] == '?')
                iargs_.push_back(line.substr(1));
            else
                args_.push_back(line);

        while (getline(stream, line))
            if (line[0] == '?')
                iattacks_.push_back(Attack(line.substr(1)));
            else
                attacks_.push_back(Attack(line));
    }

    void IAF::parse_from_apx(istream &stream)
    {
        // TODO wrong lines ignored
        // TODO handle exceptions
        string line;
        while (getline(stream, line))
        {
            if (line[0] == '?')
            {
                if (line.substr(1, 4) == "arg(")
                    iargs_.push_back(line.substr(5, line.find(')') - 5));
                else if (line.substr(1, 4) == "att(")
                    iattacks_.push_back(Attack(line.substr(5)));
            }
            else if (line.substr(0, 4) == "arg(")
                args_.push_back(line.substr(4, line.find(')') - 4));
            else if (line.substr(0, 4) == "att(")
                attacks_.push_back(Attack(line.substr(4)));
        }
    }

    void IAF::write_tgf(string name)
    {
        ofstream wfile = ofstream(name);
        for (string s : args_)
            wfile << s << endl;

        for (string s : iargs_)
            wfile << s << endl;

        wfile << "#" << endl;

        for (Attack a : attacks_)
            wfile << a.source << " " << a.target << endl;

        for (Attack a : iattacks_)
            wfile << "?" << a.source << " " << a.target << endl;

        wfile.close();
    }

    void IAF::print_af()
    {
        cout << "Certain arguments: " << endl;
        for (string s : args_)
        {
            cout << s << endl;
        }
        cout << "Incertain arguments: " << endl;
        for (string s : iargs_)
        {
            cout << s << endl;
        }
        cout << "Certain attacks: " << endl;
        for (Attack a : attacks_)
        {
            cout << "(" << a.source << "," << a.target << ")" << endl;
        }
        cout << "Incertain attacks: " << endl;
        for (Attack a : iattacks_)
        {
            cout << "(" << a.source << "," << a.target << ")" << endl;
        }
    }

#pragma endregion

} // namespace ter