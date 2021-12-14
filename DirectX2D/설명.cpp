// Direct2D FrameWork


//	Microsoft COM 스마트 포인터
//	Microsoft::WRL::ComPtr<T>

//	1. namespace
//		.cpp
//		using namespace Microsoft::WRL;
//		ComPtr<ID2D1Factory> spD2DFactory;

//		.h
//		Microsoft::WRL::ComPtr<ID2D1Factory> spD2DFactory;

//	2. 초기화
//
//	D2D1CreateFactory(..., &spD2DFactory.Get());
//  D2D1CreateFactory(..., &spD2DFactory.GetAddress0f());
//	D2D1CreateFactory(..., &spD2DFactory.ReleaseAndGetAddress0f());

//	3. 해제
//		자동
//		spD2DFactory.Reset();
//		spD2DFactory = nullptr;

//	4. 주의
// 
//		void func(ComPtr<T>& x)
//		{
//			x
//		}
//
//		func(spD2DFactory);
//		소유권 중복을 막기위해서 매개변수는 참조형으로