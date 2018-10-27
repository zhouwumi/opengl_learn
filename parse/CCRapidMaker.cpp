#include "CCRapidMaker.h"

#include "Data.h"

#include "rapid/rapidxml_utils.hpp"
#include "rapid/rapidxml_print.hpp"



RapidMaker::RapidMaker()
{

}

RapidMaker::~RapidMaker()
{

}

ValueMap RapidMaker::dictionaryWithContentsOfFile(const std::string& fileName, FileUtils* utils /*= nullptr*/)
{
	FileUtils *fileUtil = utils == nullptr ? FileUtils::getInstance() : utils;
	std::string  && data = fileUtil->getStringFromFile(fileName);
	if (!data.empty())
	{
		return this->dictionaryWithDataOfFile(data.c_str());
	}
	return ValueMap();
}
ValueMap RapidMaker::dictionaryWithDataOfFile(const char* filedata)
{
	rapidxml::xml_node<> * root = this->rootWithDataOfFile(this->_doc, filedata);
	return this->dictionaryWithNode(root);
}
ValueMap RapidMaker::dictionaryWithDataOfFile(const char* filedata,int filesize)
{
	rapidxml::file<> file(filedata, filesize);
	return this->dictionaryWithDataOfFile(file.data());
}

ValueVector RapidMaker::arrayWithContentsOfFile(const std::string& fileName, FileUtils* utils /*= nullptr*/)
{
	FileUtils *fileUtil = utils == nullptr ? FileUtils::getInstance() : utils;
	std::string  && data = fileUtil->getStringFromFile(fileName);
	if (!data.empty())
	{
		return this->arrayWithDataOfFile(data.c_str());
	}
	return ValueVector();
}
ValueVector RapidMaker::arrayWithDataOfFile(const char* filedata)
{
	rapidxml::xml_node<> * root = this->rootWithDataOfFile(this->_doc, filedata);
	return this->arrayWithNode(root);
}
ValueVector RapidMaker::arrayWithDataOfFile(const char* filedata,int filesize)
{
	rapidxml::file<> file(filedata, filesize);
	return this->arrayWithDataOfFile(file.data());
}

rapidxml::xml_node<> * RapidMaker::rootWithDataOfFile(const char* filedata)
{
	return this->rootWithDataOfFile(this->_doc, filedata);
}

rapidxml::xml_node<> * RapidMaker::rootWithDataOfFile(rapidxml::xml_document<> & doc, const char* filedata)
{
	doc.parse<0>(const_cast<char *>(filedata));
	auto root = doc.first_node();
	return root->first_node();
}

ValueMap RapidMaker::dictionaryWithNode(rapidxml::xml_node<> * node)
{
	ValueMap map;
	auto child = node->first_node();
	while (child)
	{
		auto key = child->value();
		child = child->next_sibling();
		map[key] = valueWithNode(child);
		child = child->next_sibling();
	}
	return std::move(map);
}
ValueVector RapidMaker::arrayWithNode(rapidxml::xml_node<> * node)
{
	ValueVector vector;
	auto child = node->first_node();
	while (child)
	{
		vector.push_back(valueWithNode(child));
		child = child->next_sibling();
	}
	return std::move(vector);
}

Value RapidMaker::valueWithNode(rapidxml::xml_node<> * node)
{
	auto name = node->name();
	switch (*name)
	{
	case 97: // array
		return Value(arrayWithNode(node));
	case 100: // dict
		return Value(dictionaryWithNode(node));
	case 102: // false
		return Value(false);
	case 105: // integer
		return Value(atoi(node->value()));
	case 114: // real
		return Value(atof(node->value()));
	case 115: // true
		return Value(node->value());
	case 116: // true
		return Value(true);
	default:
		printf("there is not a key type:%s", name);
		return Value();
	}
}

