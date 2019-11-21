#include <iostream>
#include <string>
#include <fstream>
#include <cstddef>
#include "Column.h"
#include "Dataset.h"
using namespace std;
// vector<Column<float>*>* buildDatasetFloat(){
//     return(new vector<Column<float>*>());
// }
vector<Column<string>*>* buildDatasetString(){
    return(new vector<Column<string>*>());
}
bool isSymbolic(string input){
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
int main(int argc, char* argv[]){
    Dataset<string>* strDataset = nullptr;
    // Dataset<float>* fDataset = nullptr;
    int numCols = 0;
    string inputFile = " ";
    string outputFile = "";
    std::ifstream inFile;
    std::ofstream outFile;

    if(argc == 1){//0 - cmd line arg
        //use later for user input vs cmd line file input
        do{
            cout << "Please enter valid file name of input dataset: ";
            cin >> inputFile;
            inFile.open(inputFile);
        }while(!inFile.is_open());
        do{
            cout << "Please enter file name to output rules: ";
            cin >> outputFile;
            outFile.open(outputFile);
        }while(!outFile.is_open());
    }
    else if(argc == 2){//1 - cmd line arg
        inputFile = argv[1];
        inFile.open(inputFile);
        outputFile = inputFile + ".out.txt";
        outFile.open(outputFile);
    }
    else if(argc >= 3){//2 or more - cmd line arg
        inFile.open(argv[1]);
        outputFile += argv[2];
        outputFile += ".out.txt";
        outFile.open(outputFile);
    }
    
    if(inFile.is_open()){
        std::vector<std::string>* colNames = new vector<string>();
        vector<string>* row = new vector<string>();
        std::string data;
        int currColNum = 0;
        //1st line parser: parses column number
        while(inFile >> data && data != ">"){
            if(data == "<"){ numCols = 0; }
            else if(data == "a" || data == "x" || data == "d"){
                numCols++;
            }
        }
        //2nd line parser: parses column names
        while(inFile >> data && data != "]"){
            if(data != "["){
                colNames->push_back(data);
            }
        }
        //use first data point to create dataset based on type
        //also create decision column seperate from attribute cols
        // if(inFile >> data){
        //     row->push_back(data);
        //     currColNum++;
        //     if(isSymbolic(data)){
        //         cout << "found symbolic data" << "\n";
        //         strDataset = new Dataset<string>('s', colNames);
        //     }
        //     else{
        //         cout << "found numerical data" << "\n";
        //         fDataset = new Dataset<float>('f', colNames);
        //     }
        // }
        strDataset = new Dataset<string>('s', colNames);
        //parse all rows
        while(inFile >> data){
            if(data.find("!") == std::string::npos){
                row->push_back(data);
                currColNum++;
                //enough items for a whole row; add row; reset;
                if(currColNum == numCols){
                    if(strDataset != nullptr){ strDataset->addRow(row); }
                    // else{ fDataset->addRow(row); }
                    row->clear();
                    currColNum = 0;
                }
            }
            else{
                char comment[256];
                inFile.getline(comment, 256);
            }
        }
        inFile.close();
    }
    if(strDataset != nullptr){ strDataset->printDataset(); }
    // else if(fDataset != nullptr){ fDataset->printDataset(); }
    if(strDataset != nullptr){ 
        strDataset->populateAllUniques();
        cout << "finished populating uniques\n";
        strDataset->populateAllAVBlocks();
        strDataset->printAllAVBlocks();
        strDataset->LEM2();
        strDataset->printRules();
        strDataset->printRulesToFile(outFile);
        
    }
    // else if(fDataset != nullptr){
    //     fDataset->populateAllUniques();
    //     cout << "finished populating uniques\n";
    //     fDataset->populateAllAVBlocks();
    //     fDataset->printAllAVBlocks();
    //     fDataset->LEM2();
    //     fDataset->printRules();
    //     fDataset->printRulesToFile(outFile);
    // }
    outFile.close();
    return(0);
}