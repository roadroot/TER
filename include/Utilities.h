#ifndef  UTILITIES_H
#define UTILITIES_H
#include <cstdlib>
#define random_index(length) (random() % length)

template <typename T>
bool exists(std::vector<T> &vect, std::string &arg) {
    return find(vect.begin(), vect.end(), arg) != vect.end();
}

#endif // ! UTILITIES_H