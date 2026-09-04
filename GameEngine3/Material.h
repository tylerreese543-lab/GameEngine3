#pragma once
#include "GameInstance.h"
#include "TextureResource.h"
#include "Graphics/GraphicsWrapper.h"
#include "Graphics/matrix.h"
#include "Shader.h"
class Material : public Instance
{
	ImageResource* isImage(Instance* inst);
	ShaderProgram* isShader(Instance* inst);

	int textures[eMaterialTexture_MAX];
	int shader;
public:
	//Can be from another instance, independent, or it's own children

	bool textureUseFlags[eMaterialTexture_MAX];

	vec3 diffuseColor;
	float specularity;
	
	Material();

	void setTexture(ImageResource* index);
	void setShader(ShaderProgram* index);
	void setTextureById(int id, int target);
	//Will automatically look for child textures if a flag is present but no texture is bound.
	void autoPrepare(bool autoFlags);
	void SetTextureUseFlags(bool useDiffuse, bool useAmbientOcclusion, bool useNormal, bool useSpecular);
	void applyMaterial();

	int getShaderInstanceId();
	int getTextureInstanceId(int target);
	virtual Instance* clone_this();

	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int off = 0);

};

