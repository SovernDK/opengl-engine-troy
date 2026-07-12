#pragma once
#include "services/service.h"

#include <string>
#include <stack>

#include <SDL3_mixer/SDL_mixer.h>

class IAudioService : IService
{
public:
	virtual ~IAudioService() = default;

	virtual void createAudioDevice() = 0;
	virtual void playOnce(const std::string& soundName) = 0;
	/* Second argument specifiec amount of loops, use -1 for infinite loop */
	virtual void playLoop(const std::string& soundName, Sint64 loops) = 0;
	virtual void playMusic(const std::string& musicName) = 0;
	virtual void stopMusic(Uint64 fadeOutMs) = 0;

	virtual MIX_Mixer* mixer() = 0;
};

class AudioService : IAudioService
{
private:
	MIX_Mixer* m_mixer = nullptr;
	MIX_Track* musicTrack1 = nullptr;

	std::stack<MIX_Track*> trackPool;
public:
	void createAudioDevice() override;
	void playOnce(const std::string& soundName) override;
	void playLoop(const std::string& soundName, Sint64 loops) override;
	void playMusic(const std::string& musicName) override;
	void stopMusic(Uint64 fadeOutMs) override;

	MIX_Mixer* mixer() override { return m_mixer; }
private:
	void newTrack();
	void pushTrack(MIX_Track* track);
	MIX_Track* pullTrack();
};