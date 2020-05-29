#include "pch.h"
#include "MainWindow.h"
#include <shobjidl.h>   //FileOpenDialog...
#include <atlbase.h>    //CComPtr...
#include <winerror.h>   //Error codes of hr(HRESULT)
#include <windowsx.h>   //GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)    (gets pixel out of lParam)

#include "DPIScale.hpp"


#define CHECKCOM(x) if(FAILED(x)) return 0


template<typename T>
void SafeRelease(T * *ppT) {

    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }

}


LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

    wchar_t keyPressedMsg[64];
    switch (uMsg)
    {
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case ID_DRAW_MODE:
            MessageBox(m_hwnd, L"DrawMode", L"", MB_OK);
            break;
        case ID_SELECT_MODE:
            MessageBox(m_hwnd, L"SelectMode", L"", MB_OK);
            break;
        case ID_TOGGLE_MODE:
            MessageBox(m_hwnd, L"SelectOrDrawMode", L"", MB_OK);
            break;
        }
        return 0;

    case WM_SETCURSOR:
    {
        auto hCursor = LoadCursor(NULL, IDC_ARROW);
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(hCursor);
            return TRUE;
        }
        break;
    }
    return 0;
    case WM_SYSKEYDOWN:
        swprintf_s(keyPressedMsg, L"WM_SYSKEYDOWN: 0x%x\n", wParam);
        OutputDebugString(keyPressedMsg);
        break;

    case WM_SYSCHAR:
        swprintf_s(keyPressedMsg, L"WM_SYSCHAR: %c\n", (wchar_t)wParam);
        OutputDebugString(keyPressedMsg);
        break;

    case WM_SYSKEYUP:
        swprintf_s(keyPressedMsg, L"WM_SYSKEYUP: 0x%x\n", wParam);
        OutputDebugString(keyPressedMsg);
        break;

    case WM_KEYDOWN:
        swprintf_s(keyPressedMsg, L"WM_KEYDOWN: 0x%x\n", wParam);
        OutputDebugString(keyPressedMsg);
        break;

    case WM_KEYUP:
        swprintf_s(keyPressedMsg, L"WM_KEYUP: 0x%x\n", wParam);
        OutputDebugString(keyPressedMsg);
        break;

    case WM_CHAR:
        swprintf_s(keyPressedMsg, L"WM_CHAR: %c\n", (wchar_t)wParam);
        OutputDebugString(keyPressedMsg);
        break;
    case WM_CREATE:
    {
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_Factory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        DPIScale::Initialize(m_Factory);
        return 0;
    }
    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&m_Factory);
        PostQuitMessage(0);
        return 0;
    return 0;
    case WM_MOUSEWHEEL:
    {
        //Prints direction of scroll:   positive -> forward, away from the user
        //                              negative -> backwards, to the user
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        wchar_t buffer[256];
        wsprintfW(buffer, L"%d", delta);
        MessageBoxW(nullptr, buffer, buffer, MB_OK);
    }
        return 0;
    case WM_LBUTTONDOWN:
    {   
        //0x8000 contains the bit flag that tests whether the key is currently pressed.
        if (GetKeyState(VK_MENU) & 0x8000)
        {
            // ALT Key is down
            //MessageBox(m_hwnd, L"ALT KEY DOWN", L"", MB_OK);
        }

        //Checks if the user wants to drag or just click
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (DragDetect(m_hwnd, pt))
        {
            OnLButtonDown(pt.x, pt.y, (DWORD)wParam);
        }
        return 0;
    }
    case WM_LBUTTONUP:
        OnLButtonUp();
        return 0;

    case WM_MOUSEMOVE:
        OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;
    
    case WM_PAINT:
    {
        OnPaint();
        return 0;
    }
    case WM_SIZE:
    {
        Resize();
        return 0;
    }
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);

}


MainWindow::MainWindow()
    :m_Factory(NULL), m_RenderTarget(NULL), m_Brush(NULL), m_Ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0)), m_ptMouse(D2D1::Point2F()) 
{
    //Detects WM_MOUSEHOVER, WM_MOUSELEAVE events :::::: WARNING: CREATES OVERHEAD
    //TRACKMOUSEEVENT tme;
    //tme.cbSize = sizeof(tme);
    //tme.hwndTrack = m_hwnd;
    //tme.dwFlags = TME_HOVER | TME_LEAVE;
    //tme.dwHoverTime = HOVER_DEFAULT;
    //TrackMouseEvent(&tme);
}


void MainWindow::CalculateLayout() {

}

BOOL MainWindow::IsMouseWheelPresent()
{
    return (GetSystemMetrics(SM_MOUSEWHEELPRESENT) != 0);
}

HRESULT MainWindow::CreateGraphicsResources() {

    HRESULT hr = S_OK;
    if (m_RenderTarget == NULL) {

        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);
    
        hr = m_Factory->CreateHwndRenderTarget(
            D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(m_hwnd, size), 
            &m_RenderTarget
        );

        if (SUCCEEDED(hr)) {
            const D2D1_COLOR_F color = D2D1::ColorF(1.0f, 1.0f, 0);
            hr = m_RenderTarget->CreateSolidColorBrush(color, &m_Brush);
            //Adds rotation to the circle
            //m_RenderTarget->SetTransform(D2D1::Matrix3x2F::Rotation(20, D2D1::Point2F(100, 100)));
            
            //Removes rotation
            //m_RenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());

            if (SUCCEEDED(hr)) {
                CalculateLayout();
            }
        }

    }
    return hr;
}


void MainWindow::DiscardGraphicsResources() {

    SafeRelease(&m_RenderTarget);
    SafeRelease(&m_Brush);

}


void MainWindow::OnPaint() {

    HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr)) {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);

        m_RenderTarget->BeginDraw();

        m_RenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));
        m_RenderTarget->FillEllipse(m_Ellipse, m_Brush);

        hr = m_RenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }
}


void MainWindow::Resize() {

    if (m_RenderTarget != NULL) {

        RECT rc;
        GetClientRect(m_hwnd, &rc);
        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        m_RenderTarget->Resize(size);
        CalculateLayout();
        InvalidateRect(m_hwnd, NULL, FALSE);

    }

}

void MainWindow::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
    //Captures event outside of window
    SetCapture(m_hwnd);
    m_Ellipse.point = m_ptMouse = DPIScale::PixelsToDips(pixelX, pixelY);
    m_Ellipse.radiusX = m_Ellipse.radiusY = 1.0f;
    //Redraw
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void MainWindow::OnLButtonUp()
{
    //Stops capturing events outside of window
    ReleaseCapture();
}

void MainWindow::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
    if (flags & MK_LBUTTON)  //Check if LBUTTON is pressed/down
    {
        const D2D1_POINT_2F dips = DPIScale::PixelsToDips(pixelX, pixelY);

        const float width = (dips.x - m_ptMouse.x) / 2;
        const float height = (dips.y - m_ptMouse.y) / 2;
        const float x1 = m_ptMouse.x + width;
        const float y1 = m_ptMouse.y + height;

        m_Ellipse = D2D1::Ellipse(D2D1::Point2F(x1, y1), width, height);

        //Redraw
        InvalidateRect(m_hwnd, NULL, FALSE);

    }
}


void MainWindow::SetIcon(HICON hIcon) {
    wc.hIcon = hIcon;
}