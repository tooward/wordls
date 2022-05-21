#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <map>
#include <tuple>

struct dictionaryEntry
{
    std::string word;
    std::vector <std::string> definitions;
    std::vector <std::string> types;
    int count; // number of definitions in the dictionary
};