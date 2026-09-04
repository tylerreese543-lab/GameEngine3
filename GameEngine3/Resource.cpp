#include "Resource.h"


std::map<std::string, std::string> Resource::resourceFilePathMacros = {
	{"res:/", "assets"},
	{"core:/", "core"}
};

std::string Resource::resolveFilePath(const std::string& path) {
	std::string resolvedPath = path;
	for (const auto& macro : resourceFilePathMacros) {
		const std::string& macroKey = macro.first;
		const std::string& macroValue = macro.second;
		size_t pos = resolvedPath.find(macroKey);
		if (pos != std::string::npos) {
			resolvedPath.replace(pos, macroKey.length(), macroValue);
		}
	}
	printf("Resolved file path: %s\n", resolvedPath.c_str());
	return resolvedPath;
}
Resource::Resource() : Instance() {
	name = "Resource";
	type = InstanceType_Resource;
}

Resource::~Resource() {
}

Instance* Resource::clone_this() {
	Resource* r = (Resource*)registerInstance(new Resource(*this), true);
	return r;
}

FILE* Resource::openResourceFile(std::string filepath, const char* mode) {
	std::string resolvedPath = resolveFilePath(filepath);
	FILE* file;
	fopen_s(&file, resolvedPath.c_str(), mode);
	if (!file) {
		printf("ERROR: Failed to open resource file '%s'.\n", resolvedPath.c_str());
		return nullptr;
	}
	return file;
}