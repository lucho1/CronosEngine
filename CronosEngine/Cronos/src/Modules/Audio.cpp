#include "cnpch.h"
#include "Globals.h"
#include "Application.h"
#include "Audio.h"

namespace Cronos {

	Audio::Audio(Application* app, bool start_enabled) : Module(app, "Module Audio", start_enabled), music(nullptr)
	{
		ChannelsVolume = std::vector<uint>(8, 100); //Volume of the 8 existing channels initialized to 100 (max)
	}

	// Destructor
	Audio::~Audio() {}

	// Called before render is available
	bool Audio::OnInit()
	{
		LOG("Loading Audio Mixer");
		bool ret = true;
		SDL_Init(0);

		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0)
		{
			LOG("SDL_INIT_AUDIO could not initialize! SDL_Error: %s\n", SDL_GetError());
			m_ModuleActive = true;
			ret = false;
		}

		// load support for the OGG format
		int flags = MIX_INIT_OGG;
		int init = Mix_Init(flags);

		if ((init & flags) != flags)
		{
			LOG("Could not initialize Mixer lib. Mix_Init: %s", Mix_GetError());
			m_ModuleActive = false;
			ret = false;
		}

		//Initialize SDL_mixer
		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		{
			LOG("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
			m_ModuleActive = false;
			ret = false;
		}

		SetMasterVolume(m_MasterVol);
		return ret;
	}

	// Called before quitting
	bool Audio::OnCleanUp()
	{

		if (!m_ModuleActive)
			return true;

		LOG("Freeing sound FX, closing Mixer and Audio subsystem");

		if (music != nullptr)
		{
			Mix_FreeMusic(music);
		}

		for (auto& element : m_FXList)
			if (element != nullptr)
				Mix_FreeChunk(element);

		m_FXList.clear();
		Mix_CloseAudio();
		Mix_Quit();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
		return true;
	}

	// Play a music file
	bool Audio::PlayMusic(const char* path, float fade_time)
	{
		bool ret = true;

		if (!m_ModuleActive)
			return ret;

		if (music != NULL)
		{
			if (fade_time > 0.0f)
			{
				Mix_FadeOutMusic((int)(fade_time * 1000.0f));
			}
			else
			{
				Mix_HaltMusic();
			}

			// this call blocks until fade out is done
			Mix_FreeMusic(music);
		}

		music = Mix_LoadMUS(path);

		if (music == NULL)
		{
			LOG("Cannot load music %s. Mix_GetError(): %s\n", path, Mix_GetError());
			ret = false;
		}
		else
		{
			if (fade_time > 0.0f)
			{
				if (Mix_FadeInMusic(music, -1, (int)(fade_time * 1000.0f)) < 0)
				{
					LOG("Cannot fade in music %s. Mix_GetError(): %s", path, Mix_GetError());
					ret = false;
				}
			}
			else
			{
				if (Mix_PlayMusic(music, -1) < 0)
				{
					LOG("Cannot play in music %s. Mix_GetError(): %s", path, Mix_GetError());
					ret = false;
				}
			}
		}

		LOG("Successfully playing %s", path);
		return ret;
	}

	// Load WAV
	unsigned int Audio::LoadFx(const char* path)
	{
		unsigned int ret = 0;

		if (!m_ModuleActive)
			return ret;

		Mix_Chunk* chunk = Mix_LoadWAV(path);

		if (chunk == nullptr)
		{
			LOG("Cannot load wav %s. Mix_GetError(): %s", path, Mix_GetError());
		}
		else
		{
			m_FXList.push_back(chunk);
			ret = m_FXList.size();
		}

		return ret;
	}

	// Play WAV
	bool Audio::PlayFx(unsigned int id, int repeat)
	{
		bool ret = false;

		if (!m_ModuleActive)
			return ret;

		Mix_Chunk* chunk = m_FXList.at(id - 1);

		if (id > 0 && chunk)
		{
			Mix_PlayChannel(-1, chunk, repeat);
			ret = true;
		}

		return ret;
	}


	bool Audio::MusicPlaying() {

		return (Mix_PlayingMusic());

	}

	//Volume Control -------------------------------------------------------------------
	void Audio::SetMasterVolume(uint vol)
	{
		if (vol > 100)
			vol = 100;

		m_MasterVol = vol;

		uint PercentageValueVolume = PERCENTAGE_MODIFIER_VALUE(m_MusicVol) * m_MasterVol;
		Mix_VolumeMusic(PERCENTAGE_VOL_VALUE_TO_SDL(PercentageValueVolume));

		PercentageValueVolume = PERCENTAGE_MODIFIER_VALUE(m_SFXVol) * m_MasterVol;
		Mix_Volume(-1, PERCENTAGE_VOL_VALUE_TO_SDL(PercentageValueVolume));

	}


	void Audio::SetMusicVolume(uint vol)
	{
		m_MusicVol = vol;
		uint PercentageValueVolume = PERCENTAGE_MODIFIER_VALUE(m_MusicVol) * m_MasterVol;
		Mix_VolumeMusic(PERCENTAGE_VOL_VALUE_TO_SDL(PercentageValueVolume));
	}


	void Audio::SetChannelVolume(uint vol, int channel)
	{

		if (vol > 100)
			vol = 100;

		if (channel < -1)
			channel = -1;

		if (channel > 8)
			channel = 8;


		if (channel == -1)
			SetSFXVolume(vol);
		else {

			ChannelsVolume[channel] = vol;
			uint PercentageValueVol = PERCENTAGE_MODIFIER_VALUE(vol) * m_SFXVol;
			Mix_Volume(channel, PERCENTAGE_VOL_VALUE_TO_SDL(PercentageValueVol));
		}

	}


	void Audio::SetSFXVolume(uint vol) {

		m_SFXVol = vol;

		for (uint i = 0; i < 8; i++) {

			uint PercentageValueVolume = PERCENTAGE_MODIFIER_VALUE(ChannelsVolume[i]) * m_SFXVol;
			Mix_Volume(ChannelsVolume[i], PERCENTAGE_VOL_VALUE_TO_SDL(PercentageValueVolume));
		}

		for (auto& element : m_FXList) {

			if (element != nullptr) {

				uint ChunkVolume = SDL_VOL_VALUE_TO_PERCENTAGE(element->volume);
				uint realVolume = PERCENTAGE_MODIFIER_VALUE(ChunkVolume) * m_SFXVol;

				Mix_VolumeChunk(element, PERCENTAGE_VOL_VALUE_TO_SDL(realVolume));
			}
		}
	}


	void Audio::SetSFXChunkVolume(uint vol, int id)
	{

		if (id > 0 && id < (int)m_FXList.size()) {

			if (vol > 100)
				vol = 100;

			auto& element = next(m_FXList.begin(), id - 1);
			Mix_VolumeChunk(*element, PERCENTAGE_VOL_VALUE_TO_SDL(vol));
		}

	}

}