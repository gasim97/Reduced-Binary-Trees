//
//  main.cpp
//  ReducedDataStructures2
//
//  Created by Gasim Azhari on 02/04/2018.
//  Copyright © 2018 Gasim Gasim. All rights reserved.
//

#include <iostream>
#include <vector>
#include <time.h>
#include <cmath>

struct bdnode{
    std::string val;
    // this will contain:
    // "x1", "x2"... "xn" in non-leaf nodes
    // "0" or "1" in leaf nodes
    bdnode* left;
    // this will conventionally represent the 0 branch
    bdnode* right;
    // this will conventionally represent the 1 branch
    // (as usual leaf nodes will have both left and right pointing to NULL)
};
typedef bdnode* bdt;

bdt buildcompactbdt(const std::vector<std::string>& fvalues);
// builds a data structure

bdt buildcompact(bdt root, std::vector<std::string> fvalues, int varNumber, int numVariables, int numSolutions);
// builds the data structure recurseively

bdt reducebdt(bdt root);
// reduces the data structure recurseively

bool comparebdt(bdt rootA, bdt rootB);
// returns true if data structures A and B are equal

bdt deletebdt(bdt root);
// deletes a data structure recusively

std::string evalcompactbdt(bdt t, const std::string& input);
// evaluates a function represented as the data structure

bdt newNode(std::string val);
// instantiates a new node

void verifybdt(int length, std::string inputCase, bdt fbdt, std::vector<std::string> fvalues, std::vector<std::string>& errors);
// uses evalcompactbdt to evaluate a data structure for all possible inputs and prints a truth table

int countbdtnodes(bdt t);
// counts the number of nodes in a data structure

void printTree(bdt hdTree);
void depthCount(int &depthMax,int depth, bdt t);

int main(int argc, const char * argv[]) {
    
    std::vector<std::string> input;
    
    std::cout << "Please enter the number of true outputs for the structure: ";
    int numSolutions;
    std::cin >> numSolutions;
    
    std::string trueCase;
    int numTerms;
    if (numSolutions > 0)
    {
        std::cout << std::endl << "Please enter a binary input that corresponds to true output 1: ";
        std::cin >> trueCase;
        input.push_back(trueCase);
        numTerms = trueCase.length();
    }
    
    int inputSize = 0;
    bool unique;
    for (int i = 1; i < numSolutions; i++)
    {
        do
        {
            unique = true;
            std::cout << "Please enter a(n) " + std::to_string(numTerms) + "-digit binary input that corresponds to true output " + std::to_string(i+1) + ": ";
            std::cin >> trueCase;
            for (int i = 0; i <= inputSize; i++)
            {
                if (trueCase == input[i])
                {
                    unique = false;
                    i = inputSize;
                }
            }
        }while(trueCase.length() != numTerms || !unique);
        input.push_back(trueCase);
        inputSize++;
    }
    clock_t t1 = clock();
    bdt fbdt = buildcompactbdt(input);
    clock_t t2 = clock();
    float diff ((float)t2-(float)t1);
    
    std::string inputCase;
    std::vector<std::string> errors;
    std::cout << std::endl << "Tree Evaluation Truth Table:" << std::endl << std::endl;
    verifybdt(numTerms, inputCase, fbdt, input, errors);
    int count = countbdtnodes(fbdt);
    int numErrors = errors.size();
    if (numErrors != 0)
    {
        std::cout << std::endl << "An output error was detected for the following inputs: " << std::endl;
        for (int i = 0; i < numErrors; i++)
        {
            std::cout << errors[i] << std::endl;
        }
    }
    else
    {
        std::cout << std::endl << "No output errors detected" << std::endl;
    }
    std::cout << std::endl << "Number of Nodes = " << count << std::endl << std::endl;
    std::cout << "Build Time = " << diff/CLOCKS_PER_SEC << std::endl << std::endl;
    std::cout << "Visualisation of Tree:" << std::endl << std::endl;
    printTree(fbdt);
    
    return 0;
}

bdt buildcompactbdt(const std::vector<std::string>& fvalues)
{
    int numSolutions = fvalues.size();
    if (numSolutions == 0)
    {
        return nullptr;
    }
    int numVariables = fvalues[0].length();
    bdt root = newNode("0");
    if (numSolutions == pow(2, numVariables))
    {
        root->val = "1";
        return root;
    }
    root = buildcompact(root, fvalues, 1, numVariables, numSolutions);
    root = reducebdt(root);
    return root;
}

bdt buildcompact(bdt root, std::vector<std::string> fvalues, int varNumber, int numVariables, int numSolutions)
{
    std::vector<std::string> fvaluesZero;
    std::vector<std::string> fvaluesOne;
    int zeroCount = 0;
    for (int i = 0; i < numSolutions; i++)
    {
        if (fvalues[i][varNumber-1] == '0')
        {
            fvaluesZero.push_back(fvalues[i]);
            zeroCount++;
        }
        else
        {
            fvaluesOne.push_back(fvalues[i]);
        }
    }
    root->val = "x" + std::to_string(varNumber);
    if (varNumber == numVariables)
    {
        if (zeroCount != 0 && zeroCount != numSolutions)
        {
            root->val = "1";
        }
        else if (zeroCount != 0)
        {
            root->left = newNode("1");
            root->right = newNode("0");
        }
        else
        {
            root->left = newNode("0");
            root->right = newNode("1");
        }
    }
    else
    {
        if (zeroCount != 0 && zeroCount != numSolutions)
        {
            root->left = newNode("Unassigned");
            root->left = buildcompact(root->left, fvaluesZero, varNumber+1, numVariables, zeroCount);
            root->right = newNode("Unassigned");
            root->right = buildcompact(root->right, fvaluesOne, varNumber+1, numVariables, numSolutions-zeroCount);
        }
        else if (zeroCount != 0)
        {
            root->right = newNode("0");
            root->left = newNode("Unassigned");
            root->left = buildcompact(root->left, fvaluesZero, varNumber+1, numVariables, zeroCount);
        }
        else
        {
            root->left = newNode("0");
            root->right = newNode("Unassigned");
            root->right = buildcompact(root->right, fvaluesOne, varNumber+1, numVariables, numSolutions-zeroCount);
        }
    }
    
    return root;
}

bdt reducebdt(bdt root)
{
    if (root->val.length() == 1)
    {
        return root;
    }
    root->left = reducebdt(root->left);
    root->right = reducebdt(root->right);
    if (comparebdt(root->left, root->right))
    {
        bdt tmpbdtA = root->left;
        bdt tmpbdtB = root->right;
        root->val = root->left->val;
        root->left = tmpbdtA->left;
        root->right = tmpbdtA->right;
        tmpbdtB = deletebdt(tmpbdtB);
        delete tmpbdtA;
    }
    return root;
}

bool comparebdt(bdt rootA, bdt rootB)
{
    if (rootA == NULL && rootB == NULL)
    {
        return true;
    }
    if (rootA == NULL || rootB == NULL)
    {
        return false;
    }
    return (rootA->val == rootB->val && comparebdt(rootA->right, rootB->right) && comparebdt(rootA->left, rootB->left));
}

bdt deletebdt(bdt root)
{
    if (root->left == NULL)
    {
        delete root;
        return NULL;
    }
    root->left = deletebdt(root->left);
    root->right = deletebdt(root->right);
    delete root;
    return NULL;
}

std::string evalcompactbdt(bdt t, const std::string& input)
{
    if (t == NULL)
    {
        return "Solution Could Not Be Found.";
    }
    if (t->val == "0" || t->val == "1")
    {
        return t->val;
    }
    else
    {
        if (input[stoi(t->val.substr(1, t->val.size()-1))-1] == '0')
        {
            return evalcompactbdt(t->left, input);
        }
        else
        {
            return evalcompactbdt(t->right, input);
        }
    }
}

bdt newNode(std::string val)
{
    bdt newPtr = new bdnode;
    newPtr->val = val;
    newPtr->right = NULL;
    newPtr->left = NULL;
    return newPtr;
}

void verifybdt(int length, std::string inputCase, bdt t, std::vector<std::string> fvalues, std::vector<std::string>& errors)
{
    if (t == NULL)
    {
        std::cout << "NULL Structure" << std::endl;
    }
    else if (length == 0)
    {
        if (inputCase.empty())
        {
            std::cout << "ALL INPUTS| " + evalcompactbdt(t, "0") << std::endl;
        }
        else
        {
            int numfvalues = fvalues.size();
            bool error;
            std::string tmp = evalcompactbdt(t, inputCase);
            if (tmp == "1")
            {
                error = true;
                for (int i = 0; i < numfvalues; i++)
                {
                    if (inputCase == fvalues[i])
                    {
                        error = false;
                        fvalues[i].erase();
                        i = numfvalues;
                    }
                }
            }
            else
            {
                error = false;
                for (int i = 0; i < numfvalues; i++)
                {
                    if (inputCase == fvalues[i])
                    {
                        error = true;
                        i = numfvalues;
                    }
                }
            }
            if (error)
            {
                errors.push_back(inputCase);
            }
            std::cout << inputCase + "| " + tmp << std::endl;
        }
    }
    else
    {
        verifybdt(length-1, inputCase + "0", t, fvalues, errors);
        verifybdt(length-1, inputCase + "1", t, fvalues, errors);
    }
}

int countbdtnodes(bdt t)
{
    if (t == NULL)
    {
        return 0;
    }
    else
    {
        return 1 + countbdtnodes(t->right) + countbdtnodes(t->left);
    }
}

void printTree(bdt hdTree){ //FOR TESTING ONLY
    int depthmax = 0;
    depthCount(depthmax,-1, hdTree);
    
    int gapCounter = 0;
    while (gapCounter < pow(2,depthmax)-1){
        std::cout<<"  ";
        gapCounter++;
    }
    std::cout << hdTree->val << std::endl << std::endl;
    
    int counter = 1;
    int counter2 = 0;
    int counter3 = 0;
    int max = 0;
    bool isNull = false;
    bdt temp = hdTree;
    std::string binary;
    std::string binary2;
    while (counter <= depthmax){
        max = pow(2,counter);
        counter2 = 0;
        while (counter2 < max){
            binary = std::bitset<64>(counter2).to_string();
            counter3 = 0;
            binary2 = "";
            while (counter3 < counter){
                binary2 = binary2 + binary[binary.size()-counter+counter3];
                counter3++;
            }
            
            temp = hdTree;
            isNull = false;
            
            for (int i = 0; i < binary2.size(); i++){
                if (binary2[i] == '0'){
                    if (temp->left != NULL){
                        temp = temp -> left;
                    }else{
                        isNull = true;
                    }
                    
                }else{
                    if (temp->right != NULL){
                        temp = temp -> right;
                    }else{
                        isNull = true;
                    }
                }
            }
            
            gapCounter = 0;
            if (counter2 == 0){
                while (gapCounter < pow(2,depthmax-counter)-1){
                    std::cout<<"  ";
                    gapCounter++;
                }
            }else{
                while (gapCounter < pow(2,depthmax-counter+1)-1){
                    std::cout<<"  ";
                    gapCounter++;
                }
            }
            
            if (isNull){
                std::cout << "  ";
            }else if (temp->val.size() == 1){
                std::cout<<" "<<temp->val;
            }else{
                std::cout<<temp->val;
            }
            
            counter2++;
        }
        counter++;
        std::cout << std::endl << std::endl;
    }
}

void depthCount(int &depthMax,int depth, bdt t){
    depth++;
    if (t->left != NULL){
        depthCount(depthMax,depth, t->left);
    }
    if (t->right != NULL){
        depthCount(depthMax,depth, t->right);
    }
    if (depthMax < depth) depthMax = depth;
}
