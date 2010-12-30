#include <windows.h>
#include "core.h"
using namespace pichi;
//#include <commctrl.h>
//#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

LONG WINAPI WndProc(HWND, UINT, WPARAM,LPARAM);
int  WINAPI  WinMain (HINSTANCE  hInstance,
                     HINSTANCE  hPrevInstance,
                     LPSTR    lpCmdLine,
                     int    nCmdShow)
{
  HWND hwnd;
  //TCHAR szClassName[] = L"PichiClass";
  char szClassName[] = "PichiClass";
  MSG msg;
  WNDCLASS w;
  memset(&w,0,sizeof(WNDCLASS));
  w.style = CS_HREDRAW | CS_VREDRAW;
  w.lpfnWndProc = WndProc;
  w.hInstance = hInstance;
  w.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
  HWND hMainWindow = NULL;
  w.lpszClassName = szClassName;
  RegisterClass(&w);
  
  //InitCommonControls();

  hwnd = CreateWindow(szClassName, "Pichi", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
  
  ShowWindow(hwnd,nCmdShow);
  UpdateWindow(hwnd);
  
  while(GetMessage(&msg,NULL,0,0))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return msg.wParam;
}

LONG WINAPI WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	static HMENU hMainMenu, hStartMenu;
	static HWND pichiListBox, editWindow, sendEdit;
	static HINSTANCE hIns = (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE);
	static RECT rect;

	switch (message)
	{
	case WM_CREATE:
		GetClientRect(hWnd, &rect);
		hMainMenu = CreateMenu();
		hStartMenu = CreatePopupMenu();
		AppendMenu(hStartMenu, MF_STRING, 1001, "About Pichi");
		AppendMenu(hMainMenu,MF_ENABLED|MF_POPUP,(UINT)hStartMenu, "pichi");
		SetMenu(hWnd,hMainMenu);

		pichiListBox = CreateWindowEx(NULL, "LISTBOX", "pichiList", WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_EXTENDEDSEL,
			rect.right - (rect.right/5), rect.top + 20, (rect.right/5) - 20, rect.bottom - 50,hWnd, (HMENU)1101, hIns, NULL);

		editWindow = CreateWindowEx(NULL, "EDIT", "Тест", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER | WS_VSCROLL,
			10, rect.top + 20, rect.right - (rect.right/5) - 20, rect.bottom - 90,hWnd, (HMENU)1102, hIns, NULL);

		sendEdit = CreateWindowEx(NULL, "EDIT", "сообщние", WS_VISIBLE | WS_CHILD | ES_MULTILINE | WS_BORDER | WS_VSCROLL,
			10, rect.bottom - 60, rect.right - (rect.right/5) - 100, 30, hWnd, (HMENU)1102, hIns, NULL);

		for(int i = 0; i < 10; i++)
			SendDlgItemMessage(hWnd, 1101, LB_ADDSTRING,0, (LPARAM)"Троль");

		static core bot;
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Разобрать выбор в меню:
		switch (wmId)
		{
		case 1001:
			MessageBox(hWnd, "Pichi v.", "About Pichi", MB_OK);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
