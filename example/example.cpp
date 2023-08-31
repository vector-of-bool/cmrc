#include <cmrc/cmrc.hpp>
#include <iostream>

// Declare the resource library. Use the NAMESPACE specified in CMakeFile.txt.
CMRC_DECLARE(rc);

// Small helper to make output tidy
void header(const char *s) {
    std::cout << "\n\n---===[ " << s << " ]===---\n\n";
}

int main() {
    // Get the filesystem object from the specified namespace (under cmrc::)
    auto fs = cmrc::rc::get_filesystem();

    // Access the text file resource in all possible ways and display the contents each time

    header("open() returning iterable cmrc::file object");
    auto license_file = fs.open("LICENSE.txt");
    for(auto i = license_file.begin(); i != license_file.end(); ++i) {
        std::cout << *i;
    }
    std::cout << "\n";

    header("String (std::string), will return a copy of the data");
    auto license_string= fs.get_as_string("LICENSE.txt");
    std::cout << license_string << "\n";

    header("String view (std::string_view), read-only view, avoids a copy of the data");
    auto license_string_view = fs.get_as_string_view("LICENSE.txt");
    std::cout << license_string_view << "\n";

    header("String view (std::string_view) again, accessed using a raw pointer");
    auto license_string_view2 = fs.get_as_string_view("LICENSE.txt");
    auto license_string_view2_data = license_string_view2.data(); // const char*
    auto license_string_view2_size = license_string_view2.size(); // std::size_t
    std::cout << std::string(license_string_view2_data, license_string_view2_size) << "\n";

    header("Raw pointer (const char*), relying on CMRC implicit trailing NULL byte");
    auto license_raw_ptr = fs.get_as_raw_ptr("LICENSE.txt");
    std::cout << std::string(license_raw_ptr) << "\n"; // Trailing NULL byte needed!

    header("Raw pointer (const char*) with size return argument");
    std::size_t license_raw_ptr2_size;
    auto license_raw_ptr2 = fs.get_as_raw_ptr("LICENSE.txt", license_raw_ptr2_size);
    std::cout << std::string(license_raw_ptr2, license_raw_ptr2_size) << "\n";

    header("Raw pointer (const char*) and size (std::size_t)");
    auto license_raw_ptr3 = fs.get_as_raw_ptr("LICENSE.txt");
    auto license_raw_ptr3_size = fs.get_size("LICENSE.txt");
    std::cout << std::string(license_raw_ptr3, license_raw_ptr3_size) << "\n";

    return 0;
}
