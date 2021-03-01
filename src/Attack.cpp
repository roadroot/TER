#include <Attack.h>
#include <string>

namespace ter
{
    Attack::Attack() {}
    Attack::Attack(string source, string target) : source(source), target(target) {}
    Attack::Attack(string attack)
    {
        // TODO check if attack is valid
        size_t pos = attack.find(" ");
        if (pos != string::npos)
        {
            source = attack.substr(0, pos);
            target = attack.substr(pos + 1);
        }
        else
        {
            pos = attack.find(",");
            if (pos != string::npos)
            {
                source = attack.substr(0, pos);
                target = attack.substr(pos + 1);
                target = target.substr(0, target.length() - 2);
            }
            else
                throw std::invalid_argument("attack (" + attack + ") does not contain a space");
        }
    }
} // namespace ter
