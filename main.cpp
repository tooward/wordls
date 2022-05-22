#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <regex>
#include <utility>
#include "unorderedmap.h"
#include "dawg.h"
using namespace std;
namespace fs = std::filesystem;

int BasicNode::highestId = 0;

int main(int argc, char* argv[], char* envp[]){

    int maxWordSize = 5;
    int minWordSize = 5;
    int maxRecords = 2000000;

// Load word book counts
    std::string path = std::__fs::filesystem::current_path();
    path += "/data/wordcounts";
    std::map<string, int> wordCounts;
    std::string wordBuffer;
    int countBuffer;
    
    int bookCount=0;
    std::cout << "Loaded books: " << bookCount << std::flush;

    for (const auto & entry : fs::directory_iterator(path)){
        //std::cout << entry.path() << std::endl;
        std::ifstream instream(entry);
        if (!instream.is_open()){
            instream.close();
            throw std::runtime_error("Could not load file");
        }

        bool stillreading = true;
        int counter=0;
        std::string line;

        while (stillreading && std::getline(instream, line)){

            // get the word
            regex regexpw("(\\D+)(?=,)");
            smatch w;
            std::regex_search(line, w, regexpw);

            int rc = 0;
            for (auto x : w){
                if (rc== 1){
                    string result = x;
                    if(result.length() <= maxWordSize && result.length() >= minWordSize){
                        if(containsOnlyLetters(x)){
                            wordBuffer=x;
                        }
                    }
                }
                rc++;
            }

            // get the number
            regex regexpn("(\\d+)(,\\s*\\d+)*");
            smatch n;
            std::regex_search(line, n, regexpn);
            rc = 0;
            for (auto x : n){
                if (rc== 1){
                    countBuffer = std::stoi(x);                        
                }
                rc++;
            }
            wordCounts.insert(make_pair(wordBuffer, countBuffer));
        }
        bookCount++;
        if (bookCount < 10)
            std::cout << "\b" << bookCount << std::flush;
        else
            std::cout << "\b\b" << bookCount << std::flush;
    }
    std::cout << "(books loaded unique word count size: " << wordCounts.size() << ")" << std::endl;

// load dictionary into a String Vector
    std::string curdir = std::__fs::filesystem::current_path();
    curdir += "/data/csvDictionary.csv";
    StringVec dictionaryWords;
    dictionaryWords = loadDictionary(curdir, false, maxRecords, minWordSize, maxWordSize);

    std::cout << "(loaded " << dictionaryWords.size() << ", " << maxWordSize << " char words)" << endl;

// convert word list to dawg
    dawg * dict = new dawg();
    //std::cout << "-- Root id is: " << dict->Root.Id << std::endl;
    for (auto w : dictionaryWords){
        dict->Insert(w);
    }

    BasicNode rootNode = dict->Finish();
    // cout << "(loaded dawg)" << endl;
    //std::cout << "DAWG has " << rootNode.Children.size() << " children" << endl;

    std::map<char, std::pair<int,int>> characterFrequencyMap; // first int field is count, second is value
    std::map<char, std::vector<std::pair<std::string, int>>> charMap; // character to word mapping. Word pair holds point value

// calculate the character frequency
    for (auto result : dictionaryWords){

        // get frequency value from wordCounts (frequency word appears in books)
        auto word = wordCounts.find(result);
        int frequency = 0;
        if (word != wordCounts.end())
        {
            frequency = word->second;
        }

        // calculate point value of word based on letter frequency across words
        for (char letter : result){
            if (std::isalpha(letter)){
                if(characterFrequencyMap.count(letter))
                {
                    characterFrequencyMap[letter].first = characterFrequencyMap[letter].first + 1;
                }
                else
                    characterFrequencyMap[letter].first = 1;

                // TODO - move this to point to word in trie
                auto search = charMap.find(letter);
                if (search != charMap.end()){
                    // update by adding word
                    std::pair<std::string, int> rp{result, frequency};
                    search->second.push_back(rp);
                }
                else{
                    // insert
                    vector<std::pair<std::string, int>> words;
                    words.push_back(std::pair<string, int>{result, frequency});
                    std::pair<char, std::vector<std::pair<std::string, int>>> charword{letter, words};
                    charMap.insert(charword);
                }
            }
        }
    }

    // convert map to pair vector for sorting
    // character = alphabetic char, first int = count (times char appears in word) and rank (sum of times appears)
    std::vector<std::pair<char, pair<int, int>>> characterCountAndRankVector;
    for (auto c = characterFrequencyMap.begin(); c != characterFrequencyMap.end(); ++c){
        characterCountAndRankVector.push_back(*c);
    }

    //std::cout << "CharFrequency " << characterFrequencyMap.size() << std::endl;

    // sort by frequency
    std::sort(characterCountAndRankVector.begin(), characterCountAndRankVector.end(), [=](std::pair<char, pair<int, int>>& a, std::pair<char, pair<int, int>>& b){
        return a.second.first < b.second.first;
    });

// calculate value for letter
    int count=0;
    for (auto c = characterCountAndRankVector.begin(); c != characterCountAndRankVector.end(); ++c){
        c->second.second = count;
        count++;
    }

// update the map to have the value for the letter
    for (auto c : characterCountAndRankVector){
        characterFrequencyMap.find(c.first)->second.second = c.second.second;
    }

    // for (auto c = characterFrequencyMap.begin(); c != characterFrequencyMap.end(); ++c){
    //     std::cout << c->first << " : " << c->second.first << " : " << c->second.second << std::endl;
    // }

// Print letter frequency & point values
    std::cout << "Letter Frequency and value \n" << std::endl;
    for (auto c = characterCountAndRankVector.begin(); c != characterCountAndRankVector.end(); ++c){
        std::cout << c->first << " : " << c->second.first << " : " << c->second.second << std::endl;
    }

// calculate point value for words
    std::vector<std::pair<std::string, std::pair<int, int>>> wordsWithFrequencyAndCount;
    for (auto letter : charMap){
        for (auto word : letter.second){

            // calculate letter total count
            int wordCharCount=0;
            for (auto c : word.first)
            {
                wordCharCount += characterFrequencyMap.find(c)->second.second;
            }

            std::pair<int, int> frequencyCount{word.second, wordCharCount};
            std::pair<std::string, std::pair<int, int>> wordFrequencyCount{word.first, frequencyCount};
            wordsWithFrequencyAndCount.push_back(wordFrequencyCount);
        }
    }

// Suggest first word. If > first loop suggests starting word based on ignored characters.
    std::vector<char> ignoreChars;
    std::cout << "Suggested first word: " << calculateBestStartWord(ignoreChars, wordsWithFrequencyAndCount) << std::endl;

/** INPUT LOOP **/

// prompt user for letters to query
    bool exit;
    std::string input;
    bool firstLoop = true;
    while (!exit){

    // find words that contain all the characters in specific positions
        std::cout << "(type 'exit' to exit):"  << std::endl;
        std::cout << "Input characters to search for in order with _ for empty character (ex: _ _ e _ _):" << std::endl;
        std::cin >> input;
        if (input == "exit"){
            exit = true;
            break;
        }
        
        bool firstChar = true;
        StringIntPairVec results;
        StringIntPairVec rightResults;

    // find words that match positions
        for (int i=0; i < input.size(); i++){
                // ignore position markers _
            if (input[i] != '_' && std::isalpha(input[i])){
                //std::cout << "- is alpha and not _" << std::endl;
                if (firstChar){
                //std::cout << "- first char" << std::endl;
                    results = charMap.find(input[i])->second; // obtains all words that this letter appears in
                    //std::cout << "-- search first char (" << input[i] << ") results = " << results.size() << std::endl;
                    firstChar=false;
                }

                if (!firstChar){
                    //std::cout << "- not first char" << std::endl;
                    // find words in this array that are in the other array
                    rightResults = charMap.find(input[i])->second;
                    //std::cout << "-- search next char (" << input[i] << ") results = " << rightResults.size() << std::endl;
                    // assume vectors are sorted
                    results = intersectionStringIntPair(&results, &rightResults);
                    //std::cout << "-- search intersection results = " << results.size() << std::endl;
                }

                // filter out words with character not in this position
                //std::cout << "- filtering out non-positional words" << std::endl;
                StringIntPairVec temp;
                for(auto word : results){
                    if (word.first[i] == input[i])
                    {
                        temp.push_back(word);
                    }
                }
                results = temp;
            }
        }

    std::cout << "-- results count " << results.size() << std::endl;

    // find words that must have character but not in position
            std::cout << "Input characters to search without position in positional format (ex: _ _ _ a r):"  << std::endl;
            std::cin >> input;
            if (input == "exit"){
                exit = true;
                break;
            }
    
    //std::cout << "-- search first char (" << input[0] << ") results = " << results.size() << std::endl;
            if (input.size() > 0){
                //std::cout << "-- in characters without position, starting results: " << results.size() << std::endl;

                for(int pos=0; pos < input.size(); pos++){
                    if(std::isalpha(input[pos])){
                        // may not have any results yet if word had no hits
                        if (results.size() == 0) {
                            results = charMap.find(input[pos])->second; // obtains all words that this letter appears in
                            std::cout << "-- no results so getting first result set, obtained results from first char " << input[pos] << " count: " << results.size() << std::endl;
                        }
                        else{
                        // find words in this array that are in the other array
                            rightResults = charMap.find(input[pos])->second;
                            // std::cout << "-- search next char (" << input[pos] << ") results = " << rightResults.size() << std::endl;
                            // assume vectors are sorted
                            results = intersectionStringIntPair(&results, &rightResults);
                            //std::cout << "-- search intersection results count = " << results.size() << std::endl;
                        }
                    }
                }

                // std::cout << "-- starting results count = " << results.size() << std::endl;
                // remove words that have characters in the positions indicated in entry
                int removedCount = 0;
                int processedCount = 0;
                int addedCount = 0;
                StringIntPairVec removedCharPositions;

                for(int pos=0; pos < results.size(); pos++){
                        processedCount++;

                        for (int w=0; w < input.size(); w++){

                            if(std::isalpha(input[w])){

                                if (results[pos].first[w] == input[w]){
                                    //std::cout << "-- found word " << results[pos].first << " with char " << input[w] << " in position " << w + 1 << std::endl;
                                    removedCount++;
                                    break;
                                }
                            }
                            if (w == input.size()-1){
                                removedCharPositions.push_back(results[pos]);
                                addedCount++;
                            }
                    }

                }

                results.clear();
                //std::cout << "-- processed " << processedCount << " added " << addedCount << " & removed " << removedCount << " words. removedCharPositions = " << removedCharPositions.size() << std::endl;

                results = removedCharPositions;
                //std::cout << "-- updated results count = " << results.size() << std::endl;

            }// if input size > 0

    // Exclude characters
        std::cout << "Exclude characters (- for none):"  << std::endl;
        std::string exclude;
        std::cin >> exclude;
        if (containsOnlyLetters(exclude)){

            if (exclude == "exit"){
                exit = true;
                break;
            }

            for(int pos=0; pos < exclude.size(); pos++){
                results = removeContainsCharStringIntPair(&results, exclude[pos]);
    //            std::cout << "-- search next char (" << exclude[pos] << ") results = " << results.size()<< std::endl;
            }

            for (auto c : exclude)
            {
                ignoreChars.push_back(c);
            }

        }

    // sort results by point value before printing (descending order)
        std::sort(results.begin(), results.end(), [=](std::pair<string, int>& a, std::pair<string, int>& b){
            return a.second < b.second;
        });

    // print results
        if (results.size() > 0){
            for(auto result : results){
                std::cout << result.first << " (" << result.second << ")" << std::endl;
            }
            std::cout << results.size() <<  " results: " << std::endl;
        }
        else{
            std::cout << "No results!" << std::endl;
//            std::cout << "ignoreChars count: " << ignoreChars.size() << std::endl;
            std::cout << "Suggested retry word: " << calculateBestStartWord(ignoreChars, wordsWithFrequencyAndCount) << std::endl;
        }

    }

}