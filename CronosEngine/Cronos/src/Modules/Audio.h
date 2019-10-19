#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "Module.h"
#include "SDL_mixer\include\SDL_mixer.h"

#define PERCENTAGE_VOL_VALUE_TO_SDL(volume) (volume * MIX_MAX_VOLUME/100)
#define SDL_VOL_VALUE_TO_PERCENTAGE(volume) (volume * 100/MIX_MAX_VOLUME)
#define PERCENTAGE_MODIFIER_VALUE(volume) (volume/100)

namespace Cronos {

	enum class song_type {

		NONE = -1,
		MAIN_MENU,
		IN_GAME,
		WIN,
		LOSE
	};

	struct Music_Track {

		std::string trackName = "NULL";
		char* path = nullptr;
		int num_loops = 0;

		char* nextTrack_id = "NULL";
		Music_Track* nextTrack = nullptr;
	};

	struct Music_Song {

		std::string songName = "NULL";
		song_type songType = song_type::NONE;
		std::vector<Music_Track*> m_SongtracksList;

		char* nextSong_id = "NULL";
		Music_Song* nextSong = nullptr;
	};


	class Audio : public Module
	{
	public:

		Audio(Application* app, bool start_enabled = true);
		virtual ~Audio();

		bool OnInit() override;
		bool OnCleanUp() override;

		// Play a music file
		bool PlayMusic(const char* path, float fade_time = 2.0f);

		//Check if something is being played
		bool MusicPlaying();

		// Load a WAV in memory
		unsigned int LoadFx(const char* path);

		// Play a previously loaded WAV
		bool PlayFx(unsigned int fx, int repeat = 0);

		//Save/Load
		virtual void SaveModuleData(json& JSONFile) const override;
		virtual void LoadModuleData(json& JSONFile) override;

	public:

		//--------------------------------------------------------------------------------------------------------
		//Volume Control (Paremeter input and variables use percentage 0 - 100 values, Mixer uses 0 - 128 values)
		//Master Volume
		void SetMasterVolume(uint vol);

		//Music Volume
		void SetMusicVolume(uint vol);

		//Whole SFX Volume
		void SetSFXVolume(uint vol);

		//SFX Volume by Channels
		void SetChannelVolume(uint vol, int channel = -1);

		//Chunck Volume
		void SetSFXChunkVolume(uint vol, int id = -1);

		//Get functions
		uint* GetMasterVolume() { return &m_MasterVol; }
		uint* GetSFXVolume() { return &m_SFXVol; }
		uint* GetMusicVolume() { return &m_MusicVol; }

	private:

		Mix_Music* music = nullptr;
		std::vector<Mix_Chunk*>	m_FXList;

		// 0% - 100% Range
		uint m_MasterVol = 100;
		uint m_MusicVol = 100;
		uint m_SFXVol = 100;
		std::vector<uint>ChannelsVolume;

	};

}

#endif // __ModuleAudio_H__