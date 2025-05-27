#include <iostream>
#include <ctime>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <thread>
#include <string>

namespace fs = std::filesystem;

void read_file(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    std::cout << "Reading file content:\n";
    std::string line;
    while (std::getline(infile, line)) {
        std::cout << line << "\n";
    }
    std::cout << "------------------------\n";
}

int main() {
    std::string filename = "temp/data.dat";

    if (!fs::exists(filename)) {
        std::cerr << "File does not exist: " << filename << "\n";
        return 1;
    }

    auto last_time = fs::last_write_time(filename);
    read_file(filename);

    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        try {
            auto current_time = fs::last_write_time(filename);
            if (current_time != last_time) {
                last_time = current_time;
                std::cout << "[File modified - re-reading]\n";
                read_file(filename);
            }
        } catch (const fs::filesystem_error& e) {
            std::cerr << "Error checking file: " << e.what() << "\n";
        }
    }

    return 0;
}