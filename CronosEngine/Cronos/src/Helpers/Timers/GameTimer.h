#ifndef __GAMETIMER_H__
#define __GAMETIMER_H__

namespace Cronos {

	class GameTimer
	{
	private:

		float m_StartingTime = 0.0f;

		float m_PausedAt = 0.0f;

		bool m_TimerActive = false;
		bool m_TimerPaused = true;

	public:

		GameTimer() { Start(); }

		void Start();
		void Stop();

		void Play();
		void Pause();

		void Restart() { Start(); Play(); }

		Uint32 Read() const;
		float ReadSec() const;

		bool IsActive() const { return m_TimerActive; }
		bool IsPaused() const { return m_TimerPaused; }
	};
}
#endif