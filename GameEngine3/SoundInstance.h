#pragma once
#include "GameInstance.h"
#include "Resource.h"
#include "Graphics/matrix.h"
#include "Graphics/GraphicsWrapper.h"

#include <MINIAUDIO/minimp3.h>
#include <fstream>
#include <OPENAL_1.1/include/al.h>
#include <OPENAL_1.1/include/alc.h>
class AudioTrack {
public:
	std::vector<short> buffer;
	mp3dec_frame_info_t info;
	int size;
	bool loaded;
	unsigned int bufferId;
	ALenum AL_FORMAT;
	std::string filePath;

	AudioTrack();
	void loadFromFile(std::string filepath);
	int serializeToFile(FILE* file);
	int getNumSaveBytes();
	static int deserializeFromFile(void* data, int offset = 0);
};
class Audio : public Instance
{
protected:
public:
public:
	static std::map<std::string, AudioTrack> cachedAudioTracks;
	static int InitializeAudioEngine();
	vec3 position;
	//Whether the sound is positional
	int renderLayer;
	std::string audioName;
	bool isPlaying;
	bool isEditor;
	int group;
	unsigned int audioId;
	Audio();
	virtual ~Audio();
	virtual void update(float deltaTime, bool isStudio);
	void setAudioSource(std::string audio);
	static void loadSound(std::string filepath);
	virtual Instance* clone_this();
	virtual int getNumSaveBytes();
	virtual int loadInstanceFromFile(void* data, int off = 0);
	virtual int saveInstanceToFile(FILE* file);
	virtual void finalizeLoad(inst_conv_map* conversionMap);

	void play();
	void resume();
	void pause();
	void stop();

	float getTimestampSeconds();
	void setTimestampSeconds(float seconds);

	int getTimestampSamples();
	void setTimestampSamples(int samples);

	int getSourceSampleRate();

	float getMaxSeconds();
	int getMaxSamples();
};
