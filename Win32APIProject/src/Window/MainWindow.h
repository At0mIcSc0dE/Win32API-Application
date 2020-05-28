#pragma once

#define EventHandlers


#include "pch.h"
#include "BaseWindow.hpp"


class MainWindow : public BaseWindow<MainWindow>
{
private:
	ID2D1Factory*			m_Factory;
	ID2D1HwndRenderTarget*	m_RenderTarget;
	ID2D1SolidColorBrush*	m_Brush;
	D2D1_ELLIPSE			m_Ellipse;
	D2D1_POINT_2F			m_ptMouse;


	void CalculateLayout();
	HRESULT CreateGraphicsResources();	
	void DiscardGraphicsResources();
	void OnPaint();
	void Resize();

	BOOL IsMouseWheelPresent();

public:
	void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
	void OnLButtonUp();
	void OnMouseMove(int pixelX, int pixelY, DWORD flags);


public:

	MainWindow();


	PCWSTR ClassName() const override { return L"Sample Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void SetIcon(HICON hIcon);
};

