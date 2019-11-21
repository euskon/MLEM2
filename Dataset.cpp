#include "Dataset.h"

template <class T>
Dataset<T>::Dataset(char attrColType, vector<string>* colNames){
    vector<string>::iterator lastElem = --colNames->end();

    for(vector<string>::iterator it = colNames->begin(); it != colNames->end(); ++it){
        if(it != lastElem){
            m_attrs.push_back(new Column<T>(attrColType, *it));
        }
        else{
            m_decision = new Column<string>('s', *it);
        }
    }
    m_All_avBlocks = new vector<set<int>*>();
    finalRules = new vector<string>();
    finalRulesBBValues = new vector<string>();
}
template <>
void Dataset<float>::addRow(vector<string>* newRow){
    vector<string>::iterator rowItr = newRow->begin();
    for(vector<Column<float>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
        cout << "attr entry: " << *rowItr << "\n";
        (*it)->addValue(stof(*rowItr));
        ++rowItr;
    }
    cout << "decision entry: " << *rowItr << "\n";
    m_decision->addValue(*rowItr);
}
template <>
void Dataset<string>::addRow(vector<string>* newRow){
    vector<string>::iterator rowItr = newRow->begin();
    for(vector<Column<string>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){  
        cout << "attr entry: " << *rowItr << "\n";
        (*it)->addValue(*rowItr);
        ++rowItr;
    }
    cout << "decision entry: " << *rowItr << "\n";
    m_decision->addValue(*rowItr);
}
// template <class T>
// void Dataset<T>::print(){
//     int caseNum = 0;
//     vector<vector<float>::iterator > attrColItrs;
//     for(vector<Column<float>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
//         attrColItrs.push_back((*it)->m_values->begin());
//     }
//     for(vector<string>::iterator it = (m_decision->m_values)->begin(); it != m_decision->m_values->end(); ++it){
//         cout << caseNum << ": ";
//         for(vector<vector<float>::iterator>::iterator attr = attrColItrs.begin(); attr != attrColItrs.end(); ++attr){
//             cout << **attr << " ";
//             ++(*attr);
//         }
//         cout << *it << "\n";
//         caseNum++;
//     }
// }
template <>
void Dataset<float>::printDataset(){
    int caseNum = 0;
    vector<vector<float>::iterator > attrColItrs;
    for(vector<Column<float>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
        attrColItrs.push_back((*it)->m_values->begin());
    }
    for(vector<string>::iterator it = (m_decision->m_values)->begin(); it != m_decision->m_values->end(); ++it){
        cout << caseNum << ": ";
        for(vector<vector<float>::iterator>::iterator attr = attrColItrs.begin(); attr != attrColItrs.end(); ++attr){
            cout << **attr << " ";
            ++(*attr);
        }
        cout << *it << "\n";
        caseNum++;
    }
}
template <>
void Dataset<string>::printDataset(){
    int caseNum = 0;
    vector<vector<string>::iterator> attrColItrs;
    for(vector<Column<string>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
        attrColItrs.push_back((*it)->m_values->begin());
    }
    for(vector<string>::iterator it = (m_decision->m_values)->begin(); it != m_decision->m_values->end(); ++it){
        cout << caseNum << ": ";
        for(vector<vector<string>::iterator>::iterator attr = attrColItrs.begin(); attr != attrColItrs.end(); ++attr){
            cout << **attr << " ";
            ++(*attr);
        }
        cout << *it << "\n";
        caseNum++;
    }
}
template <class T>
void Dataset<T>::populateAllUniques(){
    int colIndex = 0;
    for(typename vector<Column<T>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
        cout << "attr:col#" << colIndex << "\n";
        (*it)->populateUniques();
        colIndex++;
    }
    cout << "decision:col#" << colIndex << "\n";
    m_decision->m_colType = 'd';
    m_decision->populateUniques();
    m_decision->m_colType = 's';
}
template <class T>
void Dataset<T>::populateAllAVBlocks(){
    int colIndex = 0;
    for(typename vector<Column<T>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
        (*it)->populateAVBlocks();
        colIndex++;
    }
    m_decision->populateAVBlocks();
}

template <class T>
void Dataset<T>::printAllAVBlocks(){
    cout << "\nAVBlocks: \n(a,v) = [(a,v)]\n";
    for(typename vector<Column<T>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
        (*it)->printAVBlocks();
    }
    cout << "Decision Blocks: \n";
    m_decision->printAVBlocks();
}
template <class T>
void Dataset<T>::printAllUniqueSyms(){
    cout << "Printing All Unique Symbols...\n";
    for(typename vector<Column<T>*>::iterator it = m_attrs.begin(); it != m_attrs.end(); ++it){
        (*it)->printUniqueSym();
    }
}
template <class T>
void Dataset<T>::LEM2(){
    cout << "\nBegin LEM2\n";
    
    //current goal intersected with AVBlocks
    vector<set<int>*>* intersectedBlocks = new vector<set<int>*>();
    //size of each original AVBlock before intersection for reference during selection
    vector<int>* originAVCardinality = new vector<int>();
    //(a,v) printable strings
    vector<string>* blockNames = new vector<string>();
    //(d,v) printable strings
    vector<string>* conceptNames = new vector<string>();
    //current rules' set after intersection with each selected AV
    set<int>* ruleCoverBlock = nullptr;
    //indexes of selected AVBlocks for current rule being generated
    vector<int>* indexesSelected = new vector<int>();
    int currSelection;
    int conceptIndex = 0;

    //initializing blockNames and originAVCardinality
    init_BlockNames_OriginAVCardinality(originAVCardinality, blockNames);
    //initializing conceptNames
    init_ConceptNames(conceptNames);
    
    //START: looping through concepts
    for(vector<set<int>*>::iterator concept = m_decision->m_avBlocks->begin(); concept != m_decision->m_avBlocks->end(); ++concept){
        set<int>* goal = new set<int>(**concept);
        set<int>* main_goal = new set<int>(**concept);
        bool newIntersectionNeeded = true;

        cout << "\n\nNEXT------------------------CONCEPT--------------------------------->\n";
        while(!main_goal->empty()){   //!goal->empty()     
            //STEP1: looping through all AVBlocks of all cols
            // if(newIntersectionNeeded){
            //     // for(typename vector<Column<T>*>::iterator attrCol = m_attrs.begin(); attrCol != m_attrs.end(); ++attrCol){
            //     //     //looping through blocks in current col
            //     //     vector<set<int>*>* currColBlocks = (*attrCol)->m_avBlocks;
            //     //     for(vector<set<int>*>::iterator block = currColBlocks->begin(); block != currColBlocks->end(); ++block){
            //     //         set<int>* result = new set<int>();
            //     //         //intersect curr block with goal
            //     //         set_intersection((*block)->begin(),(*block)->end(),goal->begin(),goal->end(), inserter(*result,result->begin()));
            //     //         intersectedBlocks->push_back(result);
            //     //     }
            //     // }
            //     for(vector<set<int>*>::iterator block = m_All_avBlocks->begin(); block != m_All_avBlocks->end(); ++block){
            //         set<int>* result = new set<int>();
            //         set_intersection((*block)->begin(),(*block)->end(),goal->begin(),goal->end(), inserter(*result,result->begin()));
            //         intersectedBlocks->push_back(result);
            //     }
            // }
            //print step1
            // int blockNum = 1;
            // vector<set<int>*>::iterator avBlock = m_All_avBlocks->begin();
            // for(vector<set<int>*>::iterator block = intersectedBlocks->begin(); block != intersectedBlocks->end(); ++block){
            //     cout << "block#" << blockNum << ": ";
            //     printSet(*avBlock);
            //     cout << "              | ";
            //     printSet(*block);
            //     cout << "\n";
            //     ++avBlock;
            //     blockNum++;
            // }
            //inner while
            for(vector<set<int>*>::iterator block = m_All_avBlocks->begin(); block != m_All_avBlocks->end(); ++block){
                set<int>* result = new set<int>();
                set_intersection((*block)->begin(),(*block)->end(),main_goal->begin(),main_goal->end(), inserter(*result,result->begin()));
                intersectedBlocks->push_back(result);
            }
            while(indexesSelected->empty() || !isSubset(ruleCoverBlock, (* concept))){
                // cout << "\n\nINNER LOOP BEGIN: concept\n\t";
                // printSet(*concept);
                // cout << "INNER LOOP BEGIN: main goal\n\t";
                // printSet(main_goal);
                // cout << "INNER LOOP BEGIN: initial goal\n\t";
                // printSet(goal);
                // cout << "INNER LOOP BEGIN: covered by rule in progress\n\t";
                // if(ruleCoverBlock != nullptr){ printSet(ruleCoverBlock); }
                // else{ cout << "{empty}\n"; }
                // cout << "\n\n";
                //STEP2: choose best entry in column
                currSelection = selectBestIndex(intersectedBlocks, originAVCardinality);
                //STEP3: save the vector index of the cases used in the current rule
                indexesSelected->push_back(currSelection);
                if(ruleCoverBlock == nullptr){//update rule cover with selection
                    //result of first column of rule generation filling current rule cover with original AVBlock set
                    ruleCoverBlock = new set<int>(*(m_All_avBlocks->at(currSelection)));
                    //cout << "ruleCover due to FIRST selection: selected block=" << currSelection << "\n\t";
                    printSet(ruleCoverBlock);
                }
                else{
                    //result following columns of rule being generated intersecting rule cover with current selection 
                    set<int>* newRuleCover = new set<int>();
                    set_intersection(m_All_avBlocks->at(currSelection)->begin(),m_All_avBlocks->at(currSelection)->end(),ruleCoverBlock->begin(),ruleCoverBlock->end(), inserter(*newRuleCover,newRuleCover->begin()));
                    if(!newRuleCover->empty()){
                        delete ruleCoverBlock;
                        ruleCoverBlock = new set<int>(*newRuleCover);
                        delete newRuleCover;
                    }
                    //cout << "ruleCover after adding new selection: selected block=" << currSelection << "\n\t";
                    printSet(ruleCoverBlock);
                }
                //STEP4: created subgoal, although usually will be same
                set<int>* subGoal = new set<int>();
                //cout << "goal size: " << goal->size() << "\nrule size: " << ruleCoverBlock->size() << "\n";
                set_intersection(goal->begin(), goal->end(), m_All_avBlocks->at(currSelection)->begin(), m_All_avBlocks->at(currSelection)->end(), inserter(*subGoal, subGoal->begin()));
                
                //STEP5: re-intersect the subgoal with the AVBlocks and fill new column of LEM2 table
                if(*goal != *subGoal && !subGoal->empty()){ 
                    delete goal;
                    goal = new set<int>(*subGoal);
                    delete subGoal;
                    for(vector<set<int>*>::iterator block = intersectedBlocks->begin(); block != intersectedBlocks->end(); ++block){
                        (*block)->clear();
                    }
                    intersectedBlocks->clear();                   
                    for(vector<set<int>*>::iterator block = m_All_avBlocks->begin(); block != m_All_avBlocks->end(); ++block){
                        set<int>* result = new set<int>();
                        set_intersection((*block)->begin(),(*block)->end(),goal->begin(),goal->end(), inserter(*result,result->begin()));
                        intersectedBlocks->push_back(result);
                    }
                }
                //STEP6: Clear row entry that was selected this iteration(i.e. put dash in row box already used)
                for(vector<int>::iterator index = indexesSelected->begin(); index != indexesSelected->end(); ++index){
                    set<int>* lemColBlock = intersectedBlocks->at(*index);
                    if(!lemColBlock->empty()){
                        lemColBlock->clear();
                    }
                }
                // cout << "\n\nINNER LOOP END: concept\n\t";
                // printSet(*concept);
                // cout << "INNER LOOP END: main goal\n\t";
                // printSet(main_goal);
                // cout << "INNER LOOP END: initial goal\n\t";
                // printSet(goal);
                // cout << "INNER LOOP END: covered by rule in progress\n\t";
                // printSet(ruleCoverBlock);
                // cout << "\n\n";

            }
            // cout << "OUTER LOOP END: cases covered by rule produced\n\t";
            //     printSet(ruleCoverBlock);
            //STEP7: Save generated rule in final rule set container 
            finalRules->push_back(prodRuleStr(blockNames, indexesSelected, conceptNames->at(conceptIndex)));
            finalRulesBBValues->push_back("("+to_string(indexesSelected->size())+","+to_string(ruleCoverBlock->size())+","+to_string(ruleCoverBlock->size())+")");
            //STEP8: UPDATE MAIN GOAL
            set<int>* res = new set<int>();
            cout << "\n";
            //cout << "goal size: " << goal->size() << "\nrule size: " << ruleCoverBlock->size() << "\n";
            set_difference(main_goal->begin(), main_goal->end(), ruleCoverBlock->begin(), ruleCoverBlock->end(), inserter(*res, res->begin()));
            delete main_goal;
            main_goal = new set<int>(*res);
            delete goal;
            goal = new set<int>(*main_goal);
            //cout << "NEXT GOAL WITHIN CONCEPT: ";
            //printSet(goal);

            //RESET FOR NEW RULE
            for(vector<set<int>*>::iterator block = intersectedBlocks->begin(); block != intersectedBlocks->end(); ++block){
                (*block)->clear();
            }
            intersectedBlocks->clear();
            indexesSelected->clear();
            delete ruleCoverBlock;
            ruleCoverBlock = nullptr;

            
            // //update cover with selection
            // if(ruleCoverBlock == nullptr){
            //     //result of first column of rule generation filling current rule cover with original AVBlock set
            //     ruleCoverBlock = new set<int>(*(m_All_avBlocks->at(currSelection)));
            //     cout << "ruleCover due to FIRST selection: selected block=" << currSelection << "\n";
            //     printSet(ruleCoverBlock);
            // }
            // else{
            //     //result following columns of rule being generated intersecting rule cover with current selection 
            //     set<int>* newRuleCover = new set<int>();
            //     set_intersection(m_All_avBlocks->at(currSelection)->begin(),m_All_avBlocks->at(currSelection)->end(),ruleCoverBlock->begin(),ruleCoverBlock->end(), inserter(*newRuleCover,newRuleCover->begin()));
            //     if(!newRuleCover->empty()){
            //         delete ruleCoverBlock;
            //         ruleCoverBlock = newRuleCover;
            //     }
            //     cout << "ruleCover after adding new selection: selected block=" << currSelection << "\n";
            //     printSet(ruleCoverBlock);
            // }
            
            // //check if rule is subset of concept, if so its finished
            // if(isSubset(ruleCoverBlock, (* concept))){
            //     finalRules->push_back(prodRuleStr(blockNames, indexesSelected, conceptNames->at(conceptIndex)));
            //     finalRulesBBValues->push_back("("+to_string(indexesSelected->size())+","+to_string(ruleCoverBlock->size())+","+to_string(ruleCoverBlock->size())+")");
            //     set<int>* res = new set<int>();
            //     cout << "goal size: " << goal->size() << "\nrule size: " << ruleCoverBlock->size() << "\n";
            //     set_difference(goal->begin(), goal->end(), ruleCoverBlock->begin(), ruleCoverBlock->end(), inserter(*res, res->begin()));
            //     delete goal;
            //     goal = res;
            //     cout << "\nNEW GOAL:\n";
            //     if(goal != nullptr){
            //         printSet(goal);
            //     }
            //     else{cout << "goal is nullptr\n";}
            //     cout << "\n\nNEXT--------------------------------------------------------->\n";
            //     for(vector<set<int>*>::iterator block = intersectedBlocks->begin(); block != intersectedBlocks->end(); ++block){
            //         (*block)->clear();
            //         //delete *block;
            //     }
            //     intersectedBlocks->clear();
            //     indexesSelected->clear();
            //     delete ruleCoverBlock;
            //     ruleCoverBlock = nullptr;//delete and set to nullptr so the next iteration inits ruleCoverBlock with first currSelection AVBlock
            //     newIntersectionNeeded = true;
            // }//continue generating unfinished rule
            // else{
            //     cout << "\nCover in progress: ";
            //     printSet(ruleCoverBlock);
            //     cout << "GOAL IN PROGRESS: ";
            //     printSet(goal);
            //     set<int>* subGoal = new set<int>();
            //     cout << "goal size: " << goal->size() << "\nrule size: " << ruleCoverBlock->size() << "\n";
            //     set_intersection(goal->begin(), goal->end(), m_All_avBlocks->at(currSelection)->begin(), m_All_avBlocks->at(currSelection)->end(), inserter(*subGoal, subGoal->begin()));
            //     delete goal;
            //     goal = subGoal;
            //     //LEM2 table index that was selected while generating current SINGLE rule is cleared(i.e. put dash in row box already used)
            //     for(vector<int>::iterator index = indexesSelected->begin(); index != indexesSelected->end(); ++index){
            //         set<int>* lemColBlock = intersectedBlocks->at(*index);
            //         if(!lemColBlock->empty()){
            //             lemColBlock->clear();
            //         }
            //     }
            //     newIntersectionNeeded = false;
            // }
        }
        cout << "GOAL WHEN MOVING ONTO NEXT CONCEPT: ";
        printSet(goal);
        cout << "\n";
        conceptIndex++;
    }  
}

// #include <iostream>     // std::cout
// #include <algorithm>    // std::set_difference, std::sort
// #include <vector>       // std::vector
// #include <set>
// #include <iterator>
// using namespace std;
// void printSet(set<int>* inSet){
//     cout << "{";
//     for(set<int>::iterator singleCase = inSet->begin(); singleCase != inSet->end(); ++singleCase){
//             if(*singleCase != *(inSet->rbegin())){
//                     cout << *singleCase << ", ";
//             }
//             else{
//                     cout << *singleCase;
//             }
//     }
//     cout << "}\n";
// }
// int main () {
//   int ruleArr[] = {3, 7, 9, 17, 24, 26, 36, 39, 44, 52, 61, 75, 79, 100, 104, 107, 140, 189, 197, 200, 202, 205, 212, 273};
//   int goalArr[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 149, 150, 151, 152, 153, 154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 165, 166, 167, 168, 169, 170, 171, 172, 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 196};

//  set<int>* rule = new set<int>(ruleArr, ruleArr+23);
//  set<int>* goal = new set<int>(goalArr, goalArr+195);

//     set<int>* copyGoal = new set<int>(*goal);
//     set<int>* res = new set<int>();
//     set_difference(copyGoal->begin(), copyGoal->end(), rule->begin(), rule->end(), inserter(*res, res->begin()));

//   printSet(res);

//   return 0;
// }
template <class T>
int Dataset<T>::selectBestIndex(vector<set<int>*>* inters, vector<int>* sizes){
    vector<set<int>*>::iterator interBlock = inters->begin();
    vector<int>::iterator sizeOfAV = sizes->begin();
    int vectIndex = 0;
    int max_vectIndex = 0;
    int min_AV = *sizeOfAV;
    int max_Inter = (*interBlock)->size();
    while(interBlock != inters->end() && sizeOfAV != sizes->end()){
        if((*interBlock)->size() > max_Inter){
            max_Inter = (*interBlock)->size();
            max_vectIndex = vectIndex;
            min_AV = *sizeOfAV;
        }
        else if((*interBlock)->size() == max_Inter){
            if(*sizeOfAV < min_AV){
                max_vectIndex = vectIndex;
                min_AV = *sizeOfAV;
            }
        }
        ++interBlock;
        ++sizeOfAV;
        vectIndex++;
    }
    return max_vectIndex;
    //Are you selecting the best index via the 3 focal ideas: relevance, 
}
template <class T>
bool Dataset<T>::isSubset(set<int>* lhs, set<int>* rhs){
    set<int> res;
    set_difference(lhs->begin(), lhs->end(), rhs->begin(), rhs->end(),inserter(res, res.begin()));
    if(res.empty()){
        return true;
    }
    return false;
}
template <class T>
set<int>* Dataset<T>::newGoal(set<int>* rule, set<int>* goal){
    set<int>* res = new set<int>();
    set<int> copyGoal(*goal);
    printSet(&copyGoal);
    set<int> copyRule(*rule);
    printSet(rule);
    set_difference(copyGoal.begin(), copyGoal.end(), copyRule.begin(), copyRule.end(), inserter(*res, res->begin()));
    cout << "\nCALCING NEW GOAL:\n";
    printSet(res);
    return res;
}
template <class T>
string Dataset<T>::prodRuleStr(vector<string>* avNames, vector<int>* avIndex, string concept){
    string rule;
    for(vector<int>::iterator index = avIndex->begin(); index != avIndex->end(); ++index){
        if(index == avIndex->begin()){
            rule = avNames->at(*index);
        }
        else{
            rule = rule + "&" + avNames->at(*index);
        }
    }
    rule = rule + "->" + concept;
    cout << "Rule produced: " << rule << "\n";
    return rule;
}
template <class T>
void Dataset<T>::printSet(set<int>* inSet){
    cout << "{";
    for(set<int>::iterator singleCase = inSet->begin(); singleCase != inSet->end(); ++singleCase){
            if(*singleCase != *(inSet->rbegin())){
                    cout << *singleCase << ", ";
            }
            else{
                    cout << *singleCase;
            }
    }
    cout << "}\n";
}

template <class T>
void Dataset<T>::init_BlockNames_OriginAVCardinality(vector<int>* originAVCardinality, vector<string>* blockNames){
    for(typename vector<Column<T>*>::iterator attrCol = m_attrs.begin(); attrCol != m_attrs.end(); ++attrCol){
        set<string>::iterator uniq = (*attrCol)->m_uniqueSym->begin();
        vector<set<int>*>::iterator avBlock = (*attrCol)->m_avBlocks->begin();
        while(uniq != (*attrCol)->m_uniqueSym->end() && avBlock != (*attrCol)->m_avBlocks->end()){
            blockNames->push_back("(" + (*attrCol)->m_name + "," + *uniq + ")");
            originAVCardinality->push_back((*avBlock)->size());
            m_All_avBlocks->push_back(*avBlock);
            ++uniq;
            ++avBlock;
        }
    }
}
template <class T>
void Dataset<T>::init_ConceptNames(vector<string>* conceptNames){
    for(set<string>::iterator decStr = m_decision->m_uniqueSym->begin(); decStr != m_decision->m_uniqueSym->end(); ++decStr){
        conceptNames->push_back("(" + m_decision->m_name + "," + *decStr + ")");
    }
}
template <class T>
void Dataset<T>::printRules(){
    cout << "\nFINAL RULES: \n";
    vector<string>::iterator rule = finalRules->begin();
    vector<string>::iterator BBTuple = finalRulesBBValues->begin();
    while(rule != finalRules->end() && BBTuple != finalRulesBBValues->end()){
        cout << *BBTuple << "\n" << *rule << "\n\n";
        ++rule;
        ++BBTuple;
    }
}
template <class T>
void Dataset<T>::printRulesToFile(std::ofstream& out){
    vector<string>::iterator rule = finalRules->begin();
    vector<string>::iterator BBTuple = finalRulesBBValues->begin();
    while(rule != finalRules->end() && BBTuple != finalRulesBBValues->end()){
        out << *BBTuple << "\n" << *rule << "\n\n";
        ++rule;
        ++BBTuple;
    }
}