#ifndef FILEWATCHER_HPP
#define FILEWATCHER_HPP

#include <string>
#include <optional>
#include <filesystem>

namespace fs = std::filesystem;

std::optional<fs::directory_entry> get_latest_file(const std::string &dir_path);
std::string read_file(const std::string &filename);

#endif
