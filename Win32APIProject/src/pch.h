#pragma once

#include <iostream>
#include <windows.h>
#include <vector>
#include <string>

#include <shobjidl.h>   //FileOpenDialog...
#include <atlbase.h>    //CComPtr...
#include <winerror.h>   //Error codes of hr(HRESULT)
#include <WinUser.h>	//KeyCodes
#include <tchar.h>		//KeyCode to User-Readable format

#include <d2d1.h>
#pragma comment(lib, "d2d1")