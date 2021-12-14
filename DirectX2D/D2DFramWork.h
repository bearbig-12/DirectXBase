#pragma once

#include <d2d1.h>
#include <wrl/client.h>
#include <exception>
#include <sstream>

class com_exception : public std::exception
{
private:
	HRESULT result;
public:
	com_exception(HRESULT hr) : result{hr} {}
	virtual const char* what() const override
	{
		static char buf[128];

		sprintf_s(buf, 128, "Failure with HRESULT of 0X%08X", result);
	
		return buf;
	}

};
// inline 
//		장점: 호출스택을 사용하지 않아서 빠름
//		단점: 코드 길어짐

// 선언/정의
//	inline void f()
//	{
//	}
// 
//	선언과 정의
//	void f();
//  inline void f();

// 클래스의 경우 .h에 정의
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw com_exception(hr);
	}
}

class D2DFramWork
{
private:
	const LPCWSTR WND_CLASS_NAME{ L"MyWindowClass" };

protected:
	HWND mHwnd{};
	Microsoft::WRL::ComPtr<ID2D1Factory> mspD2DFactory{};
	Microsoft::WRL::ComPtr<ID2D1HwndRenderTarget> mspRenderTarget{};

protected:
	HRESULT InitWindow(HINSTANCE hInstance, LPCWSTR title,
		UINT width, UINT height);		// 윈도우 초기화 함수.

	HRESULT InitD2D();
	virtual HRESULT CreateDeviceResources();

public:
	virtual HRESULT Initialize(HINSTANCE hInstance, 
		LPCWSTR title = L"MyFrameWork", 
		UINT width = 1024, 
		UINT height = 768);
	virtual void Release();
	virtual int GameLoop();
	virtual void Render();

	static void ShowErrorMsg(LPCWSTR msg, LPCWSTR title = L"Error");

public:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

};

