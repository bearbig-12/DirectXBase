#pragma once
#include <winsdkver.h>
#define _WIN32_WINNT 0x0A00

#include <winsdkver.h>

// ������ min, max ������
#define NOMINMAX
// GDI ������
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// �� ������
#define NOMCX

// ���� ��� ������
#define NOSERVICE

// ���� ������ - deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN


//---------------------------------
// ���� ���Ǵ� ���
#include <Windows.h>

// directx ����
#include <wrl/client.h>
#include <d3d11_1.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#ifdef _DEBUG
#include <dxgidebug.h>
#endif // _DEBUG

// c++ ����
#include <algorithm>
#include <exception>
#include <memory>
#include <stdexcept>

// c��Ÿ�� ���
#include <stdio.h>

#pragma warning(push)			//��� ����
#pragma warning(disable:26812)	//��� ����

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

#pragma warning(pop)		//��� ����

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