#include "Material.h"

Material::Material() {

	diffuseColor = col3(1.0);
	specularity = 1.0f;

	//Texture Resource Initialization
	textures[eMaterialTexture_Diffuse] = nullptr;
	textures[eMaterialTexture_AmbientOcclusion] = nullptr;
	textures[eMaterialTexture_Normal] = nullptr;
	textures[eMaterialTexture_Specular] = nullptr;

	SetTextureUseFlags(false, false, false, false);
}

void Material::SetTextureUseFlags(bool useDiffuse, bool useAmbientOcclusion, bool useNormal, bool useSpecular) {
	textureUseFlags[eMaterialTexture_Diffuse] = useDiffuse;
	textureUseFlags[eMaterialTexture_AmbientOcclusion] = useAmbientOcclusion;
	textureUseFlags[eMaterialTexture_Normal] = useNormal;
	textureUseFlags[eMaterialTexture_Specular] = useSpecular;
}

//Searches through all of it's children for viable textures.
void Material::autoSearchTextures(bool autoFlags) {
	for (int i = eMaterialTexture_Diffuse; i < eMaterialTexture_MAX; ++i) {
		if (textureUseFlags[i] == false && (!autoFlags)) continue;
		for (Instance*& child : children) {
			if (child->type != InstanceType_ImageResource) continue; //Skip any non-resources
			ImageResource* img = dynamic_cast<ImageResource*>(child);
			if (img->textureType < 0 || img->textureType > eMaterialTexture_MAX || !img->loaded) continue; // -1 = not a texture resource, don't use this
			
			if (textures[img->textureType] != nullptr) continue; //Check if there isn't already a valid texture-preset

			textures[img->textureType] = img;
			if (autoFlags) {
				textureUseFlags[img->textureType] = true;
			}

		}
	}
}

void Material::applyMaterial(ShaderProgram* program) {
	if (program == nullptr) return;
	
	for (int i = 0; i < eMaterialTexture_MAX; ++i) {
		if (textureUseFlags[i] == true) {
			program->bindTexture(program->shaderTextureNames[i], textures[i]);
		}
	}

	program->setUniform3f(program->shaderUniformNames[eSUniform_Material_DiffuseColor], diffuseColor.R, diffuseColor.G, diffuseColor.B);
	program->setUniform3f(program->shaderUniformNames[eSUniform_Material_Specularity], specularity);
}