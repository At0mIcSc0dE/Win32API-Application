#pragma once

#include "BaseWindow.h"


class MainWindow : public BaseWindow<MainWindow>
{
public:
	PCWSTR ClassName() const override { return L"Sample Window Class"; }
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

	void SetIcon(HICON hIcon);
};

