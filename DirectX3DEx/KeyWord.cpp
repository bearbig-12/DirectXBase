//	D3D

//	스왑체인 생성
//	후면버퍼 -> 렌더타겟
//	깊이 스텐실 버퍼
//	뷰포트

//	후면버퍼 지우기
//	깊이 버퍼 지우기
//	그리기
//	Present()

// 셰이더 & 파이프라인

//	 컴파일
//	 버텍스셰이더 & 픽셀 셰이더 생성
//	 Input Layout 구조
//	 파이프라인에 연결
//	 IA
//	 VS
//	 PS
// 그리기


// Device

//	CreateBuffer
//	CreateVertexShader
//	CreatePixelShader
//	CreateInputLayout
//	CreateRenderTargetView
//	CreateTexture2D
//	CreateDepthStencilView

// Device Context

//	Map, Unmap
//	VSSetShader
//	PSSetShader
//	IASetInputLayout
//	IASetVertexBuffers
//	IASetPrimitiveTopology
//	RSSetViewports

//	ClearRenderTargetView
//	ClearDepthStencilView
//	Draw


// 텍스쳐

//		1. 버텍스 정보에 텍스쳐 좌표 (u, v) 추가
//		2. 텍스쳐 생성 (ID3D11Texture2D)
//		3. 픽셀 데이터 = 배열
//		4. 3번에 대한 리소스 뷰를 생성 (ID3D11ShaderResourceView)
//		5. 입력 레이아웃 변경 (1번)
//		6. 랜더링 ( 파이프 라인에 텍스쳐 정보추가)