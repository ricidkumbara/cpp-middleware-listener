#include "file_watcher.hpp"
#include "data_parser.hpp"
#include "udp_sender.hpp"
#include <iostream>
#include <windows.h>
#include <thread>
#include <fstream>
#include <sstream>
#include <filesystem>

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

void process_file_if_changed(const fs::directory_entry &file, fs::file_time_type &last_time, std::string &last_content) {
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

            // std::string message = result.kanban + result.code1;
            std::string message = result.kanban;
            send_msg(message.c_str());
        }
    }
}

std::string formatYYMM(int year, int month) {
    if (month == 0) {
        month = 12;
        year -= 1;
    }

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << (year % 100)
        << std::setw(2) << std::setfill('0') << month;
    return oss.str();
}

std::string buildFolderPath(const std::string& yymm) {
    std::string path = folderDir;
    size_t pos = path.find("{var}");
    if (pos != std::string::npos) {
        path.replace(pos, 5, yymm);
    }
    return path;
}

int folder_checker(const std::string &dir_path) {
    if (fs::exists(dir_path) && fs::is_directory(dir_path)) {
        return 1;
    }
    
    // std::cout << "Directory does NOT exist: " << dir_path << std::endl;
    return 0;
}

void watch_directory(const std::string &directory)
{
    std::optional<fs::directory_entry> current_file;
    fs::file_time_type last_time;
    std::string last_content;

    while (true)
    {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);

        int year = now->tm_year % 100;  // Get last two digits of year
        int month = now->tm_mon + 1;    // tm_mon is 0-based

        std::ostringstream oss;
        oss << std::setw(2) << std::setfill('0') << year
            << std::setw(2) << std::setfill('0') << month;

        std::string current_month = oss.str();
        std::string folder_path = folderDir;
        size_t pos = folder_path.find("{var}");

        if (pos != std::string::npos) {
            folder_path.replace(pos, 5, current_month); // 5 is the length of "{var}"
        }

        if (folder_checker(folder_path) == 0) {
            std::string prevYYMM = formatYYMM(year, month - 1);
            folder_path = buildFolderPath(prevYYMM);

            if (folder_checker(folder_path) == 0) {
                std::cout << "Directory not found" << std::endl;
            }
        }
        
        auto latest = get_latest_file(folder_path);
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