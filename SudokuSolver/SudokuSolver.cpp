#include <iostream>
#include <vector>
#include <tuple>
#include <windows.h>
#include <thread>
#include <string>

class SudokuGameAutoSolver
{
public:
	void ExecuteOrder66()
	{
		int tempTimeDelay = TimeDelay;
		TimeDelay = TimeDelay == 0 ? 1 : TimeDelay;
		std::thread keyPressThread([&]
		{
			checkForKeyPress();
		});
		GetGameCopyPos();
		Calibrate();
		std::cout << "Left click on 'Classic'" << std::endl;
		while (true)
		{
			if (GetKeyState(VK_LBUTTON) & 0x8000)
				break;
			Sleep(10);
		}
		Sleep(1000);
		std::cout << "Select difficulty" << std::endl;
		while (true)
		{
			if (GetKeyState(VK_LBUTTON) & 0x8000)
			{
				GetCursorPos(&difficulty);
				break;
			}
			Sleep(10);
		}
		Sleep(1000);
		std::cout << "starting" << std::endl;
		TimeDelay = tempTimeDelay;
		while (!stopFlag)
		{
			SetCursorPos(difficulty.x, difficulty.y);
			PressMouse(VK_LBUTTON);
			Sleep(1000);
			SolveSudoku();
			Sleep(2000);
		}
		keyPressThread.join();
	}
	int TimeDelay = 10;
private:

#pragma region Stop when the user want to
	bool stopFlag = false;
	void checkForKeyPress()
	{
		while (!stopFlag)
		{
			if (GetKeyState('S') & 0x8000)
			{
				stopFlag = true;
				std::cout << "Stopping the solver..." << std::endl;
			}
			Sleep(10);
		}
	}
#pragma endregion
#pragma region Calibrating
	std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> MatchCodes;
	void Calibrate()
	{
		//Fill MatchCodes with values
		for (byte i = 0; i < 9; i++)
		{
			std::vector<std::vector<std::tuple<std::string, int>>> innerList;
			for (int j = 0; j < 9; j++)
			{
				std::vector<std::tuple<std::string, int>> innerList2;
				innerList.push_back(innerList2);
			}
			MatchCodes.push_back(innerList);
		}
		//Making sure the position is in the top left corner
		for (byte y = 0; y < 9; y++)
		{
			PressKey(VK_UP);
			PressKey(VK_LEFT);
		}
		//Getting giving MatchCodes their real values
		for (byte i = 1; i <= 9; i++)
		{
			for (byte y = 0; y < 5; y++)
			{
				for (byte x = 0; x < 9; x++)
				{
					PressKey(0x30 + i);
					PressKey(VK_RIGHT);
				}
				if (y < 4)
				{
					PressKey(VK_DOWN);
					for (byte x = 0; x < 9; x++)
					{
						PressKey(0x30 + i);
						PressKey(VK_LEFT);
					}
					PressKey(VK_DOWN);
				}
			}
			for (byte y = 0; y < 9; y++)
			{
				PressKey(VK_LEFT);
				PressKey(VK_UP);
			}

			auto numberCode = GetGameCodes();
			for (int y = 0; y < numberCode.size(); y++)
				for (int x = 0; x < numberCode[y].size(); x++)
					MatchCodes[y][x].push_back(std::make_tuple(numberCode[y][x], i));
		}
	}
#pragma endregion
#pragma region Solving
	POINT difficulty{};
	int LevenshteinDistance(const char* s1, const char* s2)
	{
#define MIN(x,y) ((x) < (y) ? (x) : (y)) //calculate minimum between two values
		int i, j, l1, l2, t, track;
		int dist[50][50]{};
		//stores the lenght of strings s1 and s2
		l1 = strlen(s1);
		l2 = strlen(s2);
		for (i = 0; i <= l1; i++)
			dist[0][i] = i;
		for (j = 0; j <= l2; j++)
			dist[j][0] = j;
		for (j = 1; j <= l1; j++)
			for (i = 1; i <= l2; i++)
			{
				if (s1[i - 1] == s2[j - 1])
					track = 0;
				else
					track = 1;
				t = MIN((dist[i - 1][j] + 1), (dist[i][j - 1] + 1));
				dist[i][j] = MIN(t, (dist[i - 1][j - 1] + track));
			}
		return dist[l2][l1];
	}
	void SolveSudoku()
	{
		auto numberCode = GetGameCodes();
		std::vector<std::vector<int>> SudokuValues;
		for (int y = 0; y < MatchCodes.size(); y++)
		{
			std::vector<int> row;
			for (int x = 0; x < MatchCodes[y].size(); x++)
			{
				float oldNum = 100;
				int num = 0;
				for (std::tuple<std::string, int> item : MatchCodes[y][x])
				{
					if (numberCode[y][x] != "")
					{
						float testMatch = (double)LevenshteinDistance(std::get<0>(item).c_str(), numberCode[y][x].c_str()) / std::get<0>(item).size();

						std::cout << std::get<1>(item) << ": " << std::get<0>(item) << " and " << numberCode[y][x] << " = " << testMatch << std::endl;
						if (testMatch < oldNum)
						{
							num = std::get<1>(item);
							oldNum = testMatch;
						}
					}
				}

				row.push_back(num);
			}
			SudokuValues.push_back(row);
		}
		for (byte i_1 = 0; i_1 < SudokuValues.size(); i_1++)
		{
			if (i_1 % 3 == 0 && i_1 != 0)
				std::cout << "-----------------------" << std::endl;
			for (byte i_2 = 0; i_2 < SudokuValues[i_1].size(); i_2++)
				if (i_2 % 3 == 0 && i_2 != 0)
					std::cout << " | " << SudokuValues[i_1][i_2];
				else
					std::cout << " " << SudokuValues[i_1][i_2];
			std::cout << std::endl;
		}
		CalculateSudokuWithValues(SudokuValues);
	}
#pragma region Actual Sudoku Solver
	void CalculateSudokuWithValues(std::vector<std::vector<int>> Grid)
	{
		grid = Grid;
		if (grid.size() >= 9)
			Solve();
	}
	std::vector<std::vector<int>> grid;
	bool Possible(int x, int y, int n)
	{
		//This is the brain of the program!

		//This for loop iterates from 0 to 9, these values can be changed to fit the sudoku play
		for (int i = 0; i < 9; i++)
			//Here we check if n is equal to any of the rows (manipulated by 'i') at the column of x
			if (grid[i][x] == n && i != y)
				//Return false if n is in the row
				return false;

		//This for loop iterates from 0 to 9, these values can be changed to fit the sudoku play
		for (int i = 0; i < 9; i++)
			//Here we check if n is equal to any of the columns (manipulated by 'i') at the row of y
			if (grid[y][i] == n && i != x)
				//Return false if n is in the column
				return false;

		//This will switch through the columns and rows. 
		//So that when x = 0 or 1 or 2 we dont run. When x = 3 we do run. 
		//The same goes for y
		int x0 = (x / 3) * 3;
		int y0 = (y / 3) * 3;
		//We run 3 times for each 3cell in row
		for (int X = x0; X < (x0 + 3); X++)
			//We run 3 times for each 3cell in column
			for (int Y = y0; Y < (y0 + 3); Y++)
				//We check if any of the values is equal to n and return false if it is
				if (grid[Y][X] == n)
					return false;
		return true;
	}
	void Print(std::vector<std::vector<int>> matrix)
	{
		std::vector<int> list;
		//Here we add all of the rows into one big list
		for (std::vector<int> row : matrix)
			list.insert(list.end(), row.begin(), row.end());
		std::cout << std::endl;
		for (byte i_1 = 0; i_1 < matrix.size(); i_1++)
		{
			if (i_1 % 3 == 0 && i_1 != 0)
				std::cout << "-----------------------" << std::endl;
			for (byte i_2 = 0; i_2 < matrix[i_1].size(); i_2++)
				if (i_2 % 3 == 0 && i_2 != 0)
					std::cout << " | " << matrix[i_1][i_2];
				else
					std::cout << " " << matrix[i_1][i_2];
			std::cout << std::endl;
		}

		int counterGoBack = 1;
		//Here we iterate through each of the int in the list
		for (int num : list)
		{
			//Here we press the individual key
			PressKey(0x30 + num);
			//Here we go to the right, so that we can type another number
			PressKey(VK_RIGHT);
			if (counterGoBack % 9 == 0)
			{
				PressKey(VK_DOWN);
				for (int i = 0; i < 18; i++)
					PressKey(VK_LEFT);
			}
			counterGoBack++;
		}
	}
	void Solve()
	{
		//This will just check if the cell is 0, if it is, then it will try and check which number it can be.
		for (int y = 0; y < 9; y++)
			for (int x = 0; x < 9; x++)
				if (grid[y][x] == 0)
				{
					for (int n = 1; n < 10; n++)
						if (Possible(x, y, n))
						{
							grid[y][x] = n;
							Solve();
							grid[y][x] = 0;
						}
					return;
				}

		Print(grid);
	}
#pragma endregion
#pragma endregion
#pragma region Inputs
	void PressMouse(int virtKey, int duration = 0)
	{
		INPUT input{};
		input.type = INPUT_MOUSE;
		if (VK_LBUTTON == virtKey)
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		else if (VK_RBUTTON == virtKey)
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		SendInput(1, &input, sizeof(input));
		ZeroMemory(&input, sizeof(input));
		Sleep(duration);
		input.type = INPUT_MOUSE;
		if (VK_LBUTTON == virtKey)
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		else if (VK_RBUTTON == virtKey)
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		SendInput(1, &input, sizeof(input));
	}
	void PressKey(WORD virtualKey)
	{
		//Very basic way of sending keys and mouse events to the computer
		if (virtualKey == 0)
			return;
		INPUT ip{};
		ip.type = INPUT_KEYBOARD;
		ip.ki.wScan = 0;
		ip.ki.time = 0;
		ip.ki.dwExtraInfo = 0;

		//Virtual-key code
		ip.ki.wVk = virtualKey;
		//0 for key press 
		ip.ki.dwFlags = 0;
		SendInput(1, &ip, sizeof(ip));

		//Sleep for the desired time, can be changed by the user.
		Sleep(TimeDelay);
	}
#pragma endregion
#pragma region Converters
	HDC ConvertHandleToHDC(HANDLE hHandle)
	{
		HDC hDC = NULL;

		// Check if the HANDLE represents a bitmap, for example
		if (GetObjectType(hHandle) == OBJ_BITMAP)
		{
			hDC = CreateCompatibleDC(NULL);
			SelectObject(hDC, hHandle);
		}

		return hDC;
	}
	HBITMAP ConvertHandleToHBitmap(HANDLE hHandle)
	{
		HBITMAP hBitmap = NULL;

		// Copy the image using CopyImage
		hBitmap = (HBITMAP)CopyImage(hHandle, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);

		return hBitmap;
	}
#pragma endregion
#pragma region Value Checkers
	int IsGameColor(COLORREF color)
	{
		int red = GetRValue(color);
		int green = GetGValue(color);
		int blue = GetBValue(color);
		if (red == 52 && green == 72 && blue == 97)
			return 1;
		if (red == 229 && green == 92 && blue == 108)
			return 2;
		return 0;
	}
#pragma endregion
#pragma region GetGame
private:
	POINT rightClick{};
	POINT leftClick{};

	//Get "Copy Points"
	void GetGameCopyPos()
	{
		//Copy image
		std::cout << "Right click on game" << std::endl;
		while (true)
		{
			if (GetKeyState(VK_RBUTTON) < 0)
			{
				GetCursorPos(&rightClick);
				break;
			}
			Sleep(10);
		}
		std::cout << "Left click on copy" << std::endl;
		while (true)
		{
			if (GetKeyState(VK_LBUTTON) < 0)
			{
				GetCursorPos(&leftClick);
				break;
			}
			Sleep(10);
		}
		EmptyClipboard();
	}

	//Get Game
	std::vector<std::vector<std::string>> GetGameCodes(int compressValue = 6)
	{
		EmptyClipboard();
		//Copy Game
		SetCursorPos(rightClick.x, rightClick.y);
		PressMouse(VK_RBUTTON, 0);
		Sleep(50);
		SetCursorPos(leftClick.x, leftClick.y);
		PressMouse(VK_LBUTTON, 0);

		Sleep(500);

		//Get data as HDC from Clipboard and convert that to std::vector<std::vector<bool>>
		OpenClipboard(NULL);
		HANDLE handle = GetClipboardData(CF_BITMAP);
		HBITMAP hbitmap = ConvertHandleToHBitmap(handle);
		HDC hdc = ConvertHandleToHDC(handle);
		BITMAP bitmap{};
		GetObject(hbitmap, sizeof(BITMAP), &bitmap);
		CloseClipboard();
		std::vector<std::vector<bool>> game;
		for (int y = 0; y < bitmap.bmHeight; y++)
			if (y % compressValue == 0)
			{
				std::vector<bool> tempGame;
				for (int x = 0; x < bitmap.bmWidth; x++)
					if (x % compressValue == 0)
						tempGame.push_back(IsGameColor(GetPixel(hdc, x, y)) > 0);
				game.push_back(tempGame);
			}


		//Remove lines
		std::vector<std::vector<bool>> tempGame;
		int removedX = 0;
		for (int y = 0; y < game.size(); y++)
		{
			int countX = 0;
			for (int x = 0; x < game[y].size(); x++)
				if (game[y][x])
					countX++;
			if (game[y].size() / 2 > countX)
				tempGame.push_back(game[y]);
			else
				removedX++;
		}
		for (int x = 0; x < tempGame[0].size(); x++)
		{
			int countY = 0;
			for (int y = 0; y < tempGame.size(); y++)
				if (tempGame[y][x])
					countY++;

			if (tempGame.size() / 1.2 - removedX < countY)
				for (int y = 0; y < tempGame.size(); y++)
					tempGame[y][x] = false;
		}
		game = tempGame;

		std::vector<std::vector<std::string>> NumberCode;
		int jumpY = int(game.size()) / 9;
		int jumpX = int(game[0].size()) / 9;
		for (int Y = 0; Y < 9; Y++)
		{
			std::vector<std::string> tempNumbers2;
			for (int X = 0; X < 9; X++)
			{
				std::string tempNumbers = "";
				for (int y = jumpY * Y; y < jumpY * (1 + Y); y++)
				{
					int codeCount = 0;
					for (int x = jumpX * X; x < jumpX * (1 + X); x++)
						if (game[y][x])
							codeCount++;
					if (codeCount > 0)
						tempNumbers += std::to_string(codeCount);
				}
				tempNumbers2.push_back(tempNumbers);
			}
			NumberCode.push_back(tempNumbers2);
		}

		return NumberCode;
	}
#pragma endregion
};

int main()
{
	//To stop the code press 's'
	SudokuGameAutoSolver gameSolver;
	gameSolver.TimeDelay = 1;
	gameSolver.ExecuteOrder66();
}
