//
//  FileAccess.cpp
//  Voltex
//
//  Created by Samuel Dewan on 2015-05-29.
//
//

#define FILE_TYPE_VERSION 0.1
#define FILE_TYPE_MIN 0.1
#define FILE_TYPE_MAX 1.0

#include "FileAccess.h"

#include <fstream>  //ifstream/ofstream
#include <iomanip>  //setprecision()
#include <sstream>
#include <string>
using namespace std;


int writeAllToFile (char filePath[], Voltex* voltex, bool overWrite) {
    //Check if the file exists
    ifstream fileTest(filePath);
    if (fileTest.good() && !overWrite) {
        //File exists
        return 1;
    }
    fileTest.close();
    
    voltex->GetParam(0);
    
    
    //Write to the file
    fstream file(filePath, ios::out /*open for output*/| ios::trunc /*If file exists, replace*/);
//    file << FILE_TYPE_VERSION << endl;
    file << FILE_TYPE_VERSION;
    
    //Set precision
    file.setf(ios::fixed, ios::floatfield);
    file.precision(15); //more than enough
    
    //wavetables
    for (int i = 0; i < NUM_TABLES; i++) {
        std::tr1::array<double, TABLE_LENGTH> table = voltex->waveTables[i]->getValues();
//        file << table[0];
        for (int j = 0; j < voltex->waveTables[i]->size(); j++) {
            file << " " << table[j];
        }
//        file << endl;
    }
    
    //params
//    file << voltex->GetParam(0)->Value();
    for (int i = 10; i < voltex->getNumParams(); i++) {
        file << " " << voltex->GetParam(i)->Value();
    }
//    file << endl;
    
    
    return 0;
}

int readAllFromFile (char filePath[], Voltex* voltex) {
    ifstream file(filePath);
    if (!file.good()) {
        //opening file failed
        //Pressing cancel on the open dialog will cause execution to end up here
        return 1;
    }
    
    //Check file version
    string version;
    std::getline(file, version, ' ');
    double ver = getStringAsDouble(version);
    if ((ver < FILE_TYPE_MIN) || (ver > FILE_TYPE_MAX)) {
        //this file is not compatable with this version of the plugin
        return 2;
    }
    
    //Load wavetables
    for (int i = 0; i < NUM_TABLES; i++) {
        std::tr1::array<double, 2048> values;
        string value;
        int j = 0;
        for (;(j < voltex->waveTables[i]->size()) && std::getline(file, value, ' '); j++) {
            values[j] = getStringAsDouble(value);
            value.clear();
        }
        voltex->waveTables[i]->setValues(values);
        voltex->vectorSpaces[i]->setValues(voltex->waveTables[i]->getValues(), voltex->getVectorSpacePrecision());
        if (j < voltex->waveTables[i]->size()) {
            return 3; //the file was not properly formated
        }
    }
    
    
    //Load params
    string value;
    for (int i = 0;(i < voltex->getNumParams()) && std::getline(file, value, ' '); i++) {
        voltex->GetParam(i)->Set(getStringAsDouble(value));
        cout << ":" << getStringAsDouble(value) << "|" << value;
        value.clear();
    }
    for (int i = 0; i < voltex->getNumParams(); i++) {
        voltex->OnParamChange(i);
    }
    voltex->GetGUI()->SetAllControlsDirty();
    
    return 0;
}

double getStringAsDouble (string s) {
    double i;
    stringstream convert;
    convert<<s; //write the string into the string stream
    convert>>i; //convert string into double and store it
    convert.str(""); //clear the stringstream
    convert.clear();
    return i;
}