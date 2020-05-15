#include "pch.h"
#include "MainWindow.h"
#include <shobjidl.h>

#define CHECKCOM(x) if(FAILED(x)) return 0


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_LBUTTONDOWN:
    {
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        CHECKCOM(hr);

        IFileOpenDialog* pFileOpen;
        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&pFileOpen));
        CHECKCOM(hr);

        hr = pFileOpen->Show(NULL);
        CHECKCOM(hr);

        IShellItem* pItem;
        hr = pFileOpen->GetResult(&pItem);
        CHECKCOM(hr);

        PWSTR pszFilePath;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        CHECKCOM(hr);

        MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
        CoTaskMemFree(pszFilePath);

        pItem->Release();
        pFileOpen->Release();

        CoUninitialize();
        return 0;

    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(m_hwnd, &ps);
    }
    return 0;

    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;

}


void MainWindow::SetIcon(HICON hIcon) {
    wc.hIcon = hIcon;
}