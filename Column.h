#ifndef COLUMN_H
#define COLUMN_H
#include <iostream>
#include <set>
#include <iterator>
#include <string>
#include <utility>
#include <vector>
using namespace std;
template <class T>
class Column{
    public:
        //s = string, f = float
        char m_colType;
        string m_name;
        set<T>* m_uniqueVals;
        set<float>* f_uniqueVals;
        vector<T>* m_values;
        //LEM2 part: attr & decision cols - set of cases that match each uniqueSym
        vector<set<int>*>* m_avBlocks;
        /*(string) - if str column m_uniqueVals == m_uniqueSym
          (float) - symbolic counter parts after discretization 
                    in numerical data*/
        set<string>* m_uniqueSym;
        //Constructor for cols 2-parameter edition
        Column(char colType, string name);
        //Insert new value into m_values
        void addValue(T val);
        void populateUniques();
        void populateAVBlocks();

        void printAVBlocks();
        void printUniqueSym();

        bool isSymbolic(string input);
};
#include "Column.cpp"
#endif