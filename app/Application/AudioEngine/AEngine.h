#ifndef AENGINE_H
#define AENGINE_H

#include <AL/al.h>
#include <AL/alc.h>
#include <map>
#include <string>
#include <vector>
#include <Types.h>
#include <fmt/base.h>
#include <cmath>

class SoundEngine {
private:
    SoundEngine() {
        device = nullptr;
        context = nullptr;
    }

    ~SoundEngine() {
        shotdown();
    }

    ALCdevice* device;
    ALCcontext* context;
    std::vector<u32> m_sources;
    std::map<std::string, ALuint> m_soundBuffers;

    void checkAlError(const std::string& msg) {
        ALenum error = alGetError();
        if (error != AL_NO_ERROR) {
            fmt::print("OpenAL error during {} : {}\n", msg, alGetString(error));
        }
    }
    u32 findFreeSources() {
        for (ALuint source : m_sources) {
            ALint state;
            alGetSourcei(source, AL_SOURCE_STATE, &state);
            if (state != AL_PLAYING) {
                return source;
            }
        }
        return 0; // 0 means no free sources available
    }

public:

    static SoundEngine& instance() {
        static SoundEngine inst;
        return inst;
    }
    bool init();
    void shotdown();

    void setListenerPosition(float x, float y, float z);

    void generateAndLoadSineWave(const std::string& name, float frequencyHz, float durationSeconds = 2.0f, float amplitude = 0.5f, u32 SAMPLE_RATE = 44100);

    void playSound(const std::string& name, bool loop);

};

#endif //AENGINE_H