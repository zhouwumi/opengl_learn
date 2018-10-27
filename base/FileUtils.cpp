#include "FileUtils.h"
#include <algorithm>
#include <stdio.h>
#include <stack>
#include <assert.h>

#include "CCRapidMaker.h"
#include "CCRapidParser.h"
#include "CCSAXParser.h"

typedef enum
{
	SAX_NONE = 0,
	SAX_KEY,
	SAX_DICT,
	SAX_INT,
	SAX_REAL,
	SAX_STRING,
	SAX_ARRAY
}SAXState;

typedef enum
{
	SAX_RESULT_NONE = 0,
	SAX_RESULT_DICT,
	SAX_RESULT_ARRAY
}SAXResult;

class DictMaker : public SAXDelegator
{
public:
	SAXResult _resultType;
	ValueMap _rootDict;
	ValueVector _rootArray;

	std::string _curKey;   ///< parsed key
	std::string _curValue; // parsed value
	SAXState _state;

	ValueMap*  _curDict;
	ValueVector* _curArray;

	std::stack<ValueMap*> _dictStack;
	std::stack<ValueVector*> _arrayStack;
	std::stack<SAXState>  _stateStack;

public:
	DictMaker()
		: _resultType(SAX_RESULT_NONE)
	{
	}

	~DictMaker()
	{
	}

	ValueMap dictionaryWithContentsOfFile(const std::string& fileName)
	{
		_resultType = SAX_RESULT_DICT;

		RapidParser parser;
		assert(parser.init("UTF-8"));
		parser.setDelegator(this);

		parser.parse(fileName);
		return _rootDict;
	}

	ValueMap dictionaryWithDataOfFile(const char* filedata, int filesize)
	{
		_resultType = SAX_RESULT_DICT;
		RapidParser parser;

		assert(parser.init("UTF-8"));
		parser.setDelegator(this);

		parser.parse(filedata, filesize);
		return _rootDict;
	}

	ValueVector arrayWithContentsOfFile(const std::string& fileName)
	{
		_resultType = SAX_RESULT_ARRAY;
		RapidParser parser;

		assert(parser.init("UTF-8"));
		parser.setDelegator(this);

		parser.parse(fileName);
		return _rootArray;
	}

	void startElement(void *ctx, const char *name, const char **atts)
	{
		CC_UNUSED_PARAM(ctx);
		CC_UNUSED_PARAM(atts);
		const std::string sName(name);
		if (sName == "dict")
		{
			if (_resultType == SAX_RESULT_DICT && _rootDict.empty())
			{
				_curDict = &_rootDict;
			}

			_state = SAX_DICT;

			SAXState preState = SAX_NONE;
			if (!_stateStack.empty())
			{
				preState = _stateStack.top();
			}

			if (SAX_ARRAY == preState)
			{
				// add a new dictionary into the array
				_curArray->push_back(Value(ValueMap()));
				_curDict = &(_curArray->rbegin())->asValueMap();
			}
			else if (SAX_DICT == preState)
			{
				// add a new dictionary into the pre dictionary
				assert(!_dictStack.empty());
				ValueMap* preDict = _dictStack.top();
				(*preDict)[_curKey] = Value(ValueMap());
				_curDict = &(*preDict)[_curKey].asValueMap();
			}

			// record the dict state
			_stateStack.push(_state);
			_dictStack.push(_curDict);
		}
		else if (sName == "key")
		{
			_state = SAX_KEY;
		}
		else if (sName == "integer")
		{
			_state = SAX_INT;
		}
		else if (sName == "real")
		{
			_state = SAX_REAL;
		}
		else if (sName == "string")
		{
			_state = SAX_STRING;
		}
		else if (sName == "array")
		{
			_state = SAX_ARRAY;

			if (_resultType == SAX_RESULT_ARRAY && _rootArray.empty())
			{
				_curArray = &_rootArray;
			}
			SAXState preState = SAX_NONE;
			if (!_stateStack.empty())
			{
				preState = _stateStack.top();
			}

			if (preState == SAX_DICT)
			{
				(*_curDict)[_curKey] = Value(ValueVector());
				_curArray = &(*_curDict)[_curKey].asValueVector();
			}
			else if (preState == SAX_ARRAY)
			{
				assert(!_arrayStack.empty());
				ValueVector* preArray = _arrayStack.top();
				preArray->push_back(Value(ValueVector()));
				_curArray = &(_curArray->rbegin())->asValueVector();
			}
			// record the array state
			_stateStack.push(_state);
			_arrayStack.push(_curArray);
		}
		else
		{
			_state = SAX_NONE;
		}
	}

	void endElement(void *ctx, const char *name)
	{
		CC_UNUSED_PARAM(ctx);
		SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
		const std::string sName((char*)name);
		if (sName == "dict")
		{
			_stateStack.pop();
			_dictStack.pop();
			if (!_dictStack.empty())
			{
				_curDict = _dictStack.top();
			}
		}
		else if (sName == "array")
		{
			_stateStack.pop();
			_arrayStack.pop();
			if (!_arrayStack.empty())
			{
				_curArray = _arrayStack.top();
			}
		}
		else if (sName == "true")
		{
			if (SAX_ARRAY == curState)
			{
				_curArray->push_back(Value(true));
			}
			else if (SAX_DICT == curState)
			{
				(*_curDict)[_curKey] = Value(true);
			}
		}
		else if (sName == "false")
		{
			if (SAX_ARRAY == curState)
			{
				_curArray->push_back(Value(false));
			}
			else if (SAX_DICT == curState)
			{
				(*_curDict)[_curKey] = Value(false);
			}
		}
		else if (sName == "string" || sName == "integer" || sName == "real")
		{
			if (SAX_ARRAY == curState)
			{
				if (sName == "string")
					_curArray->push_back(Value(_curValue));
				else if (sName == "integer")
					_curArray->push_back(Value(atoi(_curValue.c_str())));
				else
					_curArray->push_back(Value(atof(_curValue.c_str())));
			}
			else if (SAX_DICT == curState)
			{
				if (sName == "string")
					(*_curDict)[_curKey] = Value(_curValue);
				else if (sName == "integer")
					(*_curDict)[_curKey] = Value(atoi(_curValue.c_str()));
				else
					(*_curDict)[_curKey] = Value(atof(_curValue.c_str()));
			}

			_curValue.clear();
		}

		_state = SAX_NONE;
	}

	void textHandler(void *ctx, const char *ch, int len)
	{
		CC_UNUSED_PARAM(ctx);
		if (_state == SAX_NONE)
		{
			return;
		}

		SAXState curState = _stateStack.empty() ? SAX_DICT : _stateStack.top();
		const std::string text = std::string((char*)ch, len);

		switch (_state)
		{
		case SAX_KEY:
			_curKey = text;
			break;
		case SAX_INT:
		case SAX_REAL:
		case SAX_STRING:
		{
			if (curState == SAX_DICT)
			{
				assert(!_curKey.empty());
			}

			_curValue.append(text);
		}
		break;
		default:
			break;
		}
	}
};

void FileUtils::startRapidMaker()
{
	if (this->_rapidMaker)
	{
		return;
	}
	this->_rapidMaker = new RapidMaker();
}

void FileUtils::endRapidMaker()
{
	CC_SAFE_DELETE(this->_rapidMaker);
}


ValueMap FileUtils::getValueMapFromFile(const std::string& filename)
{
	if (this->_rapidMaker)
	{
		return this->_rapidMaker->dictionaryWithContentsOfFile(filename, this);
	}
	else
	{
		RapidMaker rMaker;
		return rMaker.dictionaryWithContentsOfFile(filename, this);
	}
}

ValueMap FileUtils::getValueMapFromData(const char* filedata, int filesize)
{
	if (this->_rapidMaker)
	{
		return this->_rapidMaker->dictionaryWithDataOfFile(filedata, filesize);
	}
	else
	{
		RapidMaker tMaker;
		return tMaker.dictionaryWithDataOfFile(filedata, filesize);
	}
}

ValueVector FileUtils::getValueVectorFromFile(const std::string& filename)
{
	if (this->_rapidMaker)
	{
		return this->_rapidMaker->arrayWithContentsOfFile(filename, this);
	}
	else
	{
		RapidMaker tMaker;
		return tMaker.arrayWithContentsOfFile(filename, this);
	}
}


FileUtils* FileUtils::getInstance()
{
	static FileUtils fileUtils;
	return &fileUtils;
}

bool FileUtils::isFileExistInternal(const std::string& strFilePath) const
{
	if (0 == strFilePath.length())
	{
		return false;
	}

	std::string strPath = strFilePath;
	WCHAR  utf16Buf[CC_MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, strPath.c_str(), -1, utf16Buf, sizeof(utf16Buf) / sizeof(utf16Buf[0]));

	DWORD attr = GetFileAttributesW(utf16Buf);
	if (attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY))
		return false;   //  not a file
	return true;
}

bool FileUtils::isFileExist(std::string fileName)
{
	if (isAbsolutePath(fileName))
	{
		return isFileExistInternal(fileName);
	}
	
	return false;
}

bool FileUtils::isAbsolutePath(const std::string& path) const
{
	if (path.length() > 2
		&& ((path[0] >= 'a' && path[0] <= 'z') || (path[0] >= 'A' && path[0] <= 'Z'))
		&& path[1] == ':')
	{
		return true;
	}
	return false;
}

std::string FileUtils::fullPathForFilename(const std::string &filename)
{
	// The file wasn't found, return empty string.

	return filename;
}

Data FileUtils::getDataFromFile(const std::string& filename)
{
	return getDataForFilename(filename, false);
}

Data FileUtils::getDataFromPath(const std::string& fullPath)
{
	return getData(fullPath, false);
}

Data FileUtils::getDataForFilename(const std::string& filename, bool forString)
{
	if (filename.empty())
	{
		return Data::Null;
	}

	return this->getData(this->fullPathForFilename(filename), forString);
}

std::string FileUtils::getStringFromFile(const std::string& filename)
{
	Data data = getDataForFilename(filename, true);
	if (data.isNull())
		return "";

	std::string ret((const char*)data.getBytes());
	return std::move(ret);
}


Data FileUtils::getData(const std::string& fullPath, bool forString)
{
	if (fullPath.empty())
	{
		return Data::Null;
	}

	Data ret;
	unsigned char* buffer = nullptr;
	size_t size = 0;
	size_t readsize;
	const char* mode = nullptr;

	if (forString)
		mode = "rt";
	else
		mode = "rb";

	do
	{
		// Read the file from hardware
		//std::string fullPath = fullPathForFilename(filename);
		FILE *fp = fopen(fullPath.c_str(), mode);
		CC_BREAK_IF(!fp);
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if (forString)
		{
			buffer = (unsigned char*)malloc(sizeof(unsigned char) * (size + 1));
			buffer[size] = '\0';
		}
		else
		{
			buffer = (unsigned char*)malloc(sizeof(unsigned char) * size);
		}

		readsize = fread(buffer, sizeof(unsigned char), size, fp);

		fclose(fp);

		if (forString && readsize < size)
		{
			buffer[readsize] = '\0';
		}
	} while (0);

	if (nullptr == buffer || 0 == readsize)
	{
		std::string msg = "Get data from file(";
		msg.append(fullPath).append(") failed!");
	}
	else
	{
		int outSize = readsize;
		ret.fastSet(buffer, outSize);
	}

	return std::move(ret);
}

bool FileUtils::createDirectory(const std::string& path)
{
	CC_ASSERT(!path.empty());

	if (isDirectoryExist(path))
		return true;

	// Split the path
	size_t start = 0;
	size_t found = path.find_first_of("/\\", start);
	std::string subpath;
	std::vector<std::string> dirs;

	if (found != std::string::npos)
	{
		while (true)
		{
			subpath = path.substr(start, found - start + 1);
			if (!subpath.empty())
				dirs.push_back(subpath);
			start = found + 1;
			found = path.find_first_of("/\\", start);
			if (found == std::string::npos)
			{
				if (start < path.length())
				{
					dirs.push_back(path.substr(start));
				}
				break;
			}
		}
	}


	WIN32_FILE_ATTRIBUTE_DATA wfad;
	std::wstring wpath(path.begin(), path.end());
	if (!(GetFileAttributesEx(wpath.c_str(), GetFileExInfoStandard, &wfad)))
	{
		subpath = "";
		for (unsigned int i = 0; i < dirs.size(); ++i)
		{
			subpath += dirs[i];
			if (i > 0 && !isDirectoryExist(subpath))
			{
				std::wstring wsubpath(subpath.begin(), subpath.end());
				BOOL ret = CreateDirectory(wsubpath.c_str(), NULL);
				if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
				{
					return false;
				}
			}
		}
	}
	return true;

}

bool FileUtils::isDirectoryExistInternal(const std::string& dirPath) const
{
	WIN32_FILE_ATTRIBUTE_DATA wfad;
	std::wstring wdirPath(dirPath.begin(), dirPath.end());
	if (GetFileAttributesEx(wdirPath.c_str(), GetFileExInfoStandard, &wfad))
	{
		return true;
	}
	return false;
}


bool FileUtils::isDirectoryExist(const std::string& dirPath)
{
	//CCASSERT(!dirPath.empty(), "Invalid path");

	CC_ASSERT(!dirPath.empty());

	if (isAbsolutePath(dirPath))
	{
		return isDirectoryExistInternal(dirPath);
	}

	return false;
}

bool FileUtils::writeToFile(std::string path, char* buff, size_t writeSize)
{
	bool ret = true;
	FILE* wf = fopen(path.c_str(), "wb");
	do 
	{
		if (wf){
			if (buff == NULL)
			{
				ret = false;
				break;
			}

			writeSize = writeSize >= strlen(buff) ? strlen(buff) : writeSize;
			fwrite(buff, sizeof(char), writeSize, wf);
		}
		else{
			ret = false;
			printf("open file fail");
			break;
		}
	} while (0);

	if (wf)
	{
		fclose(wf);
		wf = NULL;
	}

	return ret;
}

void FileUtils::setDefaultSearchRoot(std::string root)
{
	_defaultSearcheRoot = root;
}

void FileUtils::addSearchPath(std::string path, bool font)
{
	std::string prefix = _defaultSearcheRoot;
	if (!isAbsolutePath(path))
	{
		path = prefix + path;
	}

	if (path.length() > 0 && path[path.length() - 1] != '/')
	{
		path += "/";
	}
	if (font)
	{
		_searchPathArray.insert(_searchPathArray.begin(), path);
	}
	else{
		_searchPathArray.push_back(path);
	}

}

void FileUtils::removeSearchPath(std::string path)
{
	std::string prefix = _defaultSearcheRoot;
	if (!isAbsolutePath(path))
	{
		path = prefix + path;
	}
	if (path.length() > 0 && path[path.length() - 1] != '/')
	{
		path += "/";
	}

	for (auto it = _searchPathArray.begin(); it < _searchPathArray.end();it++)
	{
		if (path.compare(*it) == 0)
		{
			_searchPathArray.erase(it);
		}
	}
}