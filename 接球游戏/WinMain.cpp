#include<Windows.h>
#include<tchar.h>
#include<time.h>
#include<stdlib.h>

//全局变量
TCHAR szChildClass[]=TEXT("CatchBall_Child");
int idFocus;

LRESULT CALLBACK WndProcChild(HWND hwnd,UINT uimsg,WPARAM wparam,LPARAM lparam)
{
	static RECT Rect[5];
	static RECT rpRect;
	HDC hdc;
	PAINTSTRUCT ps;
	if(uimsg==WM_CREATE)
	{
		SetWindowLong(hwnd,0,0);
		for(int i=0;i<5;i++)
			SetRect(&Rect[i],0,0,100,100);
		SetRect(&rpRect,0,400,100,500);
		return 0;
	}
	else if(uimsg==WM_TIMER)
	{
		HBRUSH hbrush;
		hbrush=CreateSolidBrush(RGB(255,0,0));
		hdc=GetDC(hwnd);
		InvalidateRect(hwnd,NULL,true);
		UpdateWindow(hwnd);
		SelectObject(hdc,GetStockObject(NULL_PEN));
		SelectObject(hdc,hbrush);
		Ellipse(hdc,Rect[wparam-1].left,Rect[wparam-1].top,Rect[wparam-1].right,Rect[wparam-1].bottom);
		if(Rect[wparam-1].bottom>450 && hwnd==GetFocus())
		{
			InvalidateRect(hwnd,NULL,true);
			UpdateWindow(hwnd);
			SetRect(&Rect[wparam-1],0,0,100,100);
			KillTimer(hwnd,wparam);
			SetWindowLong(hwnd,0,0);
			ReleaseDC(hwnd,hdc);
			DeleteObject(hbrush);
			return 0;
		}
		if(Rect[wparam-1].bottom>550)
		{
			KillTimer(hwnd,wparam);
			ReleaseDC(hwnd,hdc);
			DeleteObject(hbrush);
			SendMessage((HWND)GetWindowLong(hwnd,GWL_HWNDPARENT),WM_CLOSE,0,0);
			return 0;
		}
		OffsetRect(&Rect[wparam-1],0,100);
		ReleaseDC(hwnd,hdc);
		DeleteObject(hbrush);
		return 0;
	}
	else if(uimsg==WM_SETFOCUS)
	{
		idFocus=GetWindowLong(hwnd,GWL_ID);
		InvalidateRect(hwnd,&rpRect,false);
		return 0;
	}
	else if(uimsg==WM_KILLFOCUS)
	{
		InvalidateRect(hwnd,&rpRect,true);
		return 0;
	}
	else if(uimsg==WM_KEYDOWN)
	{
		SendMessage((HWND)GetWindowLong(hwnd,GWL_HWNDPARENT),uimsg,wparam,lparam);
		return 0;
	}
	else if(uimsg==WM_PAINT)
	{
		hdc=BeginPaint(hwnd,&ps);
		if(hwnd==GetFocus())
		{
			SelectObject(hdc,GetStockObject(NULL_BRUSH));
			Rectangle(hdc,0,400,100,500);
		}
		EndPaint(hwnd,&ps);
		return 0;
	}
	return DefWindowProc(hwnd,uimsg,wparam,lparam);
}

LRESULT CALLBACK WndProc(HWND hwnd,UINT uimsg,WPARAM wparam,LPARAM lparam)
{
	static HWND ChildHwnd[5];
	if(uimsg==WM_CREATE)
	{
		for(int i=0;i<5;i++)
			ChildHwnd[i]=CreateWindow(szChildClass,NULL,WS_CHILD | WS_VISIBLE,i*100,0,100,500,hwnd,(HMENU)i,
			(HINSTANCE)GetWindowLong(hwnd,GWL_HINSTANCE),NULL);
		SetTimer(hwnd,10,1000,NULL);
		return 0;
	}
	else if(uimsg==WM_TIMER)
	{
		int i;
		for(i=0;i<5;i++)
			if(!GetWindowLong(ChildHwnd[i],0))
				break;
		if(i==5)
			return 0;
		i=rand()%5;
		while(GetWindowLong(ChildHwnd[i],0))
		{
			 i=rand()%5;
		}
		SetTimer(ChildHwnd[i],i+1,300,NULL);
		SetWindowLong(ChildHwnd[i],0,1);
		return 0;
	}
	else if(uimsg==WM_SETFOCUS)
	{
		SetFocus(ChildHwnd[idFocus]);
		return 0;
	}
	else if(uimsg==WM_KEYDOWN)
	{
		if(wparam==VK_LEFT)
		{
			if(idFocus>=1)
				idFocus--;
		}
		else if(wparam==VK_RIGHT)
		{
			if(idFocus<=3)
				idFocus++;
		}
		SetFocus(ChildHwnd[idFocus]);
		return 0;
	}
	else if(uimsg==WM_DESTROY)
	{
		KillTimer(hwnd,10);
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd,uimsg,wparam,lparam);
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPreInstance,LPSTR szCmdLine,int iCmdShow)
{
	TCHAR szAPPName[]=TEXT("CatchBall");
	HWND hwnd;
	MSG msg;
	WNDCLASS wndcls;
	wndcls.cbClsExtra=0;
	wndcls.cbWndExtra=0;
	wndcls.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wndcls.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndcls.hIcon=LoadIcon(NULL,IDI_APPLICATION);
	wndcls.hInstance=hInstance;
	wndcls.lpfnWndProc=WndProc;
	wndcls.lpszClassName=szAPPName;
	wndcls.lpszMenuName=NULL;
	wndcls.style=CS_VREDRAW | CS_HREDRAW;
	RegisterClass(&wndcls);

	wndcls.hIcon=NULL;
	wndcls.lpfnWndProc=WndProcChild;
	wndcls.lpszClassName=szChildClass;
	wndcls.cbWndExtra=sizeof(long);
	RegisterClass(&wndcls);

	srand(time(0));

	hwnd=CreateWindow(szAPPName,TEXT("Catch Ball"),WS_OVERLAPPED | WS_SYSMENU,CW_USEDEFAULT,CW_USEDEFAULT,
		500+2*GetSystemMetrics(SM_CXBORDER),500+2*GetSystemMetrics(SM_CYBORDER)+GetSystemMetrics(SM_CYCAPTION),
		NULL,NULL,hInstance,NULL);
	ShowWindow(hwnd,iCmdShow);
	UpdateWindow(hwnd);

	while(1)
	{
		bool bGetMsg=GetMessage(&msg,NULL,0,0);
		if(bGetMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			break;
	}
	MessageBox(NULL,TEXT("没能接住球"),TEXT("Title"),MB_OK);
}