#pragma once

#include <d2d1.h>
#include "BaseWindow.hpp"


class MainWindow : public BaseWindow<MainWindow>
{
private:
	ID2D1Factory*			m_Factory;
	ID2D1HwndRenderTarget*	m_RenderTarget;
	ID2D1SolidColorBrush*	m_Brush;
	D2D1_ELLIPSE			m_Ellipse;


	void CalculateLayout();
	HRESULT CreateGraphicsResources();	
	void DiscardGraphicsResources();
	void OnPaint();
	void Resize();


public:

	MainWindow();


	PCWSTR ClassName() const override { return L"Sample Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void SetIcon(HICON hIcon);
};

