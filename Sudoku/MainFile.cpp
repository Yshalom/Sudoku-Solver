#include<Windows.h>

static int Index = 0;

HWND Board_input[9][9];
byte Board_input_data[9][9];

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);
byte** solve(byte[9][9]);
byte** RecSolve(byte**);
DWORD WINAPI SolveThreadProc(_In_ LPVOID lpParameter);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//Create Window
	HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"Static", L"Sudoku Solver", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 500, 150, 665, 730, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		char s1[10] = { 0 };
		_itoa_s(GetLastError(), s1, 10);
		char s2[16];
		strcpy_s(s2, "Error: ");
		strcat_s(s2, s1);
		MessageBoxA(NULL, s2, "CreateWindowEx", NULL);
		return 1;
	}

	SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG)WndProc);

	//Create the input panle
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
		{
			Board_input[i][j] = CreateWindowEx(0, L"Static", L"", WS_CHILD | WS_VISIBLE, 60 + j * 60, 110 + i * 60, 50, 50, hWnd, NULL, hInstance, NULL);
			if (!Board_input[i][j])
			{
				char s1[10] = { 0 };
				_itoa_s(GetLastError(), s1, 10);
				char s2[16];
				strcpy_s(s2, "Error: ");
				strcat_s(s2, s1);
				MessageBoxA(NULL, s2, "CreateWindowEx", NULL);
				return 1;
			}
		}

	MessageBoxA(NULL, "Press the 0-1 key to fill the board.\nPress any other key to fill with space.\nPress enter to get the solved board.", "Notice", NULL);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			DeleteObject(Board_input[i][j]);
	DeleteObject(hWnd);

	return 0;
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	switch (message)
	{
	//Don't let resize the window
	case WM_SIZING:
	{
		RECT Window_Pos;
		GetWindowRect(hWnd, &Window_Pos);
		*((PRECT)lParam) = Window_Pos;
	}
		break;
	 case WM_SIZE:
		if (wParam != SIZE_MINIMIZED)
		{
			ShowWindow(hWnd, SW_NORMAL);
			SetWindowPos(hWnd, 0, 500, 150, 665, 730, SWP_NOZORDER);
		}
		break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc;
		hdc = BeginPaint(hWnd, &ps);
		
		//Paint background color
		RECT rect;
		GetWindowRect(hWnd, &rect);
		rect.left = rect.right - rect.left;
		rect.bottom = rect.bottom - rect.top;
		rect.top = 0;
		rect.right = 0;
		HBRUSH brush = CreateSolidBrush(RGB(250, 245, 255));
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);

		//Paint the board's boarder
		rect.top = 100;
		rect.left = 50;
		rect.bottom = 650;
		rect.right = 600;
		brush = CreateSolidBrush(RGB(0, 0, 0));
		FillRect(hdc, &rect, brush);
		DeleteObject(brush);

		brush = CreateSolidBrush(RGB(150, 150, 150));
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				rect.top = 110 + i * 180;
				rect.left = 60 + j * 180;
				rect.bottom = 280 + i * 180;
				rect.right = 230 + j * 180;
				FillRect(hdc, &rect, brush);
			}
		DeleteObject(brush);

		SetBkColor(hdc, RGB(250, 245, 255));
		SetTextColor(hdc, RGB(0, 180, 230));
		HFONT font = CreateFontA(60, 0, 0, 0, 700, FALSE, TRUE, FALSE, NULL, NULL, NULL, NULL, NULL, NULL);
		SelectObject(hdc, font);
		DeleteObject(font);
		TextOutA(hdc, 150, 20, "Sudoku solver", strlen("Sudoku solver"));

		EndPaint(hWnd, &ps);
	}
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CTLCOLORSTATIC:
	{
		bool IsBoard = false;
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if ((HWND)lParam == Board_input[i][j])
				{
					IsBoard = true;
					break;
				}

		if (IsBoard)
		{
			SetBkColor((HDC)wParam, RGB(255, 255, 247));
			SetTextColor((HDC)wParam, RGB(255, 80, 0));
			HFONT font = CreateFontA(53, 0, 0, 0, 100, FALSE, FALSE, FALSE, NULL, NULL, NULL, NULL, NULL, NULL);
			SelectObject((HDC)wParam, font);
			DeleteObject(font);
			return (LRESULT)CreateSolidBrush(RGB(255, 255, 247));
		}
	}
	break;

	//Input the board
	case WM_KEYDOWN:
		if (0x31 <= wParam && wParam <= 0x39 && Index < 81) //For each key 0 to 9
		{
			char str[3] = "  ";
			str[1] = (char)(wParam - 0x30 + '0'); //get the key number (0-9)
			SetWindowTextA(Board_input[Index / 9][Index % 9], str);
			Board_input_data[Index / 9][Index % 9] = wParam - 0x30;
			Index++;
		}
		else if (wParam == VK_BACK && Index >= 1) //For BackSpace key
		{
			Index--;
			SetWindowTextA(Board_input[Index / 9][Index % 9], "");
			Board_input_data[Index / 9][Index % 9] = -1;
		}
		else if (Index < 81 && wParam != VK_BACK && wParam != VK_RETURN) //For any other key
		{
			SetWindowTextA(Board_input[Index / 9][Index % 9], " \xB7");
			Board_input_data[Index / 9][Index % 9] = 0;
			Index++;
		}
		else if (wParam == VK_RETURN)
		{
			//if the bord not full
			if (Index != 81)
			{
				MessageBoxA(NULL, "The board Is Not Full", "Notice", 0);
				break;
			}

			byte** NewBoard = solve(Board_input_data);

			if (NewBoard == NULL)
			{
				MessageBoxA(NULL, "Invalid input", "Notice", 0);
				break;
			}

			//Updata the board
			for (int i = 0; i < 9; i++)
			{
				for (int j = 0; j < 9; j++)
				{
					Board_input_data[i][j] = NewBoard[i][j];

					char str[3] = "  ";
					if (NewBoard[i][j])
						str[1] = (char)(NewBoard[i][j] + '0');
					else
						str[1] = '\xB7';
					SetWindowTextA(Board_input[i][j], str);
				}
				delete[] NewBoard[i];
			}
			delete[] NewBoard;
			NewBoard = NULL;
		}
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

byte** solve(byte Board[9][9])
{
	//Check the board out
	for (int i = 0; i < 9; i++)
	{
		int RowCount[10] = { 0 };
		int ColunmCount[10] = { 0 };
		int SquareCount[10] = { 0 };

		for (int j = 0; j < 9; j++)
		{
			RowCount[Board[i][j]]++;
			ColunmCount[Board[j][i]]++;
			SquareCount[Board[(i / 3) * 3 + (j / 3)][(i % 3) * 3 + (j % 3)]]++;
		}

		for (int i = 1; i < 10; i++)
			if (RowCount[i] > 1 || ColunmCount[i] > 1 || SquareCount[i] > 1)
				return NULL;
	}

	//Copy the board
	byte** NewBoard = new byte* [9];
	for (int i = 0; i < 9; i++)
	{
		NewBoard[i] = new byte[9];
		for (int j = 0; j < 9; j++)
			NewBoard[i][j] = Board[i][j];
	}

	void* parameter = new void*[2];
	*((byte***)parameter) = NewBoard;

	HANDLE thread = CreateThread(NULL, 0, SolveThreadProc, parameter, 0, NULL);

	if (thread == NULL)
	{
		char s1[10] = { 0 };
		_itoa_s(GetLastError(), s1, 10);
		char s2[16];
		strcpy_s(s2, "Error: ");
		strcat_s(s2, s1);
		MessageBoxA(NULL, s2, "CreateThread", NULL);
		return NULL;
	}

	WaitForSingleObject(thread, INFINITE);

	CloseHandle(thread);

	byte** res = *((byte***)parameter + 1);

	for (int i = 0; i < 9; i++)
		delete[] NewBoard[i];
	delete[] NewBoard;

	return res;
}

byte** RecSolve(byte** Board)
{
	//Copy the board
	byte** NewBoard = new byte*[9];
	for (int i = 0; i < 9; i++)
	{
		NewBoard[i] = new byte[9];
		for (int j = 0; j < 9; j++)
			NewBoard[i][j] = Board[i][j];
	}

	//Fill the board as much as possible
	bool change = true;
	while (change)
	{
		change = false;
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				if (NewBoard[i][j] == 0)
				{
					bool options[10]; //The place i is a flag of i
					for (int k = 0; k < 10; k++)
						options[k] = true;

					for (int k = 0; k < 9; k++)
					{
						options[NewBoard[i][k]] = false; //In the same row
						options[NewBoard[k][j]] = false; //In the same column
						options[NewBoard[(i / 3) * 3 + (k / 3)][(j / 3) * 3 + (k % 3)]] = false; //In the same block
					}

					int OptionCount = 0;
					for (int k = 0; k < 10; k++)
						if (options[k])
							OptionCount++;
					if (OptionCount == 1)
						for (int k = 0; k < 10; k++)
							if (options[k])
							{
								NewBoard[i][j] = k;
								change = true;
								break;
							}
					if (OptionCount == 0)
					{
						for (int k = 0; k < 9; k++)
							delete[] NewBoard[k];
						delete[] NewBoard;
						return NULL;
					}
				}
			}
		}
	}

	//If the board full return it
	bool full = true;
	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			if (NewBoard[i][j] == 0)
			{
				full = false;
				break;
			}
	if (full)
		return NewBoard;

	//Find the square that has the least options
	int MinOptions[3]; //The number of the options in place 0, and i,j in 1,2
	MinOptions[0] = -1;
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (NewBoard[i][j] == 0)
			{
				bool options[10]; //The place i is a flag of i
				for (int k = 0; k < 10; k++)
					options[k] = true;

				for (int k = 0; k < 9; k++)
				{
					options[NewBoard[i][k]] = false; //In the same row
					options[NewBoard[k][j]] = false; //In the same column
					options[NewBoard[(i / 3) * 3 + (k / 3)][(j / 3) * 3 + (k % 3)]] = false; //In the same block
				}

				int OptionCount = 0;
				for (int k = 1; k < 10; k++)
					if (options[k])
						OptionCount++;

				if (MinOptions[0] == -1 || MinOptions[0] > OptionCount)
				{
					MinOptions[0] = OptionCount;
					MinOptions[1] = i;
					MinOptions[2] = j;
				}
			}
		}
	}

	bool options[10]; //The place i is a flag of i
	for (int k = 0; k < 10; k++)
		options[k] = true;
	for (int k = 0; k < 9; k++)
	{
		options[NewBoard[MinOptions[1]][k]] = false; //In the same row
		options[NewBoard[k][MinOptions[2]]] = false; //In the same column
		options[NewBoard[(MinOptions[1] / 3) * 3 + (k / 3)][(MinOptions[2] / 3) * 3 + (k % 3)]] = false; //In the same block
	}

	for (int i = 1; i < 10; i++) 
	{
		if (options[i] == true)
		{
			NewBoard[MinOptions[1]][MinOptions[2]] = i;
			byte** res = RecSolve(NewBoard);
			if (res != NULL)
			{
				for (int j = 0; j < 9; j++)
					delete[] NewBoard[j];
				delete[] NewBoard;

				return res;
			}
		}
	}

	for (int i = 0; i < 9; i++)
		delete[] NewBoard[i];
	delete[] NewBoard;

	return NULL;
}

DWORD WINAPI SolveThreadProc(_In_ LPVOID lpParameter)
{
	void* parameter = lpParameter;
	
	byte** NewBoard = *((byte***)parameter);
	
	*((byte***)parameter + 1) = RecSolve(NewBoard);
	
	return 0;
}

