#include <iostream>
#include <fstream>
#include <string>
#include "ReadFile.h"
#include "windows.h"

using namespace std;

int findSize(string location) {
    ifstream myfile;
    myfile.open(location);
    cout << "file read" << endl;

    string x;
    int size = 0;

    while (myfile >> x) {
        if (x[0] == '>') {
            size += 1;
        }
    }

    myfile.close();

    return size;
}

string* readDna(string location, int size) {

    ifstream myfile;

    string* dnaArray;
    dnaArray = new string[size];

    myfile.open(location);

    size = -1;
    string temp, x;
    while (myfile >> x) {
        if (x[0] == '>') {
            if (size > -1) {
                //temp += "$";
                dnaArray[size] = temp;
            }
            temp = "";
            size += 1;
        }
        else {
            temp += x;
        }
    }

    //temp += "$";
    dnaArray[size] = temp;

    myfile.close();
    cout << endl << "file closed" << endl;

    return dnaArray;
}

void writeLog(string location, string fileName, int version, DWORDLONG memoryDna, DWORDLONG memoryFingerPrint, DWORDLONG memoryCompressed, int memoryVar, int time) {
    fstream myfile;
    myfile.open(location, fstream::app);
    cout << "file opened" << endl;

    // + memoryDna+ ";"+ memoryFingerPrint + ";" + memoryCompressed + ";" + memoryVar + ";" + time;
    myfile << "\n";
    myfile << fileName << ";" << version << ";" << memoryDna << ";" << memoryFingerPrint << ";" << memoryCompressed << ";" << memoryVar << ";" << time;
    myfile.close();
    cout << "file closed" << endl;
}
