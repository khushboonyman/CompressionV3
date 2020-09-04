#pragma once
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
	int getIndexRelative() {
		return this->indexRelative;
	}
	int getLength() {
		return this->length;
	}
	int getIndexCString() {
		return this->indexCString;
	}
};

