#include "TextGeometry.h"

FT_Library Font::ft;
std::map<std::string, Font> TextGeometry::fontCharacterMaps;
std::vector<std::string> TextGeometry::fontNames;
void Font::loadFromFile(std::string filepath, unsigned int fontSize) {
    this->fontSize = fontSize;
	this->filepath = filepath;
    FT_Face face;
    if (FT_New_Face(ft, Resource::resolveFilePath(filepath).c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    name = std::string(face->family_name);
	debug_log("Loaded font: %s from file: %s\n", name.c_str(), filepath.c_str());
    // Set size to load glyphs as (0 means dynamically calculate width based on height)
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // ----------------------------------------------------
    // STEP 1: Generate and Initialize the Texture Array
    // ----------------------------------------------------s
    
    Characters.clear();
    if (textureId != 0) {
        glDeleteTextures(1, &textureId);
        textureId = 0;
    }
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureId);

    // We will load the standard ASCII printable characters (128 total layers)
    // We use GL_R8 because FreeType bitmaps are 8-bit single-channel grayscale maps
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY,
        0,                 // Mipmap level
        GL_R8,             // Internal Format (Single channel 8-bit)
        fontSize,          // Max Width (using fontSize ensures a safe bounding box)
        fontSize,          // Max Height
        128,               // Number of layers (Capacity for ASCII characters)
        0,                 // Border (Must be 0)
        GL_RED,            // Format of source data
        GL_UNSIGNED_BYTE,  // Data type of source data
        nullptr            // Don't pass data yet, just allocate memory block
    );

    // ----------------------------------------------------
    // STEP 2: Configure FreeType Alignment & Load Glyphs
    // ----------------------------------------------------
    // CRITICAL: FreeType bitmaps are 1-byte aligned. OpenGL defaults to 4-byte rows.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++) {
        // Load character glyph 
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph " << c << std::endl;
            continue;
        }

        // Push the loaded glyph bitmap into its dedicated layer in the array
        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,                             // Mipmap level
            0, 0, c,                       // X offset, Y offset, Z offset (Layer index matches ASCII value)
            face->glyph->bitmap.width,     // Source Width
            face->glyph->bitmap.rows,      // Source Height
            1,                             // Sub-image depth (1 layer at a time)
            GL_RED,                        // Source layout
            GL_UNSIGNED_BYTE,              // Source type
            face->glyph->bitmap.buffer     // The raw pixel array from FreeType
        );

        // Store character metrics for custom vertex scaling during rendering
        Character character = {
            c, // Layer index matches ASCII decimal directly here
            (int)face->glyph->bitmap.width,
            (int)face->glyph->bitmap.rows,
            face->glyph->bitmap_left,
            face->glyph->bitmap_top,
            (unsigned int)face->glyph->advance.x
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }

    // ----------------------------------------------------
    // STEP 3: Set Texture Parameters & Cleanup
    // ----------------------------------------------------
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Reset pixel store alignment back to default layout safety
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    // Free FreeType resources
    FT_Done_Face(face);
    this->loaded = true;
}

Font::Font() {
    name = "";
    textureId = 0;
    fontSize = 0;
}
void TextGeometry::loadFont(const std::string& path) {
    if (fontCharacterMaps.find(path) == fontCharacterMaps.end()) {
        Font newFont;
        newFont.loadFromFile(path, 128);
        fontCharacterMaps[newFont.name] = newFont;
        fontNames.push_back(newFont.name);
    }
}

void TextGeometry::generateGeometry() {
    if (fontCharacterMaps.find(font) == fontCharacterMaps.end()) {
        debug_log("Font '%s' not loaded. Call 'loadFont()' first.\n", font.c_str());
        return;
	}

    if (!isDirty) return;
    debug_log("Text Is Dirty\n");
    Font* f = &fontCharacterMaps[font];
	//Only need these 3 VBOs for text rendering: positions, UVs, and extra data (like color or other attributes)
    if (vbos[eVBO_Positions].data != nullptr) {
		delete[] vbos[eVBO_Positions].data;
        vbos[eVBO_Positions].data = nullptr;
    }
    if (vbos[eVBO_UV].data != nullptr) {
		delete[] vbos[eVBO_UV].data;
        vbos[eVBO_UV].data = nullptr;
    }
    if (vbos[eVBO_Extra].data != nullptr) {
		delete[] vbos[eVBO_Extra].data;
        vbos[eVBO_Extra].data = nullptr;
    }
	vbos[eVBO_Positions].data = new vec3[text.length() * 6]; // 6 vertices per character, 3 coordinates each
	vbos[eVBO_UV].data = new vec2[text.length() * 6]; // 6 vertices per character, 2 UV coordinates each
	vbos[eVBO_Extra].data = new float[text.length() * 6]; // 6 vertices per character, 1 extra attribute (layer index) each

	vbos[eVBO_Positions].element_size = sizeof(vec3); // For positions
	vbos[eVBO_UV].element_size = sizeof(vec2); // For UVs
	vbos[eVBO_Extra].element_size = sizeof(float); // For extra attributes

    vbos[eVBO_Positions].calcSize(text.length() * 6);
    vbos[eVBO_UV].calcSize(text.length() * 6);
    vbos[eVBO_Extra].calcSize(text.length() * 6);

    int index = 0;

	vec2 current; // Starting X position for the first character
	float scale = 1.0f; // You can adjust this scale based on your needs
    debug_log("Updating Text for '%s' to '%s'\n", name.c_str(), text.c_str());
    for (char c : text) {
        if (f->Characters.find(c) == f->Characters.end()) {
            debug_log("Character '%c' not found in font '%s'.\n", c, font.c_str());
            continue;
        }
        Character ch = f->Characters[c];

		float xpos = current.X + ch.BearingX * scale;
		float ypos = 0.0f - (ch.SizeY - ch.BearingY) * scale;

		float width = ch.SizeX * scale;
        float height = ch.SizeY * scale;
        
        float texWidth = ch.SizeX / (float)f->fontSize;
        float texHeight = ch.SizeY / (float)f->fontSize;
        float texLayer = (float)ch.LayerIndex;
        // Here you would generate the vertex data for each character based on its metrics
        // and store it in your VBOs. This is a placeholder for actual geometry generation logic.
        
        ((float*)(vbos[eVBO_Extra].data))[index] =      texLayer;
        ((float*)(vbos[eVBO_Extra].data))[index + 1] =  texLayer;
        ((float*)(vbos[eVBO_Extra].data))[index + 2] =  texLayer;
        ((float*)(vbos[eVBO_Extra].data))[index + 3] =  texLayer;
        ((float*)(vbos[eVBO_Extra].data))[index + 4] =  texLayer;
        ((float*)(vbos[eVBO_Extra].data))[index + 5] =  texLayer;


		((vec3*)(vbos[eVBO_Positions].data))    [index] = vec3(xpos, ypos + height, 0.0f); // Placeholder for vertex position
        ((vec2*)(vbos[eVBO_UV].data))           [index] = vec2(0.0f, 0.0f);

        ((vec3*)(vbos[eVBO_Positions].data))    [index + 1] = vec3(xpos, ypos, 0.0f); // Placeholder for vertex position
        ((vec2*)(vbos[eVBO_UV].data))           [index + 1] = vec2(0.0f, texHeight);

        ((vec3*)(vbos[eVBO_Positions].data))    [index + 2] = vec3(xpos + width, ypos, 0.0f); // Placeholder for vertex position
        ((vec2*)(vbos[eVBO_UV].data))           [index + 2] = vec2(texWidth, texHeight);

		((vec3*)(vbos[eVBO_Positions].data))    [index + 3] = vec3(xpos, ypos + height, 0.0f); // Placeholder for vertex position
        ((vec2*)(vbos[eVBO_UV].data))           [index + 3] = vec2(0.0f, 0.0f);

        ((vec3*)(vbos[eVBO_Positions].data))    [index + 4] = vec3(xpos + width, ypos, 0.0f); // Placeholder for vertex position
        ((vec2*)(vbos[eVBO_UV].data))           [index + 4] = vec2(texWidth, texHeight);

        ((vec3*)(vbos[eVBO_Positions].data))    [index + 5] = vec3(xpos + width, ypos + height, 0.0f); // Placeholder for vertex position
        ((vec2*)(vbos[eVBO_UV].data))           [index + 5] = vec2(texWidth, 0.0f);
        index += 6;
        current.X += (ch.Advance >> 6) * scale;
	}


    vbos[eVBO_Positions].update();
    vbos[eVBO_UV].update();
    vbos[eVBO_Extra].update();
    isDirty = false;
}

TextGeometry::TextGeometry() : Mesh() {
    name = "TextCreator";
    text = "Hello, World!";

    if (fontNames.size() > 0) {
        font = fontNames[0];
    }
    else {
        font = "";
    }
    type = InstanceType_TextGeometry;
    isDirty = true;
    cull = false;
    vao = 0;
}

void TextGeometry::render(Camera* camera, bool transparent, int renderLayerTarget) {

    if (renderLayer != renderLayerTarget) return;
    generateGeometry();

    if (fontCharacterMaps.find(font) == fontCharacterMaps.end()) {
        debug_log("Font '%s' not loaded. Call 'loadFont()' first.\n", font.c_str());
        return;
	}
    if (!visible) return;
	if (camera == nullptr) {
		debug_log("Abort rendering of '%s' - has no camera attached.\n", name.c_str());
		return;
	}
	Material* m = (Material*)getInstanceById(material);
	if (!m) {
		return;
	}
	ShaderProgram* program = nullptr;
	if (prepareMaterial(&program) != 0) {
		return;
	}

	if (vao == 0 || VAODirty) {
		generateVAO(&program->layout);
        VAODirty = false;
	}

    Font* f = &fontCharacterMaps[font];
	//printf("VAO for ID: %d: %d\n", geometry, g->vao);
	setupRenderSettings(transparent, cull);
	calcTransform();
	m->applyMaterial();
	program->setUniformSMat(program->shaderUniformNames[eSUniform_ModelTransformMatrix], transform, GL_TRUE, 4);
	program->setUniform3f(program->shaderUniformNames[eSUniform_CameraPosition], camera->position);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraLookMatrix], camera->look, GL_TRUE, 3);
	program->setUniformSMat(program->shaderUniformNames[eSUniform_CameraProjectionMatrix], camera->projection, GL_TRUE, 4);
	program->setUniform1f(program->shaderUniformNames[eSUniform_CurrentTime], Graphics::getInstance()->lastFrameTime);

    program->unbindTextureArray(program->shaderUniformNames[eSUniform_Text_Font], eMaterialTexture_MAX);
	glActiveTexture(GL_TEXTURE0 + eMaterialTexture_MAX);
	glBindTexture(GL_TEXTURE_2D_ARRAY, f->textureId);
    program->setUniform1i(program->shaderUniformNames[eSUniform_Text_Font], eMaterialTexture_MAX);

    //debug_log("Size Of VBO: Cnt %d, Ech %d\n", vbos[eVBO_Positions].getCount(), vbos[eVBO_Positions].element_size);
    DrawVAOArrays(vao, 0, vbos[eVBO_Positions].getCount());
}

int TextGeometry::generateVAO(BufferLayout* layout) {

    for (int i = eVBO_Positions; i < eVBO_MAX; ++i) {
        vbos[i].generate();
    }

    return GenVAO(&vao, vbos, nullptr, layout, eVBO_MAX);
}


Instance* TextGeometry::clone_this() {
    TextGeometry* c = (TextGeometry*)registerInstance(new TextGeometry(*this), true);

    c->text = this->text;
    c->font = this->font;

    c->isDirty = true;
    return c;
}

int TextGeometry::getNumSaveBytes() {
    int size = Mesh::getNumSaveBytes();

    size += text.length() + 1;
    size += font.length() + 1;

    return size;
}

int TextGeometry::saveInstanceToFile(FILE* file) {
    int offset = Mesh::saveInstanceToFile(file);

    offset += fwrite(text.c_str(), sizeof(char), text.length() + 1, file);
    offset += fwrite(font.c_str(), sizeof(char), font.length() + 1, file);
    return offset;
}

int TextGeometry::loadInstanceFromFile(void* data, int off) {
    int o = off;
    o = Mesh::loadInstanceFromFile(data, o);
    text = std::string((char*)offset(data, o));
    o += text.length() + 1;
    font = std::string((char*)offset(data, o));
    o += font.length() + 1;
    isDirty = true;
    return o - off;
}

void TextGeometry::setText(std::string newText) {
    text = newText;
    isDirty = true;
}

void TextGeometry::setFont(std::string newFont) {
    font = newFont;
    isDirty = true;
}

int Font::serializeToFile(FILE* file) {

	int offset = 0;
	offset += fwrite(&fontSize, sizeof(int), 1, file);
	offset += fwrite(filepath.c_str(), sizeof(char), filepath.length() + 1, file);

    return offset;
}

int Font::deserializeFromFile(void* data, int off) {
    int o = off;

    int fontSize = 0;
    memcpy(&fontSize, (char*)offset(data, o), sizeof(int));
    o += sizeof(int);
    int length = strlen((char*)offset(data, o)) + 1;
    char* filePathBuffer = new char[length];
    strcpy_s(filePathBuffer, length, (char*)offset(data, o));
   std::string filepath = std::string(filePathBuffer);
    o += length * sizeof(char);
    TextGeometry::loadFont(filepath);
    return o - off;
}

int Font::getNumSaveBytes() {
    return sizeof(int) + filepath.length() + 1;
}

void TextGeometry::setupRenderSettings(bool transparent, bool backfaceCulling) {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	if (backfaceCulling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	if (transparent) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDepthMask(GL_FALSE);
	}
	else {
		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
	}

	if (renderLayer == eRenderLayer_UI) {
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glDisable(GL_CULL_FACE);
	}
}

void TextGeometry::destroyFreeType() {
    FT_Done_FreeType(Font::ft);
    Font::ft = nullptr;
}