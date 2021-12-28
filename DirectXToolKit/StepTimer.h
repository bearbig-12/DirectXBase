#pragma once

#include <cmath>
#include <cstdint>



namespace DX
{
	class StepTimer
	{
	private:
		LARGE_INTEGER m_qpcFrequency;
		LARGE_INTEGER m_qpcLastTime;
		uint64_t m_qpcMaxDelta;

		uint64_t m_elapsedTicks;
		uint64_t m_totalTicks;
		uint64_t m_leftOverTicks;

		uint32_t m_frameCount;
		uint32_t m_framesPerSecond;
		uint32_t m_framesThisSecond;
		uint64_t m_qpcSecondCounter;
		
		bool m_isFixedTimeStep;

		uint64_t m_targetElapsedTicks;


	public:
		static const uint64_t TicksPerSecond = 10000000;
		static constexpr double TicksToSeconds(uint64_t ticks) noexcept	// constexpr noexcept는 c++ 11에 자료있음
		{
			return static_cast<double>(ticks) / TicksPerSecond;
		}
		static constexpr uint64_t SecondsToTicks(double seconds) noexcept
		{
			return static_cast<uint64_t>(seconds * TicksPerSecond);
		}
	public:
		StepTimer() noexcept(false) :
			m_elapsedTicks(0),
			m_totalTicks(0),
			m_leftOverTicks(0),
			m_frameCount(0),
			m_framesPerSecond(0),
			m_framesThisSecond(0),
			m_qpcSecondCounter(0),
			m_isFixedTimeStep(false),
			m_targetElapsedTicks(TicksPerSecond / 60)
		{
			if (!QueryPerformanceFrequency(&m_qpcFrequency))
			{
				throw std::exception("QueryPerformanceFrequency");
			}
			if (!QueryPerformanceCounter(&m_qpcLastTime))
			{
				throw std::exception("QueryPerformanceCounter");

			}
			m_qpcMaxDelta = static_cast<uint64_t>(m_qpcFrequency.QuadPart / 10);
		}

		uint64_t GetElapsedTicks() const noexcept { return m_elapsedTicks; }
		double GetElapsedSeconds() const noexcept { return TicksToSeconds(m_elapsedTicks); }
		uint64_t GetTotalTicks() const noexcept { return m_totalTicks; }
		double GetTotalSeconds() const noexcept { return TicksToSeconds(m_totalTicks); }

		uint32_t GetFrameCount() const noexcept { return m_frameCount; }
		uint32_t GetFramePerSeconds() const noexcept { return m_framesPerSecond; }

		void SetFixedTimeStep(bool isFixed) noexcept { m_isFixedTimeStep = isFixed; }
		void SetTargetElapsedTicks(uint64_t targetElapsed) noexcept
		{
			m_targetElapsedTicks = targetElapsed;
		}
		void SetTargetElapsedSeconds(double targetElapsed) noexcept
		{
			m_targetElapsedTicks = SecondsToTicks(targetElapsed);
		}

	public:
		void ResetElapsedTime()
		{
			if (!QueryPerformanceCounter(&m_qpcLastTime))
			{
				throw std::exception("QueryPerformanceCounter");
			}
			m_leftOverTicks = 0;
			m_framesPerSecond = 0;
			m_framesThisSecond = 0;
			m_qpcSecondCounter = 0;
		}

		template<typename TUpdate>
		void Tick(const TUpdate& update)
		{
			LARGE_INTEGER currentTime;
			if (!QueryPerformanceCounter(&currentTime))
			{
				throw std::exception("QueryPerformanceCounter");
			}
			uint64_t timeDelta = static_cast<uint64_t>(
				currentTime.QuadPart - m_qpcLastTime.QuadPart
				);
			m_qpcLastTime = currentTime;
			m_qpcSecondCounter += timeDelta;

			if (timeDelta > m_qpcMaxDelta)
			{
				timeDelta = m_qpcMaxDelta;
			}

			timeDelta *= TicksPerSecond;
			timeDelta /= static_cast<uint64_t>(m_qpcFrequency.QuadPart);

			uint32_t lastFrameCount = m_frameCount;

			if (m_isFixedTimeStep)
			{
				// 고정 프레임

				// 4000 : 1s = 1000ms
				if (static_cast<uint64_t>(
					std::abs(static_cast<int64_t>(timeDelta - m_targetElapsedTicks))				//절대값 abs 사용
					) < TicksPerSecond / 4000)
				{
					timeDelta = m_targetElapsedTicks;
				}

				m_leftOverTicks += timeDelta;

				while (m_leftOverTicks >= m_targetElapsedTicks)
				{
					m_elapsedTicks = m_targetElapsedTicks;
					m_totalTicks += m_targetElapsedTicks;
					m_leftOverTicks -= m_targetElapsedTicks;
					m_frameCount++;

					update();
				}
			}
			else
			{
				// 가변 프레임
				m_elapsedTicks = timeDelta;
				m_totalTicks = timeDelta;
				m_leftOverTicks = 0;
				m_frameCount++;

				update();
			}

			if (m_frameCount != lastFrameCount)
			{
				m_framesThisSecond++;

			}
			if (m_qpcSecondCounter >= static_cast<uint64_t>(m_qpcFrequency.QuadPart))
			{
				m_framesPerSecond = m_framesThisSecond;
				m_framesThisSecond = 0;
				m_qpcSecondCounter %= static_cast<uint64_t>(m_qpcFrequency.QuadPart);
			}
		}
	};

}
