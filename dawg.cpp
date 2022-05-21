#include <tuple>
#include <stack>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <iostream>

#include "dawg.h"

BasicNode::BasicNode(){
    this->Id = BasicNode::highestId;
    BasicNode::highestId = BasicNode::highestId + 1;
//    std::cout << "-- created new Basic Node id: " << this->Id << ", highest Id: " << BasicNode::highestId << std::endl;
}

dawg::dawg(){ }

void  dawg::Insert(std::string word){

//    std::cout << "\nProcessing word " << word << std::endl;

    int commonPrefix = CommonPrefix(word);
    Minimize(commonPrefix);

    BasicNode* node = _uncheckedNodes.size() == 0 ? &this->Root : std::get<2>(_uncheckedNodes.top());
//    std::cout << "- Parent node to insert word to id: " << node->Id << " with " << node->Children.size() << " children" << std::endl;

    for(int pos = commonPrefix; pos < word.size(); pos++){
        BasicNode* nextNode = new BasicNode();
//        std::cout << "-- New node id: " << nextNode->Id << ", character is " << word[pos] << std::endl;

// ERROR - for common root this overwrites the original child (ex: root of I and word of Iamb)
        node->Children[word[pos]] = nextNode;
//        std::cout << "-- Parent node " << node->Id << " now has " << node->Children.size() << " child letters" << std::endl;
        _uncheckedNodes.push(std::tuple<BasicNode*, char, BasicNode*>(node, word[pos], nextNode));
        node = nextNode;
    }

    node->IsTerminal = true;
    _previousWord = word;
//    std::cout << "Inserted: " << word << std::endl;
//    std::cout << "Full trie: " << std::endl;
//    printRecursion(&Root, "");    
}

BasicNode dawg::Finish(){
    Minimize(0);
    _minimizedNodes.empty();
    _uncheckedNodes.empty();

    return Root;
}

int dawg::CommonPrefix(std::string word){
//    std::cout << "-- Searching for common prefix with word: " << word << "and previous word: " << _previousWord << std::endl;

    for (int commonPrefix = 0; commonPrefix < std::min(word.length(), _previousWord.length()); commonPrefix++)
    {
 //       std::cout << "-- index: " << commonPrefix << " | letter " << word[commonPrefix] << " previous word letter " << _previousWord[commonPrefix] << std::endl;

        if (word[commonPrefix] != _previousWord[commonPrefix])
        {
//            std::cout << "-- Common prefix found: " << commonPrefix << " character " << word[commonPrefix] << " in word " << word << std::endl;
            return commonPrefix;
        }
        else if(commonPrefix == 0 && (std::min(word.length(), _previousWord.length()) == 1) && word[commonPrefix] == _previousWord[commonPrefix]){
//            std::cout << "-- Common prefix found at root char: " << commonPrefix << " character " << word[commonPrefix] << " in word " << word << std::endl;
            commonPrefix = 1;
            return commonPrefix;
        }
        else if(commonPrefix == std::min(word.length(), _previousWord.length()) - 1 && commonPrefix > 0){
//            std::cout << "-- Common prefix found at end char: " << commonPrefix << " character " << word[commonPrefix] << " in word " << word << std::endl;
            commonPrefix = commonPrefix + 1;
            return commonPrefix;
        }

    }

//    std::cout << "-- No Common Prefix Found" << std::endl;
    return 0;
}

void dawg::Minimize(int downTo){

//    std::cout << "-- Minimizing down to " << downTo << std::endl;
//    std::cout << "-- _uncheckedNodes count is: " << _uncheckedNodes.size() << std::endl;

    for (int i = _uncheckedNodes.size() - 1; i > downTo - 1; i--)
    {
        std::tuple<BasicNode*, char, BasicNode*> unNode = _uncheckedNodes.top();
        _uncheckedNodes.pop();
        BasicNode* parent = std::get<0>(unNode);
        char letter = std::get<1>(unNode);
        BasicNode* child = std::get<2>(unNode);

//        std::cout << "-- Minimizing - have parent " << parent->Id << ", letter (" << letter << ") and child " << child->Id << std::endl;
//        std::cout << "-- _minimizedNodes count is: " << _minimizedNodes.size() << std::endl;

        if(_minimizedNodes.count(child)) // (parent->Children.count(letter)) // 
        {
            std::cout << "found matching letter" << std::endl;
            std::map<BasicNode*, BasicNode*>::iterator it;
            it = _minimizedNodes.find(child);
            if (it != _minimizedNodes.end()){
//                std::cout << "-- Minimizing - found child in minimized nodes list, remapping letter" << std::endl;
//                std::cout << "-- child id = " << it->second->Id << std::endl;
                parent->Children[letter] = it->second;
            }
        }
        else
        {
            _minimizedNodes.insert(std::pair<BasicNode*, BasicNode*>(child, child));
        }
    }
}

/*
void dawg::Minimize(int downTo){

    std::cout << "-- Minimizing down to " << downTo << std::endl;
    std::cout << "-- _uncheckedNodes count is: " << _uncheckedNodes.size() << std::endl;

    for (int i = _uncheckedNodes.size() - 1; i > downTo - 1; i--)
    {
        std::tuple<BasicNode*, char, BasicNode*> unNode = _uncheckedNodes.top();
        _uncheckedNodes.pop();
        BasicNode* parent = std::get<0>(unNode);
        char letter = std::get<1>(unNode);
        BasicNode* child = std::get<2>(unNode);

        std::cout << "-- Minimizing - have parent " << parent->Id << ", letter (" << letter << ") and child " << child->Id << std::endl;
        std::cout << "-- _minimizedNodes count is: " << _minimizedNodes.size() << std::endl;

        if(_minimizedNodes.count(child)) // (parent->Children.count(letter)) // 
        {
            std::cout << "found matching letter" << std::endl;
            std::map<BasicNode*, BasicNode*>::iterator it;
            it = _minimizedNodes.find(child);
            if (it != _minimizedNodes.end()){
                std::cout << "-- Minimizing - found child in minimized nodes list, remapping letter" << std::endl;
                std::cout << "-- child id = " << it->second->Id << std::endl;
                parent->Children[letter] = it->second;
            }
        }
        else
        {
            _minimizedNodes.insert(std::pair<BasicNode*, BasicNode*>(child, child));
        }
    }
}
*/

std::vector<std::string> dawg::dfs(std::string query, BasicNode* rootNode){

//    std::cout << "-- In DFS for " << query << std::endl;

    // navigate to the point in tree where query string matches
    std::vector<std::string> results;
    BasicNode* nodeToCheck = rootNode;
//    std::cout << "-- Root node " << rootNode->Id << " with " << rootNode->Children.size() << " children" << std::endl;

    nodeToCheck = dawg::navigateToNode(rootNode, query);

    // if the characters exist do DFS from this point to capture all words with this prefix
    if(nodeToCheck != NULL){
//        std::cout << "--- Node to check was not null. Number is " << nodeToCheck->Id << std::endl;

        // print results for test
//        std::cout << "DFS Results: " << std::endl;
        printRecursion(nodeToCheck, query);
        // return results as vector
        dawg::rdfs(&results, nodeToCheck, query);
    }

    return results;
}

BasicNode* dawg::navigateToNode(BasicNode* nodeToCheck, std::string query){
    
//    std::cout << "-- Navigating to node " << std::endl;
    for (auto c : query){
        BasicNode* child = dawg::returnNodeIfChild(c, nodeToCheck);
        // follow each branch to IsTerminal point
        if (child != NULL)
        {
            nodeToCheck = child;
        }
        else
        {
            nodeToCheck = NULL;
            break;
        }
    }

    if (nodeToCheck != NULL)
        std::cout << "- Node returned for last char is " << nodeToCheck->Id << std::endl;
    else
        std::cout << "- Node is null in navigate to node return" << std::endl;
    return nodeToCheck;
}

void dawg::rdfs(std::vector<std::string>* results, BasicNode* n, std::string curString){
//    std::cout << "-- rdfs: curString " << curString << std::endl;

    if (n->Children.size() > 0) {
        
        if (n->IsTerminal){
//            std::cout << "-- rdfs terminal string : " << curString << std::endl;
            results->push_back(curString);
        }

        for(auto grandchild : n->Children){
            rdfs(results, grandchild.second, curString + grandchild.first);
        }
    }
    else {
        results->push_back(curString);
//        std::cout << "--- rdfs: no children returning string." << curString <<  " results count: " << results->size() << std::endl;
        return;
    }
}

BasicNode* dawg::returnNodeIfChild(char c, BasicNode* n){
    std::cout << "- returnNodeIfChild - searching for " << c << " with node id " << n->Id << " which has " << n->Children.size() << " children" << std::endl;
    if (n->Children.count(c)){
        std::cout << "- returnNodeIfChild returning node matching " << c << " Id is: " << n->Children[c]->Id << std::endl;
        return n->Children[c];
    }
    else{
        std::cout << "- returnNodeIfChild no result " << std::endl;
        return NULL;
    }
}

void dawg::printDawg(){
    // start at root
    BasicNode * currentNode;
    currentNode = &this->Root;
    printRecursion(currentNode, "");
}

void dawg::printRecursion(BasicNode* curNode, std::string curString){
//    std::cout << "Node " << curNode->Id << " has " << curNode->Children.size() << " children " << std::endl;
    if (curNode->Children.size() > 0){
//        std::cout << "print recursion current node: " << curNode->Id << " with children count: " << curNode->Children.size() << std::endl;

        // if this node is terminal, but has children print it (will print if has no children in below statement)
        if (curNode->IsTerminal){
            std::cout << curString + "--" << std::endl;
        }

        // do dfs printing out characters as move forward
        for(auto child : curNode->Children){
            printRecursion(child.second, curString + child.first); // BasicNode*
        }
    }
    else{
        // stop at terminal point, or when have no children and print the word
        std::cout << curString + "++" << std::endl;
    }
}