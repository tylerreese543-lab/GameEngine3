#include "Material.h"

Material::Material() {

	diffuseColor = col3(1.0);
	specularity = 1.0f;

	//Texture Resource Initialization
	textures[eMaterialTexture_Diffuse] = -1;
	textures[eMaterialTexture_AmbientOcclusion] = -1;
	textures[eMaterialTexture_Normal] = -1;
	textures[eMaterialTexture_Specular] = -1;

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
			ImageResource* img = isImage(child);

			if (!img) continue;
			if (!img->loaded) img->loadFromFile(img->filePath);
			


			textures[img->textureType] = img->getId();
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
			ImageResource* img = isImage(getInstanceById(textures[i]));
			if (!img) continue;
			program->bindTexture(program->shaderTextureNames[i], img);
		}
	}

	program->setUniform3f(program->shaderUniformNames[eSUniform_Material_DiffuseColor], diffuseColor.R, diffuseColor.G, diffuseColor.B);
	program->setUniform3f(program->shaderUniformNames[eSUniform_Material_Specularity], specularity);
}

ImageResource* Material::isImage(Instance* image) {
	if (!image) return nullptr;
	if (image->type != InstanceType_ImageResource) return nullptr; //Skip any non-resources
	if (!isRegistered(image)) return nullptr;
	ImageResource* img = dynamic_cast<ImageResource*>(image);
	if (img->textureType < 0 || img->textureType > eMaterialTexture_MAX) return nullptr; // -1 = not a texture resource, don't use this
	return img;
}