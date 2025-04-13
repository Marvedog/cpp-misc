#include "config.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

void ConfigLoader::load(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto delimiter_pos = line.find('=');
        if (delimiter_pos == std::string::npos) continue;

        std::string key = line.substr(0, delimiter_pos);
        std::string value = line.substr(delimiter_pos + 1);
        config_[key] = value;
    }
}

std::string ConfigLoader::get(const std::string& key) const {
    auto it = config_.find(key);
    return (it != config_.end()) ? it->second : "";
}