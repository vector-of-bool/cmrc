#include <cmrc/cmrc.hpp>

#include <iostream>

CMRC_DECLARE(simple_string_view);

int main() {
    auto fs = cmrc::simple_string_view::get_filesystem();
    std::string_view a_string_view = fs.get_as_string_view("hello.txt");
    std::cout << a_string_view << '\n';
}