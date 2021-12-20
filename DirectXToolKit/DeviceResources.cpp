#include "pch.h"
#include "DeviceResources.h"

using namespace DirectX;
using namespace DX;
using Microsoft::WRL::ComPtr;

#ifdef __clang__
#pragma clang diagnostic ignored "-Wcovered-switch-default"
#pragma clang diagnostic ignored "-Wswitch-enum"
#endif

#pragma warning(disable : 4061)

//이 cpp 에서만 쓸 전역변수
namespace 
{
#if defined(_DEBUG)
	inline bool SdkLayersAvailable() noexcept		// 디버그용으로 디바이스를 만들수있느냐 없느냐를 확인만하는 함수
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			nullptr,
			nullptr,
			nullptr
		);
		return SUCCEEDED(hr);
	}
#endif
	inline DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt) noexcept 
	{
		switch (fmt)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
			return DXGI_FORMAT_R8G8B8A8_UNORM;

		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8A8_UNORM;

		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			return DXGI_FORMAT_B8G8R8X8_UNORM;
		default:
			return fmt;
		}
	}
}


void DeviceResources::CreateFactory()
{
}

void DeviceResources::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
{
}

void DeviceResources::UpdateColorSpace()
{
}

DeviceResources::DeviceResources()
{
}

void DeviceResources::CreateDeviceResources()
{
}

void DeviceResources::CreateWindowSizeDependentResources()
{
}

void DeviceResources::SetWindow(HWND window, int width, int height) noexcept
{
}

void DeviceResources::WindowSizeChanged(int width, int height)
{
}

void DeviceResources::HandleDeviceLost()
{
}

void DeviceResources::Present()
{
}
