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
#if defined(_DEBUG) && (_WIN32_WINNT >= 0x603)	// 윈도우 8.1이상부터 작동
	bool debugDXGI = false;
	{
		ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			debugDXGI = true;

			ThrowIfFailed(CreateDXGIFactory2(
				DXGI_CREATE_FACTORY_DEBUG,
				IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())
			));

			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, 
				DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, 
				DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

			DXGI_INFO_QUEUE_MESSAGE_ID hide[]{		//모니터를 가져오는 함수
				80,
			};
			DXGI_INFO_QUEUE_FILTER filter{};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
		}

	}

	if (!debugDXGI)
#endif
	{
		ThrowIfFailed(CreateDXGIFactory1(
			IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())
		));
	}
}

void DeviceResources::GetHardwareAdapter(IDXGIAdapter1** ppAdapter)
{
	*ppAdapter = nullptr;

	ComPtr<IDXGIAdapter1> adapter;

#if defined(__dxgi1_6_h__) && defined(NTDDI_WIN10_RS4)
	ComPtr<IDXGIFactory6> factory6;
	HRESULT hr = m_dxgiFactory.As(&factory6);
	if (SUCCEEDED(hr))
	{
		for (UINT adapterIndex = 0;
			SUCCEEDED(factory6->EnumAdapterByGpuPreference(
				adapterIndex,
				DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
				IID_PPV_ARGS(adapter.ReleaseAndGetAddressOf())))
			; adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			ThrowIfFailed(adapter->GetDesc1(&desc));

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
#ifdef _DEBUG
			wchar_t buf[256]{};
			swprintf_s(buf, L"Direct3D Adapter (%u): VID: %04X, PID:%04X - %ls\n",
				adapterIndex,
				desc.VendorId,
				desc.DeviceId,
				desc.Description);
			OutputDebugStringW(buf);
#endif // _DEBUG

			break;
		}
			
			
	}
#endif

	if (!adapter)
	{
		for (UINT adapterIndex = 0;
			SUCCEEDED(m_dxgiFactory->EnumAdapters1(
				adapterIndex,
				adapter.ReleaseAndGetAddressOf()))
			; adapterIndex++)
		{
			DXGI_ADAPTER_DESC1 desc;
			ThrowIfFailed(adapter->GetDesc1(&desc));
			
			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				continue;
			}
#ifdef _DEBUG
			wchar_t buf[256]{};
			swprintf_s(buf, L"Direct3D Adapter (%u): VID: %04X, PID:%04X - %ls\n",
				adapterIndex,
				desc.VendorId,
				desc.DeviceId,
				desc.Description);
			OutputDebugStringW(buf);
#endif // _DEBUG

			break;
		}
	}
	*ppAdapter = adapter.Detach();	// move랑 비슷한 detach

}

void DeviceResources::UpdateColorSpace()
{
	DXGI_COLOR_SPACE_TYPE colorSpace{ DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 };

	bool isDisplayHDR10 = false;

#if defined(NTDDI_WIN10_RS2)
	if (m_swapChain)
	{
		ComPtr<IDXGIOutput> output;

		if (SUCCEEDED(m_swapChain->GetContainingOutput(output.GetAddressOf())))
		{
			ComPtr<IDXGIOutput6> output6;
			if (SUCCEEDED(output.As(&output6)))
			{
				DXGI_OUTPUT_DESC1 desc;
				ThrowIfFailed(output6->GetDesc1(&desc));
				if (desc.ColorSpace == DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020)
				{
					isDisplayHDR10 = true;
				}
			}
		}
	}
#endif

	if ((m_options & c_EnableHDR) && isDisplayHDR10)
	{
		switch (m_backBufferFormat)
		{
		case DXGI_FORMAT_R10G10B10A2_UNORM:
			colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020;
			break;
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
			colorSpace = DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709;
			break;

		default:
			break;
		}
	}

	
	m_colorSpace = colorSpace;

	//c_EnableHDR 옵션 + DXGI_FORMAT_R10G10B10A2_UNORM;

	ComPtr<IDXGISwapChain3> swapChain3;
	if (SUCCEEDED(m_swapChain.As(&swapChain3)))
	{
		UINT colorSpaceSupport = 0;
		if (SUCCEEDED(swapChain3->CheckColorSpaceSupport(m_colorSpace, &colorSpaceSupport)) &&
			colorSpaceSupport & DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT)
		{
			ThrowIfFailed(swapChain3->SetColorSpace1(colorSpace));
		}


	}
}

DeviceResources::DeviceResources(
	DXGI_FORMAT backBufferFormat,
	DXGI_FORMAT depthBufferFormat,
	UINT backBufferCount,
	D3D_FEATURE_LEVEL minFeatureLevel,
	unsigned int flags) noexcept :
	m_screenViewport{},
	m_backBufferFormat{ backBufferFormat },
	m_depthBufferFormat{ depthBufferFormat },
	m_backBufferCount{ backBufferCount },
	m_d3dMinFeatureLevel{ minFeatureLevel },
	m_window{ nullptr },
	m_d3dFeatureLevel{ D3D_FEATURE_LEVEL_9_1 },
	m_outputSize{ 0,0,1,1 },
	m_colorSpace{ DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709 },
	m_options{ flags | c_FlipPresent },
	m_deviceNotify{ nullptr }
{

}



void DeviceResources::CreateDeviceResources()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
	if (SdkLayersAvailable())
	{
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
	else
	{
		OutputDebugStringA("WARNING : Direct3D Debug Device is not available! \n");

	}
#endif

	CreateFactory();

	if (m_options & c_AllowTearing)
	{
		BOOL allowTearing = FALSE;
		
		ComPtr<IDXGIFactory5> factory5;
		HRESULT hr = m_dxgiFactory.As(&factory5);	// As는 형변환을 해주는 기능
		if (SUCCEEDED(hr))
		{
			hr = factory5->CheckFeatureSupport(
				DXGI_FEATURE_PRESENT_ALLOW_TEARING,
				&allowTearing,
				sizeof(allowTearing)
			);
		}

		if (FAILED(hr) || !allowTearing)
		{
			m_options &= ~c_AllowTearing;
#ifdef _DEBUG
			OutputDebugStringA("WARNING : VBR(Variable Refresh Rate not supported)\n");
#endif
		}
	}
	// HDR
	if(m_options & c_EnableHDR)
	{
		ComPtr<IDXGIFactory5> factory5;
		if (FAILED(m_dxgiFactory.As(&factory5)))
		{
			m_options &= ~c_EnableHDR;

#ifdef _DEBUG
			OutputDebugStringA("WARNING : HDR not supported \n");
#endif
		}
	}

	// Filp
	if (m_options & c_FlipPresent)
	{
		ComPtr<IDXGIFactory4> factory4;
		if (FAILED(m_dxgiFactory.As(&factory4)))
		{
			m_options &= ~c_FlipPresent;

#ifdef _DEBUG
			OutputDebugStringA("WARNING : FilpPresent not supported \n");
#endif
		}
	}
	
	static const D3D_FEATURE_LEVEL s_featureLevels[]{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	UINT featureLevelCount{ 0 };
	for (; featureLevelCount < _countof(s_featureLevels);++featureLevelCount)
	{
		if (s_featureLevels[featureLevelCount] < m_d3dMinFeatureLevel)
		{
			break;
		}
	}

	if (!featureLevelCount) // 0일때
	{
		throw std::out_of_range("Min Feature Level is too high");
	}

	// 이제 드디어 디바이스
	ComPtr<IDXGIAdapter1> adapter;
	GetHardwareAdapter(adapter.GetAddressOf());

	ComPtr<ID3D11Device> device;
	ComPtr<ID3D11DeviceContext> context;

	HRESULT hr = E_FAIL;
	if (adapter)
	{
		hr = D3D11CreateDevice(
			adapter.Get(),
			D3D_DRIVER_TYPE_UNKNOWN,
			nullptr,
			creationFlags,
			s_featureLevels,
			featureLevelCount,
			D3D11_SDK_VERSION,
			device.GetAddressOf(),
			&m_d3dFeatureLevel,
			context.GetAddressOf()
		);
	}
#if defined(NDEBUG)	// 조건부 컴파일
	else
	{
		throw std::exception("No Direct3D hardware device found");
	}
#else
	if (FAILED(hr))
	{
		hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_WARP,
			nullptr,
			creationFlags,
			s_featureLevels,
			featureLevelCount,
			D3D11_SDK_VERSION,
			device.GetAddressOf(),
			&m_d3dFeatureLevel,
			context.GetAddressOf()
		);
		if (SUCCEEDED(hr))
		{
			OutputDebugStringA("Direct3D Adapter - WARP\n");
		}
	}
#endif
	ThrowIfFailed(hr);

#ifndef NDEBUG
	ComPtr<ID3D11Debug> d3dDebug;
	if (SUCCEEDED(device.As(&d3dDebug)))
	{
		ComPtr<ID3D11InfoQueue> d3dInfoQueue;
		if (SUCCEEDED(d3dDebug.As(&d3dInfoQueue)))
		{
#ifdef _DEBUG
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true); 
			//디바이스 및 리소스 손상수준
			d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
			//에러수준

#endif // _DEBUG
			D3D11_MESSAGE_ID hide[] =

			{
				D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
			};
			D3D11_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = _countof(hide);
			filter.DenyList.pIDList = hide;
			d3dInfoQueue->AddStorageFilterEntries(&filter);
		}
	}
#endif // NDEBUG
	ThrowIfFailed(device.As(&m_d3dDevice));
	ThrowIfFailed(context.As(&m_d3dContext));
	ThrowIfFailed(context.As(&m_d3dAnnotation));
}

void DeviceResources::CreateWindowSizeDependentResources()
{
	if (!m_window)
	{
		throw std::exception("Call SetWindow with a valid Win32 window handle");
	}

	ID3D11RenderTargetView* nullViews[] = { nullptr };
	m_d3dContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);
	m_d3dRenderTargetView.Reset();
	m_d3dDepthStencilView.Reset();
	m_renderTarget.Reset();
	m_depthStencil.Reset();
	m_d3dContext->Flush();

	const UINT backBufferWidth = std::max<UINT>(					// 가로세로 크기 구하기
		static_cast<UINT>(m_outputSize.right - m_outputSize.left),
		1u															// max를 사용하여 크키가 1보다 작아지는걸 방지
		);
	const UINT backBufferHeight = std::max<UINT>(
		static_cast<UINT>(m_outputSize.bottom - m_outputSize.top),
		1u
		);
	const DXGI_FORMAT backBufferFormat =
		(m_options & (c_FlipPresent | c_AllowTearing | c_EnableHDR)) ?
		NoSRGB(m_backBufferFormat) : m_backBufferFormat;

	// 스왑체인
	if (m_swapChain)
	{
		HRESULT hr = m_swapChain->ResizeBuffers(
			m_backBufferCount,
			backBufferWidth,
			backBufferHeight,
			backBufferFormat,
			(m_options & c_AllowTearing) ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u
		);

		if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
		{
#ifdef _DEBUG
			char buff[64] = {};
			sprintf_s(buff, "Device Lost on ResizeBuffers : Reaseon code 0x%08X\n",
				static_cast<unsigned int>((hr == DXGI_ERROR_DEVICE_REMOVED) ? 
					m_d3dDevice->GetDeviceRemovedReason() : hr));
			OutputDebugStringA(buff);
#endif
			HandleDeviceLost();

			return;
		}
		else
		{
			ThrowIfFailed(hr);
		}
	}
	else
	{
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));
		swapChainDesc.Width = backBufferWidth;
		swapChainDesc.Height = backBufferHeight;
		swapChainDesc.Format = backBufferFormat;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = m_backBufferCount;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.SwapEffect =
			(m_options & (c_FlipPresent | c_AllowTearing | c_EnableHDR)) ?
			DXGI_SWAP_EFFECT_FLIP_DISCARD : DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Flags =
			(m_options & c_AllowTearing) ?
			DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fsSwapChainDesc{};
		ZeroMemory(&fsSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
		fsSwapChainDesc.Windowed = TRUE;

		ThrowIfFailed(m_dxgiFactory->CreateSwapChainForHwnd(
			m_d3dDevice.Get(),
			m_window,
			&swapChainDesc,
			&fsSwapChainDesc,
			nullptr, 
			m_swapChain.ReleaseAndGetAddressOf()
		));

		ThrowIfFailed(m_dxgiFactory->MakeWindowAssociation(
			m_window, DXGI_MWA_NO_ALT_ENTER)
		);
	}

	// 렌더 타겟 뷰
	UpdateColorSpace();

	ThrowIfFailed(
		m_swapChain->GetBuffer(
			0, IID_PPV_ARGS(m_renderTarget.ReleaseAndGetAddressOf())
		)
	);

	CD3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc(
		D3D11_RTV_DIMENSION_TEXTURE2D, m_backBufferFormat
	);
	ThrowIfFailed(
		m_d3dDevice->CreateRenderTargetView
		(
			m_renderTarget.Get(),
			&renderTargetViewDesc,
			m_d3dRenderTargetView.ReleaseAndGetAddressOf()
		)
	);

	//깊이 스텐실 버퍼
	if (m_depthBufferFormat != DXGI_FORMAT_UNKNOWN)
	{
		CD3D11_TEXTURE2D_DESC depthStencilDesc(
			m_depthBufferFormat,
			backBufferWidth,
			backBufferHeight,
			1,
			1,
			D3D11_BIND_DEPTH_STENCIL
		);
		ThrowIfFailed(m_d3dDevice->CreateTexture2D(
			&depthStencilDesc,
			nullptr,
			m_depthStencil.ReleaseAndGetAddressOf()
		));

		CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(
			D3D11_DSV_DIMENSION_TEXTURE2D
		);
		ThrowIfFailed(m_d3dDevice->CreateDepthStencilView(
			m_depthStencil.Get(),
			&depthStencilViewDesc,
			m_d3dDepthStencilView.ReleaseAndGetAddressOf()
		));

	}

	// 뷰포트
	m_screenViewport = CD3D11_VIEWPORT(
		0.0f,
		0.0f,
		static_cast<float>(backBufferWidth),
		static_cast<float>(backBufferHeight)
	);
}

void DeviceResources::SetWindow(HWND window, int width, int height) noexcept
{
	m_window = window;

	m_outputSize.left = m_outputSize.top = 0;
	m_outputSize.right = width;
	m_outputSize.bottom = height;
}

bool DeviceResources::WindowSizeChanged(int width, int height)
{
	RECT newRc{
		0,0,
		width, height
	};
	/*newRc.left = newRc.top = 0;
	newRc.right = width;
	newRc.bottom = height;*/

	if (newRc == m_outputSize)
	{
		UpdateColorSpace();

		return false;
	}
	m_outputSize = newRc;

	CreateWindowSizeDependentResources();

	return true;
}

void DeviceResources::HandleDeviceLost()
{
	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceLost();
	}

	m_d3dDepthStencilView.Reset();
	m_d3dRenderTargetView.Reset();
	m_renderTarget.Reset();
	m_depthStencil.Reset();
	m_swapChain.Reset();
	m_d3dContext.Reset();
	m_d3dAnnotation.Reset();

#ifdef _DEBUG
	{
		ComPtr<ID3D11Debug> d3dDebug;
		if (SUCCEEDED(m_d3dDevice.As(&d3dDebug)))
		{
			d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY);
		}
	}

#endif
	m_d3dDevice.Reset();
	m_dxgiFactory.Reset();

	CreateDeviceResources();
	CreateWindowSizeDependentResources();


	if (m_deviceNotify)
	{
		m_deviceNotify->OnDeviceRestored();
	}
}

void DeviceResources::Present()
{
	HRESULT hr = E_FAIL;

	if (m_options & c_AllowTearing)
	{
		hr = m_swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
	}
	else
	{
		hr = m_swapChain->Present(1, 0);
	}

	m_d3dContext->DiscardView(m_d3dRenderTargetView.Get());
	if (m_d3dDepthStencilView)
	{
		m_d3dContext->DiscardView(m_d3dDepthStencilView.Get());
	}

	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
#ifdef _DEBUG
		char buf[64]{};
		sprintf_s(buf, "Device Lost on Present(): 0x%08X\n",
			static_cast<unsigned int>(hr == DXGI_ERROR_DEVICE_REMOVED ? 
				m_d3dDevice->GetDeviceRemovedReason() : hr)
		);
		OutputDebugStringA(buf);
		
#endif // _DEBUG
		HandleDeviceLost();
	}
	else
	{
		ThrowIfFailed(hr);

		// HDR : 정보량이 많음 -> 캐시(Cache)
		if (!m_dxgiFactory->IsCurrent()) // 팩토리가 다르면 다시 만들어야함
		{
			CreateFactory();
		}
	}
}
