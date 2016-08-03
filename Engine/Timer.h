#pragma once
#include <chrono>

namespace ArkanoidEngine
{
	class Timer
	{
	public:
		//ctors
		explicit Timer() = default;

		//dtor
		~Timer() = default;

		//copy
		Timer(const Timer&) = delete;
		Timer& operator=(const Timer&) = delete;

		//move
		Timer(Timer&&) = default;
		Timer& operator=(Timer&&) = default;

		void start();		
		void stop();

		void tick();

		//in milliseconds
		float deltaTime()const;

	private:

		using BaseClock = std::chrono::high_resolution_clock;
		using TimePoint = BaseClock::time_point;
		using DeltaDuration = std::chrono::duration<float, std::milli>;

		TimePoint m_prevTime{};
		float m_deltaTime{ 0.0f };
		bool m_running{ false };
	};
		
	inline float Timer::deltaTime()const
	{
		return m_deltaTime;
	}
}
