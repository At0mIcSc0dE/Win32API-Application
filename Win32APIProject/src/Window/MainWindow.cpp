#include "pch.h"
#include "MainWindow.h"
#include <shobjidl.h>   //FileOpenDialog...
#include <atlbase.h>    //CComPtr...
#include <winerror.h>   //Error codes of hr(HRESULT)

#define CHECKCOM(x) if(FAILED(x)) return 0


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_LBUTTONDOWN:
    {
        {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
            CHECKCOM(hr);

            CComPtr<IFileOpenDialog> pFileOpen;
            hr = CoCreateInstance(__uuidof(FileOpenDialog), NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));
            CHECKCOM(hr);

            CComPtr<IFileDialogCustomize> pCustom;
            hr = pFileOpen->QueryInterface(IID_PPV_ARGS(&pCustom)); //IID_PPV_ARGS prevents casting to wrong type
            CHECKCOM(hr);

            hr = pFileOpen->Show(NULL);
            CHECKCOM(hr);

            CComPtr<IShellItem> pItem;
            hr = pFileOpen->GetResult(&pItem);
            CHECKCOM(hr);

            PWSTR pszFilePath;
            hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
            CHECKCOM(hr);

            MessageBox(NULL, pszFilePath, L"File Path", MB_OK);
            CoTaskMemFree(pszFilePath);
        }

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