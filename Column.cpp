#include "Column.h"

template <class T>
Column<T>::Column(char colType, string name){
        m_colType = colType;
        m_name = name;
        m_uniqueVals = new set<T>();
        m_values = new vector<T>();
        m_avBlocks = new vector<set<int>*>();

        f_uniqueVals = nullptr;
}
template <class T>
void Column<T>::addValue(T val){
        m_values->push_back(val);
}
template <>
void Column<float>::populateUniques(){
        pair<set<float>::iterator, bool> ret;
        for(vector<float>::iterator it = m_values->begin(); it != m_values->end(); ++it){
                ret = m_uniqueVals->insert(*it);
                if(ret.second){
                        cout << "\tinserted: " << *it << "\n";
                }
        }
        cout << "Finished populating Uniques before discretization...\n";
        cout << "Discretization:\n";
        //discretization: creates cutpoint and builds m_uniqueSym 
        //                based on m_uniqueVals and the cutpoints
        set<float>::iterator it = m_uniqueVals->begin();
        set<float>::iterator itPlusOne = ++m_uniqueVals->begin();
        set<float>::iterator beg = m_uniqueVals->begin();
        set<float>::reverse_iterator end = m_uniqueVals->rbegin();
        set<string>* intervals = new set<string>();
        while(itPlusOne != m_uniqueVals->end()){
                float avg = (*it + *itPlusOne)/2;
                intervals->insert(to_string(*beg) + ".." + to_string(avg));
                cout << "\tinserted: " << to_string(*beg) + ".." + to_string(avg) << "\n";
                intervals->insert(to_string(avg) + ".." + to_string(*end));
                cout << "\tinserted: " << to_string(avg) + ".." + to_string(*end) << "\n";
                ++it;
                ++itPlusOne;
        }
        cout << "Finished Discretization.\n";
        m_uniqueSym = intervals;
}
template <>
void Column<string>::populateUniques(){
        if(isSymbolic(*m_values->begin()) || m_colType == 'd'){
                pair<set<string>::iterator, bool> ret;
                for(vector<string>::iterator it = m_values->begin(); it != m_values->end(); ++it){
                        ret = m_uniqueVals->insert(*it);
                        if(ret.second){
                                cout << "\tinserted: " << *it << "\n";
                        }
                }
                m_uniqueSym = m_uniqueVals;
        }
        else{
                m_colType = 'f';
                f_uniqueVals = new set<float>();
                pair<set<string>::iterator, bool> ret;
                for(vector<string>::iterator it = m_values->begin(); it != m_values->end(); ++it){
                        ret = m_uniqueVals->insert(*it);
                        if(ret.second){
                                f_uniqueVals->insert(stof(*it));//could be moved outside the if
                                cout << "\tinserted: " << *it << "\n";
                        }
                }
                cout << "Finished populating Uniques before discretization...\n";
                cout << "Discretization:\n";
                //discretization: creates cutpoint and builds m_uniqueSym 
                //                based on m_uniqueVals and the cutpoints
                set<float>::iterator it = f_uniqueVals->begin();
                set<float>::iterator itPlusOne = ++f_uniqueVals->begin();
                set<float>::iterator beg = f_uniqueVals->begin();
                set<float>::reverse_iterator end = f_uniqueVals->rbegin();
                set<string>* intervals = new set<string>();
                while(itPlusOne != f_uniqueVals->end()){
                        float avg = (*it + *itPlusOne)/2;
                        intervals->insert(to_string(*beg) + ".." + to_string(avg));
                        cout << "\tinserted: " << to_string(*beg) + ".." + to_string(avg) << "\n";
                        intervals->insert(to_string(avg) + ".." + to_string(*end));
                        cout << "\tinserted: " << to_string(avg) + ".." + to_string(*end) << "\n";
                        ++it;
                        ++itPlusOne;
                }
                cout << "Finished Discretization.\n";
                m_uniqueSym = intervals;
        }

}
template<>
void Column<float>::populateAVBlocks(){
        int caseIndex = 1;
        float maxValOfInterval = *(m_uniqueVals->rbegin());
        for(set<string>::iterator it = m_uniqueSym->begin(); it != m_uniqueSym->end(); ++it){
                m_avBlocks->push_back(new set<int>());
        }
        vector<set<int>*>::iterator block = m_avBlocks->begin();
        set<string>::iterator uniq = m_uniqueSym->begin();
        while(block != m_avBlocks->end() && uniq != m_uniqueSym->end()){
                caseIndex = 1;
                float left = stof((*uniq).substr(0, (*uniq).find("..")));
                float right = stof((*uniq).substr((*uniq).find("..") + 2));
                for(vector<float>::iterator val = m_values->begin(); val != m_values->end(); ++val){
                        //checking left side of interval with inclusive [L, R)
                        if(left <= *val && *val < right){
                                (*block)->insert(caseIndex);
                        }
                        else if(right == maxValOfInterval && left <= *val && *val <= right){
                                (*block)->insert(caseIndex);
                        }
                        caseIndex++;
                }
                ++block;
                ++uniq;
        }
}
template<>
void Column<string>::populateAVBlocks(){
        // int caseIndexMax = m_uniqueVals->size();
        if(m_colType == 's'){

                int caseIndex = 1;
                for(set<string>::iterator it = m_uniqueSym->begin(); it != m_uniqueSym->end(); ++it){
                        m_avBlocks->push_back(new set<int>());
                }
                vector<set<int>*>::iterator block = m_avBlocks->begin();
                set<string>::iterator uniq = m_uniqueSym->begin();
                while(block != m_avBlocks->end() && uniq != m_uniqueSym->end()){
                        caseIndex = 1;
                        for(vector<string>::iterator val = m_values->begin(); val != m_values->end(); ++val){
                                if(*val == *uniq){
                                        (*block)->insert(caseIndex);
                                }
                                caseIndex++;
                        }
                        ++block;
                        ++uniq;
                }
        }
        else{
                int caseIndex = 1;
                float maxValOfInterval = *(f_uniqueVals->rbegin());
                for(set<string>::iterator it = m_uniqueSym->begin(); it != m_uniqueSym->end(); ++it){
                        m_avBlocks->push_back(new set<int>());
                }
                vector<set<int>*>::iterator block = m_avBlocks->begin();
                set<string>::iterator uniq = m_uniqueSym->begin();
                while(block != m_avBlocks->end() && uniq != m_uniqueSym->end()){
                        caseIndex = 1;
                        float left = stof((*uniq).substr(0, (*uniq).find("..")));
                        float right = stof((*uniq).substr((*uniq).find("..") + 2));
                        for(vector<string>::iterator val = m_values->begin(); val != m_values->end(); ++val){
                                //checking left side of interval with inclusive [L, R)
                                if( (left <= stof(*val)) && (stof(*val) < right) ){
                                        (*block)->insert(caseIndex);
                                }
                                else if( (right == maxValOfInterval) && (left <= stof(*val)) && (stof(*val) <= right) ){
                                        (*block)->insert(caseIndex);
                                }
                                caseIndex++;
                        }
                        ++block;
                        ++uniq;
                }
        }
}

template <class T>
void Column<T>::printAVBlocks(){
        set<string>::iterator uniq = m_uniqueSym->begin();
        vector<set<int>*>::iterator cases = m_avBlocks->begin();
        while(uniq != m_uniqueSym->end() && cases != m_avBlocks->end()){
                cout << "(" << m_name << ", " << *uniq << ") = {";
                for(set<int>::iterator singleCase = (*cases)->begin(); singleCase != (*cases)->end(); ++singleCase){
                        if(*singleCase != *((*cases)->rbegin())){
                                cout << *singleCase << ", ";
                        }
                        else{
                                cout << *singleCase;
                        }
                }
                cout << "}\n";
                ++uniq;
                ++cases;
        }
}
template <class T>
void Column<T>::printUniqueSym(){
        for(set<string>::iterator it = m_uniqueSym->begin(); it != m_uniqueSym->end(); ++it){
                cout << *it << "\n";
        }
        cout << "FIRST ELEMENT: " << *(m_uniqueSym->begin()) << "\n";
}
template <class T>
bool Column<T>::isSymbolic(string input){
    if((input.find("..") == std::string::npos) && (input.find_first_of("abcdefghijklmnopqrstuvwxyz-_,/") == std::string::npos)){
        try{
            stof(input);
        }
        catch(exception& e){
            return true;
        }
        return false;
    }
    return true;
}
// template class StrColumn<int>;
// template class StrColumn<string>;
