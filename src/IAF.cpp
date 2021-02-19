#include <string>
#include <IAF.h>
#include <fstream>
#include <iostream>
#include <regex>
#include <boost/algorithm/string.hpp>

namespace ter
{
    IAF::IAF() {}

    // IAF::IAF(vector<string> const &args, vector<Attack> const &attacks, vector<string> const &iargs = {}, vector<Attack> const &iattacks = {})
    // {
    //     args_ = args;
    //     iargs_ = iargs;
    //     attacks_ = attacks;
    //     iattacks_ = iattacks;
    // }

    // IAF::IAF(vector<string> args, vector<Attack> attacks)
    // {
    //     args_ = args;
    //     attacks_ = attacks;
    // }

    void IAF::add_arg(string arg)
    {
        args_.push_back(arg);
    }
    void IAF::delete_arg(string arg)
    {
        int pos = -1;
        for (int i = 0; i < args_.size(); i++)
        {
            if (arg.compare(args_[i]) == 0)
            {
                pos = i;
                break;
            }
        }
        if (pos >= 0)
        {
            args_.erase(args_.begin() + pos);
        }
    }
    void IAF::delete_arg(int pos)
    {
        args_.erase(args_.begin() + pos);
    }

    void IAF::add_iarg(string iarg)
    {
        args_.push_back(iarg);
    }

    void IAF::delete_iarg(string iarg)
    {
        int pos = -1;
        for (int i = 0; i < iargs_.size(); i++)
        {
            if (iarg.compare(iargs_[i]) == 0)
            {
                pos = i;
                break;
            }
        }
        if (pos >= 0)
        {
            iargs_.erase(args_.begin() + pos);
        }
    }

    void IAF::delete_iarg(int pos)
    {
        iargs_.erase(args_.begin() + pos);
    }

    string IAF::get_arg(int pos)
    {
        return args_[pos];
    }
    string IAF::get_iarg(int pos)
    {
        return iargs_[pos];
    }
    string IAF::get_abs_arg(int pos)
    {
        int size = args_.size();
        if (pos > size)
        {
            return iargs_[size - pos];
        }
        else
        {
            return args_[pos];
        }
    }

    Attack IAF::get_attack(int pos)
    {
        return attacks_[pos];
    }
    Attack IAF::get_iattack(int pos)
    {
        return iattacks_[pos];
    }
    Attack IAF::get_abs_attack(int pos)
    {
        int size = attacks_.size();
        if (pos > size)
        {
            return iattacks_[size - pos];
        }
        else
        {
            return attacks_[pos];
        }
    }
    bool IAF::attack_exists(string arg1, string arg2)
    {
        for (Attack a : attacks_)
        {
            if (a.get_arg1().compare(arg1) == 0 && a.get_arg2().compare(arg2) == 0)
            {
                return true;
            }
        }
        return false;
    }
    bool IAF::attack_exists(Attack attack)
    {
        for (Attack a : attacks_)
        {
            if (a.get_arg1().compare(attack.get_arg1()) == 0 && a.get_arg2().compare(attack.get_arg2()) == 0)
            {
                return true;
            }
        }
        return false;
    }

    bool IAF::iattack_exists(string arg1, string arg2)
    {
        for (Attack a : iattacks_)
        {
            if (a.get_arg1().compare(arg1) == 0 && a.get_arg2().compare(arg2) == 0)
            {
                return true;
            }
        }
        return false;
    }

    bool IAF::iattack_exists(Attack attack)
    {
        for (Attack a : iattacks_)
        {
            if (a.get_arg1().compare(attack.get_arg1()) == 0 && a.get_arg2().compare(attack.get_arg2()) == 0)
            {
                return true;
            }
        }
        return false;
    }

    void IAF::add_attack(Attack &attack)
    {
        attacks_.push_back(attack);
    }

    void IAF::add_iattack(Attack &attack)
    {
        iattacks_.push_back(attack);
    }

    void IAF::parse_from_tgf(ifstream &tgf_file)
    {
        // TODO wrong lines ignored
        // TODO handle exceptions
        string line;
        while (getline(tgf_file, line) && line.compare("#") != 0)
            if (line[0] == '?')
                iargs_.push_back(line.substr(1));
            else
                args_.push_back(line);

        while (getline(tgf_file, line))
            if (line[0] == '?')
                iattacks_.push_back(Attack(line.substr(1)));
            else
                attacks_.push_back(Attack(line));
    }

    void IAF::parse_from_apx(ifstream &apx_file)
    {
        // TODO wrong lines ignored
        // TODO handle exceptions
        string line;
        while (getline(apx_file, line))
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
        ofstream wfile;
        wfile.open(name);
        for (string s : args_)
        {
            wfile << s << endl;
        }
        for (string s : iargs_)
        {
            wfile << s << endl;
        }
        wfile << "#" << endl;
        for (Attack a : attacks_)
        {
            wfile << a.get_arg1() << " " << a.get_arg2() << endl;
        }
        for (Attack a : iattacks_)
        {
            wfile << "?" << a.get_arg1() << " " << a.get_arg2() << endl;
        }
        wfile.close();
    }
    // TODO
    // ifstream IAF::write_apx()
    // {
    // }

    vector<string> IAF::get_args()
    {
        return args_;
    }

    vector<string> IAF::get_iargs()
    {
        return iargs_;
    }

    vector<Attack> IAF::get_attacks()
    {
        return attacks_;
    }

    vector<Attack> IAF::get_iattacks()
    {
        return iattacks_;
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
            cout << "(" << a.get_arg1() << "," << a.get_arg2() << ")" << endl;
        }
        cout << "Incertain attacks: " << endl;
        for (Attack a : iattacks_)
        {
            cout << "(" << a.get_arg1() << "," << a.get_arg2() << ")" << endl;
        }
    }

} // namespace ter