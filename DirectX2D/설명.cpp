// Direct2D FrameWork


//	Microsoft COM ����Ʈ ������
//	Microsoft::WRL::ComPtr<T>

//	1. namespace
//		.cpp
//		using namespace Microsoft::WRL;
//		ComPtr<ID2D1Factory> spD2DFactory;

//		.h
//		Microsoft::WRL::ComPtr<ID2D1Factory> spD2DFactory;

//	2. �ʱ�ȭ
//
//	D2D1CreateFactory(..., &spD2DFactory.Get());
//  D2D1CreateFactory(..., &spD2DFactory.GetAddress0f());
//	D2D1CreateFactory(..., &spD2DFactory.ReleaseAndGetAddress0f());

//	3. ����
//		�ڵ�
//		spD2DFactory.Reset();
//		spD2DFactory = nullptr;

//	4. ����
// 
//		void func(ComPtr<T>& x)
//		{
//			x
//		}
//
//		func(spD2DFactory);
//		������ �ߺ��� �������ؼ� �Ű������� ����������