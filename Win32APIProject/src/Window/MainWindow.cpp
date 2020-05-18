#include "pch.h"
#include "MainWindow.h"
#include <shobjidl.h>   //FileOpenDialog...
#include <atlbase.h>    //CComPtr...
#include <winerror.h>   //Error codes of hr(HRESULT)

#define CHECKCOM(x) if(FAILED(x)) return 0


template<typename T>
void SafeRelease(T * *ppT) {

    if (*ppT) {
        (*ppT)->Release();
        *ppT = NULL;
    }

}



LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg)
    {
    case WM_CREATE:
    {
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_Factory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        return 0;
    }
    case WM_DESTROY:
        DiscardGraphicsResources();
        SafeRelease(&m_Factory);
        PostQuitMessage(0);
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
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;

}


MainWindow::MainWindow()
    :m_Factory(NULL), m_RenderTarget(NULL), m_Brush(NULL) {}


void MainWindow::CalculateLayout() {

    if (m_RenderTarget != NULL) {
        D2D1_SIZE_F size = m_RenderTarget->GetSize();
        const float x = size.width / 2;
        const float y = size.height / 2;
        const float radius = min(x, y);
        m_Ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radius, radius);

    }

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


void MainWindow::SetIcon(HICON hIcon) {
    wc.hIcon = hIcon;
}