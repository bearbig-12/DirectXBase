#include "D3DFrameWork.h"

#include <sstream>

#pragma comment (lib, "d3d11.lib")

void D3DFramework::Initialize(HINSTANCE hInstance, int width, int height)
{
	mScreenWidth = width;
	mScreenHeight = height;
	mPaused = false;

	InitWindow(hInstance);
	InitD3D();
}

void D3DFramework::InitWindow(HINSTANCE hInstance)
{
	WNDCLASSEX wc;


	ZeroMemory(&wc, sizeof(WNDCLASSEX));


	mInstance = hInstance;


	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpszClassName = mClassName.c_str();
	wc.hInstance = mInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpfnWndProc = WindowProc;
	wc.cbSize = sizeof(WNDCLASSEX);


	if (!RegisterClassEx(&wc))
	{
		return;
	}


	RECT wr{ 0, 0, mScreenWidth, mScreenHeight };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

	mTitleText = mTitle;

	mHwnd = CreateWindowEx(
		NULL,
		mClassName.c_str(),
		mTitleText.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wr.right - wr.left,
		wr.bottom - wr.top,
		NULL,
		NULL,
		mInstance,
		NULL
	);


	if (mHwnd == nullptr)
	{
		return;
	}

	SetWindowLongPtr(mHwnd, GWLP_USERDATA,
		reinterpret_cast<LONG_PTR>(this));


	ShowWindow(mHwnd, SW_SHOW);


	SetForegroundWindow(mHwnd);
	SetFocus(mHwnd);


	UpdateWindow(mHwnd);

}

void D3DFramework::InitD3D()
{
	// Device 만들기
	// SwapChain 만들기


	DXGI_SWAP_CHAIN_DESC scd{};


	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Width = mScreenWidth;
	scd.BufferDesc.Height = mScreenHeight;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = mHwnd;
	scd.Windowed = TRUE;
	scd.SampleDesc.Count = 1;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;




	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&scd,
		mspSwapChain.ReleaseAndGetAddressOf(),
		mspDevice.ReleaseAndGetAddressOf(),
		NULL,
		mspDeviceContext.ReleaseAndGetAddressOf()
	);


	OnResize();

}

void D3DFramework::CalculateFPS()
{
	// 매 업데이트 마다 불림
	static int FrameCount{ 0 };
	static float timeElapsed{ 0.0f }; // 경과시간

	FrameCount++;

	if (mTimer.TotalTime() - timeElapsed >= 1.0f)	// 1초마다 fps 갱신
	{
		float fps = (float)FrameCount; // 1초에 몇프레임을 그리느냐
		float mspf = 1000.0f / fps;	   // 한프레임을 걸리는데 걸리는 시간

		std::wostringstream oss;
		oss.precision(6); //소숫점 6자리까지

		oss << mTitleText << L" - " <<
			L"FPS : " << fps <<
			L", Frame Time : " << mspf << L"(ms)";

		SetWindowText(mHwnd, oss.str().c_str());

		FrameCount = 0;
		timeElapsed += 1.0f; // 1초 경과
	}

}

void D3DFramework::OnResize()
{
	/////////////////////////////////////////


	ID3D11RenderTargetView* nullViews[]{ nullptr };
	mspDeviceContext->OMSetRenderTargets(_countof(nullViews), nullViews, nullptr);


	mspRenderTargetView.Reset();
	mspDepthStencilView.Reset();
	mspRenderTarget.Reset();
	mspDepthStencil.Reset();


	mspDeviceContext->Flush();


	/////////////////////////////////////////


	mspSwapChain->ResizeBuffers(0, mScreenWidth, mScreenHeight, DXGI_FORMAT_UNKNOWN, 0);


	// 렌더타겟
	mspSwapChain->GetBuffer(0, IID_PPV_ARGS(mspRenderTarget.ReleaseAndGetAddressOf()));


	mspDevice->CreateRenderTargetView(mspRenderTarget.Get(),
		nullptr,
		mspRenderTargetView.ReleaseAndGetAddressOf());


	// 깊이-스텐실
	CD3D11_TEXTURE2D_DESC dtd(
		DXGI_FORMAT_D24_UNORM_S8_UINT,
		mScreenWidth, mScreenHeight,
		1,
		1,
		D3D11_BIND_DEPTH_STENCIL
	);


	mspDevice->CreateTexture2D(&dtd, nullptr, mspDepthStencil.ReleaseAndGetAddressOf());


	CD3D11_DEPTH_STENCIL_VIEW_DESC dsvd(D3D11_DSV_DIMENSION_TEXTURE2D);


	mspDevice->CreateDepthStencilView(
		mspDepthStencil.Get(),
		&dsvd,
		mspDepthStencilView.ReleaseAndGetAddressOf()
	);


	// 파이프라인
	mspDeviceContext->OMSetRenderTargets(
		1, mspRenderTargetView.GetAddressOf(),
		mspDepthStencilView.Get());


	// 뷰포트(View-Port)
	CD3D11_VIEWPORT viewport(0.0f, 0.0f,
		static_cast<float>(mScreenWidth), static_cast<float>(mScreenHeight));


	mspDeviceContext->RSSetViewports(1, &viewport);

}

void D3DFramework::RenderFrame()
{
	float clear_color[]{ 0.0f, 0.2f, 0.4f, 1.0f };
	mspDeviceContext->ClearRenderTargetView(mspRenderTargetView.Get(), clear_color);
	mspDeviceContext->ClearDepthStencilView(mspDepthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	Render();

	// 그리기
	mspSwapChain->Present(0, 0);
}



void D3DFramework::Destroy()
{
	mspSwapChain->SetFullscreenState(FALSE, nullptr);


	mspDepthStencilView.Reset();
	mspDepthStencil.Reset();
	mspRenderTargetView.Reset();
	mspRenderTarget.Reset();


	mspSwapChain.Reset();
	mspDevice.Reset();
	mspDeviceContext.Reset();


	DestroyWindow(mHwnd);
	UnregisterClass(mClassName.c_str(), mInstance);
}

void D3DFramework::GameLoop()
{
	mTimer.Start();
	mInput.Initialize();

	MSG msg{};
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);


			if (msg.message == WM_QUIT)
			{
				break;
			}
		}
		else
		{
			mTimer.Update();

			if (mPaused)
			{
				Sleep(100);	// 0.1초 
			}
			else
			{
				CalculateFPS();

				Update(mTimer.DeltaTime());
				// Game Loop
				RenderFrame();
			}
		
		}
	}

}



LRESULT D3DFramework::MessageHandle(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		mInput.SetKeyDown(wparam);
		break;

	case WM_KEYUP:
		mInput.SetKeyUp(wparam);
		break;

	case WM_LBUTTONDOWN:
		mInput.SetKeyDown(VK_LBUTTON);
		break;

	case WM_LBUTTONUP:
		mInput.SetKeyUp(VK_LBUTTON);
		break;
	case WM_MOUSEMOVE:
		mInput.SetCursor(LOWORD(lparam), HIWORD(lparam));
		break;

	case WM_ACTIVATE:
		if (LOWORD(wparam) == WA_INACTIVE)
		{
			mPaused = true;
			mTimer.Stop();
		}
		else
		{
			mPaused = false;
			mTimer.Resume();
		}
		break;

	case WM_PAINT:
	{
		if (mResizing)
		{
			RenderFrame();
		}
		else
		{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		}
	}
	break;


	case WM_SIZE:
		mScreenWidth = LOWORD(lparam);
		mScreenHeight = HIWORD(lparam);

		if (mspDevice)
		{
			if (wparam == SIZE_MINIMIZED)
			{
				if (!mPaused)
				{
					mTimer.Stop();
				}
				mPaused = true;

				mMinimized = true;
				mMaximized = false;
			}
			else if (wparam == SIZE_MAXIMIZED)
			{
				mTimer.Resume();
				mPaused = false;

				mMinimized = false;
				mMaximized = true;
				OnResize();
			}
			else if (wparam == SIZE_RESTORED)
			{
				if (mMinimized)
				{
					mPaused = false;
					mTimer.Resume();

					mMinimized = false;
					OnResize();
				}
				else if (mMaximized)
				{
					mPaused = false;
					mTimer.Resume();

					mMaximized = false;
					OnResize();
				}
				else if (mResizing)
				{


				}
				else
				{
					mPaused = false;
					mTimer.Resume();

					OnResize();
				}
			}
		}
		return 0;
		break;


	case WM_ENTERSIZEMOVE:
		mResizing = true;
		return 0;
		break;


	case WM_EXITSIZEMOVE:
		mResizing = false;
		OnResize();
		return 0;
		break;


	case WM_MENUCHAR:
		return MAKELRESULT(0, MNC_CLOSE);
		break;


	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lparam)->ptMinTrackSize.x = 640;
		((MINMAXINFO*)lparam)->ptMinTrackSize.y = 480;
		return 0;
		break;


	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;


	case WM_DESTROY:
		PostQuitMessage(0);
		break;


	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}

	return LRESULT();
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	D3DFramework* pFramework = reinterpret_cast<D3DFramework*>(
		GetWindowLongPtr(hwnd, GWLP_USERDATA));

	return pFramework->MessageHandle(hwnd, message, wparam, lparam);
}