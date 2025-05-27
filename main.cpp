#include "file_watcher.hpp"
#include "data_parser.hpp"
#include "udp_sender.hpp"
#include <iostream>
#include <windows.h>
#include <thread>
#include <fstream>
#include <sstream>

std::string serverIP;
int serverPort;
std::string folderDir;

void load_env_config()
{
    std::ifstream envFile(".env");
    if (!envFile)
    {
        std::cerr << "Error: .env file not found!" << std::endl;
        exit(1);
    }

    std::string line;
    while (std::getline(envFile, line))
    {
        std::istringstream ss(line);
        std::string key, value;
        if (std::getline(ss, key, '=') && std::getline(ss, value))
        {
            if (key == "SERVER_IP")
                serverIP = value;
            else if (key == "SERVER_PORT")
                serverPort = std::stoi(value);
            else if (key == "DATA_FOLDER_DIR")
                folderDir = value;
        }
    }

    if (serverIP.empty() || serverPort == 0 || folderDir.empty())
    {
        std::cerr << "Error: Invalid .env configuration!" << std::endl;
        exit(1);
    }
}

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

            std::string message = result.kanban + result.code1;
            send_msg(message.c_str());
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

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main()
{
    load_env_config();
    initialize_socket(serverIP, serverPort);
    try
    {
        watch_directory(folderDir);
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Error: " << ex.what() << "\n";
    }
    return 0;
}