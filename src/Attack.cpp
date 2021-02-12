#include<Attack.h>
#include<string>


namespace ter
{
    Attack::Attack(){

    }
    Attack::Attack(string arg1, string arg2){
        arg1_=arg1;
        arg2_=arg2;
    }

    Attack::Attack(string attack) {
        // TODO check if attack is valid
        size_t pos = attack.find(" ");
        if (pos != string::npos)
        {
            arg1_ = attack.substr(0, pos);
            arg2_ = attack.substr(pos+1);
        }
        else
            throw std::invalid_argument( "attack ("+ attack +") does not contain a space");
    }


    string Attack::Attack::get_arg1(){
        return arg1_;
    }

    string Attack::Attack:: get_arg2(){
        return arg2_;
    }

    void Attack::Attack:: set_arg1(string arg1){
        arg1_=arg1;
    }

    void Attack::Attack:: set_arg2(string arg2){
        arg2_=arg2;
    }
} // namespace ter
