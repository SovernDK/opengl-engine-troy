#pragma once
#include "services/audio_service.h"
#include "core/resources.h"

void AudioService::createAudioDevice()
{
	m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, NULL);
	if (!m_mixer)
		SDL_Log("Couldn't create mixer on default device: %s", SDL_GetError());

	musicTrack1 = MIX_CreateTrack(mixer());
	if (!musicTrack1)
		SDL_Log("Couldn't create a mixer track: %s", SDL_GetError());

	for(int i = 0; i < 2; i++)
	{
		newTrack();
	}
}

void AudioService::playOnce(const std::string& soundName)
{
	auto* track = pullTrack();
	if(track == nullptr)
	{
		newTrack();
		track = pullTrack();
	}

	auto clip = Resources::sfx[soundName];
	MIX_SetTrackAudio(track, clip);
	MIX_PlayTrack(track, 0);

	pushTrack(track);
}

void AudioService::playLoop(const std::string& soundName, Sint64 loops)
{
	auto* track = pullTrack();
	if (track == nullptr)
	{
		newTrack();
		track = pullTrack();
	}

	SDL_PropertiesID options = 0;
	options = SDL_CreateProperties();
	if (!options)
		SDL_Log("Couldn't create play options: %s", SDL_GetError());

	SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, loops);

	auto clip = Resources::sfx[soundName];
	MIX_SetTrackAudio(track, clip);
	MIX_PlayTrack(track, options);
	SDL_DestroyProperties(options);

	pushTrack(track);
}

void AudioService::playMusic(const std::string& musicName)
{
	auto clip = Resources::music[musicName];
	MIX_SetTrackAudio(musicTrack1, clip);

	SDL_PropertiesID options = 0;
	options = SDL_CreateProperties();
	if (!options)
		SDL_Log("Couldn't create play options: %s", SDL_GetError());

	SDL_SetNumberProperty(options, MIX_PROP_PLAY_LOOPS_NUMBER, -1);
	SDL_SetNumberProperty(options, MIX_PROP_PLAY_FADE_IN_MILLISECONDS_NUMBER, 5000);
	
	MIX_PlayTrack(musicTrack1, options);
	SDL_DestroyProperties(options); 
}

void AudioService::stopMusic(Uint64 fadeOutMs)
{
	Sint64 ms = MIX_TrackMSToFrames(musicTrack1, fadeOutMs);
	MIX_StopTrack(musicTrack1, ms);
}

void AudioService::newTrack()
{
	auto nTrack = MIX_CreateTrack(mixer());
	if (!nTrack)
		SDL_Log("Couldn't create a mixer track: %s", SDL_GetError());

	trackPool.push(nTrack);
}

void AudioService::pushTrack(MIX_Track* track)
{
	trackPool.push(track);
}

MIX_Track* AudioService::pullTrack()
{
	if (trackPool.empty()) return nullptr;
	MIX_Track* track = trackPool.top();
	trackPool.pop();
	return track;
}