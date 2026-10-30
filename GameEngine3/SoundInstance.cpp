#define MINIMP3_IMPLEMENTATION
#include "SoundInstance.h"
std::map<std::string, AudioTrack> Audio::cachedAudioTracks;
Audio::Audio() : Instance() {
	name = "Audio";
	type = InstanceType_Audio;
	renderLayer = eRenderLayer_World;

	audioName = "";

    audioId = 0;
    group = -1;

    isPlaying = false;
    isEditor = true;
}

int Audio::getNumSaveBytes() {
	return Instance::getNumSaveBytes() + sizeof(vec3) + (audioName.length() + 1) + sizeof(int);
}

int Audio::saveInstanceToFile(FILE* file) {
	int offset = 0;
	offset += Instance::saveInstanceToFile(file);
	offset += fwrite(audioName.c_str(), sizeof(char), audioName.length() + 1, file);
	offset += fwrite(&renderLayer, sizeof(int), 1, file);
	offset += fwrite(&position, sizeof(vec3), 1, file);
	return offset;
}

int Audio::loadInstanceFromFile(void* data, int off) {
	int o = off;
	o += Instance::loadInstanceFromFile(data, o);
	audioName = std::string((char*)offset(data, o));
	o += audioName.length() + 1;
	renderLayer = *(int*)offset(data, o);
	o += sizeof(int);
	position = *(vec3*)offset(data, o);
	o += sizeof(vec3);

	return o - off;
}

int AudioTrack::serializeToFile(FILE* file) {
    int offset = 0;

	offset += fwrite(filePath.c_str(), sizeof(char), filePath.length() + 1, file);

	return offset;
}

int AudioTrack::deserializeFromFile(void* data, int off) {
	int o = off;
	std::string filePath = std::string((char*)offset(data, o));
	o += filePath.length() + 1;
	Audio::loadSound(filePath);

    return o;
}

int AudioTrack::getNumSaveBytes() {
	return filePath.length() + 1;
}

void Audio::loadSound(std::string filePath) {
    if (cachedAudioTracks.count(filePath) > 0) {
        return;
    }

    AudioTrack aud;
    aud.loadFromFile(filePath);

    if (aud.loaded) {
        cachedAudioTracks[filePath] = aud;
    }
}
void AudioTrack::loadFromFile(std::string filepath) {
   this->filePath = filepath;
	std::ifstream a(Resource::resolveFilePath(filePath), std::ios::binary);

    if (!a) {
        debug_log("Error: file %s does not exist or loaded with errors.\n", filePath.c_str());
        loaded = false;
        return;
    }
    debug_log("Loading sound from %s\n", filepath.c_str());
    std::vector<unsigned char> abuffer((std::istreambuf_iterator<char>(a)), std::istreambuf_iterator<char>());
    mp3dec_t mp3_dec;
    mp3dec_init(&mp3_dec);
    short pcm[MINIMP3_MAX_SAMPLES_PER_FRAME];
    /*unsigned char *input_buf; - input byte stream*/
    int buffer_data_offset = 0;
    int buf_size = abuffer.size();
    int bytes_consumed = 0;
    unsigned char* buf_begin = abuffer.data();
    while (buf_size > 0) {
        int samples = mp3dec_decode_frame(&mp3_dec, buf_begin, buf_size, pcm, &info);
        if (samples > 0) {
            for (int i = 0; i < samples; ++i) {
            buffer.push_back(pcm[i]);
            }
        }
        else {
            if (info.frame_bytes == 0) {
                break;
            }
        }

        buf_size -= info.frame_bytes;
        buf_begin += info.frame_bytes;
        bytes_consumed += info.frame_bytes;
    }
    size = buffer.size() * sizeof(short);
    if (info.channels == 1 && info.hz <= 44100) { // Check sample rate suitability
        AL_FORMAT = AL_FORMAT_MONO16; // minimp3 outputs 16-bit signed PCM by default
    }
    else if (info.channels == 2 && info.hz <= 44100) {
        AL_FORMAT = AL_FORMAT_STEREO16;
    }
    else {
        // Handle unsupported format or use OpenAL Soft extensions for higher rates/formats if available
        debug_log("Audio error: %s is in unsupported format.", filePath.c_str());
        // handle error
        return;
    }

    //Used by Audio Track;
    if (bufferId != 0) {
        alDeleteBuffers(1, &bufferId);
        bufferId = 0;
    }
    alGenBuffers(1, &bufferId);
    ALenum err = alGetError();
    if (err != AL_NO_ERROR) {
        debug_log("Audio loading error code [buffer generation]:  0x%X\n", err);
    }
    alBufferData(bufferId, AL_FORMAT, buffer.data(), size, info.hz);

    loaded = true;

}

AudioTrack::AudioTrack() {
    size = 0;
	bufferId = 0;
    loaded = false;
}

void Audio::setAudioSource(std::string audio) {
    if (cachedAudioTracks.count(audio) == 0) {
        debug_log("Audio %s does not exist. Call loadSound() before setting audio source.\n", audio.c_str());
        return;
    }

    if (cachedAudioTracks[audioName].loaded == false) {
        debug_log("Audio %s did not load properly.\n", audio.c_str());
        return;
    }
    audioName = audio;

    if (audioId == 0) {
        alGenSources(1, &audioId);
        ALenum err = alGetError();
        if (err != AL_NO_ERROR) {
            debug_log("Audio loading error code [buffer generation]: 0x%X\n", err);
        }
    }
    debug_log("Audio %s loading\n", name.c_str());
    alSourcei(audioId, AL_BUFFER, cachedAudioTracks[audioName].bufferId);
}
void Audio::finalizeLoad(inst_conv_map* conversionMap) {
    this->group = conversionMap->at(this->group);
    //Re-init the source with the now loaded audio
    setAudioSource(this->audioName);
}
void Audio::update(float deltaTime, bool isStudio) {
    if (renderLayer != eRenderLayer_World && !isEditor) {
        alSourcei(audioId, AL_SOURCE_RELATIVE, AL_FALSE);
        alSourcefv(audioId, AL_POSITION, (float*)&position);
    }
    else {
        alSourcei(audioId, AL_SOURCE_RELATIVE, AL_TRUE);
        alSource3f(audioId, AL_POSITION, 0.0f, 0.0f, 0.0f);
    }
}

Instance* Audio::clone_this() {
    Audio* c = (Audio*)registerInstance(new Audio(*this), true);

    return c;
}

void Audio::play() {
    alSourceRewind(audioId);
    resume();
}

void Audio::resume() {
    alSourcePlay(audioId);
}

void Audio::pause() {
    alSourcePause(audioId);
}

void Audio::stop() {
    alSourceStop(audioId);
}

float Audio::getTimestampSeconds() {
    float seconds;
    
    alGetSourcef(audioId, AL_SEC_OFFSET, &seconds);
    if (audioId == 0) {
        seconds = 0;
    }
    return seconds;
}

void Audio::setTimestampSeconds(float seconds) {
    alSourcef(audioId, AL_SEC_OFFSET, seconds);
}
void Audio::setTimestampSamples(int samples) {
    alSourcei(audioId, AL_SAMPLE_OFFSET, samples);
}
int Audio::getTimestampSamples() {
    int samples;

    alGetSourcei(audioId, AL_SAMPLE_OFFSET, &samples);
    if (audioId == 0) {
        samples = 0;
    }
    return samples;
}

int Audio::getSourceSampleRate() {
    if (cachedAudioTracks.count(audioName) == 0) {
        return 0;
    }
    return cachedAudioTracks.at(audioName).info.hz;
}

int Audio::getMaxSamples() {
    int bufferID;
alGetSourcei(audioId, AL_BUFFER, &bufferID);

if (audioId == 0) {
    return 0;
}
int sizeInBytes, channels, bits, frequency;
alGetBufferi(bufferID, AL_SIZE, &sizeInBytes);
alGetBufferi(bufferID, AL_CHANNELS, &channels);
alGetBufferi(bufferID, AL_BITS, &bits);
alGetBufferi(bufferID, AL_FREQUENCY, &frequency);

// Calculate total samples and duration
ALint lengthInSamples = sizeInBytes * 8 / (channels * bits);
return lengthInSamples;
}

float Audio::getMaxSeconds() {
    if (audioId == 0) {
    return 0;
    }
    int bufferID;
    alGetSourcei(audioId, AL_BUFFER, &bufferID);

    int frequency;
    alGetBufferi(bufferID, AL_FREQUENCY, &frequency);
    return (float)getMaxSamples() / (float)frequency;
}
Audio::~Audio() {
    alDeleteSources(1, &audioId);
    audioId = 0;
}

int Audio::InitializeAudioEngine() {
    // 1. Open the device
ALCdevice* device = alcOpenDevice(NULL); // NULL uses the default device
if (!device) { 
    debug_log("Error in loading audio device"); 
    return -1;
}

// 2. Create the context
ALCcontext* context = alcCreateContext(device, NULL);
if (!context) { 
    debug_log("Error in loading audio context");
    return -1;
}
alcMakeContextCurrent(context);
return 0;
// 3. Make the context CURRENT (Crucial!)
}