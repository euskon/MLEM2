#include <iostream>
#include <set>
#include <iterator>
#include <string>
#include <sstream>
#include <vector>
template<typename T>
struct Column{
    Column(char i_colType, std::string i_name){
        colType = i_colType;
        name = i_name;
    }
    char colType;
    std::string name;
    set<T>* uniqueVals;
    vector<T>* values;
};

int main(){
    
    return 0;
}