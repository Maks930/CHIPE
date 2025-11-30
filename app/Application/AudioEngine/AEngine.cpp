#include <Application/AudioEngine/AEngine.h>

bool SoundEngine::init()
{
    device = alcOpenDevice(nullptr);
    if (!device) {
        return false;
    }
    context = alcCreateContext(device, nullptr);
    if (!context) {
        alcCloseDevice(device);
        return false;
    }

    alcMakeContextCurrent(context);
    checkAlError("Context creation");

    m_sources.resize(16);
    alGenSources(m_sources.size(), m_sources.data());
    checkAlError("Source generation");

    return true;
}

void SoundEngine::shotdown()
{
    for (ALuint source : m_sources) {
        alSourceStop(source);
    }
    alDeleteSources(m_sources.size(), m_sources.data());

    for (auto const& [key, val] : m_soundBuffers) {
        alDeleteBuffers(1, &val);
    }
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}

void SoundEngine::setListenerPosition(float x, float y, float z)
{
    alListener3f(AL_POSITION, x, y, z);
}

void SoundEngine::generateAndLoadSineWave(const std::string& name, float frequencyHz, float durationSeconds, float amplitude, u32 SAMPLE_RATE) {
    //const unsigned int SAMPLE_RATE = 44100; // Standard CD quality sample rate
    const unsigned int NUM_SAMPLES = static_cast<unsigned int>(SAMPLE_RATE * durationSeconds);

    std::vector<u16> samples(NUM_SAMPLES);
    for (unsigned int i = 0; i < NUM_SAMPLES; ++i) {
        float time = (float)i / (float)SAMPLE_RATE;
        //// Formula: amplitude * sin(2 * PI * frequency * time)
        float angle = 2.0f * M_PI * frequencyHz * time;
        //// Scale the float result (-1.0 to 1.0) to 16-bit integer range
        samples[i] = static_cast<u16>(std::sin(angle) * 32767.0f * amplitude);

        // Calculate the value in the range [-1.0, 1.0]
        // Formula: 2.0 * ( (time * frequency) % 1.0 ) - 1.0, simplified with floor
        float value = 2.0f * (time * frequencyHz - std::floor(time * frequencyHz + 0.5f)) - 1.0f;

        // Scale and convert to 16-bit signed integer format
        samples[i] += static_cast<i16>(value * 32767.0f * amplitude / 3.5);

    }

    ALuint bufferID;
    alGenBuffers(1, &bufferID);
    checkAlError("Generate buffer for sine wave");

    alBufferData(bufferID, AL_FORMAT_MONO16, samples.data(), samples.size() * sizeof(i16), SAMPLE_RATE);
    checkAlError("Buffer Data Load for sine wave");

    // Store the buffer ID in our map for later playback
    m_soundBuffers[name] = bufferID;
}

void SoundEngine::playSound(const std::string& name, bool loop)
{
    //ALuint source = findFreeSources();
    /*if (source == 0) {
        fmt::print("Failed to find a free audio source.\n");
        return;
    }*/
    alSourceStop(1);
    alSourcei(1, AL_BUFFER, m_soundBuffers[name]);
    alSourcei(1, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
    alSourcePlay(1);
    
    checkAlError("Playback start");
}
