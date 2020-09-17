#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <set>
#include <math.h>
#include <chrono>

using namespace std;
//GLOBAL VARIABLES THAT NEED TO BE CHANGED ACCORDINGLY
int version = 3;
int limit = 5;
int recursiveLimit = 1000;
//int runLimit = 100;
int runLimit = 1000000;
string location_main = "C:\\Users\\Bruger\\Desktop\\books\\THESIS start aug 3\\datasets\\";
//file name here
//string fileName = "test_ref_only.txt";
//THESE TWO FILES SHOULD BE LOGGED
string fileName = "Gen178.fa";
//string fileName = "embl50.h178.fa";
//change according to new version

string* dnaArray;
unordered_map<string, vector<int>> fingerPrints;
unordered_map<char, int> singleChar;
int memory = 0;
string relativeString;
int relativeSize;
int countSingleChar = 0;
int memoryVar = 0;
int memoryOld = 0;

int mb = 1024 * 1024;
int kb = 1024;

vector<int>* indexRelative;
vector<int>* indexCString;
vector<int>* originalIndex = new vector<int>;
vector<int>* pointerIndex = new vector<int>;
int recursive = 0;
string extra = "";

//FIND HOW MANY STRINGS ARE THERE
int findSize(string location) {
    ifstream myfile;
    myfile.open(location);
    cout << "dna file opened" << endl;
    string x;
    int size = 0;

    while (myfile >> x) {
        if (x[0] == '>')
            size += 1;
    }

    myfile.close();
    return size;
}

//CREATE ARRAY OF DNA STRINGS FROM THE FILE
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
        else
            temp += x;
    }

    //temp += "$";
    dnaArray[size] = temp;
    myfile.close();
    cout << endl << "dna file closed" << endl;
    return dnaArray;
}

//WRITE A LOG TO THE FILE, WHICH WILL BE USED TO PLOT TIME AND SPACE USED BY COMPRESSION TECHNIQUE
void writeLog(string location, string fileName, int version, int memoryVar, int time) {
    fstream myfile;
    myfile.open(location, fstream::app);
    cout << "log file opened" << endl;
    myfile << "\n";
    myfile << fileName << ";" << version << ";" << memoryVar << ";" << time;
    myfile.close();
    cout << "log file closed" << endl;
}

//PRINT DATA STORED BY THE COMPRESSED DATASTRUCTURE : INDEX OF MAIN STRING + INDEX OF RELATIVE STRING  
void printCompressed(vector<int>& indexRelativeElement, vector<int>& indexCString) {
    cout << " size of relative indices " << indexRelativeElement.size() << " " << indexCString.size() << endl;
    /*for (int i = 0; i < indexRelativeElement.size();i++) {
        cout << indexRelativeElement[i] << " " << indexCString[i] << endl;
    }*/
}

//PRINT THE FINGERPRINT DATA : SUBSTRING + LIST OF INDICES IN THE STRING
void printFingerPrint() {
    unordered_map<string, vector<int>>::iterator itPrint = fingerPrints.begin();
    while (itPrint != fingerPrints.end()) {
        cout << "substring : " << itPrint->first << " : ";
        vector<int> readVector = itPrint->second;
        for (vector<int>::iterator itV = readVector.begin(); itV != readVector.end(); itV++)
            cout << *itV << " , ";
        cout << endl;
        itPrint++;
    }
}

//FIND IF THE FINGERPRINT EXISTS AND WHICH INDICES ARE THERE
vector<int> findFingerPrint(string& checkFingerPrint) {
    unordered_map<string, vector<int>>::iterator itCheck = fingerPrints.find(checkFingerPrint);
    vector<int> returnVector;
    if (itCheck != fingerPrints.end())
        returnVector = itCheck->second;
    return returnVector;
}

//PRINT LIST OF SINGLE CHARACTERS AND THEIR FIRST POSITION IN THE STRING
void printSingleChar() {
    unordered_map<char, int>::iterator itChar = singleChar.begin();
    while (itChar != singleChar.end()) {
        cout << "char : " << itChar->first << " index : " << itChar->second << endl;
        itChar++;
    }
}

//FIND IF THE SINGLE CHARACTER EXISTS AND WHAT IS THE INDEX
int findSingleChar(char& checkChar) {
    unordered_map<char, int>::iterator itCheck = singleChar.find(checkChar);
    if (itCheck != singleChar.end())
        return itCheck->second;
    return -1;
}

//SET UP THE DATASTRUCTURE CONTAINING FINGERPRINTS AND SINGLE CHARACTERS ALONG WITH THE INDICES
void setFingerPrintSingleChar() {
    int i;
    for (i = 0; i <= relativeSize - limit; i++) {
        string fingerPrint = relativeString.substr(i, limit);
        char single = relativeString[i];
        unordered_map<string, vector<int>>::const_iterator it = fingerPrints.find(fingerPrint);
        unordered_map<char, int>::const_iterator itC = singleChar.find(single);
        if (itC == singleChar.end())
            singleChar[single] = i;
        if (it == fingerPrints.end()) {
            vector<int> newVector;
            newVector.push_back(i);
            fingerPrints[fingerPrint] = newVector;
        }
        else {
            vector<int> existingVector = it->second;
            existingVector.push_back(i);
            fingerPrints[fingerPrint] = existingVector;
        }
    }

    while (i < relativeSize) {
        char single = relativeString[i];
        unordered_map<char, int>::const_iterator itC = singleChar.find(single);
        if (itC == singleChar.end())
            singleChar[single] = i;
        i++;
    }
}

//ADD A NEW CHARACTER TO THE RELATIVE STRING, IF IT WASN'T EXISTING INITIALLY WHEN THE RELATIVE STRING WAS JUST THE FIRST STRING FROM THE FILE 
int expandRelative(char charToAdd) {
    relativeString += charToAdd;
    relativeSize += 1;
    singleChar[charToAdd] = relativeSize - 1;
    return relativeSize - 1;
}

//COMPRESS ONE STRING AT A TIME
void compress(string& toCompress, vector<int>& indexRelativeElement, vector<int>& indexCStringElement) {
    int start = 0;
    int end = toCompress.size();

    while (start <= end - limit) {
        vector<int> indices;
        string checkFingerPrint;
        checkFingerPrint = toCompress.substr(start, limit);
        indices = findFingerPrint(checkFingerPrint);
        //limit size substring fingerprint not found
        if (indices.size() == 0) {
            int index = findSingleChar(toCompress[start]);
            if (index == -1) {
                cout << "char not found : " << toCompress[start] << endl;
                index = expandRelative(toCompress[start]);
            }
            indexRelativeElement.push_back(index);
            indexCStringElement.push_back(start);
            start++;
            countSingleChar++;
        }
        //fingerprint(s) found, so now we find the longest substring that matches through the fingerprint(s)
        else {
            int max_length = limit;
            int max_index = -1;
            for (vector<int>::iterator itV = indices.begin(); itV != indices.end(); itV++) {
                if (max_index == -1) {
                    max_index = *itV;
                }
                int currIndexString = start + limit;
                int currIndexRelativeString = *itV + limit;
                int length = limit;
                while (true) {
                    if (currIndexString >= end || currIndexRelativeString >= relativeSize) {
                        break;
                    }
                    char stringChar = toCompress[currIndexString];
                    char relativeChar = relativeString[currIndexRelativeString];
                    if (stringChar == relativeChar) {
                        currIndexString++;
                        currIndexRelativeString++;
                        length++;
                    }
                    else {
                        break;
                    }
                }
                if (length > max_length) {
                    max_length = length;
                    max_index = *itV;
                }
            }
            indexRelativeElement.push_back(max_index);
            indexCStringElement.push_back(start);
            start += max_length;
        }
    }

    while (start < end) {
        int index = findSingleChar(toCompress[start]);
        if (index == -1) {
            cout << "char not found : " << toCompress[start] << endl;
            index = expandRelative(toCompress[start]);
        }
        indexRelativeElement.push_back(index);
        indexCStringElement.push_back(start);
        start++;
        countSingleChar++;
    }
}

//FIND LOCATION OF THE INDEX OF SEARCHED STRING IN THE RELATIVE STRING
int findLocation(vector<int>& indexCStringElement, int& charIndex) {
    int count = 0;
    int first = 0;
    int last = ((charIndex < (int)indexCStringElement.size()) ? charIndex : (indexCStringElement.size() - 1));
    //int last = indexCStringElement.size() - 1;
    int mid = last / 2;
    int indexCStringCurrent, indexCStringNext, indexCStringNextNext;
    while (true) {
        count++;
        if (count > recursiveLimit) {
            cout << "first " << first << " mid " << mid << " last " << last << endl;
        }
        if (count > 1010) {
            cout << "limit reached in findLocation" << endl;
            break;
        }
        indexCStringCurrent = indexCStringElement[mid];

        if (mid < (int)(indexCStringElement.size() - 1)) {
            indexCStringNext = indexCStringElement[mid + 1];
            if (indexCStringCurrent <= charIndex && indexCStringNext > charIndex) {
                break;
            }
            if (mid < (int)(indexCStringElement.size() - 2)) {
                indexCStringNextNext = indexCStringElement[mid + 2];
                if (indexCStringNext <= charIndex && indexCStringNextNext > charIndex) {
                    mid++;
                    break;
                }
            }
        }
        else {
            if (indexCStringCurrent <= charIndex) {
                break;
            }
        }

        if (mid == first) {
            mid = last;
        }
        else {
            if (indexCStringCurrent > charIndex) {
                last = mid;
            }
            else {
                first = mid;
            }

            mid = (first + last) / 2;
        }
    }
    return mid;
}

//FIND CHARACTER FROM COMPRESSED REFERENCE STRING
char findCharIndexRefString(int& index) {
    recursive++;
    if (recursive > recursiveLimit) {
        cout << "something went wrong in recursion " << endl;
        return '@';
    }

    int first = 0;
    int last = (index <= extra.size() ? index : extra.size() - 1);
    int mid = last / 2;
    int count = 0;
    while (true) {
        if (originalIndex->at(mid) == index) {
            if (originalIndex->at(mid) == pointerIndex->at(mid)) {
                return extra[mid];
            }
            else {
                return findCharIndexRefString(pointerIndex->at(mid));
            }
        }

        if (mid == first) {
            if (originalIndex->at(last) == pointerIndex->at(last) && pointerIndex->at(last) == index) {
                return extra[last];
            }
        }
        if (mid != extra.size() - 1) {
            int next = mid + 1;
            if (originalIndex->at(mid) < index && originalIndex->at(next) > index) {
                if (originalIndex->at(next) == index + 1)
                    return extra[mid];
                else {
                    int callIndex = pointerIndex->at(mid) + index - originalIndex->at(mid);
                    return findCharIndexRefString(callIndex);
                }
            }
            else {
                if (mid == first) {
                    mid = last;
                }
                else {
                    if (originalIndex->at(mid) > index)
                        last = mid;
                    else
                        first = mid;
                    mid = (first + last) / 2;
                }
            }
        }
        else {
            if (index == extra.size() - 1)
                return extra[mid];
            else {
                int callIndex = pointerIndex->at(mid) + index - originalIndex->at(mid);
                return findCharIndexRefString(callIndex);
            }
        }
        count++;
        if (count > 50) {
            cout << "something went wrong again " << endl;
            break;
        }
    }
    return '$';
}

//FIND WHICH CHARACTER IS PRESENT ON THE STRING INDEX
char findCharacter(vector<int>& indexRelativeElement, vector<int>& indexCStringElement, int& charIndex) {
    int indexFound = findLocation(indexCStringElement, charIndex);
    int distance = charIndex - indexCStringElement[indexFound];
    int finalIndex = indexRelativeElement[indexFound] + distance;
    recursive = 0;
    return findCharIndexRefString(finalIndex);
    //return relativeString[indexRelativeElement[indexFound] + distance];
}

//FIND SUBSTRING FROM THE SEARCHED STRING, INPUT IS START INDEX AND LENGTH OF SUBSTRING
string findSubString(vector<int>& indexRelativeElement, vector<int>& indexCStringElement, int& charIndex, int& length) {
    int indexFound = findLocation(indexCStringElement, charIndex);
    int distance = charIndex - indexCStringElement[indexFound];
    int indexOnRelative = indexRelativeElement[indexFound] + distance;
    string toReturn(1, relativeString[indexOnRelative]);
    length--;
    charIndex++;

    while (true) {
        if (length == 0)
            break;
        if (indexFound < indexRelativeElement.size()) {
            int nextIndex = indexFound + 1;
            if (charIndex < indexCStringElement[nextIndex]) {
                indexOnRelative++;
            }
            else {
                if (indexFound < (int)indexCStringElement.size() - 1) {
                    indexFound++;
                }
                else {
                    break;
                }
                indexOnRelative = indexRelativeElement[indexFound];
            }
        }
        else
            indexOnRelative++;

        toReturn += relativeString[indexOnRelative];
        length--;
        charIndex++;
    }
    return toReturn;
}

//PROCESS REQUEST FROM THE USER, WHERE INPUT IS WHICH STRING NUMBER (FROM 0) + INDEX WITHIN THE STRING
void processSingleCharRequestFromUser(int& numberOfStrings, int* sizes) {
    char response;
    int stringIndex, charIndex;

    while (true) {
        cout << " do you want to retrieve a character ? Y/N " << endl;
        cin >> response;
        if (toupper(response) != 'Y')
            break;
        cout << "enter string index starting from 0 " << endl;
        cin >> stringIndex;
        if (stringIndex < 0 || stringIndex > numberOfStrings - 1) {
            cout << "you entered a wrong string index" << endl;
            continue;
        }
        cout << "enter index within the string " << endl;
        cin >> charIndex;
        if (charIndex < 0 || charIndex > sizes[stringIndex] - 1) {
            cout << "the string is not that long " << endl;
            continue;
        }
        vector<int> indexRelativeElement = indexRelative[stringIndex];
        vector<int> indexCStringElement = indexCString[stringIndex];
        //measuring time start
        auto start = chrono::high_resolution_clock::now();

        //this function finds the character from the compressed datastructure
        char charFound = findCharacter(indexRelativeElement, indexCStringElement, charIndex);

        //measuring time end
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

        cout << "your character is " << charFound << " it took " << duration.count() << " milliseconds " << endl;
    }
}

//PROCESS REQUEST FROM THE USER, WHERE INPUT IS WHICH STRING NUMBER (FROM 0) + INDEX WITHIN THE STRING + SIZE OF THE SUBSTRING BEING SEARCHED
void processSubstringFromUser(int& numberOfStrings, int* sizes) {

    char response;
    int stringIndex, charIndex, subStringLength;

    while (true) {

        cout << " do you want to retrieve a substring ? Y/N " << endl;
        cin >> response;
        if (toupper(response) != 'Y')
            break;
        cout << "enter string index starting from 0 " << endl;
        cin >> stringIndex;
        if (stringIndex < 0 || stringIndex > numberOfStrings - 1) {
            cout << "you entered a wrong string index" << endl;
            continue;
        }
        cout << "enter index within the string " << endl;
        cin >> charIndex;
        if (charIndex < 0 || charIndex > sizes[stringIndex] - 1) {
            cout << "the string is not that long " << endl;
            continue;
        }

        cout << "enter length of substring " << endl;
        cin >> subStringLength;
        if (subStringLength < 0) {
            cout << "length can't be negative " << endl;
            continue;
        }

        vector<int> indexRelativeElement = indexRelative[stringIndex];
        vector<int> indexCStringElement = indexCString[stringIndex];
        //measuring time start
        auto start = chrono::high_resolution_clock::now();

        //this function finds the character from the compressed datastructure
        string subStringFound = findSubString(indexRelativeElement, indexCStringElement, charIndex, subStringLength);

        //measuring time end
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

        cout << "your substring is " << subStringFound << " it took " << duration.count() << " milliseconds " << endl;
    }
}

//PROCESS ONE MILLION REQUESTS OF FINDING A CHARACTER ON A RANDOM INDEX FROM A RANDOM STRING
auto processMillionRequest(int& numberOfStrings, int* sizes) {
    cout << " processing million requests " << endl;
    int counter = 0;
    int stringIndex, charIndex;
    //measuring time start
    auto start = chrono::high_resolution_clock::now();

    while (counter < runLimit) {
        if (counter % 10000 == 0) {
            cout << counter << endl;
        }
        stringIndex = rand() % (numberOfStrings - 1);
        charIndex = rand() % (sizes[stringIndex] - 1);

        vector<int> indexRelativeElement = indexRelative[stringIndex];
        vector<int> indexCStringElement = indexCString[stringIndex];
        //this function finds the character from the compressed datastructure
        char charFound = findCharacter(indexRelativeElement, indexCStringElement, charIndex);
        if (charFound != dnaArray[stringIndex][charIndex]) {
            cout << "some issue fetching character " << stringIndex << " " << charIndex << endl;
            break;
        }
        //cout << "character found at string " << stringIndex << " at index " << charIndex << " is " << charFound << " actual " << dnaArray[stringIndex][charIndex]<<endl;
        counter++;
    }
    //measuring time end
    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    return duration;
}

//FIND INDEX OF THE SUBSTRING FROM HASHMAP. IF NOT FOUND, THEN RETURN -1
int findIndexSubString(string& current, unordered_map<string, int>& subStrings) {
    unordered_map<string, int>::iterator itCheck = subStrings.find(current);
    if (itCheck != subStrings.end()) {
        return itCheck->second;
    }
    return -1;
}

//UPDATE INT, INT, CHAR VECTORS WITH ORIGINAL INDEX, POINTER INDEX AND EXTRA CHARACTER AND ALSO UPDATE THE HASHMAP TO STORE SUBSTRING TO INDEX MAPPING
void updateVector(int& i, char& additional, int& prevIndex, string& currentString, unordered_map<string, int>& subStrings, int& memory) {
    originalIndex->push_back(i);
    extra += additional;
    if (prevIndex == -1) {
        pointerIndex->push_back(i);
    }
    else {
        pointerIndex->push_back(prevIndex);
    }
    memory += 9;
    subStrings[currentString] = i;
}

//MAIN LOGIC TO COMPRESS THE REFERENCE STRING
void compressReference() {
    unordered_map<string, int> subStrings;
    string currentString = "";
    memory = sizeof(originalIndex) + sizeof(pointerIndex) + sizeof(extra) ;
    cout << "initial memory when compressing reference string " << memory << endl;

    int currentIndex = -1, prevIndex = -1, i;
    char additional;
    int baseIndex = 0;

    for (i = 0; i < relativeString.size(); i++) {
        prevIndex = currentIndex;
        additional = relativeString[i];
        currentString += additional;
        currentIndex = findIndexSubString(currentString, subStrings);

        if (currentIndex == -1) {
            updateVector(baseIndex, additional, prevIndex, currentString, subStrings, memory);
            currentString = "";
            baseIndex = i + 1;
        }
    }

    additional = '$';
    currentString += additional;
    updateVector(baseIndex, additional, currentIndex, currentString, subStrings, memory);
    
    cout << "memory used by compression of main string : " << memory << endl;
}

//PRINT THE DATA IN VARIOUS VECTORS FOR THE COMPRESSED STRING
void printReferenceString() {
    cout << "printing compressed reference string of size " << originalIndex->size() <<" " << pointerIndex->size()<<" "<<extra.size()<< endl; 
    int i = 0;
    while (i < originalIndex->size()) {
        cout << originalIndex->at(i) << " " << pointerIndex->at(i) << " " << extra[i] << endl;
        i++;
    }
}

//PROCESS REQUEST FROM THE USER, WHERE INPUT IS WHICH STRING NUMBER (FROM 0) + INDEX WITHIN THE STRING
void processCharRequestFromUserRefString() {
    char response;
    int charIndex;

    while (true) {
        cout << " do you want to retrieve a character ? Y/N " << endl;
        cin >> response;
        if (toupper(response) != 'Y')
            break;
        cout << "enter index within the string " << endl;
        cin >> charIndex;
        if (charIndex < 0 || charIndex >= relativeSize) {
            cout << "the string is not that long " << endl;
            continue;
        }

        //measuring time start
        auto start = chrono::high_resolution_clock::now();
        recursive = 0;
        //this function finds the character from the compressed datastructure
        char charFound = findCharIndexRefString(charIndex);

        //measuring time end
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);

        cout << "your character is " << charFound << " it took " << duration.count() << " milliseconds " << endl;
    }
}

int main() {
    cout << "PROGRAM STARTING!!!" << endl;

    int i;
    string location = location_main + fileName;
    int numberOfStrings = findSize(location);
    dnaArray = new string[numberOfStrings];
    dnaArray = readDna(location, numberOfStrings);
    int* sizes = new int[numberOfStrings];
    //set<int>* characters = new set<int>[numberOfStrings];

    memoryVar += (sizeof(sizes) + (4 * numberOfStrings));  //adding size of pointer for size array and each element of size array

    cout << "memory var after size array " << memoryVar << endl;

    for (i = 0; i < numberOfStrings; i++) {
        sizes[i] = dnaArray[i].size();
        memoryOld += sizes[i]; //adding length of each string
    }

    cout << "DNA ARRAY !!!" << endl;

    relativeString = dnaArray[0];
    memoryVar += sizeof(relativeString);  //adding space for reference string

    relativeSize = relativeString.size();

    cout << "total number of strings : " << numberOfStrings << " and size of relative string : " << relativeSize << endl;

    setFingerPrintSingleChar();
    printSingleChar();

    indexRelative = new vector<int>[numberOfStrings];
    indexCString = new vector<int>[numberOfStrings];

    memoryVar += (sizeof(indexRelative) + sizeof(indexCString)); //adding pointer size for both index arrays

    //to check how long it takes to compress all the data
    auto start = chrono::high_resolution_clock::now();

    cout << "BEFORE COMPRESSION !!!" << endl;

    // first string can be compressed to itself
    indexRelative[0].push_back(0);
    indexCString[0].push_back(0);

    //printCompressed(indexRelative[0], indexCString[0]);

    for (i = 1; i < numberOfStrings; i++) {
        cout << "compressing " << i << endl;
        string toCompress = dnaArray[i];
        compress(toCompress, indexRelative[i], indexCString[i]);
        //printCompressed(indexRelative[i], indexCString[i]);
        memoryVar += (8 * indexRelative[i].size()); //adding space for encoding
        //cout << "size of string " << dnaArray[i].size() << " size of compressed structure " << 8*indexRelative[i].size() << endl;
        //memoryVar += ((4 * indexRelative[i].size()) + (4 * indexCString[i].size())); //adding space for encoding
    }

    memoryVar += relativeSize; //finally adding length of relative string as it was expanded

    //delete[] dnaArray;
    cout << "AFTER COMPRESSION!!!" << endl;

    compressReference();
    //printReferenceString();
    //processCharRequestFromUserRefString();
    //auto stop = chrono::high_resolution_clock::now();
    //auto duration = chrono::duration_cast<chrono::seconds>(stop - start);
    //cout << "it took " << duration.count() << " seconds to compress " << numberOfStrings <<  endl;

    //processSingleCharRequestFromUser(numberOfStrings,sizes);
    //processSubstringFromUser(numberOfStrings, sizes);
    auto durationMillion = processMillionRequest(numberOfStrings, sizes);
    //cout << durationMillion.count() <<" milliseconds to process million requests ";
    delete[] dnaArray;
    delete[] sizes;
    delete[] indexRelative;
    delete[] indexCString;
    delete originalIndex;
    delete pointerIndex;

    cout << "PROGRAM ENDING!!! " << endl;

    cout << "old memory : " << memoryOld << " compressed memory : " << memoryVar << endl;
    //string headers = "FILE_NAME;VERSION;MEMORY;TIME";
    location = location_main + "LOGS.csv";
    int timeUsed = 0;
    writeLog(location, fileName, version, memoryVar+memory, (int)durationMillion.count());
}
