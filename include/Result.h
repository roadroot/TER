#ifndef RESULT_H
#define RESULT_H
#include "AF.h"
#include <map>

using namespace std;
namespace ter
{
    class Result
    {

    public:
        task tsk;
        map<string, float> *scores;
        long completion_number;
        int grad_meth;
        long time;

        Result();

        Result(
            task tsk = DC,
            map<string, float> *scores = NULL,
            long completion_number = 0,
            int grad_mseth = 1
        );
        string to_string() {
            string s =  std::string(tsk == DC? "credulous,": "skeptical,");
            s+= std::to_string(completion_number);
            s+= "," + std::to_string(grad_meth);
            s+= "," + std::to_string(time);
            s+= "\n";
            return s;
        }
    };
    
    Result::Result(
        task tsk,
        map<string, float> *scores,
        long completion_number,
        int grad_meth
    ) :
        tsk(tsk),
        scores(scores),
        completion_number(completion_number),
        grad_meth(grad_meth)
    {
    }
        
} // namespace ter

#endif // RESULT_H