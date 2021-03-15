#ifndef TEST_H
#define TEST_H 

class Test
{
public:
    Test();
    Test(Test &&) = default;
    Test(const Tesst &) = default;
    Test &operator=(Test &&) = default;
    Test &operator=(const Test &) = default;
    ~Test();

private:
    
};

Test::Test()
{
}

Test::~Test()
{
}

#endif // !TEST_H