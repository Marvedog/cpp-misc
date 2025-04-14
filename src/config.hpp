#pragma once
#include <string>
#include <unordered_map>

class ConfigLoader {
public:
    void load(const std::string& filename);
    std::string get(const std::string& key) const;

private:
    std::unordered_map<std::string, std::string> config_;
};