#include <Windows.h>
#include "Timer.h"

MyUtil::Timer::Timer() :
	mdSecondsPerCount{ 0.0 },
	mdDeltaTime{ -1.0 },
	mfScale{ 1.0f },
	mllBaseTime{ 0 },
	mllPausedTime{ 0 },
	mllPrevTime{ 0 },
	mllCurrTime{ 0 },
	mllStopTime{ 0 },
	mbStopped{ false }
{
	long long countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

	mdSecondsPerCount = 1.0 / static_cast<double>(countsPerSec); 
	//cpu�� �ѹ� �۵��Ҷ� ���ʰ� �ɸ���



}

void MyUtil::Timer::Start()
{
	long long currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mllBaseTime = currTime;
	mllPrevTime = currTime;
	mllStopTime = 0;

	mbStopped = false;
	
}

void MyUtil::Timer::Stop()
{
	if (!mbStopped)
	{
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		mllStopTime = currTime;
		mbStopped = false;

	}
}

void MyUtil::Timer::Resume()
{
	long long currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	if (mbStopped)	// ���������� �簳
	{
		mllPausedTime += (currTime = mllStopTime);

		mllPrevTime = currTime;
		mllStopTime = currTime;
		mbStopped = false;
	}
}

void MyUtil::Timer::Update()
{
	if (mbStopped) // �ð谡 ���� ������
	{
		mdDeltaTime = 0.0f;
		return;
	}

	__int64 currTime; // long long �� ����
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	mllCurrTime = currTime;

	mdDeltaTime = (mllCurrTime - mllPrevTime) * mdSecondsPerCount;

	mllPrevTime = mllCurrTime;

	if (mdDeltaTime < 0.0)
	{
		mdDeltaTime = 0.0;
	}

}

float MyUtil::Timer::TotalTime() const
{
	if (mbStopped)
	{
		return static_cast<float>( 
			((mllStopTime - mllPausedTime) - mllBaseTime) * mdSecondsPerCount 
			);
	}
	else
	{
		return static_cast<float>( 
			((mllCurrTime - mllPausedTime) - mllBaseTime) * mdSecondsPerCount 
			);
	}

	
}

float MyUtil::Timer::DeltaTime() const
{
	return static_cast<float>(mdDeltaTime * mfScale);
}

void MyUtil::Timer::SetScale(float scale)
{
	mfScale = scale;
}
