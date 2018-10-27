//FileUtils.h
#ifndef _FILEUTILS_H
#define _FILEUTILS_H

#include "Data.h"
#include "CommonMacros.h"
#include <windows.h>
#include <vector>
#include "CCValue.h"


class RapidMaker;

#define  CC_MAX_PATH 512

class FileUtils{
public:
	static FileUtils* getInstance();

	bool isFileExist(std::string fileName);
	bool isDirectoryExist(const std::string& dirPath);
	
	bool createDirectory(const std::string& path);
	std::string FileUtils::fullPathForFilename(const std::string &filename);
	bool isAbsolutePath(const std::string& path) const;


	Data getData(const std::string& fullPath, bool forString);
	Data getDataForFilename(const std::string& filename, bool forString);
	Data getDataFromFile(const std::string& filename);
	Data getDataFromPath(const std::string& fullPath);

	std::string getStringFromFile(const std::string& filename);

	bool writeToFile(std::string path, char* buff,size_t  writeSize);

	void addSearchPath(std::string  path, bool font = false);
	void removeSearchPath(std::string path);

	void setDefaultSearchRoot(std::string root);


	void startRapidMaker();

	void endRapidMaker();

	ValueMap getValueMapFromFile(const std::string& filename);

	 ValueMap getValueMapFromData(const char* filedata, int filesize);
	 ValueVector getValueVectorFromFile(const std::string& filename);
private:
	bool isFileExistInternal(const std::string& strFilePath) const;
	bool isDirectoryExistInternal(const std::string& dirPath) const;

	std::vector<std::string> _searchPathArray;

	std::string  _defaultSearcheRoot;

	RapidMaker* _rapidMaker;
};

#endif // !_FILEUTILS_H