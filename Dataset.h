#ifndef DATASET_H
#define DATASET_H
#include "Column.h"
#include <iterator>
#include <algorithm>
using namespace std;
template <class T>
class Dataset{
    public:
        //constructor initializes new cols == #colNames sets their type and names
        Dataset(char attrColType, vector<string>* colNames);
        vector<Column<T>*> m_attrs;
        Column<string>* m_decision;
        //all sets of case # that match each uniqueSym in each col
        vector<set<int>*>* m_All_avBlocks;
        vector<string>* finalRules;
        vector<string>* finalRulesBBValues;
        //adds row to dataset: vector passed == width of dataset
        void addRow(vector<string>* newRow);
        void printDataset();
        //LEM2 & utility functions
        void LEM2();
        void populateAllUniques();
        void populateAllAVBlocks();
        
        int selectBestIndex(vector<set<int>*>* inters, vector<int>* sizes);
        bool isSubset(set<int>* lhs, set<int>* rhs);
        set<int>* newGoal(set<int>* rule, set<int>* goal);
        string prodRuleStr(vector<string>* avName, vector<int>* avIndex, string concept);
        //initilization functions for LEM2
        void init_BlockNames_OriginAVCardinality(vector<int>* originAVCardinality, vector<string>* blockNames);
        void init_ConceptNames(vector<string>* conceptNames);
        //print functions
        void printSet(set<int>* inSet);
        void printAllAVBlocks();
        void printAllUniqueSyms();
        void printRules();
        void printRulesToFile(std::ofstream& out);
};
#include "Dataset.cpp"
#endif