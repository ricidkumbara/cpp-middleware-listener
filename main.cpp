#include "file_watcher.hpp"
#include "data_parser.hpp"
#include "udp_sender.hpp"
#include <iostream>
#include <windows.h>
#include <thread>

void process_file_if_changed(
    const fs::directory_entry &file,
    fs::file_time_type &last_time,
    std::string &last_content)
{
    auto current_time = fs::last_write_time(file.path());
    if (current_time != last_time)
    {
        last_time = current_time;
        std::string current_content = read_file(file.path().string());
        if (current_content != last_content)
        {
            last_content = current_content;
            std::string latest_line = getLatestLineFromMultilineString(current_content);
            ParsedRow result = parseLine(latest_line);
            printParsedRow(result);
        }
    }
}

void watch_directory(const std::string &directory)
{
    std::optional<fs::directory_entry> current_file;
    fs::file_time_type last_time;
    std::string last_content;

    while (true)
    {
        auto latest = get_latest_file(directory);
        if (!latest)
        {
            std::cerr << "No files found in directory.\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            continue;
        }

        if (!current_file || latest->path() != current_file->path())
        {
            current_file = latest;
            last_time = fs::file_time_type::min(); // force reprocessing
            last_content.clear();
        }

        if (current_file)
        {
            process_file_if_changed(*current_file, last_time, last_content);
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main()
{
    const std::string directory = "./temp";
    try
    {
        watch_directory(directory);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
    }
    return 0;
}