#include "pch.h"
#include "Window/MainWindow.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {

	MainWindow win;

	if (!win.CreateBaseWindow(L"Win32API Project", WS_OVERLAPPEDWINDOW))
		return 0;


	ShowWindow(win.GetWindow(), nCmdShow);

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0)) {

		TranslateMessage(&msg);
		DispatchMessage(&msg);

	}

	return 0;
}
