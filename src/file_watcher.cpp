#include "file_watcher.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

std::optional<fs::directory_entry> get_latest_file(const std::string &dir_path)
{
    std::optional<fs::directory_entry> latest_file;
    fs::file_time_type latest_time;

    for (const auto &entry : fs::directory_iterator(dir_path))
    {
        if (fs::is_regular_file(entry))
        {
            auto mod_time = fs::last_write_time(entry);
            if (!latest_file || mod_time > latest_time)
            {
                latest_file = entry;
                latest_time = mod_time;
            }
        }
    }
    return latest_file;
}

std::string read_file(const std::string &filename)
{
    std::ifstream infile(filename);
    if (!infile)
    {
        std::cerr << "Failed to open file: " << filename << "\n";
        return "";
    }

    std::ostringstream buffer;
    buffer << infile.rdbuf(); // Read entire file into buffer
    return buffer.str();
}
