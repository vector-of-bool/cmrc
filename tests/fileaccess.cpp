#include <cmrc/cmrc.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>

CMRC_DECLARE(fileaccess);

int main(int argc, char** argv) {
    // Determine size of file on actual disk
    if (argc != 2) {
        std::cerr << "Invalid arguments passed to fileaccess\n";
        return 2;
    }
    std::cout << "Reading flower from " << argv[1] << '\n';
    std::ifstream flower_fs{argv[1], std::ios_base::binary};
    if (!flower_fs) {
        std::cerr << "Invalid filename passed to fileaccess: " << argv[1] << '\n';
        return 2;
    }
    using iter         = std::istreambuf_iterator<char>;
    const auto fs_size = std::distance(iter(flower_fs), iter());
    flower_fs.seekg(0);
    std::vector<char> flower_fs_vector((iter(flower_fs)), iter());
    flower_fs.seekg(0);

    // Get handle to resource filesystem
    auto fs = cmrc::fileaccess::get_filesystem();

    // Test access using open()
    auto flower_open = fs.open("flower.jpg");
    const auto open_size = std::distance(flower_open.begin(), flower_open.end());
    if (fs_size != open_size) {
        std::cerr << "open(): Sizes do not match: FS == " << fs_size << ", RC == " << open_size
                  << "\n";
        return 1;
    }
    if (!std::equal(flower_open.begin(), flower_open.end(), iter(flower_fs))) {
        std::cerr << "open(): Contents do not match\n";
        return 1;
    }
    flower_fs.seekg(0);

    // Test access using get_as_string()
    auto flower_string = fs.get_as_string("flower.jpg");
    auto flower_string_size = flower_string.size();
    if(fs_size != flower_string_size) {
        std::cerr << "get_as_string(): Sizes do not match: FS == " << fs_size << ", RC == "
                  << flower_string_size << "\n";
        return 1;
    }
    if(0 != memcmp(flower_fs_vector.data(), flower_string.data(), flower_string_size)) {
        std::cerr << "get_as_string(): Contents do not match\n";
        return 1;
    }

#if __cplusplus >= 201703L
    // Test access using get_as_string_view()
    auto flower_string_view = fs.get_as_string_view("flower.jpg");
    auto flower_string_view_size = flower_string_view.size();
    if(fs_size != flower_string_view_size) {
        std::cerr << "get_as_string_view(): Sizes do not match: FS == " << fs_size << ", RC == "
                  << flower_string_view_size << "\n";
        return 1;
    }
    if(0 != memcmp(flower_fs_vector.data(), flower_string_view.data(), flower_string_view_size)) {
        std::cerr << "get_as_string_view(): Contents do not match\n";
        return 1;
    }
#endif

    // Test access using get_as_raw_ptr() + get_size()
    auto flower_raw_ptr = fs.get_as_raw_ptr("flower.jpg");
    auto flower_raw_ptr_size = fs.get_size("flower.jpg");
    if(fs_size != flower_raw_ptr_size) {
        std::cerr << "get_as_raw_ptr()+get_size(): Sizes do not match: FS == " << fs_size << ", RC == "
                  << flower_raw_ptr_size << "\n";
        return 1;
    }
    if(0 != memcmp(flower_fs_vector.data(), flower_raw_ptr, flower_raw_ptr_size)) {
        std::cerr << "get_as_raw_ptr()+get_size(): Contents do not match\n";
        return 1;
    }

    // Test access using get_as_raw_ptr()
    std::size_t flower_raw_ptr_size2{0};
    auto flower_raw_ptr2 = fs.get_as_raw_ptr("flower.jpg", flower_raw_ptr_size2);
    if(fs_size != flower_raw_ptr_size2) {
        std::cerr << "get_as_raw_ptr(): Sizes do not match: FS == " << fs_size << ", RC == "
                  << flower_raw_ptr_size2 << "\n";
        return 1;
    }
    if(0 != memcmp(flower_fs_vector.data(), flower_raw_ptr2, flower_raw_ptr_size2)) {
        std::cerr << "get_as_raw_ptr(): Contents do not match\n";
        return 1;
    }

    // Explictly return success
    return 0;
}