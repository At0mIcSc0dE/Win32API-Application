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

    switch (uMsg)
    {
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
    case WM_LBUTTONDOWN:
        OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

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
    default:
        return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
    return TRUE;

}


MainWindow::MainWindow()
    :m_Factory(NULL), m_RenderTarget(NULL), m_Brush(NULL), m_Ellipse(D2D1::Ellipse(D2D1::Point2F(), 0, 0)), m_ptMouse(D2D1::Point2F()) {}


void MainWindow::CalculateLayout() {

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