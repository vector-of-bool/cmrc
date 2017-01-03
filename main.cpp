#include <cmrc/cmrc.hpp>

#include <iostream>

int main() {
  CMRC_INIT(hello);
  auto data = cmrc::open("hello.txt");
  std::cout << std::string(data.begin(), data.end()) << '\n';
}