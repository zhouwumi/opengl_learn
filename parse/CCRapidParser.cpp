#include "CCRapidParser.h"

#include "CCSAXParser.h"

#include "rapid/rapidxml_utils.hpp"
#include "rapid/rapidxml_print.hpp"



RapidParser::RapidParser()
{
}


RapidParser::~RapidParser()
{
	this->_delegator = nullptr;
}

void RapidParser::visitNode(rapidxml::xml_node<> *parent)
{
	this->_delegator->startElement(this, parent->name(), nullptr);
	if (0 != *parent->value())
	{
		this->_delegator->textHandler(this, parent->value(), parent->value_size());
	}
	else
	{
		auto child = parent->first_node();
		while (nullptr != child)
		{
			this->visitNode(child);
			child = child->next_sibling();
		}
	}
	this->_delegator->endElement(this, parent->name());
}

bool RapidParser::init(const char *encoding)
{
	return true;
}
bool RapidParser::parse(const char* xmlData, size_t dataLength)
{
	rapidxml::xml_document<> doc;
	doc.parse<0>(const_cast<char *>(xmlData));
	rapidxml::xml_node<>* root = doc.first_node();
	visitNode(root);
	return true;
}
bool RapidParser::parse(const std::string& filename)
{
	try
	{
		rapidxml::file<> fdoc(filename.c_str());//¶ÁÈ¡Êý¾Ý  
		return this->parse(fdoc.data(), fdoc.size());
	}
	catch (std::exception e)
	{
		return false;
	}
}
void RapidParser::setDelegator(SAXDelegator* delegator)
{
	this->_delegator = delegator;
}


