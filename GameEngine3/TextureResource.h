#pragma once
#include "Resource.h"

class TextureResource : public Resource {
public:
	char* Data;
	int Channels;
	std::string filePath;
	int width;
	int height;

	void loadFromFile(std::string filepath);

	TextureResource();
};