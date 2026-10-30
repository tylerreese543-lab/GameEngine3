#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Mesh.h"

struct Character {
    unsigned int LayerIndex; // Which slice of the 3D array it lives on
    int SizeX, SizeY;        // Size of glyph (width, height)
    int BearingX, BearingY;  // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to next character
};
class Font {
public:
	static FT_Library ft;
	int fontSize;
    std::map<char, Character> Characters; // Map of characters to their metrics
	GLuint textureId;
	std::string name;
	std::string filepath;
	bool loaded;
    void loadFromFile(std::string filepath, unsigned int fontSize = 32);

	int serializeToFile(FILE* file);
	int getNumSaveBytes();
	static int deserializeFromFile(void* data, int offset = 0);
	Font();

};
class TextGeometry : public Mesh
{
protected:
	virtual void setupRenderSettings(bool transparent = false, bool backfaceCulling = true);
public:
	static std::vector<std::string> fontNames; // mainly for EngineGUI;
    static std::map<std::string, Font> fontCharacterMaps; // Map of font file paths to their character maps
	Buffer vbos[eVBO_MAX];
	GLuint vao;
	TextGeometry();
	static void loadFont(const std::string& path);

	std::string font;
	std::string text;
	bool isDirty;


	void setText(std::string text);
	void setFont(std::string font);
	virtual void render(Camera* camera, bool transparent = false, int renderLayerTarget = eRenderLayer_World);
	
	int generateVAO(BufferLayout* layout);

	void generateGeometry();

	virtual Instance* clone_this();

	virtual int getNumSaveBytes();
	virtual int saveInstanceToFile(FILE* file);
	virtual int loadInstanceFromFile(void* data, int offset = 0);

	static void destroyFreeType();

};

// A structural metric tracker to render the characters later

