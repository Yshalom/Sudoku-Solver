#include "View.h"
#include "Solver.h"

std::list<View*> View::allViews = std::list<View*>();

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam);
void PaintWindow(HWND hWnd);
void KeyInput(View* thisView, WPARAM wParam);

View::View(HINSTANCE hInstance)
{
	// Add this to the list of all views
	allViews.push_back(this);

	//Create Window
	hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"Static", L"Sudoku Solver", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 500, 150, 665, 730, NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		char s1[10] = { 0 };
		_itoa_s(GetLastError(), s1, 10);
		char s2[16];
		strcpy_s(s2, "Error: ");
		strcat_s(s2, s1);
		MessageBoxA(NULL, s2, "CreateWindowEx", NULL);
		throw 1;
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
				throw 1;
			}
		}
}

void View::Run()
{
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT CALLBACK WndProc(_In_ HWND hWnd, _In_ UINT message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	// Get the view object associated with this window
	View* thisView = View::GetByHWND(hWnd);
	if (!thisView)
		throw 2;

	switch (message)
	{
	//Block resize - The window stays in a fixed size
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

	// Pain the window
	case WM_PAINT:
		PaintWindow(hWnd);
		break;

	// The Window is closing
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	// Set background colors
	case WM_CTLCOLORSTATIC:
	{
		bool IsBoard = false;
		for (int i = 0; i < 9; i++)
			for (int j = 0; j < 9; j++)
				if ((HWND)lParam == thisView->Board_input[i][j])
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

	//Manage keyboard input
	case WM_KEYDOWN:
		KeyInput(thisView, wParam);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

View::~View()
{
	// Remove this from the list of all the views
	allViews.remove(this);

	for (int i = 0; i < 9; i++)
		for (int j = 0; j < 9; j++)
			DeleteObject(Board_input[i][j]);
	DeleteObject(hWnd);
}

View* View::GetByHWND(HWND hWnd)
{
	for (auto i = allViews.begin(); i != allViews.end(); i++)
		if ((*i)->hWnd == hWnd)
			return *i;

	return NULL;
}

void PaintWindow(HWND hWnd)
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

void KeyInput(View* thisView, WPARAM wParam)
{
	//For each key 0 to 9
	if (0x31 <= wParam && wParam <= 0x39 && thisView->Index < 81)
	{
		char str[3] = "  ";
		str[1] = (char)(wParam - 0x30 + '0'); //get the key number (0-9)
		SetWindowTextA(thisView->Board_input[thisView->Index / 9][thisView->Index % 9], str);
		thisView->Board_input_data[thisView->Index / 9][thisView->Index % 9] = wParam - 0x30;
		thisView->Index++;
	}
	//For a BackSpace key
	else if (wParam == VK_BACK && thisView->Index >= 1)
	{
		thisView->Index--;
		SetWindowTextA(thisView->Board_input[thisView->Index / 9][thisView->Index % 9], "");
		thisView->Board_input_data[thisView->Index / 9][thisView->Index % 9] = -1;
	}
	// For an Enter key
	else if (wParam == VK_RETURN)
	{
		//if the bord not full
		if (thisView->Index != 81)
		{
			MessageBoxA(NULL, "The board Is Not Full", "Notice", 0);
			return;
		}

		byte** NewBoard = Solver::solve(thisView->Board_input_data);

		if (NewBoard == NULL)
		{
			MessageBoxA(NULL, "Invalid input", "Notice", 0);
			return;
		}

		//Updata the board
		for (int i = 0; i < 9; i++)
		{
			for (int j = 0; j < 9; j++)
			{
				thisView->Board_input_data[i][j] = NewBoard[i][j];

				char str[3] = "  ";
				if (NewBoard[i][j])
					str[1] = (char)(NewBoard[i][j] + '0');
				else
					str[1] = '\xB7';
				SetWindowTextA(thisView->Board_input[i][j], str);
			}
			delete[] NewBoard[i];
		}
		delete[] NewBoard;
		NewBoard = NULL;
	}
	//For any other key
	else if (thisView->Index < 81 && wParam != VK_BACK && wParam != VK_RETURN)
	{
		SetWindowTextA(thisView->Board_input[thisView->Index / 9][thisView->Index % 9], " \xB7");
		thisView->Board_input_data[thisView->Index / 9][thisView->Index % 9] = 0;
		thisView->Index++;
	}
}
