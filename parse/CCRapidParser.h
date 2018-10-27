#ifndef __CCRAPIDPARSER_H__
#define __CCRAPIDPARSER_H__



#include "rapid/rapidxml.hpp"
#include <string>



class SAXDelegator;

class  RapidParser
{
	SAXDelegator*    _delegator;
public:
	RapidParser();
	~RapidParser();

	/**
	* @js NA
	* @lua NA
	*/
	bool init(const char *encoding);
	/**
	* @js NA
	* @lua NA
	*/
	bool parse(const char* xmlData, size_t dataLength);
	/**
	* @js NA
	* @lua NA
	*/
	bool parse(const std::string& filename);
	/**
	* @js NA
	* @lua NA
	*/
	void setDelegator(SAXDelegator* delegator);

private:
	void visitNode(rapidxml::xml_node<> * parent);
};



#endif
