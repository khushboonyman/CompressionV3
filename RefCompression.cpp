#include "RefCompression.h"
#include "IndexLength.h"
#include<unordered_map>
#include<iostream>
#include<string>

using namespace std;

int findIndexSubString(string& current, unordered_map<string, int>& subStrings) {
	unordered_map<string, int>::iterator itCheck = subStrings.find(current);
	if (itCheck != subStrings.end()) {
		return itCheck->second;
	}
	return -1;
}

void updateVector(vector<Node>& refCompressed, int& i, char& additional, int& prevIndex, string& currentString, unordered_map<string, int>& subStrings, int& memory) {
	Node n(i, additional);
	if (prevIndex == -1) {
		n.assignPrev(i);
	}
	else {
		n.assignPrev(prevIndex);
	}
	refCompressed.push_back(n);
	memory += sizeof(n);
	subStrings[currentString] = i;
}

vector<Node> compressReference(string& relativeString) {
	unordered_map<string, int> subStrings;
	vector<Node> refCompressed;
	string currentString = "";
	int memory = 0;
	int currentIndex = -1, prevIndex = -1, i;
	char special = '$';
	int baseIndex = 0;
	for (i = 0; i < relativeString.size(); i++) {
		prevIndex = currentIndex;
		char additional = relativeString[i];
		currentString += additional;
		currentIndex = findIndexSubString(currentString, subStrings);

		if (currentIndex == -1) {
			updateVector(refCompressed, baseIndex, additional, prevIndex, currentString, subStrings, memory);
			currentString = "";
			baseIndex = i+1;
		}
	}

	currentString += special;
	updateVector(refCompressed, baseIndex, special, currentIndex, currentString, subStrings, memory);

	cout << "memory used by compression of main string : " << memory << endl;
	return refCompressed;
}

void printReferenceString(vector<Node>& refCompressed) {
	
	for (int i = 0; i < refCompressed.size(); i++) {
		cout << refCompressed[i].getOriginaIndex() << " " << refCompressed[i].getPointerIndex() << " " << refCompressed[i].getExtra() << endl;;
	}

	cout << "printing compressed reference string of size " << refCompressed.size() << endl;
}

char findCharIndexRefString(vector<Node>& refCompressed, int& index) {
	int first = 0;
	int last = (index <= refCompressed.size() ? index : refCompressed.size()-1);
	int mid = last / 2;
	Node node;
	while (true) {
		node = refCompressed[mid];
		if (node.getOriginaIndex() == node.getPointerIndex()) {
			return node.getExtra();
		}
		if (mid != refCompressed.size() - 1) {
			Node next = refCompressed[mid + 1];
			if (node.getOriginaIndex() < index && next.getOriginaIndex() > index) {
				if (next.getOriginaIndex() == index + 1) 
					return node.getExtra();
				else {
					int callIndex = node.getPointerIndex() + index - node.getOriginaIndex();
					return findCharIndexRefString(refCompressed, callIndex);
				}
			}
			else {
				if (node.getOriginaIndex() > index) 
					last = mid;
				else 
					first = mid;
				mid = (first + last) / 2;
			}
		}
		else {
			if (index == refCompressed.size() - 1)
				return node.getExtra();
			else {
				int callIndex = node.getPointerIndex() + index - node.getOriginaIndex();
				return findCharIndexRefString(refCompressed, callIndex);
			}
		}
	}
	return '$';
}

