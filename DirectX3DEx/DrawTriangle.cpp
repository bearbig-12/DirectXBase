#include "DrawTriangle.h"

#include <fstream>
#include <vector>



void DrawTriangle::Initialize(HINSTANCE hInstance, int width, int height)
{
	D3DFramework::Initialize(hInstance, width, height);

	InitPipeline();
	InitTriangle();

	CreateTextureFromBMP();
}

void DrawTriangle::Destroy()
{
	mspBlendState.Reset();
	mspSamplerState.Reset();
	mspTextureView.Reset();
	mspTexture.Reset();
	
	mspVertexBuffer.Reset();
	mspInputLayout.Reset();
	mspPixelShader.Reset();
	mspVertexShader.Reset();

	D3DFramework::Destroy();
}

void DrawTriangle::Update(float delta)
{
	if (mInput.IsKeyDown('Q'))
	{
		mRotationZ += DirectX::XM_PI * delta;
	}
	else if (mInput.IsKeyDown('E'))
	{
		mRotationZ -= DirectX::XM_PI * delta;

	}

	if (mInput.IsKeyDown(VK_LEFT))
	{
		mX -= 1.0f * delta;
	}
	else if(mInput.IsKeyDown(VK_RIGHT))
	{
		mX += 1.0f * delta;
	}

	if (mInput.IsKeyDown(VK_UP))
	{
		mY += 1.0f * delta;
	}
	else if (mInput.IsKeyDown(VK_DOWN))
	{
		mY -= 1.0f * delta;

	}
	if (mInput.IsKeyDown('1'))
	{
		mTimer.SetScale(1.0f);
	}
	else if (mInput.IsKeyDown('2'))
	{
		mTimer.SetScale(2.0f);

	}
	else if (mInput.IsKeyDown('3'))
	{
		mTimer.SetScale(0.5f);

	}

	mWorld = DirectX::XMMatrixIdentity();
	mWorld *= DirectX::XMMatrixRotationZ(mRotationZ);
	mWorld *= DirectX::XMMatrixTranslation(mX, mY, 0.0f);
}

void DrawTriangle::Render()
{
	UINT offset = 0;
	UINT stride = sizeof(VERTEX);
	mspDeviceContext->IASetVertexBuffers(
		0, 1, mspVertexBuffer.GetAddressOf(), &stride, &offset
	);

	mspDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//mspDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_);

	mspDeviceContext->PSSetShaderResources(0, 1, mspTextureView.GetAddressOf());
	mspDeviceContext->PSSetSamplers(0, 1, mspSamplerState.GetAddressOf());
	mspDeviceContext->OMSetBlendState(mspBlendState.Get(), nullptr,0xffffffff);

	MatrixBuffer mb;
	mb.world = DirectX::XMMatrixTranspose(mWorld);
	mspDeviceContext->UpdateSubresource(mspConstantBuffer.Get(), 0, nullptr,
			&mb, 0, 0
		);

	mspDeviceContext->Draw(4,0);
	//mspDeviceContext->Draw(4, 0);
	
}

HRESULT DrawTriangle::CreateTextureFromBMP()
{
	// 1. 파일 열기
	std::ifstream file;
	file.open("D:\\Git hub\\SimpleData\\32.bmp", std::ios::binary);

	BITMAPFILEHEADER bmh;
	BITMAPINFOHEADER bmi;

	// 2. BITMAPFILEHEADER 읽기
	file.read(reinterpret_cast<char*>(&bmh), sizeof(BITMAPFILEHEADER));
	// 3. BITMAPINFOGHEADER 읽기
	file.read(reinterpret_cast<char*>(&bmi), sizeof(BITMAPINFOHEADER));
	if (bmh.bfType != 0x4D42)
	{
		return E_FAIL;
	}
	if (bmi.biBitCount != 32)
	{
		return E_FAIL;
	}

	std::vector<char> pPixels(bmi.biSizeImage);
	// 4. 픽셀로 건너뛰기
	file.seekg(bmh.bfOffBits);
	// 5. 비트맵 읽기
	int pitch = bmi.biWidth * (bmi.biBitCount / 8);

	/*for (int y = bmi.biHeight - 1; y >= 0; --y)
	{
		file.read(&pPixels[y * pitch], pitch);
	}*/

	int index{};
	char r{}, g{}, b{}, a{};

	for (int y{ bmi.biHeight - 1 }; y >= 0; --y)
	{
		index = y * pitch;
		for (int x{ 0 }; x < bmi.biWidth; ++x)
		{
			file.read(&b, 1);
			file.read(&g, 1);
			file.read(&r, 1);
			file.read(&a, 1);

			if (static_cast<unsigned char>(r) == 30 && 
				static_cast<unsigned char>(g) == 199 &&
				static_cast<unsigned char>(b) == 250)
			{
				pPixels[index] = pPixels[index + 1] = pPixels[index + 2] = pPixels[index + 3] = 0;
			}
			else
			{
				pPixels[index] = b;
				pPixels[index +1] = g;
				pPixels[index + 2] = r;
				pPixels[index + 3] = 255;

			}
			index += 4;
		}
	}
	file.close();


	// 텍스쳐 생성
	CD3D11_TEXTURE2D_DESC td(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		bmi.biWidth,
		bmi.biHeight,
		1,
		1
	);

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = (void*)&pPixels[0];
	initData.SysMemPitch = pitch;
	initData.SysMemSlicePitch = 0;

	mspDevice->CreateTexture2D(&td, &initData, mspTexture.ReleaseAndGetAddressOf());

	// View (Shader Resource View)

	CD3D11_SHADER_RESOURCE_VIEW_DESC srvd(
		D3D11_SRV_DIMENSION_TEXTURE2D,
		td.Format,
		0,
		1
	);


	mspDevice->CreateShaderResourceView(
		mspTexture.Get(),
		&srvd,
		mspTextureView.ReleaseAndGetAddressOf()
	);

	return S_OK;
}

void DrawTriangle::InitTriangle()
{
	VERTEX vertices[]
	{
		{ -0.5f,  0.5f, 0.0f, 0.0f, 0.0f},
		{  0.5f,  0.5f, 0.0f, 1.0f, 0.0f},
		{ -0.5f, -0.5f, 0.0f, 0.0f, 1.0f},
		{  0.5f, -0.5f, 0.0f, 1.0f, 1.0f}
	
	};

	// 버텍스 버퍼
	CD3D11_BUFFER_DESC bd(
		sizeof(vertices),
		D3D11_BIND_VERTEX_BUFFER,
		D3D11_USAGE_DYNAMIC,			// GPU 읽기, CPU 쓰기
		D3D11_CPU_ACCESS_WRITE
	);
	mspDevice->CreateBuffer(			//그래픽카드가 관리
		&bd, nullptr,
		mspVertexBuffer.ReleaseAndGetAddressOf()
	);

	// 그래픽 카드 버퍼에 데이터를 저장
	D3D11_MAPPED_SUBRESOURCE ms;

	mspDeviceContext->Map(
		mspVertexBuffer.Get(),
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&ms
	);
	memcpy(ms.pData, vertices, sizeof(vertices));
	mspDeviceContext->Unmap(
		mspVertexBuffer.Get(), 0);

	float border[4]{ 0.0f,0.0f,0.0f,0.0f };

	CD3D11_SAMPLER_DESC sd(
		D3D11_FILTER_MIN_MAG_MIP_POINT,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		0.0f,
		1,
		D3D11_COMPARISON_ALWAYS,
		border,
		0,
		1
	);
	mspDevice->CreateSamplerState(&sd, mspSamplerState.ReleaseAndGetAddressOf());

	D3D11_BLEND_DESC blend_desc;
	ZeroMemory(&blend_desc, sizeof(D3D11_BLEND_DESC));

	blend_desc.RenderTarget[0].BlendEnable = true;

	// source * 1 + destination * (1-alpha)
	blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;	// 픽셀쉐이더
	blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;	// 렌더타겟

	//Alpha : src * 1 + dst * 0
	blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;

	
	blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	mspDevice->CreateBlendState(&blend_desc, mspBlendState.ReleaseAndGetAddressOf());

	bd = CD3D11_BUFFER_DESC(
		sizeof(MatrixBuffer),
		D3D11_BIND_CONSTANT_BUFFER,
		D3D11_USAGE_DEFAULT
	);

	mspDevice->CreateBuffer(&bd, nullptr, mspConstantBuffer.ReleaseAndGetAddressOf());
	mspDeviceContext->VSSetConstantBuffers(0, 1, mspConstantBuffer.GetAddressOf());


	mX = mY = 0.0f;
	mRotationZ = 0.0f;
}

void DrawTriangle::InitPipeline()
{
	//셰이더 컴파일
	Microsoft::WRL::ComPtr<ID3DBlob> spVS;
	Microsoft::WRL::ComPtr<ID3DBlob> spPS;

	//버텍스 셰이더
	D3DCompileFromFile(
		L"VertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_4_0_level_9_3",
		0, 0,
		spVS.GetAddressOf(),
		nullptr
	);

	//픽셀 셰이더
	D3DCompileFromFile(
		L"PixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_4_0_level_9_3",
		0, 0,
		spPS.GetAddressOf(),
		nullptr
	);

	//버텍스 셰이더
	mspDevice->CreateVertexShader(
		spVS->GetBufferPointer(),
		spVS->GetBufferSize(),
		nullptr,
		mspVertexShader.ReleaseAndGetAddressOf()
	);

	//픽셀 셰이더
	mspDevice->CreatePixelShader(
		spPS->GetBufferPointer(),
		spPS->GetBufferSize(),
		nullptr,
		mspPixelShader.ReleaseAndGetAddressOf()
	);

	// IA
	D3D11_INPUT_ELEMENT_DESC ied[]
	{
		{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT,
			0,	// Slot number
			0,	// Offset
			D3D11_INPUT_PER_VERTEX_DATA, // Slot Classification
			0
		},

		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,
			0,
			12,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		}
	};

	mspDevice->CreateInputLayout(
		ied,
		2,
		spVS->GetBufferPointer(),
		spVS->GetBufferSize(),
		mspInputLayout.ReleaseAndGetAddressOf()
	);

		// 파이프라인 조립
		// IA
		// VS
		// PS
	mspDeviceContext->IASetInputLayout(mspInputLayout.Get());
	mspDeviceContext->VSSetShader(mspVertexShader.Get(), nullptr, 0);
	mspDeviceContext->PSSetShader(mspPixelShader.Get(), nullptr, 0);
}
