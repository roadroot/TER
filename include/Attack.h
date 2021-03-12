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
            Attack(string source, string target);
            string source;
            string target;
            const bool inline operator==(Attack atk) {
                return source == atk.source && target == target;
            }
        private:
    };
} // namespace std

#endif