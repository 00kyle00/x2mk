#include "window.h"
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>

namespace {

void TrayIcon(HWND hWnd, bool add) {
  NOTIFYICONDATA tray;
  tray.cbSize = sizeof(tray);
  tray.hIcon = LoadIcon(NULL, IDI_ASTERISK);
  tray.hWnd = hWnd;
  tray.uCallbackMessage = WM_USER;
  tray.uFlags = NIF_ICON | NIF_MESSAGE;
  tray.uID = 1;
  tray.uVersion = NOTIFYICON_VERSION_4;
  Shell_NotifyIcon(add ? NIM_ADD : NIM_DELETE, &tray);
}

HWND hWnd = 0;
void RemoveTrayIcon() {
  TrayIcon(hWnd, false);
}

LRESULT CALLBACK WndProcedure(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
  switch(Msg)
  {
  case WM_USER:
    if(LOWORD(lParam) == WM_LBUTTONDOWN || LOWORD(lParam) == WM_RBUTTONDOWN)
    {
      POINT mouse;
      GetCursorPos(&mouse);

      HMENU hPopupMenu = CreatePopupMenu();
      InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, 1, L"Exit");
      InsertMenu(hPopupMenu, 0, MF_BYPOSITION | MF_STRING, 2, L"Show/Hide console");

      int choice = TrackPopupMenu(hPopupMenu, TPM_BOTTOMALIGN | TPM_LEFTALIGN | TPM_NONOTIFY | TPM_RETURNCMD, 
        mouse.x, mouse.y, 0, hWnd, NULL);
      if(choice == 1) {
        exit(0);
      }
      if(choice == 2) {
        static bool show = true;
        ShowWindow(GetConsoleWindow(), show);
        show = !show;
      }

      DestroyMenu(hPopupMenu);
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(WM_QUIT);
    break;
  default:
    return DefWindowProc(hWnd, Msg, wParam, lParam);
  }
  return 0;
}

DWORD WINAPI ThreadFunction(void* data) {
  LPCTSTR className = L"dummy_class_name";
	WNDCLASSEX wndclass;
	wndclass.cbSize        = sizeof(WNDCLASSEX);
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProcedure;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = className;
	wndclass.hInstance     = GetModuleHandle(0);
	wndclass.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wndclass);

	hWnd = CreateWindow(className, L"none", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
	  CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle(0), NULL);
	
  TrayIcon(hWnd, true);

	MSG msg;
	while(GetMessage(&msg,NULL,0,0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
	}

	exit(0);
}

}

void SetupTray() {
  ShowWindow(GetConsoleWindow(), 0);
  CreateThread(0, 0, &ThreadFunction, 0, 0, 0);
  atexit(RemoveTrayIcon);
}

void ShowWindow() {
  ShowWindow(GetConsoleWindow(), 1);
}