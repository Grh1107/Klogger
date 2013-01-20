// AppInit_DLLs future use inject into all processes

#include <windows.h>
#include <string.h>

HWND h_Wnd;
MSG msg;
WNDCLASS wc;
bool done;
HWND hwdesk;
HWND arrcount[100] ;
int count1;
HWND h_wnd3;
HWND h_wnd6;
HWND h_wnd4;
HWND h_wnd5;
HWND hHookedWindow;
typedef int (CALLBACK* sthndl)(HWND,HWND);
sthndl SetHandle;
typedef int (CALLBACK* unsub)();
unsub UnSubClass;
typedef int (CALLBACK *filhndl)(HWND,int);
filhndl FillHandleArray;
static HHOOK hWinHook;
static HINSTANCE hLib;

LRESULT WINAPI WndProc(HWND,UINT,WPARAM,LPARAM);

bool SendKeys(char * sKeys)
{
	long length=strlen(sKeys);
	int p;
	for(p=0;p<length;p++)
	{
		if(sKeys[p]=='<')
		{
			keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY |0, 0);
			keybd_event(188, 0x45, KEYEVENTF_EXTENDEDKEY |0, 0);
			keybd_event(188, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

		}
		else if(sKeys[p]=='>')
		{
			keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY |0, 0);
			keybd_event(190, 0x45, KEYEVENTF_EXTENDEDKEY |0, 0);
			keybd_event(190, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
			keybd_event(VK_SHIFT, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

		}
		else if(sKeys[p]=='/')
		{
			keybd_event(VK_DIVIDE, 0x45, KEYEVENTF_EXTENDEDKEY |0, 0);
			keybd_event(VK_DIVIDE, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

		}
		else
		{
			keybd_event(int(sKeys[p]), 0x45, KEYEVENTF_EXTENDEDKEY |0, 0);
			keybd_event(int(sKeys[p]), 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
		}
			//	SendMessage(hHookedWindow,WM_CHAR,(WPARAM)int(sKeys[p]),0);



	}


return TRUE;
}

void SendReturnKey(long numoftimes)
{
	for(int tmp=0;tmp<numoftimes;tmp++)
	{
		SendKeys("\r\n");
	}
}
int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrev,LPSTR line,int cmd)

{
	count1=1;
	wc.lpfnWndProc=WndProc;
	wc.hInstance=hInst;
	wc.hbrBackground =(HBRUSH) GetStockObject(GRAY_BRUSH);
	wc.lpszClassName ="HOOKSUB";
	wc.hCursor = LoadCursor(NULL,IDC_ARROW);
	wc.hIcon=LoadIcon(NULL, IDI_APPLICATION);
	wc.style=CS_VREDRAW | CS_HREDRAW;
	if (!RegisterClass(&wc))
	{
		MessageBox(0,"error","error",0);
	}


	// Creates Buttons on app
	h_Wnd=CreateWindowEx(0,"HOOKSUB","HOOKSUB",WS_CAPTION | WS_BORDER | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT,200,150,0,0,hInst,0);
    h_wnd5=CreateWindowEx(0,"Button","Hook", WS_BORDER | WS_CHILD | WS_VISIBLE | SS_CENTER,60,10,60,20,h_Wnd,0,hInst,0);
    h_wnd6= CreateWindowEx(0,"Button","UnHook", WS_BORDER | WS_CHILD | WS_VISIBLE ,60,50,60,20,h_Wnd,0,hInst,0); 
	
  	ShowWindow(h_Wnd,SW_SHOWNORMAL);
	
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return(msg.wParam);

}

bool LoadL = false;
LRESULT WINAPI WndProc(HWND hwnd,UINT msg2 ,WPARAM w_param,LPARAM l_param)
{
	if (LoadL)
	{
		//hLib = LoadLibrary("hooks.dll");
		LoadL = false;
	}
	switch(msg2)
	{
		case WM_DESTROY:
			{
				PostQuitMessage(0);
				break;
			}
		
		case WM_APP +1024:
			{
				
				if(w_param==125)
				{

					SendKeys("<HTML>");
					SendReturnKey(2);
					SendKeys("<BODY>");
					SendReturnKey(3);
					SendKeys("<HEAD>");
					SendReturnKey(2);
					SendKeys("</HEAD>");
					SendReturnKey(10);
					SendKeys("</BODY>");
					SendReturnKey(2);
					SendKeys("</HTML>");

				}
				if(w_param==126)
				{
					SendKeys("<BR>");

				}
				break;

			}
			
		case WM_COMMAND:
			{
				if (w_param==BN_CLICKED)
				{
					if(l_param==(long)h_wnd5)
					{
						hHookedWindow=FindWindow(NULL,"Untitled - Notepad");						
						if(hHookedWindow==NULL)
						{
							MessageBox(0,"Could Not find a running instance of Notepad.\r\nPlease Start Notepad and try again","Error",0);
							break;
						}
						HMENU hAppMenu;
						hAppMenu=GetMenu(hHookedWindow);
						HMENU hAppendMenu;
						hAppendMenu=CreateMenu();
						AppendMenu(hAppMenu,MF_STRING + MF_POPUP,(unsigned int)hAppendMenu,"FUCK YOU");
						AppendMenu(hAppendMenu,MF_STRING,125,"Make HTML");
						AppendMenu(hAppendMenu,MF_STRING,126,"Add Line Break");
						HWND hMenuWnd;
						hLib = LoadLibrary("hooks.dll");
				        
						LoadL = true;
						hMenuWnd = GetWindow(hHookedWindow, 5);
						DWORD hThread;
						hThread=GetWindowThreadProcessId(hHookedWindow,NULL);
						SetHandle = (sthndl)GetProcAddress(hLib, "SetHandle");
						UnSubClass = (unsub)GetProcAddress(hLib, "UnSubclass");
						SetHandle(hHookedWindow,hwnd);
						FillHandleArray = (filhndl)GetProcAddress(hLib, "FillHandleArray");
						FillHandleArray(hHookedWindow,1);
						FillHandleArray(hMenuWnd,1);
						//HOOKPROC fAdd;
						//fAdd = (HOOKPROC)GetProcAddress(hLib, "CBTProc");
						//hWinHook = SetWindowsHookEx(WH_CBT, fAdd, hLib,hThread);
						ShowWindow(hHookedWindow, SW_MINIMIZE);





					}
					if(l_param==(long)h_wnd6)
					{
						//Sleep(2000);
						//UnSubClass();
						Sleep(2000);
						//UnhookWindowsHookEx(hWinHook);
						//Sleep(2000);
						FreeLibrary(hLib);

					}
					

				}
				break;


			}


	}

	return DefWindowProc(hwnd,msg2,w_param,l_param);
}
