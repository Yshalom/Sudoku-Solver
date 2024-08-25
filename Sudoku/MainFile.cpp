#include<Windows.h>
#include "View.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	View view1(hInstance);

	MessageBoxA(NULL, "Press the 0-9 key to fill the board.\nPress any other key to fill with space.\nPress enter to get the solved board.", "Notice", NULL);

	view1.Run();
	
	return 0;
}