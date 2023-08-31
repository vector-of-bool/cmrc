#include <cmrc/cmrc.hpp>

#include <iostream>

CMRC_DECLARE(simple_string);

int main() {
    auto fs = cmrc::simple_string::get_filesystem();
    std::string a_string = fs.get_as_string("hello.txt");
    std::cout << a_string << '\n';
}
