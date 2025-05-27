#include <string>
#include <ctime>

struct ParsedRow
{
    std::string datetime;
    std::string part1;
    std::string part2;
    std::string part3;
    std::string status1;
    std::string status2;
    std::string status3;
    std::string status4;
    std::string kanban;
    std::string code1;
    std::string code2;
};

struct ParsedLine
{
    std::string originalLine;
    std::string datetime;
    std::tm timeStruct{};
    bool valid = false;
};

ParsedRow parseLine(const std::string &line);
void printParsedRow(const ParsedRow &row);
std::tm parseDateTime(const std::string &dt);
ParsedLine parseLineForDatetime(const std::string &line);
std::string getLatestLineFromMultilineString(const std::string &multiline);
