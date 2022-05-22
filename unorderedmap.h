#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <map>

using StringIntPair = std::pair<std::string, int>;
using StringIntMap = std::unordered_map<std::string, int>;
using StringIntPairVec = std::vector<StringIntPair>;
using StringVec = std::vector<std::string>;

std::string cleanLine(std::string);
StringVec loadDictionary(const std::string, const bool, const int, int, int);
StringIntMap makeWordCounts(const StringVec&);
StringVec intersection(StringIntPairVec* left, StringIntPairVec* right);
StringIntPairVec intersectionStringIntPair(StringIntPairVec* left, StringIntPairVec* right);
StringVec outerJoin(StringVec* left, StringVec* right);
StringVec removeContainsChar(StringVec* list, char character);
std::vector<std::pair<std::string, int>> removeContainsCharStringIntPair(StringIntPairVec* list, char character);
bool containsLetters(std::string const &str, char character);
bool containsOnlyLetters(std::string const &str);
std::string calculateBestStartWord (std::vector<char> ignoreChars, std::vector<std::pair<std::string, std::pair<int, int>>> wordsWithFrequencyAndCount);