#include "data_parser.hpp"

#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include <ctime>
#include <algorithm>

ParsedRow parseLine(const std::string &line)
{
    ParsedRow row;
    std::string cleanLine = line;
    cleanLine.erase(std::remove(cleanLine.begin(), cleanLine.end(), '\r'), cleanLine.end());

    if (cleanLine.size() >= 19)
        row.datetime = cleanLine.substr(0, 19);
    else
        return row;

    if (cleanLine.size() >= 32)
        row.part1 = cleanLine.substr(20, 14);
    else
        return row;

    if (cleanLine.size() >= 44)
        row.part2 = cleanLine.substr(35, 14);
    else
        return row;

    if (cleanLine.size() >= 59)
        row.part3 = cleanLine.substr(50, 29);
    else
        return row;

    if (cleanLine.size() >= 60)
        row.status1 = cleanLine.substr(80, 1);
    else
        return row;

    if (cleanLine.size() >= 64)
        row.status2 = cleanLine.substr(82, 3);
    else
        return row;

    if (cleanLine.size() >= 68)
        row.status3 = cleanLine.substr(86, 3);
    else
        return row;

    if (cleanLine.size() >= 72)
        row.status4 = cleanLine.substr(90, 4);
    else
        return row;

    if (cleanLine.size() >= 90)
        row.kanban = cleanLine.substr(95, 81);
    else
        return row;

    if (cleanLine.size() >= 124)
        row.code1 = cleanLine.substr(177, 19);
    else
        return row;

    if (cleanLine.size() >= 136)
        row.code2 = cleanLine.substr(197, 1);
    else
        return row;

    auto trim = [](std::string &s)
    {
        size_t start = s.find_first_not_of(' ');
        size_t end = s.find_last_not_of(' ');
        if (start == std::string::npos || end == std::string::npos)
        {
            s = "";
        }
        else
        {
            s = s.substr(start, end - start + 1);
        }
    };

    trim(row.part1);
    trim(row.part2);
    trim(row.part3);
    trim(row.status1);
    trim(row.status2);
    trim(row.status3);
    trim(row.status4);
    trim(row.kanban);
    trim(row.code1);
    trim(row.code2);

    return row;
}

void printParsedRow(const ParsedRow &row)
{
    std::cout << "datetime : " << row.datetime << "\n";
    std::cout << "part1    : " << row.part1 << "\n";
    std::cout << "part2    : " << row.part2 << "\n";
    std::cout << "part3    : " << row.part3 << "\n";
    std::cout << "status1  : " << row.status1 << "\n";
    std::cout << "status2  : " << row.status2 << "\n";
    std::cout << "status3  : " << row.status3 << "\n";
    std::cout << "status4  : " << row.status4 << "\n";
    std::cout << "kanban   : " << row.kanban << "\n";
    std::cout << "code1    : " << row.code1 << "\n";
    std::cout << "code2    : " << row.code2 << "\n";
}

std::tm parseDateTime(const std::string &dt)
{
    std::tm tm{};
    std::istringstream ss(dt);
    ss >> std::get_time(&tm, "%Y/%m/%d %H:%M:%S");
    return tm;
}

ParsedLine parseLineForDatetime(const std::string &line)
{
    ParsedLine parsed;
    if (line.size() < 19)
        return parsed;

    parsed.datetime = line.substr(0, 19);
    parsed.timeStruct = parseDateTime(parsed.datetime);
    parsed.originalLine = line;
    parsed.valid = true;
    return parsed;
}

std::string getLatestLineFromMultilineString(const std::string &multiline)
{
    std::istringstream stream(multiline);
    std::string line;
    std::vector<ParsedLine> entries;

    while (std::getline(stream, line))
    {
        auto parsed = parseLineForDatetime(line);
        if (parsed.valid)
            entries.push_back(parsed);
    }

    if (entries.empty())
        return "";

    auto latest = std::max_element(entries.begin(), entries.end(), [](const ParsedLine &a, const ParsedLine &b)
                                   { return std::mktime(const_cast<std::tm *>(&a.timeStruct)) < std::mktime(const_cast<std::tm *>(&b.timeStruct)); });

    return latest->originalLine;
}