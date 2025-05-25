#include <iostream>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <thread>
#include <string>
#include <optional>

namespace fs = std::filesystem;

std::optional<fs::directory_entry> get_latest_file(const std::string& dir_path) {
    std::optional<fs::directory_entry> latest_file;
    fs::file_time_type latest_time;

    for (const auto& entry : fs::directory_iterator(dir_path)) {
        if (fs::is_regular_file(entry)) {
            auto mod_time = fs::last_write_time(entry);
            if (!latest_file || mod_time > latest_time) {
                latest_file = entry;
                latest_time = mod_time;
            }
        }
    }
    return latest_file;
}

void read_file(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    std::cout << "Reading file: " << filename << "\n";
    std::string line;
    while (std::getline(infile, line)) {
        std::cout << line << "\n";
    }
}

int main() {
    std::string directory = "./temp";
    std::optional<fs::directory_entry> current_file;
    fs::file_time_type last_time;

    while (true) {
        auto latest = get_latest_file(directory);
        if (!latest) {
            std::cerr << "No files found in directory.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        if (!current_file || latest->path() != current_file->path()) {
            current_file = latest;
            last_time = fs::last_write_time(current_file->path());
            read_file(current_file->path().string());
        } else {
            auto current_time = fs::last_write_time(current_file->path());
            if (current_time != last_time) {
                last_time = current_time;
                std::cout << "[File modified detected]\n";
                read_file(current_file->path().string());
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
