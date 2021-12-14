#include <Windows.h>
#include "D2DFramWork.h"


LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
void OnPaint(HWND hwnd);

D2DFramWork myFramework;


int WINAPI WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd
)
{	
	int ret{ 0 };
	try 
	{
		D2DFramWork myFrameWork;
		if (SUCCEEDED(myFrameWork.Initialize(hInstance)))
		{
			ret = myFrameWork.GameLoop();
		}
		
	}
	catch (com_exception& e)
	{
		static wchar_t wstr[128]{};
		size_t len;
		mbstowcs_s(&len, wstr, e.what(), 128);	// 아스키코드를 유니코드로
		
		D2DFramWork::ShowErrorMsg(wstr);
	}

	//myFramework.Release();

	return ret;
}

