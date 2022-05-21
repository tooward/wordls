#pragma once

#include <string>
#include <vector>
#include <utility>
#include <unordered_map>
#include <map>
#include <tuple>
#include <stack>

// https://jbp.dev/blog/dawg-first-implementation.html
// TODO - remove both include and the dictionaryEntry stuct definition here and move to a template 
// #include "../dictionary/dictionary.h"

class dictionaryEntry
{
    public:
        std::string word;
        std::vector <std::string> definitions;
        std::vector <std::string> types;
        int count; // number of definitions in the dictionary
};

class BasicNode
{
    public:
        bool IsTerminal;
        int Id;
        std::map<char, BasicNode*> Children;
        static int highestId;
        BasicNode();

        const inline bool operator< (const BasicNode& rhs) const
        {
            return Id < rhs.Id;
        }

        const inline bool operator!= (const BasicNode& rhs) const
        {
            return Id != rhs.Id;
        }

};

class dawg
{
    public:
        BasicNode Root;
        void Insert(std::string);
        BasicNode Finish();
        std::vector<std::string> dfs(std::string, BasicNode*);
        void rdfs(std::vector<std::string>* results, BasicNode* n, std::string);
        static BasicNode* returnNodeIfChild(char, BasicNode*);
        static BasicNode* navigateToNode(BasicNode*, std::string);
        void printDawg();
        dawg();

    private:
        std::string _previousWord = "";
        std::map<BasicNode*, BasicNode*> _minimizedNodes;
        std::stack<std::tuple<BasicNode*, char, BasicNode*>> _uncheckedNodes;
        int CommonPrefix(std::string);
        void Minimize(int);
        void printRecursion(BasicNode*, std::string);
        BasicNode* containsLetterNode(std::string letter); // TODO - need to expand this so looks at terminal or not
};