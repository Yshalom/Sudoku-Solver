#pragma once
#include<Windows.h>
#include<list>

class View
{
private:
	static std::list<View*> allViews;

	HWND Board_input[9][9];
	byte Board_input_data[9][9];
	
	int Index = 0;
	HWND hWnd;

	friend LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	friend void KeyInput(View* thisView, WPARAM wParam);


public:
	View(HINSTANCE hInstance);
	~View();

	static View* GetByHWND(HWND hWnd);

	void Run();
};