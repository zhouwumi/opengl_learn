#ifndef  _ENCRYPE_TOOL_DATA_H
#define  _ENCRYPE_TOOL_DATA_H


#include <stdlib.h>
#include <string>
#include <utility>

class Data{
public:
	static const Data Null;
	Data();
	Data(const Data& data);
	Data(Data && data);
	Data& operator = (const Data& data);
	Data& operator = (Data && data);
	
	void fastSet(unsigned char* buff, size_t size);
	void copy(unsigned char* buff, size_t size);
	~Data();
	unsigned char* getBuff(){ return this->bytes; };
	size_t getSize(){ return this->size; };
	void Data::clear();

	/**
	* @js NA
	* @lua NA
	*/
	unsigned char* getBytes() const;

	/** Check whether the data is null. */
	bool isNull() const;
private:
	void move(Data& data);
	unsigned char* bytes;
	size_t size;
};

#endif