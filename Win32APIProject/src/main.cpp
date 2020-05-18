#include "pch.h"
#include "Window/MainWindow.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {


	MainWindow win;

	//win.SetIcon((HICON)LoadImage(hInstance, L"test.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE));
	if (!win.Create(L"Win32API Project", WS_OVERLAPPEDWINDOW))
		return 0;

	ShowWindow(win.Window(), nCmdShow);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}
	
	return 0;
}

