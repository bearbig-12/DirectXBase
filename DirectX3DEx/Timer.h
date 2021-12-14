#pragma once

namespace MyUtil
{
	class Timer
	{
	public:
		Timer();

	public:
		void Start();
		void Stop();
		void Resume();
		void Update();

		float TotalTime() const;
		float DeltaTime() const;
		void SetScale(float scale);

	private:
		double mdSecondsPerCount;
		double mdDeltaTime;
		float mfScale;
		
		long long mllBaseTime;
		long long mllPausedTime;
		long long mllStopTime;
		long long mllPrevTime;
		long long mllCurrTime;

		bool mbStopped;
	};
}


