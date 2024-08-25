#pragma once
#include<Windows.h>


static class Solver
{
private:
	static byte** RecSolve(byte**);
	static DWORD WINAPI SolveThreadProc(_In_ LPVOID lpParameter);


public:
	static byte** solve(byte[9][9]);

};