#include "Timer.h"
#include <cassert>

using namespace ArkanoidEngine;

void Timer::start()
{
	assert(!m_running);
	m_running = true;
	m_prevTime = BaseClock::now();
}

void Timer::stop()
{
	assert(m_running);
	m_running = false;
}

void Timer::tick()
{
	if (m_running)
	{
		auto currTime = BaseClock::now();
		m_deltaTime = std::chrono::duration_cast<DeltaDuration>(currTime - m_prevTime).count();
		m_prevTime = currTime;
	}
	else
	{
		m_deltaTime = 0.0f;
	}
}

