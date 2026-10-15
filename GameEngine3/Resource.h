#pragma once

#include "GameInstance.h"
#include <filesystem>
class Resource : public Instance {
protected:
	static std::map<std::string, std::string> resourceFilePathMacros;
	static FILE* openResourceFile(std::string filepath, const char* mode = "rb");
public:
	static std::string resolveFilePath(const std::string& path);
	std::string filePath;
	Resource();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);
	virtual int getNumSaveBytes();
	virtual ~Resource();
	virtual Instance* clone_this();
};