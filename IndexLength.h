#include <iostream>
class IndexLength
{
private:
	int indexRelative;
	int length;
	int indexCString;
public:
	IndexLength(int indexRelative, int length, int indexCString) {
		this->indexRelative = indexRelative;
		this->length = length;
		this->indexCString = indexCString;
	}
	IndexLength() {
		IndexLength(0, 0, 0);
	}
	~IndexLength() {

	}
	int getIndexRelative() {
		return indexRelative;
	}
	int getLength() {
		return length;
	}
	int getIndexCString() {
		return indexCString;
	}
};

