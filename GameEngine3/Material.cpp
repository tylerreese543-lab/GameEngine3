#include "Material.h"

Material::Material() : Instance() {

	diffuseColor = vec3(1.0);
	specularity = 1.0f;

	//Texture Resource Initialization
	textures[eMaterialTexture_Diffuse] = -1;
	textures[eMaterialTexture_AmbientOcclusion] = -1;
	textures[eMaterialTexture_Normal] = -1;
	textures[eMaterialTexture_Specular] = -1;
	shader = -1;
	type = InstanceType_Material;
	name = "Material";
	SetTextureUseFlags(false, false, false, false);
}

void Material::SetTextureUseFlags(bool useDiffuse, bool useAmbientOcclusion, bool useNormal, bool useSpecular) {
	textureUseFlags[eMaterialTexture_Diffuse] = useDiffuse;
	textureUseFlags[eMaterialTexture_AmbientOcclusion] = useAmbientOcclusion;
	textureUseFlags[eMaterialTexture_Normal] = useNormal;
	textureUseFlags[eMaterialTexture_Specular] = useSpecular;
}

//Searches through all of it's children for viable textures.
void Material::autoPrepare(bool autoFlags) {
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
	for (Instance*& child : children) {
		ShaderProgram* shdr = isShader(child);
		if (!shdr) continue;
		shader = shdr->getId();
		break;
	}
}

void Material::applyMaterial() {
	
	ShaderProgram* program = isShader(getInstanceById(shader));
	if (!program) return;
	
	program->useProgram();
	for (int i = 0; i < eMaterialTexture_MAX; ++i) {
		if (textureUseFlags[i] == true) {
			ImageResource* img = isImage(getInstanceById(textures[i]));
			if (!img) continue;
			program->bindTexture(program->shaderTextureNames[i], img);
		}
	}

	program->setUniform3f(program->shaderUniformNames[eSUniform_Material_DiffuseColor], diffuseColor.X, diffuseColor.Y, diffuseColor.Z);
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

ShaderProgram* Material::isShader(Instance* shdr) {
	if (!shdr) return nullptr;
	if (shdr->type != InstanceType_ShaderProgram) return nullptr; //Skip any non-resources
	if (!isRegistered(shdr)) return nullptr;
	ShaderProgram* shader = dynamic_cast<ShaderProgram*>(shdr);
	return shader;
}

void Material::setTexture(ImageResource* i) {
	i = isImage(i);
	if (!i) return;
	textures[i->textureType] = i->getId();
}
void Material::setShader(ShaderProgram* i) {
	i = isShader(i);
	if (!i) return;
	shader = i->getId();
}

int Material::getShaderInstanceId() {
	return shader;
}
int Material::getTextureInstanceId(int target) {
	if (target < 0 || target > eMaterialTexture_MAX) return -1;
	return textures[target];
}
Instance* Material::clone_this() {
	return registerInstance(new Material(*this), true);
}