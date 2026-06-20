#pragma once
#include "GameInstance.h"
#include "TextureResource.h"
#include "Graphics/GraphicsWrapper.h"
#include "Graphics/matrix.h"
#include "Shader.h"
class Material : public Instance
{
public:
	//Can be from another instance, independent, or it's own children
	ImageResource* textures[eMaterialTexture_MAX];
	bool textureUseFlags[eMaterialTexture_MAX];

	col3 diffuseColor;
	float specularity;
	
	Material();

	//Will automatically look for child textures if a flag is present but no texture is bound.
	void autoSearchTextures(bool autoFlags);
	void SetTextureUseFlags(bool useDiffuse, bool useAmbientOcclusion, bool useNormal, bool useSpecular);
	void applyMaterial(ShaderProgram* program);
};

