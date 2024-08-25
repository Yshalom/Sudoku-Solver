#include "Solver.h"

byte** Solver::solve(byte Board[9][9])
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
	byte** NewBoard = new byte * [9];
	for (int i = 0; i < 9; i++)
	{
		NewBoard[i] = new byte[9];
		for (int j = 0; j < 9; j++)
			NewBoard[i][j] = Board[i][j];
	}

	void* parameter = new void* [2];
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

byte** Solver::RecSolve(byte** Board)
{
	//Copy the board
	byte** NewBoard = new byte * [9];
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

DWORD WINAPI Solver::SolveThreadProc(_In_ LPVOID lpParameter)
{
	void* parameter = lpParameter;

	byte** NewBoard = *((byte***)parameter);

	*((byte***)parameter + 1) = RecSolve(NewBoard);

	return 0;
}

