#pragma once

#include "pch.h"


template<class DERIVED_TYPE>
class BaseWindow
{
private:
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

		DERIVED_TYPE* pThis = NULL;

		if (uMsg == WM_NCCREATE) {
			CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
			pThis = (DERIVED_TYPE*)pCreate->lpCreateParams;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG)pThis);

			pThis->m_hwnd = hwnd;
		}
		else {
			pThis = (DERIVED_TYPE*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if (pThis) {
			return pThis->HandleMessage(uMsg, wParam, lParam);
		}
		else {
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

	}

public:

	BOOL CreateBaseWindow(PCWSTR lpWindowName,
		DWORD dwStyle,
		DWORD dwExtStyle = 0,
		int x = CW_USEDEFAULT,
		int y = CW_USEDEFAULT,
		int nWidth = CW_USEDEFAULT,
		int nHeight = CW_USEDEFAULT,
		HWND hwndParent = 0,
		HMENU hMenu = 0
	) {

		WNDCLASS wc = {};
		wc.lpfnWndProc = DERIVED_TYPE::WindowProc;
		wc.hInstance = GetModuleHandle(NULL);
		wc.lpszClassName = ClassName();

		RegisterClass(&wc);

		m_hwnd = CreateWindowExW(dwExtStyle, ClassName(), lpWindowName, dwStyle, x, y, nWidth, nHeight, hwndParent, hMenu, GetModuleHandle(NULL), this);

		return (m_hwnd ? TRUE : FALSE);
	
	}

	HWND GetWindow() const { return m_hwnd; }

	BaseWindow() : m_hwnd(NULL) {}

protected:
	virtual PCWSTR ClassName() const = 0;
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

	HWND m_hwnd;
};

