#include <stdexcept>
#include <iostream>
#include <fstream>
#include <string>
#include <set>
#include <vector>
#include <cctype>
#include <algorithm>
#include <regex>
#include <unordered_map>
#include <map>
#include <utility>

#include "unorderedmap.h"

using namespace std;


/*
 * Returns the intersection of two vectors of strings
 */
std::vector<std::pair<std::string, int>> intersectionStringIntPair(std::vector<std::pair<std::string, int>>* left, std::vector<std::pair<std::string, int>>* right){
    std::vector<std::pair<std::string, int>> results;
//    std::cout << "--- intersection - left count: " << left->size() << ", right count: " << right->size() << std::endl;
    for(std::pair<std::string, int> l : *left){
        for(std::pair<std::string, int> r : *right){
            if (r.first.compare(l.first) == 0){
//                std::cout << "--- intersection - match: " << l << " = " << r << std::endl;
                results.push_back(r);
                break;
            }
        }
    }
//    std::cout << "--- intersection - results count: " << results.size() << std::endl;
    return results;
}

StringVec intersection(StringVec* left, StringVec* right){
    StringVec results;
//    std::cout << "--- intersection - left count: " << left->size() << ", right count: " << right->size() << std::endl;
    for(std::string l : *left){
        for(std::string r : *right){
            if (r.compare(l) == 0){
//                std::cout << "--- intersection - match: " << l << " = " << r << std::endl;
                results.push_back(r);
                break;
            }
        }
    }
//    std::cout << "--- intersection - results count: " << results.size() << std::endl;
    return results;
}

bool containsOnlyLetters(std::string const &str) {
    return std::all_of(str.begin(), str.end(), [](char const &c) {
        return std::isalpha(c);
    });
}

std::vector<std::pair<std::string, int>> removeContainsCharStringIntPair(std::vector<std::pair<std::string, int>>* list, char character){

    std::vector<std::pair<std::string, int>> results;

    for(std::pair<std::string, int> w : *list){
//        std::cout << "evaulating " << w << std::endl;
        if (!containsLetters(w.first, character))
        {
            results.push_back(w);
        }
    }
    list->clear();
    return results;
}

StringVec removeContainsChar(StringVec* list, char character){
    StringVec results;
    for(std::string w : *list){
//        std::cout << "evaulating " << w << std::endl;
        if (!containsLetters(w, character))
        {
            results.push_back(w);
        }
    }
    list->clear();
    return results;
}

bool containsLetters(std::string const &str, char character) {
    for(char l : str){
        if(l == character)
            return true;
    }

    return false;
    /*
    return std::all_of(str.begin(), str.end(), [character](char const &c) {
        if (c == character)
            std::cout << "Character found in word" << std::endl;
        return c == character;
    });
    */
}

/*
 * Removes a set of strings from main vector
 */
StringVec outerJoin(StringVec* left, StringVec* right){
    StringVec* results;
    bool found=false;
    std::cout << "--- outerJoin - left count: " << left->size() << ", right count: " << right->size() << std::endl;
    for(std::string l : *left){
        std::cout << "-- intersection - looking for word: " << l << std::endl;

        for(std::string r : *right){
//            std::cout << "--- intersection - comparison: " << l << " = " << r << std::endl;

            if (l.compare(r) == 0){
                    std::cout << "--- intersection - match: " << l << " = " << r << std::endl;
                    found = true;
                    break;
            }
        }

        if (!found){
            std::cout << "--- intersection: adding " << l << std::endl;
            results->push_back(l);
            found = false;
        }
        else{
            found = false;
        }
    }

//    std::cout << "--- outerJoin - results count: " << results.size() << std::endl;
//    left->clear();
    return *results;
}

/* 
 * a set of regex to find 'troublesome' string patterns 
 */
std::string cleanLine(std::string line){

        // std::cout << " reading line: " << line << std::endl;
        // cout << "-- Starting Line: " << line << endl;

        // replace backticks with single quote
        while (line.find("’") != std::string::npos){
            // cout << "--- Removed ' : " << line << endl;
            line = std::regex_replace(line, std::regex("’"), "'");
        }

        // replace double dash with a space to preserve single hyphens
        while (line.find("--") != std::string::npos){
            // cout << "-- Removed -- : " << line << endl;
            line = std::regex_replace(line, std::regex("--"), " ");
        }

        //        cout << "-- Cleaned Line before loop: " << line << endl;

        int consumedChars = 0;
        char prevChar = ' ';
        for (char& c : line){

            consumedChars++;
            // lower case a capital (preserve escape characters)
            if (std::isalpha(c)) {
                char thisChar = std::tolower(c);
                if (prevChar == '\''){
                    line += "'";
                }
                else if (prevChar == '-'){
                    line += "-'";
                }
                line += thisChar;
                prevChar = thisChar;
            }
            else if ('\'' == c){
            // detect escape character for possesives ('s)
                if (std::isalpha(prevChar)){
                    prevChar = '\'';
                }
                else{
                    prevChar = ' ';
                }
            }
            else if ('-' == c){
            // detect escape character for hypenation (go-to)
                if (std::isalpha(prevChar)){
                    prevChar = '-';
                }
                else{
                    prevChar = ' ';
                }
            }
        } // end char analysis loop

        std::transform(line.begin(), line.end(), line.begin(),
            [](unsigned char c){ return std::tolower(c); });

        return line;

} // end getLine()

// may need to revisit this to manage full set of UTF-8 characters (or save files to avoid this)
// Improvements: load list of common words to be eliminated (for auto-dictionary use)
StringVec loadDictionary(const std::string filename, const bool DEBUGGING, const int DEBUGGING_MAX_WORDS, int min_word_length, int max_word_length) {

    StringVec bookstrings;     // vector of strings to hold unique string list
    std::set<string> uniqueWords;

    std::ifstream instream(filename);
    if (!instream.is_open()){
        instream.close();
        throw std::runtime_error("Could not load file: " + filename);
    }

    bool stillreading = true;
    int counter=0;
    std::string line;
    string lastWord;
    while (stillreading && std::getline(instream, line)){
//        cout << "Pre-clean: " << line << endl;
//        cout << "Reading line" << counter << endl;
        line = cleanLine(line);
//        cout << "Clean: " << line << endl;

        // add words to map. Only take the first word in double quotes.
        regex regexp("\"(.*?)\"");
        smatch m;
        regex_search(line, m, regexp);

        int rc = 0;
         for (auto x : m){
            if (rc== 1){
                string result = x;
                if(result.length() <= max_word_length && result.length() >= min_word_length){
                    if(containsOnlyLetters(x)){
                        if (x.compare(lastWord) != 0){
                            uniqueWords.insert(x);
                            lastWord = x;
                        }
                    }
                    
//                    std::cout << "Inserted word into set: " << x << std::endl;
                }//end if(result.length() <= max_word_length && result.length() >= min_word_length){
            }
            rc++;
        }

        if (DEBUGGING && (uniqueWords.size() >= DEBUGGING_MAX_WORDS || counter > DEBUGGING_MAX_WORDS))
        {
            stillreading = false;
            break;
        }
        counter++;

    }

//    std::cout << "** Copying set to string vector ** " << std::endl;
    std::copy(uniqueWords.begin(), uniqueWords.end(), std::back_inserter(bookstrings));
//    std::cout << "Bookstring size is: " << bookstrings.size() << std::endl;

//    std::cout << "** Copied set to string vector ** " << std::endl;
    std::sort(bookstrings.begin(), bookstrings.end());

    std::cout << "(processed " << counter << " records)" << std::endl;

    return bookstrings;
}

// makeWordCounts: Given a vector of (non-unique) strings, returns a
// StringIntMap where each string is mapped to its number of occurences.
StringIntMap makeWordCounts(const StringVec& words) {
  StringIntMap wordcount_map;

  // loop through the vector
  for (unsigned int i=0; i < words.size(); i++){
    // lookup value in map
    bool contains = wordcount_map.count(words[i]);
    if (contains){
      wordcount_map[words[i]]++;
    }
    else{
      StringIntPair wordcount{words[i], 1};
      wordcount_map.insert(wordcount);
    }
  }

  return wordcount_map;
}

std::map<char, std::pair<int,int>> calculateCharacterFrequency(StringVec dictionaryWords, std::map<char, std::vector<std::pair<std::string, int>>> charMap){

    // first int field is count, second is value
    std::map<char, std::pair<int,int>> characterFrequencyMap; 

    // calculate the character frequency
    for (auto result : dictionaryWords){
        /** Character Frequency **/
        for (char letter : result){
            if (std::isalpha(letter)){
                if(characterFrequencyMap.count(letter))
                {
                    characterFrequencyMap[letter].first = characterFrequencyMap[letter].first+ 1;
                }
                else
                    characterFrequencyMap[letter].first = 1;

    // TODO - move this to point to word in trie
                auto search = charMap.find(letter);
                if (search != charMap.end()){
                    // update by adding word
                    std::pair<std::string, int> rp{result, 0} ;
                    search->second.push_back(rp);
                }
                else{
                    // insert
                    vector<std::pair<std::string, int>> words;
                    words.push_back(std::pair<string, int>{result, 0});
                    std::pair<char, std::vector<std::pair<std::string, int>>> charword{letter, words};
                    charMap.insert(charword);
                }
            }
        }
    }

    return characterFrequencyMap;
}

std::string calculateBestStartWord (
            std::vector<char> ignoreChars,
            std::vector<std::pair<char, pair<int, int>>> characterCountAndRankVector,
            std::map<char, std::vector<std::pair<std::string, int>>> charMap)
{

    // need to refactor this
    // - this requires the earlier letters in the word. ex: a will always be in the word. may not be highest value word
    // - implement ignore chars so can recommend a follow-up word if first word has no hits

    std::string result;

    // sort characterCountAndRankVector by frequency
    std::sort(characterCountAndRankVector.begin(), characterCountAndRankVector.end(), [=](std::pair<char, pair<int, int>>& a, std::pair<char, pair<int, int>>& b){
        return a.second.first > b.second.first;
    });

    StringIntPairVec results;
    StringIntPairVec tempResults;
    results = charMap.find(characterCountAndRankVector[0].first)->second; // obtains all words that this letter appears in
    StringIntPairVec rightResults;
    int capturedChars=10;

    std::cout << "** search for best first word **" << std::endl;
    // get words from charMap for first ten letters, grab highest ranked words
    for (int i=1; i < capturedChars; i++){
        std::cout << " - evaluating letter " << characterCountAndRankVector[i].first << std::endl;
        rightResults = charMap.find(characterCountAndRankVector[i].first)->second;
        tempResults = intersectionStringIntPair(&results, &rightResults);
        if (tempResults.size() > 0){
            results = tempResults;
        }
        else{
            std::cout << " -- no common results with prior letter " << std::endl;
        }       
    }

    // rank by value
    std::sort(results.begin(), results.end(), [=](std::pair<string, int>& a, std::pair<string, int>& b){
        return a.second < b.second;
    });
    std::cout << "(point value of suggested word is " << results[0].second << std::endl;

    return results[0].first;
}