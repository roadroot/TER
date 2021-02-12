#ifndef ATTACK_H
#define ATTACK_H

#include<vector>
#include <string>
#include<iostream> 

using namespace std;

namespace ter
{
    class Attack {
        public:
            Attack();
            Attack(string attack);
            Attack(string arg1, string arg2);
            string get_arg1();
            string get_arg2();
            void set_arg1(string arg1);
            void set_arg2(string arg2);
        private:
            string arg1_;
            string arg2_;
    };
} // namespace std

#endif