#include "vApp.hpp"

int main()
{
    vApp a;
    try
{
    // code that could cause exception
       a.run();
}
catch (const std::exception &exc)
{
    // catch anything thrown within try block that derives from std::exception
    std::cerr << exc.what();
    std::cout << "aaa" << exc.what();
}
 
    return 0;
}