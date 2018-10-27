#ifndef __CCRAPIDMAKER_H__
#define __CCRAPIDMAKER_H__



#include "CCValue.h"
#include <string>
#include "rapid/rapidxml.hpp"
#include "FileUtils.h"



class RapidMaker
{
public:
	RapidMaker();
	virtual ~RapidMaker();

	ValueMap dictionaryWithContentsOfFile(const std::string& fileName, FileUtils* utils = nullptr);
	ValueMap dictionaryWithDataOfFile(const char* filedata);
	ValueMap dictionaryWithDataOfFile(const char* filedata, int filesize);

	ValueVector arrayWithContentsOfFile(const std::string& fileName, FileUtils* utils = nullptr);
	ValueVector arrayWithDataOfFile(const char* filedata);
	ValueVector arrayWithDataOfFile(const char* filedata, int filesize);
private:
	ValueMap dictionaryWithNode(rapidxml::xml_node<> * node);
	ValueVector arrayWithNode(rapidxml::xml_node<> * node);
	Value valueWithNode(rapidxml::xml_node<> * node);
	rapidxml::xml_node<> * rootWithDataOfFile(const char* filedata);
	rapidxml::xml_node<> * rootWithDataOfFile(rapidxml::xml_document<>& doc, const char* filedata);

	rapidxml::xml_document<> _doc;
};

#endif

