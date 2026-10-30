#pragma once
#include "Resource.h"
#include "Graphics/graphics_include.h"
#define STB_IMAGE_IMPLEMENTATION
class ImageResource : public Resource {
private:
	int width;
	int height;
	unsigned char* Data;
	int imageSizeRaw;
	int Channels;
	unsigned int GLTextureSlot;
	GLenum typeLoadFile;
	GLenum GLTextureParameters[4];

public:
	unsigned int GLTextureID;
	bool loaded;
	int textureType;


	static const std::string textureTypeNames[5];
	void loadFromFile(std::string filepath, GLenum fileType = -1);
	void SetGLLoadParams(GLenum WRAP_U, GLenum WRAP_V, GLenum MIN_FILTER, GLenum MAG_FILTER);
	void BindSlot(int slot);
	ImageResource(int textureType = -1, std::string filepath = "");
	virtual ~ImageResource();
	int getWidth();
	int getHeight();
	char* getImageData();
	int getImageBufferLength();
	int getNumChannels();

	virtual Instance* clone_this();

	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off);
	virtual int getNumSaveBytes();
};
