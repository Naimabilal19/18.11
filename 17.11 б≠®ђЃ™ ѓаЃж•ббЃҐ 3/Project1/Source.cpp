#include <windows.h>
#include <tchar.h>
#include <string>
#include <commctrl.h>
#include <tlhelp32.h>
#include <time.h>
#include "resource.h"

#pragma comment(lib,"comctl32")

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);

HWND hReload;
HWND hTerminate;
HWND hNew;
HWND hProcName;
HWND hProcList;
HWND hStatID, time1;
HANDLE Ti, Timer;

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
	INITCOMMONCONTROLSEX icc = { sizeof(INITCOMMONCONTROLSEX) };
	icc.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&icc);

	return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
}

void ShowProcList(HWND hList)
{
	EnableWindow(hTerminate, FALSE);
	EnableWindow(hNew, TRUE);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 processInfo = { sizeof(PROCESSENTRY32) };

	BOOL res = Process32First(hSnapshot, &processInfo);

	if (res == TRUE) {
		do {
			LRESULT index = SendMessage(hList, LB_ADDSTRING, 0, LPARAM(processInfo.szExeFile));
			SendMessage(hList, LB_SETITEMDATA, WPARAM(index), LPARAM(processInfo.th32ProcessID));
		} while (Process32Next(hSnapshot, &processInfo));
	}
	CloseHandle(hSnapshot);
}

DWORD WINAPI Tread(LPVOID lp)        
{
	while (true)
	{
		SendMessage(hProcList, LB_RESETCONTENT, 0, 0);
		ShowProcList(hProcList);
		Sleep(3000);
	}
}

DWORD WINAPI Tim(LPVOID lp)       
{
	static time_t t;
	static TCHAR str[50];
	while (true)
	{
		t = time(NULL);	
		lstrcpy(str, _tctime(&t));  
		str[lstrlen(str) - 1] = '\0';
		SetWindowText(time1, str);
	}
}    

BOOL CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp)
{
	switch (mes)
	{
	case WM_INITDIALOG:
	{	
		Ti = CreateThread(NULL, 0, Tread, hWnd, 0, NULL);
		Timer = CreateThread(NULL, 0, Tim, hWnd, 0, NULL);
		time1 = GetDlgItem(hWnd, IDC_EDIT3);
		hNew = GetDlgItem(hWnd, IDC_BUTTON3);
		hProcName = GetDlgItem(hWnd, IDC_EDIT1);
		hProcList = GetDlgItem(hWnd, IDC_LIST1);
		hStatID = GetDlgItem(hWnd, IDC_EDIT2);
	   	ShowProcList(hProcList);
	}
	return TRUE;

	case WM_COMMAND:
	{
		if (LOWORD(wp) == IDC_BUTTON3) 
		{
			wchar_t buf[260]{ 0 };
			GetWindowText(hProcName, buf, 260);
			STARTUPINFO info = { sizeof(info) };
			PROCESS_INFORMATION processInfo;
			if (CreateProcess(buf, buf, NULL, NULL, TRUE, 0, NULL, NULL, &info, &processInfo))
			{
			   ShowProcList(hProcList);
			}	
		}
		else if (HIWORD(wp) == LBN_SELCHANGE) 
		{
			EnableWindow(hTerminate, TRUE);
			LRESULT index = SendMessage(hProcList, LB_GETCURSEL, 0, 0);
			int procId = SendMessage(hProcList, LB_GETITEMDATA, WPARAM(index), 0);
			TCHAR buf[30];
			wsprintf(buf, TEXT("10 %d"), procId);
			SendMessage(hStatID, WM_SETTEXT, 0, LPARAM(buf));			
		}
	}
	break;
	
	case WM_CLOSE:
		DestroyWindow(hWnd);
		EndDialog(hWnd, 0);
		return TRUE;
	}
	return FALSE;
}