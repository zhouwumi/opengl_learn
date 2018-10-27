#include "Data.h"

const Data Data::Null;

Data::Data() :bytes(nullptr), size(0)
{

}

Data::Data(const Data& data)
{
	copy(data.bytes, data.size);
}

Data::Data(Data && data)
{
	move(data);
}

Data& Data::operator=(const Data& data)
{
	copy(data.bytes, data.size);
	return *this;
}

Data& Data::operator=(Data && data)
{
	move(data);
	return *this;
}

void Data::fastSet(unsigned char* buff, size_t size)
{
	this->bytes = buff;
	this->size = size;
}

void Data::copy(unsigned char* buff,size_t size)
{
	clear();
	if (size > 0){
		this->size = size;
		this->bytes = (unsigned char*)malloc(this->size);
		memcpy_s(this->bytes, size* sizeof(unsigned char), buff, size*sizeof(unsigned char));
	}
	
}

void Data::move(Data& data)
{
	this->size = data.size;
	this->bytes = data.bytes;
	data.bytes= nullptr;
}

bool Data::isNull() const
{
	return (bytes == nullptr || size == 0);
}

unsigned char* Data::getBytes() const
{
	return bytes;
}


Data::~Data()
{
	clear();
}

void Data::clear()
{
	if (this->bytes){
		free(this->bytes);
		this->bytes = nullptr;
	}
	this->size = 0;
}