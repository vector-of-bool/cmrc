#include <cmrc/cmrc.hpp>

#include <iostream>

CMRC_DECLARE(hello);

int main() {
    // CMRC_INIT(hello);
    auto fs = cmrc::hello::get_filesystem();
    auto data = fs.open("hello.txt");
    std::cout << std::string(data.begin(), data.end()) << '\n';
}
