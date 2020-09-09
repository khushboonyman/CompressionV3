#include<unordered_map>
#include<iostream>
#include<string>

using namespace std;

class Node
{
private:
	int originalIndex;
	int pointerIndex;
	char extra;
public:
	Node(int originalIndex, char extra) {
		this->originalIndex = originalIndex;
		this->extra = extra;
		this->pointerIndex = -1;
	}
	Node() {
		Node(0, '$');
	}
	void assignPrev(int pointerIndex) {
		this->pointerIndex = pointerIndex;
	}
	int getOriginaIndex() {
		return this->originalIndex;
	}
	int getPointerIndex() {
		return this->pointerIndex;
	}
	char getExtra() {
		return this->extra;
	}
};

vector<Node> compressReference(string& relativeString);

int findIndexSubString(string& current, unordered_map<string, int>& subStrings);

void updateVector(vector<Node>& refCompressed, int& i, char& additional, int& prevIndex, string& currentString, unordered_map<string, int>& subStrings, int& memory);

void printReferenceString(vector<Node>& refCompressed);

char findCharIndexRefString(vector<Node>& refCompressed, int& index);

