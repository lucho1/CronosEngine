#include "Providers/cnpch.h"
#include "GameTimer.h"
#include "mmgr/mmgr.h"


namespace Cronos {

	void GameTimer::Start()
	{
		m_TimerActive = true;
		m_TimerPaused = false;
		m_StartingTime = SDL_GetTicks();
		m_PausedAt = 0.0f;
	}

	void GameTimer::Stop()
	{
		m_TimerActive = false;
		m_TimerPaused = true;
		m_StartingTime = 0.0f;
		m_PausedAt = 0.0f;
	}

	void GameTimer::Play()
	{
		if (!m_TimerPaused)
			return;

		m_TimerPaused = false;
		m_StartingTime = (SDL_GetTicks() + m_StartingTime) - m_PausedAt;
		//m_StartingTime += m_PausedAt;
	}

	void GameTimer::Pause()
	{
		if (m_TimerPaused)
			return;

		m_TimerPaused = true;
		m_PausedAt = SDL_GetTicks();
	}

	Uint32 GameTimer::Read() const
	{
		if (!m_TimerActive)
			return 0;

		if (!m_TimerPaused)
			return (SDL_GetTicks() - m_StartingTime);
		else
			return (m_PausedAt - m_StartingTime);
	}

	float GameTimer::ReadSec() const
	{
		if (!m_TimerActive)
			return 0.0f;

		if (!m_TimerPaused)
			return (float)((SDL_GetTicks() - m_StartingTime)/1000.0f);
		else
			return (float)((m_PausedAt - m_StartingTime)/1000.0f);
	}
}