#include "TextureResource.h"
#include "stb_image/stb_image.h"
#include "Graphics/GraphicsWrapper.h"
ImageResource::ImageResource(int textureType, std::string filepath) : Resource() {
	name = "Texture";
	width = 0;
	height = 0;
	Channels = 0;
	Data = nullptr;
	typeLoadFile = GL_RGB;
	imageSizeRaw = 0;
	GLTextureID = 0;
	type = InstanceType_ImageResource;
	SetGLLoadParams(GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST);
	loaded = false;
	//Do not use this for any textures
	this->textureType = textureType;
	loadFromFile(filepath);
}
void ImageResource::loadFromFile(std::string filepath, GLenum fileType) {
	filePath = filepath;
	if (loaded == true || filePath == "") return;
	loaded = true;

	//Auto determines the load type depending on arguments passed. Makes code easier to read but also allows complex modification.
	if (textureType == -1) {
		if (fileType == -1) {
			typeLoadFile = GL_RGB;
		}
		else {
			typeLoadFile = fileType;
		}
	}
	else {
		if (fileType == -1) {
			switch (textureType) {
				case eMaterialTexture_Diffuse:
				case eMaterialTexture_Normal:
					typeLoadFile = GL_RGB;
					break;
				case eMaterialTexture_AmbientOcclusion:
				case eMaterialTexture_Specular:
					typeLoadFile = GL_RED;
					break;
				default:
					typeLoadFile = GL_RGB;
					break;
			}
		}
		else {
			typeLoadFile = fileType;
		}
	}


	stbi_set_flip_vertically_on_load(1);
	glGenTextures(1, &GLTextureID);

	glBindTexture(GL_TEXTURE_2D, GLTextureID);
	int channels = 0;
	switch (typeLoadFile) {
	case GL_RED:
		channels = 1;
		break;
	case GL_RG:
		channels = 2;
		break;
	case GL_RGB:
		channels = 3;
		break;
	case GL_RGBA:
		channels = 4;
		break;
	default:
		typeLoadFile = GL_RGB;
		channels = 3;
		break;
	}
	Data = stbi_load(filePath.c_str(), &width, &height, &Channels, channels);
	
	if (!Data) {
		printf("ERROR: Couldn't load texture: %s", stbi_failure_reason());
		return;
	}
	if (textureType > -1) {
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, typeLoadFile, width, height, 0, typeLoadFile, GL_UNSIGNED_BYTE, Data);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GLTextureParameters[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GLTextureParameters[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GLTextureParameters[2]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLTextureParameters[3]);

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		if (Data) {
			stbi_image_free(Data);
			Data = nullptr;
		}
	}
}
void ImageResource::SetGLLoadParams(GLenum WRAP_U, GLenum WRAP_V, GLenum MIN_FILTER, GLenum MAG_FILTER) {
	GLTextureParameters[0] = WRAP_U;
	GLTextureParameters[1] = WRAP_V;
	GLTextureParameters[2] = MIN_FILTER;
	GLTextureParameters[3] = MAG_FILTER;
}

void ImageResource::BindSlot(int s) {
	GLTextureSlot = s;
	glActiveTexture(GL_TEXTURE0 + GLTextureSlot);
	glBindTexture(GL_TEXTURE_2D, GLTextureID);
}

ImageResource::~ImageResource() {
	if (Data) {
		stbi_image_free(Data);
		Data = nullptr;
	}
}

Instance* ImageResource::clone_this() {
	ImageResource* i = (ImageResource*)registerInstance(new ImageResource(*this), true);
	loaded = false;
	loadFromFile(filePath);
	return i;
}