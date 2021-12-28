#pragma once
#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00

#include <winsdkver.h>

// 윈도우 min, max 사용안함
#define NOMINMAX
// GDI 사용안함
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// 모뎀 사용안함
#define NOMCX

// 서비스 기능 사용안함
#define NOSERVICE

// 도움말 사용안함 - deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN


//---------------------------------
// 자주 사용되는 헤더
#include <Windows.h>

// directx 관련
#include <wrl/client.h>
#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif // _DEBUG

// c++ 관련
#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>

// c스타일 헤더
#include <stdio.h>

#pragma warning(push)			//경고 막기
#pragma warning(disable:26812)	//경고 막기

#include "Audio.h"
#include "CommonStates.h"
#include "WICTextureLoader.h"
#include "Effects.h"
#include "GamePad.h"
#include "GeometricPrimitive.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Model.h"
#include "PrimitiveBatch.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

#pragma warning(pop)		//경고 막기

#include "StepTimer.h"

namespace DX
{
	class com_exception : public std::exception
	{
	private:
		HRESULT result;
	public:
		com_exception(HRESULT hr) noexcept : result{hr} {}

		const char* what() const override
		{
			static char s_str[64]{};

			sprintf_s(
				s_str,
				"Faliure with HRESULT %08X",
				static_cast<unsigned int>(result)
			);
			return s_str;
		}

	};

	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}