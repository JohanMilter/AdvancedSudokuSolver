#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <tuple>
#include <sstream>
#include <algorithm>
#include <unordered_set>

class ManipulateSudoku
{
public:
	void ExecuteOrder66()
	{
		std::cout << "Go to the solver on the website." << std::endl;
		Calibrate();

		SetCursorPos(button.x, button.y);
		PressMouse(true);
		Sleep(2000);
		system("cls");
		char ready;
		std::cout << "Hover the mouse over the difficulty you want to solve: " << std::endl;
		do
		{
			std::cout << "Ready (y/n)? ";
			std::cin >> ready;
			if (ready == 'y')
				GetCursorPos(&button);
		} while (ready == 'n');

		SolveSudoku();
	}
private:
#pragma region ImageAnalyser
	BITMAP bitmap{};
	HBITMAP hbitmap{};
	HDC hDC = NULL;
	COLORREF GetPixelColor(HDC hdc, std::tuple<int, int> xy)
	{
		return GetPixel(hdc, std::get<0>(xy), std::get<1>(xy));
	}
	void GetScreenshot(int width, int height, std::tuple<int, int> xy)
	{
		// copy screen to bitmap
		HDC     hScreen = GetDC(NULL);
		hDC = CreateCompatibleDC(hScreen);
		hbitmap = CreateCompatibleBitmap(hScreen, width, height);
		SelectObject(hDC, hbitmap);
		BitBlt(hDC, 0, 0, width, height, hScreen, std::get<0>(xy), std::get<1>(xy), SRCCOPY);

		GetObject(hbitmap, sizeof(BITMAP), &bitmap);

		// save bitmap to clipboard
		OpenClipboard(NULL);
		EmptyClipboard();
		SetClipboardData(CF_BITMAP, hbitmap);
		CloseClipboard();

		// clean up
		//DeleteDC(hDC);
		ReleaseDC(NULL, hScreen);
		//DeleteObject(hBitmap);

	}
#pragma endregion
#pragma region Sudoku Solver
	void CalculateSudokuWithValues(std::vector<std::vector<int>> Grid)
	{
		grid = Grid;
		if (grid.size() >= 9)
			Solve();
	}
	std::vector<std::vector<int>> grid;
	void GetValues()
	{
		std::string row;
		do
		{
			std::vector<int> ints;
			std::cout << "Row " << grid.size() + 1 << ": ";
			std::getline(std::cin, row);
			if (row.size() != 9)
			{
				std::cout << "Der skal være 9 tal!" << std::endl;
				break;
			}
			for (char num : row)
				ints.push_back(int(num - '0'));
			grid.push_back(ints);
		} while (grid.size() < 9);
	}
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
		TimeDelay = 1;
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
			PressKey(0x30 + num, INPUT_KEYBOARD);
			//Here we go to the right, so that we can type another number
			PressKey(VK_RIGHT, INPUT_KEYBOARD);
			if (counterGoBack % 9 == 0)
			{
				PressKey(VK_DOWN, INPUT_KEYBOARD);
				for (int i = 0; i < 9; i++)
					PressKey(VK_LEFT, INPUT_KEYBOARD);
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
#pragma region GetGamePlate
	void TestScreenshot()
	{
		GetScreenshot(width, height, std::make_tuple(startX, startY));
	}
	int matchColor = 2;
	std::vector<std::vector<std::string>> GetIndividualNumbers()
	{
		auto game = GetGame();

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
	int IsGameColor(COLORREF color)
	{
		int red = GetRValue(color);
		int green = GetGValue(color);
		int blue = GetBValue(color);
		if ((red == 52 && green == 72 && blue == 97) ||
			(red == 52 && green == 73 && blue == 97) ||
			(red == 53 && green == 73 && blue == 97) ||
			(red == 53 && green == 73 && blue == 98) ||
			(red == 53 && green == 74 && blue == 98) ||
			(red == 53 && green == 74 && blue == 99) ||
			(red == 54 && green == 73 && blue == 98) ||
			(red == 54 && green == 74 && blue == 98) ||
			(red == 54 && green == 74 && blue == 99) ||
			(red == 54 && green == 75 && blue == 100) ||
			(red == 55 && green == 74 && blue == 99) ||
			(red == 55 && green == 75 && blue == 99) ||
			(red == 55 && green == 75 && blue == 100) ||
			(red == 55 && green == 76 && blue == 101) ||
			(red == 56 && green == 75 && blue == 100) ||
			(red == 56 && green == 76 && blue == 100) ||
			(red == 56 && green == 76 && blue == 101) ||
			(red == 56 && green == 77 && blue == 102) ||
			(red == 57 && green == 76 && blue == 101) ||
			(red == 57 && green == 77 && blue == 101) ||
			(red == 57 && green == 77 && blue == 102) ||
			(red == 57 && green == 78 && blue == 103) ||
			(red == 58 && green == 77 && blue == 101) ||
			(red == 58 && green == 77 && blue == 102) ||
			(red == 58 && green == 78 && blue == 102) ||
			(red == 58 && green == 78 && blue == 103) ||
			(red == 58 && green == 79 && blue == 104) ||
			(red == 59 && green == 78 && blue == 102) ||
			(red == 59 && green == 78 && blue == 103) ||
			(red == 59 && green == 79 && blue == 103) ||
			(red == 59 && green == 80 && blue == 105) ||
			(red == 60 && green == 79 && blue == 103) ||
			(red == 60 && green == 79 && blue == 104) ||
			(red == 60 && green == 80 && blue == 104) ||
			(red == 61 && green == 80 && blue == 104) ||
			(red == 61 && green == 81 && blue == 105) ||
			(red == 61 && green == 82 && blue == 106) ||
			(red == 61 && green == 82 && blue == 107) ||
			(red == 62 && green == 81 && blue == 105) ||
			(red == 62 && green == 82 && blue == 106) ||
			(red == 62 && green == 84 && blue == 109) ||
			(red == 63 && green == 81 && blue == 105) ||
			(red == 63 && green == 82 && blue == 105) ||
			(red == 63 && green == 82 && blue == 106) ||
			(red == 63 && green == 83 && blue == 107) ||
			(red == 63 && green == 83 && blue == 108) ||
			(red == 64 && green == 82 && blue == 106) ||
			(red == 64 && green == 83 && blue == 106) ||
			(red == 64 && green == 83 && blue == 107) ||
			(red == 64 && green == 84 && blue == 107) ||
			(red == 64 && green == 84 && blue == 108) ||
			(red == 64 && green == 84 && blue == 109) ||
			(red == 64 && green == 85 && blue == 110) ||
			(red == 65 && green == 83 && blue == 107) ||
			(red == 65 && green == 84 && blue == 107) ||
			(red == 65 && green == 84 && blue == 108) ||
			(red == 65 && green == 85 && blue == 108) ||
			(red == 65 && green == 85 && blue == 109) ||
			(red == 65 && green == 85 && blue == 110) ||
			(red == 66 && green == 84 && blue == 108) /*||
			(red == 66 && green == 85 && blue == 108) ||
			(red == 66 && green == 85 && blue == 109) ||
			(red == 66 && green == 88 && blue == 113) ||
			(red == 67 && green == 85 && blue == 109) ||
			(red == 67 && green == 86 && blue == 109) ||
			(red == 67 && green == 86 && blue == 110) ||
			(red == 68 && green == 86 && blue == 109) ||
			(red == 68 && green == 86 && blue == 110) ||
			(red == 68 && green == 87 && blue == 109) ||
			(red == 68 && green == 87 && blue == 110) ||
			(red == 68 && green == 87 && blue == 111) ||
			(red == 69 && green == 87 && blue == 110) ||
			(red == 69 && green == 88 && blue == 110) ||
			(red == 69 && green == 88 && blue == 111) ||
			(red == 69 && green == 88 && blue == 112) ||
			(red == 70 && green == 88 && blue == 111) ||
			(red == 70 && green == 89 && blue == 112) ||
			(red == 70 && green == 89 && blue == 113) ||
			(red == 70 && green == 92 && blue == 118) ||
			(red == 71 && green == 89 && blue == 112) ||
			(red == 71 && green == 90 && blue == 113) ||
			(red == 72 && green == 90 && blue == 113) ||
			(red == 72 && green == 90 && blue == 114) ||
			(red == 72 && green == 91 && blue == 113) ||
			(red == 72 && green == 91 && blue == 114) ||
			(red == 72 && green == 92 && blue == 116) ||
			(red == 73 && green == 91 && blue == 113) ||
			(red == 73 && green == 91 && blue == 114) ||
			(red == 73 && green == 92 && blue == 114) ||
			(red == 73 && green == 92 && blue == 115) ||
			(red == 73 && green == 96 && blue == 122) ||
			(red == 74 && green == 91 && blue == 114) ||
			(red == 74 && green == 92 && blue == 114) ||
			(red == 74 && green == 92 && blue == 115) ||
			(red == 74 && green == 93 && blue == 115) ||
			(red == 75 && green == 93 && blue == 115) ||
			(red == 75 && green == 93 && blue == 116) ||
			(red == 75 && green == 94 && blue == 116) ||
			(red == 75 && green == 94 && blue == 117) ||
			(red == 75 && green == 95 && blue == 119) ||
			(red == 76 && green == 93 && blue == 115) ||
			(red == 76 && green == 94 && blue == 117) ||
			(red == 76 && green == 95 && blue == 117) ||
			(red == 76 && green == 96 && blue == 120) ||
			(red == 77 && green == 94 && blue == 116) ||
			(red == 77 && green == 95 && blue == 117) ||
			(red == 77 && green == 95 && blue == 118) ||
			(red == 77 && green == 96 && blue == 118) ||
			(red == 77 && green == 96 && blue == 119) ||
			(red == 77 && green == 97 && blue == 121) ||
			(red == 77 && green == 100 && blue == 126) ||
			(red == 78 && green == 96 && blue == 117) ||
			(red == 78 && green == 96 && blue == 118) ||
			(red == 78 && green == 97 && blue == 119) ||
			(red == 79 && green == 96 && blue == 118) ||
			(red == 79 && green == 97 && blue == 118) ||
			(red == 79 && green == 97 && blue == 119) ||
			(red == 79 && green == 97 && blue == 120) ||
			(red == 79 && green == 98 && blue == 120) ||
			(red == 80 && green == 97 && blue == 119) ||
			(red == 80 && green == 98 && blue == 119) ||
			(red == 80 && green == 98 && blue == 120) ||
			(red == 80 && green == 99 && blue == 121) ||
			(red == 80 && green == 104 && blue == 130) ||
			(red == 81 && green == 98 && blue == 119) ||
			(red == 81 && green == 98 && blue == 120) ||
			(red == 81 && green == 99 && blue == 120) ||
			(red == 81 && green == 99 && blue == 121) ||
			(red == 81 && green == 99 && blue == 122) ||
			(red == 81 && green == 104 && blue == 130) ||
			(red == 82 && green == 99 && blue == 120) ||
			(red == 82 && green == 99 && blue == 121) ||
			(red == 82 && green == 100 && blue == 122) ||
			(red == 82 && green == 105 && blue == 131) ||
			(red == 83 && green == 100 && blue == 121) ||
			(red == 83 && green == 101 && blue == 123) ||
			(red == 84 && green == 101 && blue == 122) ||
			(red == 84 && green == 102 && blue == 124) ||
			(red == 84 && green == 103 && blue == 125) ||
			(red == 85 && green == 102 && blue == 123) ||
			(red == 85 && green == 103 && blue == 124) ||
			(red == 85 && green == 105 && blue == 128) ||
			(red == 86 && green == 103 && blue == 123) ||
			(red == 86 && green == 103 && blue == 124) ||
			(red == 86 && green == 107 && blue == 130) ||
			(red == 87 && green == 103 && blue == 126) ||
			(red == 88 && green == 104 && blue == 125) ||
			(red == 88 && green == 105 && blue == 125) ||
			(red == 88 && green == 105 && blue == 126) ||
			(red == 88 && green == 106 && blue == 127) ||
			(red == 88 && green == 108 && blue == 131) ||
			(red == 88 && green == 108 && blue == 132) ||
			(red == 89 && green == 105 && blue == 125) ||
			(red == 89 && green == 106 && blue == 126) ||
			(red == 89 && green == 107 && blue == 128) ||
			(red == 89 && green == 112 && blue == 138) ||
			(red == 89 && green == 114 && blue == 140) ||
			(red == 90 && green == 106 && blue == 126) ||
			(red == 90 && green == 107 && blue == 129) ||
			(red == 90 && green == 108 && blue == 129) ||
			(red == 91 && green == 107 && blue == 127) ||
			(red == 91 && green == 108 && blue == 127) ||
			(red == 91 && green == 108 && blue == 129) ||
			(red == 91 && green == 108 && blue == 130) ||
			(red == 91 && green == 109 && blue == 130) ||
			(red == 92 && green == 108 && blue == 128) ||
			(red == 92 && green == 109 && blue == 129) ||
			(red == 92 && green == 109 && blue == 130) ||
			(red == 92 && green == 113 && blue == 136) ||
			(red == 92 && green == 116 && blue == 143) ||
			(red == 93 && green == 109 && blue == 129) ||
			(red == 93 && green == 110 && blue == 131) ||
			(red == 93 && green == 111 && blue == 131) ||
			(red == 93 && green == 111 && blue == 132) ||
			(red == 94 && green == 110 && blue == 129) ||
			(red == 94 && green == 110 && blue == 130) ||
			(red == 94 && green == 111 && blue == 132) ||
			(red == 94 && green == 112 && blue == 132) ||
			(red == 94 && green == 112 && blue == 133) ||
			(red == 94 && green == 114 && blue == 137) ||
			(red == 94 && green == 119 && blue == 145) ||
			(red == 94 && green == 120 && blue == 145) ||
			(red == 95 && green == 110 && blue == 130) ||
			(red == 95 && green == 111 && blue == 130) ||
			(red == 95 && green == 111 && blue == 131) ||
			(red == 95 && green == 115 && blue == 138) ||
			(red == 95 && green == 120 && blue == 146) ||
			(red == 96 && green == 112 && blue == 131) ||
			(red == 96 && green == 112 && blue == 132) ||
			(red == 96 && green == 113 && blue == 133) ||
			(red == 96 && green == 113 && blue == 134) ||
			(red == 96 && green == 114 && blue == 134) ||
			(red == 97 && green == 112 && blue == 132) ||
			(red == 97 && green == 113 && blue == 132) ||
			(red == 97 && green == 114 && blue == 135) ||
			(red == 98 && green == 113 && blue == 132) ||
			(red == 98 && green == 113 && blue == 133) ||
			(red == 98 && green == 114 && blue == 133) ||
			(red == 98 && green == 115 && blue == 135) ||
			(red == 98 && green == 115 && blue == 136) ||
			(red == 98 && green == 116 && blue == 136) ||
			(red == 99 && green == 115 && blue == 134) ||
			(red == 99 && green == 116 && blue == 136) ||
			(red == 99 && green == 116 && blue == 137) ||
			(red == 99 && green == 119 && blue == 142) ||
			(red == 99 && green == 124 && blue == 150) ||
			(red == 100 && green == 115 && blue == 134) ||
			(red == 100 && green == 116 && blue == 134) ||
			(red == 100 && green == 117 && blue == 137) ||
			(red == 100 && green == 117 && blue == 138) ||
			(red == 100 && green == 118 && blue == 138) ||
			(red == 100 && green == 126 && blue == 152) ||
			(red == 101 && green == 116 && blue == 135) ||
			(red == 101 && green == 117 && blue == 136) ||
			(red == 101 && green == 118 && blue == 138) ||
			(red == 101 && green == 118 && blue == 139) ||
			(red == 102 && green == 117 && blue == 135) ||
			(red == 102 && green == 117 && blue == 136) ||
			(red == 102 && green == 118 && blue == 139) ||
			(red == 103 && green == 118 && blue == 136) ||
			(red == 103 && green == 118 && blue == 137) ||
			(red == 103 && green == 123 && blue == 146) ||
			(red == 104 && green == 119 && blue == 137) ||
			(red == 104 && green == 119 && blue == 138) ||
			(red == 104 && green == 120 && blue == 140) ||
			(red == 104 && green == 121 && blue == 141) ||
			(red == 105 && green == 120 && blue == 138) ||
			(red == 105 && green == 121 && blue == 141) ||
			(red == 105 && green == 121 && blue == 142) ||
			(red == 105 && green == 122 && blue == 141) ||
			(red == 105 && green == 122 && blue == 142) ||
			(red == 105 && green == 125 && blue == 147) ||
			(red == 106 && green == 120 && blue == 138) ||
			(red == 106 && green == 121 && blue == 139) ||
			(red == 106 && green == 121 && blue == 140) ||
			(red == 106 && green == 122 && blue == 142) ||
			(red == 106 && green == 126 && blue == 149) ||
			(red == 107 && green == 122 && blue == 140) ||
			(red == 107 && green == 123 && blue == 143) ||
			(red == 107 && green == 124 && blue == 143) ||
			(red == 107 && green == 128 && blue == 150) ||
			(red == 108 && green == 122 && blue == 140) ||
			(red == 108 && green == 123 && blue == 141) ||
			(red == 108 && green == 124 && blue == 143) ||
			(red == 108 && green == 125 && blue == 144) ||
			(red == 109 && green == 123 && blue == 141) ||
			(red == 109 && green == 124 && blue == 142) ||
			(red == 109 && green == 125 && blue == 145) ||
			(red == 109 && green == 129 && blue == 152) ||
			(red == 110 && green == 124 && blue == 142) ||
			(red == 110 && green == 126 && blue == 145) ||
			(red == 110 && green == 127 && blue == 145) ||
			(red == 110 && green == 127 && blue == 146) ||
			(red == 111 && green == 125 && blue == 142) ||
			(red == 111 && green == 125 && blue == 143) ||
			(red == 111 && green == 127 && blue == 146) ||
			(red == 111 && green == 127 && blue == 147) ||
			(red == 111 && green == 128 && blue == 147) ||
			(red == 112 && green == 126 && blue == 143) ||
			(red == 112 && green == 126 && blue == 144) ||
			(red == 112 && green == 127 && blue == 144) ||
			(red == 112 && green == 129 && blue == 148) ||
			(red == 113 && green == 127 && blue == 144) ||
			(red == 113 && green == 127 && blue == 145) ||
			(red == 113 && green == 129 && blue == 147) ||
			(red == 113 && green == 129 && blue == 148) ||
			(red == 113 && green == 133 && blue == 155) ||
			(red == 113 && green == 140 && blue == 166) ||
			(red == 114 && green == 128 && blue == 145) ||
			(red == 114 && green == 128 && blue == 146) ||
			(red == 114 && green == 130 && blue == 149) ||
			(red == 115 && green == 129 && blue == 146) ||
			(red == 115 && green == 131 && blue == 150) ||
			(red == 116 && green == 130 && blue == 146) ||
			(red == 116 && green == 130 && blue == 147) ||
			(red == 116 && green == 132 && blue == 150) ||
			(red == 116 && green == 132 && blue == 151) ||
			(red == 117 && green == 133 && blue == 151) ||
			(red == 117 && green == 133 && blue == 152) ||
			(red == 117 && green == 144 && blue == 171) ||
			(red == 118 && green == 131 && blue == 148) ||
			(red == 118 && green == 132 && blue == 148) ||
			(red == 118 && green == 132 && blue == 149) ||
			(red == 118 && green == 134 && blue == 152) ||
			(red == 118 && green == 134 && blue == 153) ||
			(red == 119 && green == 132 && blue == 149) ||
			(red == 119 && green == 135 && blue == 153) ||
			(red == 119 && green == 146 && blue == 173) ||
			(red == 120 && green == 133 && blue == 150) ||
			(red == 120 && green == 134 && blue == 150) ||
			(red == 120 && green == 136 && blue == 153) ||
			(red == 120 && green == 136 && blue == 154) ||
			(red == 120 && green == 140 && blue == 162) ||
			(red == 120 && green == 147 && blue == 174) ||
			(red == 121 && green == 134 && blue == 150) ||
			(red == 121 && green == 135 && blue == 151) ||
			(red == 121 && green == 135 && blue == 154) ||
			(red == 121 && green == 136 && blue == 155) ||
			(red == 121 && green == 137 && blue == 155) ||
			(red == 122 && green == 135 && blue == 151) ||
			(red == 122 && green == 135 && blue == 154) ||
			(red == 122 && green == 135 && blue == 155) ||
			(red == 122 && green == 136 && blue == 155) ||
			(red == 122 && green == 138 && blue == 156) ||
			(red == 122 && green == 149 && blue == 176) ||
			(red == 123 && green == 136 && blue == 152) ||
			(red == 123 && green == 136 && blue == 155) ||
			(red == 123 && green == 139 && blue == 157) ||
			(red == 123 && green == 143 && blue == 165) ||
			(red == 124 && green == 137 && blue == 153) ||
			(red == 124 && green == 139 && blue == 157) ||
			(red == 124 && green == 140 && blue == 158) ||
			(red == 124 && green == 151 && blue == 179) ||
			(red == 125 && green == 138 && blue == 153) ||
			(red == 125 && green == 138 && blue == 154) ||
			(red == 125 && green == 140 && blue == 158) ||
			(red == 126 && green == 138 && blue == 154) ||
			(red == 126 && green == 139 && blue == 154) ||
			(red == 126 && green == 141 && blue == 159) ||
			(red == 126 && green == 146 && blue == 168) ||
			(red == 127 && green == 140 && blue == 156) ||
			(red == 127 && green == 142 && blue == 159) ||
			(red == 128 && green == 140 && blue == 156) ||
			(red == 128 && green == 143 && blue == 160) ||
			(red == 128 && green == 143 && blue == 161) ||
			(red == 128 && green == 144 && blue == 161) ||
			(red == 129 && green == 142 && blue == 157) ||
			(red == 129 && green == 144 && blue == 162) ||
			(red == 129 && green == 150 && blue == 171) ||
			(red == 130 && green == 142 && blue == 157) ||
			(red == 130 && green == 142 && blue == 158) ||
			(red == 130 && green == 143 && blue == 159) ||
			(red == 131 && green == 143 && blue == 158) ||
			(red == 131 && green == 146 && blue == 162) ||
			(red == 131 && green == 146 && blue == 164) ||
			(red == 131 && green == 152 && blue == 173) ||
			(red == 132 && green == 144 && blue == 159) ||
			(red == 132 && green == 145 && blue == 160) ||
			(red == 132 && green == 148 && blue == 164) ||
			(red == 132 && green == 153 && blue == 174) ||
			(red == 133 && green == 145 && blue == 160) ||
			(red == 133 && green == 145 && blue == 162) ||
			(red == 133 && green == 148 && blue == 165) ||
			(red == 133 && green == 149 && blue == 166) ||
			(red == 133 && green == 162 && blue == 189) ||
			(red == 133 && green == 163 && blue == 190) ||
			(red == 134 && green == 146 && blue == 160) ||
			(red == 134 && green == 146 && blue == 161) ||
			(red == 134 && green == 149 && blue == 165) ||
			(red == 134 && green == 163 && blue == 190) ||
			(red == 135 && green == 147 && blue == 161) ||
			(red == 135 && green == 147 && blue == 162) ||
			(red == 135 && green == 150 && blue == 167) ||
			(red == 135 && green == 151 && blue == 167) ||
			(red == 135 && green == 164 && blue == 192) ||
			(red == 136 && green == 147 && blue == 162) ||
			(red == 136 && green == 148 && blue == 162) ||
			(red == 136 && green == 148 && blue == 163) ||
			(red == 136 && green == 150 && blue == 167) ||
			(red == 136 && green == 151 && blue == 167) ||
			(red == 136 && green == 151 && blue == 168) ||
			(red == 137 && green == 148 && blue == 163) ||
			(red == 137 && green == 151 && blue == 168) ||
			(red == 137 && green == 152 && blue == 169) ||
			(red == 138 && green == 150 && blue == 164) ||
			(red == 138 && green == 153 && blue == 169) ||
			(red == 138 && green == 157 && blue == 178) ||
			(red == 138 && green == 158 && blue == 179) ||
			(red == 139 && green == 150 && blue == 164) ||
			(red == 139 && green == 150 && blue == 165) ||
			(red == 139 && green == 150 && blue == 166) ||
			(red == 139 && green == 151 && blue == 165) ||
			(red == 139 && green == 153 && blue == 170) ||
			(red == 139 && green == 154 && blue == 170) ||
			(red == 139 && green == 168 && blue == 196) ||
			(red == 140 && green == 151 && blue == 166) ||
			(red == 140 && green == 152 && blue == 166) ||
			(red == 140 && green == 154 && blue == 170) ||
			(red == 140 && green == 154 && blue == 171) ||
			(red == 140 && green == 155 && blue == 171) ||
			(red == 140 && green == 170 && blue == 198) ||
			(red == 141 && green == 152 && blue == 165) ||
			(red == 141 && green == 152 && blue == 166) ||
			(red == 141 && green == 153 && blue == 166) ||
			(red == 141 && green == 153 && blue == 167) ||
			(red == 141 && green == 155 && blue == 172) ||
			(red == 141 && green == 156 && blue == 172) ||
			(red == 142 && green == 153 && blue == 167) ||
			(red == 142 && green == 156 && blue == 173) ||
			(red == 143 && green == 154 && blue == 168) ||
			(red == 143 && green == 155 && blue == 168) ||
			(red == 143 && green == 157 && blue == 173) ||
			(red == 144 && green == 155 && blue == 168) ||
			(red == 144 && green == 155 && blue == 169) ||
			(red == 144 && green == 158 && blue == 174) ||
			(red == 144 && green == 159 && blue == 175) ||
			(red == 145 && green == 155 && blue == 169) ||
			(red == 145 && green == 159 && blue == 175) ||
			(red == 146 && green == 157 && blue == 170) ||
			(red == 146 && green == 160 && blue == 175) ||
			(red == 146 && green == 160 && blue == 176) ||
			(red == 146 && green == 161 && blue == 176) ||
			(red == 147 && green == 158 && blue == 171) ||
			(red == 147 && green == 159 && blue == 171) ||
			(red == 148 && green == 158 && blue == 171) ||
			(red == 148 && green == 159 && blue == 172) ||
			(red == 148 && green == 162 && blue == 177) ||
			(red == 148 && green == 169 && blue == 189) ||
			(red == 149 && green == 159 && blue == 172) ||
			(red == 149 && green == 160 && blue == 172) ||
			(red == 149 && green == 160 && blue == 173) ||
			(red == 149 && green == 163 && blue == 178) ||
			(red == 150 && green == 160 && blue == 173) ||
			(red == 150 && green == 161 && blue == 173) ||
			(red == 150 && green == 161 && blue == 174) ||
			(red == 150 && green == 163 && blue == 179) ||
			(red == 150 && green == 164 && blue == 179) ||
			(red == 150 && green == 169 && blue == 190) ||
			(red == 151 && green == 161 && blue == 174) ||
			(red == 151 && green == 165 && blue == 180) ||
			(red == 152 && green == 162 && blue == 175) ||
			(red == 153 && green == 163 && blue == 175) ||
			(red == 153 && green == 163 && blue == 176) ||
			(red == 153 && green == 166 && blue == 181) ||
			(red == 153 && green == 167 && blue == 181) ||
			(red == 153 && green == 167 && blue == 182) ||
			(red == 154 && green == 164 && blue == 176) ||
			(red == 154 && green == 164 && blue == 177) ||
			(red == 154 && green == 168 && blue == 183) ||
			(red == 155 && green == 165 && blue == 177) ||
			(red == 155 && green == 169 && blue == 183) ||
			(red == 155 && green == 169 && blue == 184) ||
			(red == 155 && green == 175 && blue == 195) ||
			(red == 156 && green == 166 && blue == 178) ||
			(red == 156 && green == 169 && blue == 184) ||
			(red == 157 && green == 167 && blue == 178) ||
			(red == 157 && green == 167 && blue == 179) ||
			(red == 158 && green == 168 && blue == 179) ||
			(red == 158 && green == 168 && blue == 180) ||
			(red == 158 && green == 171 && blue == 186) ||
			(red == 158 && green == 172 && blue == 186) ||
			(red == 158 && green == 177 && blue == 198) ||
			(red == 158 && green == 178 && blue == 198) ||
			(red == 159 && green == 168 && blue == 180) ||
			(red == 159 && green == 179 && blue == 200) ||
			(red == 159 && green == 180 && blue == 200) ||
			(red == 160 && green == 169 && blue == 180) ||
			(red == 160 && green == 169 && blue == 181) ||
			(red == 160 && green == 174 && blue == 188) ||
			(red == 160 && green == 192 && blue == 220) ||
			(red == 161 && green == 170 && blue == 182) ||
			(red == 161 && green == 174 && blue == 188) ||
			(red == 162 && green == 171 && blue == 183) ||
			(red == 162 && green == 172 && blue == 183) ||
			(red == 162 && green == 175 && blue == 189) ||
			(red == 162 && green == 175 && blue == 190) ||
			(red == 162 && green == 176 && blue == 190) ||
			(red == 163 && green == 172 && blue == 183) ||
			(red == 163 && green == 172 && blue == 184) ||
			(red == 163 && green == 176 && blue == 190) ||
			(red == 163 && green == 183 && blue == 203) ||
			(red == 163 && green == 184 && blue == 204) ||
			(red == 163 && green == 194 && blue == 223) ||
			(red == 163 && green == 196 && blue == 224) ||
			(red == 164 && green == 173 && blue == 184) ||
			(red == 164 && green == 196 && blue == 225) ||
			(red == 165 && green == 174 && blue == 185) ||
			(red == 165 && green == 178 && blue == 191) ||
			(red == 165 && green == 178 && blue == 192) ||
			(red == 165 && green == 179 && blue == 192) ||
			(red == 166 && green == 174 && blue == 185) ||
			(red == 166 && green == 175 && blue == 185) ||
			(red == 166 && green == 175 && blue == 186) ||
			(red == 166 && green == 179 && blue == 192) ||
			(red == 166 && green == 179 && blue == 193) ||
			(red == 166 && green == 186 && blue == 206) ||
			(red == 166 && green == 199 && blue == 228) ||
			(red == 167 && green == 175 && blue == 185) ||
			(red == 167 && green == 176 && blue == 187) ||
			(red == 167 && green == 180 && blue == 193) ||
			(red == 167 && green == 180 && blue == 194) ||
			(red == 168 && green == 180 && blue == 194) ||
			(red == 169 && green == 177 && blue == 187) ||
			(red == 169 && green == 177 && blue == 188) ||
			(red == 169 && green == 182 && blue == 195) ||
			(red == 169 && green == 189 && blue == 209) ||
			(red == 169 && green == 202 && blue == 231) ||
			(red == 170 && green == 178 && blue == 189) ||
			(red == 170 && green == 179 && blue == 189) ||
			(red == 170 && green == 183 && blue == 196) ||
			(red == 170 && green == 190 && blue == 210) ||
			(red == 170 && green == 203 && blue == 231) ||
			(red == 171 && green == 179 && blue == 189) ||
			(red == 171 && green == 179 && blue == 190) ||
			(red == 171 && green == 180 && blue == 190) ||
			(red == 171 && green == 183 && blue == 197) ||
			(red == 171 && green == 184 && blue == 197) ||
			(red == 172 && green == 180 && blue == 190) ||
			(red == 172 && green == 181 && blue == 191) ||
			(red == 172 && green == 185 && blue == 198) ||
			(red == 173 && green == 181 && blue == 191) ||
			(red == 173 && green == 182 && blue == 191) ||
			(red == 173 && green == 185 && blue == 198) ||
			(red == 173 && green == 186 && blue == 198) ||
			(red == 174 && green == 182 && blue == 191) ||
			(red == 174 && green == 182 && blue == 192) ||
			(red == 174 && green == 186 && blue == 199) ||
			(red == 174 && green == 187 && blue == 199) ||
			(red == 174 && green == 187 && blue == 200) ||
			(red == 175 && green == 183 && blue == 192) ||
			(red == 175 && green == 183 && blue == 193) ||
			(red == 175 && green == 187 && blue == 200) ||
			(red == 175 && green == 188 && blue == 200) ||
			(red == 175 && green == 188 && blue == 201) ||
			(red == 175 && green == 209 && blue == 237) ||
			(red == 176 && green == 184 && blue == 193) ||
			(red == 176 && green == 189 && blue == 201) ||
			(red == 176 && green == 210 && blue == 239) ||
			(red == 177 && green == 185 && blue == 194) ||
			(red == 177 && green == 185 && blue == 195) ||
			(red == 177 && green == 189 && blue == 202) ||
			(red == 177 && green == 190 && blue == 202) ||
			(red == 177 && green == 190 && blue == 203) ||
			(red == 177 && green == 197 && blue == 217) ||
			(red == 178 && green == 185 && blue == 194) ||
			(red == 178 && green == 186 && blue == 195) ||
			(red == 178 && green == 212 && blue == 241) ||
			(red == 179 && green == 186 && blue == 195) ||
			(red == 179 && green == 186 && blue == 196) ||
			(red == 179 && green == 187 && blue == 195) ||
			(red == 179 && green == 191 && blue == 203) ||
			(red == 179 && green == 191 && blue == 204) ||
			(red == 179 && green == 199 && blue == 218) ||
			(red == 180 && green == 187 && blue == 196) ||
			(red == 180 && green == 188 && blue == 197) ||
			(red == 180 && green == 192 && blue == 205) ||
			(red == 180 && green == 214 && blue == 243) ||
			(red == 181 && green == 188 && blue == 197) ||
			(red == 181 && green == 188 && blue == 198) ||
			(red == 181 && green == 189 && blue == 198) ||
			(red == 181 && green == 192 && blue == 205) ||
			(red == 181 && green == 193 && blue == 205) ||
			(red == 181 && green == 193 && blue == 206) ||
			(red == 181 && green == 215 && blue == 244) ||
			(red == 182 && green == 189 && blue == 198) ||
			(red == 182 && green == 190 && blue == 198) ||
			(red == 182 && green == 194 && blue == 206) ||
			(red == 182 && green == 216 && blue == 245) ||
			(red == 183 && green == 190 && blue == 198) ||
			(red == 183 && green == 190 && blue == 199) ||
			(red == 183 && green == 195 && blue == 207) ||
			(red == 183 && green == 196 && blue == 207) ||
			(red == 183 && green == 203 && blue == 222) ||
			(red == 183 && green == 218 && blue == 247) ||
			(red == 184 && green == 191 && blue == 199) ||
			(red == 184 && green == 191 && blue == 200) ||
			(red == 184 && green == 204 && blue == 223) ||
			(red == 184 && green == 218 && blue == 247) ||
			(red == 184 && green == 219 && blue == 247) ||
			(red == 185 && green == 192 && blue == 200) ||
			(red == 185 && green == 192 && blue == 201) ||
			(red == 185 && green == 196 && blue == 208) ||
			(red == 185 && green == 197 && blue == 208) ||
			(red == 186 && green == 192 && blue == 201) ||
			(red == 186 && green == 193 && blue == 201) ||
			(red == 186 && green == 193 && blue == 202) ||
			(red == 186 && green == 198 && blue == 209) ||
			(red == 186 && green == 206 && blue == 225) ||
			(red == 186 && green == 221 && blue == 250) ||
			(red == 187 && green == 193 && blue == 201) ||
			(red == 187 && green == 193 && blue == 202) ||
			(red == 187 && green == 194 && blue == 202) ||
			(red == 187 && green == 198 && blue == 210) ||
			(red == 187 && green == 199 && blue == 210) ||
			(red == 187 && green == 199 && blue == 211) ||
			(red == 187 && green == 207 && blue == 226) ||
			(red == 187 && green == 207 && blue == 227) ||
			(red == 187 && green == 222 && blue == 250) ||
			(red == 188 && green == 195 && blue == 203) ||
			(red == 188 && green == 199 && blue == 211) ||
			(red == 188 && green == 200 && blue == 211) ||
			(red == 188 && green == 208 && blue == 227) ||
			(red == 189 && green == 195 && blue == 203) ||
			(red == 189 && green == 196 && blue == 203) ||
			(red == 189 && green == 196 && blue == 204) ||
			(red == 189 && green == 200 && blue == 212) ||
			(red == 189 && green == 209 && blue == 228) ||
			(red == 190 && green == 196 && blue == 204) ||
			(red == 190 && green == 197 && blue == 205) ||
			(red == 190 && green == 202 && blue == 213) ||
			(red == 190 && green == 209 && blue == 229) ||
			(red == 190 && green == 210 && blue == 229) ||
			(red == 191 && green == 197 && blue == 205) ||
			(red == 191 && green == 198 && blue == 205) ||
			(red == 191 && green == 198 && blue == 212) ||
			(red == 191 && green == 198 && blue == 213) ||
			(red == 191 && green == 202 && blue == 213) ||
			(red == 191 && green == 202 && blue == 214) ||
			(red == 191 && green == 211 && blue == 230) ||
			(red == 192 && green == 198 && blue == 206) ||
			(red == 192 && green == 199 && blue == 206) ||
			(red == 192 && green == 199 && blue == 207) ||
			(red == 192 && green == 199 && blue == 212) ||
			(red == 192 && green == 199 && blue == 213) ||
			(red == 192 && green == 203 && blue == 214) ||
			(red == 192 && green == 203 && blue == 215) ||
			(red == 192 && green == 212 && blue == 231) ||
			(red == 192 && green == 212 && blue == 232) ||
			(red == 193 && green == 199 && blue == 206) ||
			(red == 193 && green == 199 && blue == 207) ||
			(red == 193 && green == 200 && blue == 214) ||
			(red == 193 && green == 204 && blue == 215) ||
			(red == 193 && green == 213 && blue == 232) ||
			(red == 194 && green == 200 && blue == 207) ||
			(red == 194 && green == 205 && blue == 216) ||
			(red == 194 && green == 214 && blue == 233) ||
			(red == 195 && green == 201 && blue == 208) ||
			(red == 195 && green == 206 && blue == 217) ||
			(red == 196 && green == 202 && blue == 209) ||
			(red == 196 && green == 207 && blue == 218) ||
			(red == 196 && green == 208 && blue == 219) ||
			(red == 197 && green == 203 && blue == 210) ||
			(red == 197 && green == 207 && blue == 218) ||
			(red == 197 && green == 208 && blue == 218) ||
			(red == 197 && green == 208 && blue == 219) ||
			(red == 198 && green == 203 && blue == 210) ||
			(red == 198 && green == 204 && blue == 210) ||
			(red == 198 && green == 204 && blue == 211) ||
			(red == 198 && green == 209 && blue == 220) ||
			(red == 199 && green == 204 && blue == 211) ||
			(red == 199 && green == 205 && blue == 211) ||
			(red == 199 && green == 205 && blue == 218) ||
			(red == 199 && green == 210 && blue == 220) ||
			(red == 199 && green == 210 && blue == 221) ||
			(red == 200 && green == 205 && blue == 212) ||
			(red == 200 && green == 206 && blue == 213) ||
			(red == 200 && green == 211 && blue == 221) ||
			(red == 200 && green == 211 && blue == 222) ||
			(red == 201 && green == 206 && blue == 212) ||
			(red == 201 && green == 206 && blue == 213) ||
			(red == 201 && green == 207 && blue == 213) ||
			(red == 201 && green == 212 && blue == 222) ||
			(red == 202 && green == 207 && blue == 213) ||
			(red == 202 && green == 207 && blue == 214) ||
			(red == 202 && green == 208 && blue == 214) ||
			(red == 202 && green == 212 && blue == 223) ||
			(red == 202 && green == 213 && blue == 223) ||
			(red == 203 && green == 208 && blue == 214) ||
			(red == 203 && green == 209 && blue == 220) ||
			(red == 203 && green == 211 && blue == 223) ||
			(red == 203 && green == 213 && blue == 223) ||
			(red == 203 && green == 213 && blue == 224) ||
			(red == 203 && green == 214 && blue == 224) ||
			(red == 204 && green == 208 && blue == 214) ||
			(red == 204 && green == 209 && blue == 215) ||
			(red == 204 && green == 209 && blue == 216) ||
			(red == 204 && green == 209 && blue == 220) ||
			(red == 204 && green == 210 && blue == 216) ||
			(red == 204 && green == 211 && blue == 223) ||
			(red == 204 && green == 215 && blue == 225) ||
			(red == 205 && green == 210 && blue == 216) ||
			(red == 205 && green == 215 && blue == 225) ||
			(red == 205 && green == 216 && blue == 226) ||
			(red == 206 && green == 210 && blue == 216) ||
			(red == 206 && green == 211 && blue == 216) ||
			(red == 206 && green == 216 && blue == 226) ||
			(red == 206 && green == 217 && blue == 226) ||
			(red == 206 && green == 217 && blue == 227) ||
			(red == 207 && green == 211 && blue == 217) ||
			(red == 207 && green == 212 && blue == 217) ||
			(red == 207 && green == 212 && blue == 218) ||
			(red == 207 && green == 218 && blue == 227) ||
			(red == 208 && green == 212 && blue == 218) ||
			(red == 208 && green == 213 && blue == 218) ||
			(red == 208 && green == 213 && blue == 219) ||
			(red == 208 && green == 218 && blue == 228) ||
			(red == 208 && green == 219 && blue == 228) ||
			(red == 209 && green == 213 && blue == 219) ||
			(red == 209 && green == 214 && blue == 219) ||
			(red == 209 && green == 219 && blue == 229) ||
			(red == 210 && green == 214 && blue == 219) ||
			(red == 210 && green == 214 && blue == 220) ||
			(red == 210 && green == 215 && blue == 220) ||
			(red == 210 && green == 215 && blue == 224) ||
			(red == 210 && green == 218 && blue == 229) ||
			(red == 210 && green == 220 && blue == 229) ||
			(red == 210 && green == 220 && blue == 230) ||
			(red == 210 && green == 221 && blue == 230) ||
			(red == 211 && green == 215 && blue == 220) ||
			(red == 211 && green == 215 && blue == 221) ||
			(red == 211 && green == 216 && blue == 221) ||
			(red == 211 && green == 221 && blue == 230) ||
			(red == 212 && green == 216 && blue == 221) ||
			(red == 212 && green == 216 && blue == 222) ||
			(red == 212 && green == 217 && blue == 222) ||
			(red == 212 && green == 222 && blue == 231) ||
			(red == 212 && green == 222 && blue == 232) ||
			(red == 213 && green == 217 && blue == 222) ||
			(red == 213 && green == 217 && blue == 223) ||
			(red == 213 && green == 218 && blue == 223) ||
			(red == 213 && green == 223 && blue == 232) ||
			(red == 214 && green == 218 && blue == 223) ||
			(red == 214 && green == 218 && blue == 227) ||
			(red == 214 && green == 219 && blue == 224) ||
			(red == 214 && green == 224 && blue == 233) ||
			(red == 215 && green == 219 && blue == 223) ||
			(red == 215 && green == 219 && blue == 224) ||
			(red == 215 && green == 220 && blue == 224) ||
			(red == 215 && green == 225 && blue == 234) ||
			(red == 216 && green == 220 && blue == 224) ||
			(red == 216 && green == 220 && blue == 225) ||
			(red == 216 && green == 226 && blue == 235) ||
			(red == 217 && green == 220 && blue == 225) ||
			(red == 217 && green == 221 && blue == 225) ||
			(red == 217 && green == 221 && blue == 226) ||
			(red == 217 && green == 226 && blue == 235) ||
			(red == 217 && green == 227 && blue == 236) ||
			(red == 218 && green == 221 && blue == 226) ||
			(red == 218 && green == 222 && blue == 226) ||
			(red == 218 && green == 227 && blue == 236) ||
			(red == 218 && green == 228 && blue == 236) ||
			(red == 218 && green == 228 && blue == 237) ||
			(red == 219 && green == 223 && blue == 227) ||
			(red == 219 && green == 228 && blue == 237) ||
			(red == 219 && green == 229 && blue == 237) ||
			(red == 219 && green == 229 && blue == 238) ||
			(red == 220 && green == 223 && blue == 227) ||
			(red == 220 && green == 223 && blue == 228) ||
			(red == 220 && green == 224 && blue == 228) ||
			(red == 220 && green == 229 && blue == 238) ||
			(red == 220 && green == 230 && blue == 238) ||
			(red == 221 && green == 224 && blue == 228) ||
			(red == 221 && green == 225 && blue == 229) ||
			(red == 221 && green == 230 && blue == 239) ||
			(red == 221 && green == 231 && blue == 239) ||
			(red == 222 && green == 225 && blue == 229) ||
			(red == 222 && green == 226 && blue == 229) ||
			(red == 222 && green == 226 && blue == 230) ||
			(red == 222 && green == 231 && blue == 239) ||
			(red == 222 && green == 231 && blue == 240) ||
			(red == 223 && green == 226 && blue == 230) ||
			(red == 223 && green == 232 && blue == 240) ||
			(red == 223 && green == 232 && blue == 241) ||
			(red == 224 && green == 227 && blue == 230) ||
			(red == 224 && green == 227 && blue == 231) ||
			(red == 224 && green == 228 && blue == 231) ||
			(red == 224 && green == 233 && blue == 241) ||
			(red == 224 && green == 234 && blue == 242) ||
			(red == 225 && green == 228 && blue == 231) || 
			(red == 225 && green == 228 && blue == 232) ||
			(red == 225 && green == 229 && blue == 232) ||
			(red == 225 && green == 229 && blue == 235) ||
			(red == 225 && green == 234 && blue == 242) ||
			(red == 225 && green == 234 && blue == 243) ||
			(red == 225 && green == 235 && blue == 243) ||
			(red == 226 && green == 229 && blue == 232) ||
			(red == 227 && green == 230 && blue == 233) ||
			(red == 228 && green == 230 && blue == 233) ||
			(red == 228 && green == 230 && blue == 234) ||
			(red == 228 && green == 231 && blue == 234) ||
			(red == 229 && green == 231 && blue == 234) ||
			(red == 229 && green == 232 && blue == 235) ||
			(red == 230 && green == 232 && blue == 235) ||
			(red == 230 && green == 233 && blue == 235) ||
			(red == 230 && green == 233 && blue == 236) ||
			(red == 231 && green == 233 && blue == 236) ||
			(red == 231 && green == 234 && blue == 236) ||
			(red == 231 && green == 234 && blue == 237) ||
			(red == 232 && green == 234 && blue == 237) ||
			(red == 233 && green == 235 && blue == 238) ||
			(red == 234 && green == 236 && blue == 238) ||
			(red == 234 && green == 236 && blue == 239) ||
			(red == 235 && green == 237 && blue == 239) ||
			(red == 235 && green == 237 && blue == 240) ||
			(red == 236 && green == 238 && blue == 240) ||
			(red == 236 && green == 238 && blue == 241) ||
			(red == 237 && green == 238 && blue == 240) ||
			(red == 237 && green == 239 && blue == 241) ||
			(red == 238 && green == 239 && blue == 241) ||
			(red == 238 && green == 239 && blue == 242) ||
			(red == 238 && green == 240 && blue == 242) ||
			(red == 239 && green == 240 && blue == 242) ||
			(red == 240 && green == 241 && blue == 243) ||
			(red == 240 && green == 242 && blue == 243) ||
			(red == 240 && green == 242 && blue == 244) ||
			(red == 241 && green == 242 && blue == 244) ||
			(red == 241 && green == 243 && blue == 244) ||
			(red == 242 && green == 243 && blue == 245) ||
			(red == 242 && green == 244 && blue == 245) ||
			(red == 243 && green == 244 && blue == 245) ||
			(red == 243 && green == 244 && blue == 246) ||
			(red == 243 && green == 245 && blue == 246) ||
			(red == 244 && green == 245 && blue == 246) ||
			(red == 244 && green == 245 && blue == 247) ||
			(red == 246 && green == 247 && blue == 248) ||
			(red == 247 && green == 247 && blue == 248) ||
			(red == 247 && green == 248 && blue == 249) ||
			(red == 248 && green == 248 && blue == 249) ||
			(red == 248 && green == 249 && blue == 249) ||
			(red == 248 && green == 249 && blue == 250) ||
			(red == 249 && green == 249 && blue == 250) ||
			(red == 249 && green == 250 && blue == 250) ||
			(red == 249 && green == 250 && blue == 251) ||
			(red == 250 && green == 250 && blue == 251) ||
			(red == 250 && green == 251 && blue == 251) ||
			(red == 251 && green == 251 && blue == 252) ||
			(red == 251 && green == 252 && blue == 252) ||
			(red == 252 && green == 252 && blue == 252) ||
			(red == 252 && green == 252 && blue == 253) ||
			(red == 252 && green == 253 && blue == 253) ||
			(red == 253 && green == 253 && blue == 253) ||
			(red == 253 && green == 253 && blue == 254) ||
			(red == 253 && green == 254 && blue == 254) ||
			(red == 254 && green == 254 && blue == 254) ||
			(red == 254 && green == 254 && blue == 255)*/)
			return 1;

		else if ((red == 229 && green == 92 && blue == 108) ||
				 (red == 229 && green == 93 && blue == 109) ||
				 (red == 229 && green == 94 && blue == 110) ||
				 (red == 229 && green == 95 && blue == 110) ||
				 (red == 229 && green == 95 && blue == 111) ||
				 (red == 229 && green == 96 && blue == 111) ||
				 (red == 230 && green == 95 && blue == 111) ||
				 (red == 230 && green == 96 && blue == 111)/* ||
				 (red == 230 && green == 96 && blue == 112) ||
				 (red == 230 && green == 97 && blue == 112) ||
				 (red == 230 && green == 97 && blue == 113) ||
				 (red == 230 && green == 98 && blue == 113) ||
				 (red == 230 && green == 98 && blue == 114) ||
				 (red == 230 && green == 99 && blue == 114) ||
				 (red == 230 && green == 99 && blue == 115) || 
				 (red == 101 && green == 106 && blue == 126) ||
				 (red == 121 && green == 135 && blue == 154) ||
				 (red == 122 && green == 135 && blue == 154) ||
				 (red == 123 && green == 135 && blue == 154) ||
				 (red == 132 && green == 136 && blue == 155) ||
				 (red == 137 && green == 137 && blue == 155) ||
				 (red == 149 && green == 139 && blue == 155) ||
				 (red == 187 && green == 220 && blue == 249) ||
				 (red == 187 && green == 221 && blue == 250) ||
				 (red == 187 && green == 222 && blue == 250) ||
				 (red == 188 && green == 218 && blue == 246) ||
				 (red == 189 && green == 215 && blue == 243) ||
				 (red == 189 && green == 216 && blue == 244) ||
				 (red == 190 && green == 212 && blue == 240) ||
				 (red == 191 && green == 198 && blue == 212) ||
				 (red == 192 && green == 198 && blue == 212) ||
				 (red == 193 && green == 198 && blue == 212) ||
				 (red == 193 && green == 201 && blue == 228) ||
				 (red == 195 && green == 197 && blue == 224) ||
				 (red == 197 && green == 191 && blue == 218) ||
				 (red == 198 && green == 188 && blue == 213) ||
				 (red == 198 && green == 199 && blue == 212) ||
				 (red == 202 && green == 199 && blue == 212) ||
				 (red == 202 && green == 200 && blue == 212) ||
				 (red == 204 && green == 171 && blue == 195) ||
				 (red == 205 && green == 165 && blue == 188) ||
				 (red == 208 && green == 154 && blue == 176) ||
				 (red == 208 && green == 157 && blue == 180) ||
				 (red == 208 && green == 201 && blue == 212) ||
				 (red == 211 && green == 201 && blue == 213) ||
				 (red == 220 && green == 120 && blue == 139) ||
				 (red == 221 && green == 203 && blue == 213) ||
				 (red == 222 && green == 115 && blue == 133) ||
				 (red == 223 && green == 112 && blue == 130) ||
				 (red == 225 && green == 104 && blue == 121) ||
				 (red == 228 && green == 95 && blue == 111) ||
				 (red == 228 && green == 95 && blue == 112) ||
				 (red == 230 && green == 100 && blue == 115) ||
				 (red == 230 && green == 101 && blue == 116) ||
				 (red == 230 && green == 101 && blue == 117) ||
				 (red == 231 && green == 102 && blue == 117) ||
				 (red == 231 && green == 102 && blue == 118) ||
				 (red == 231 && green == 103 && blue == 118) ||
				 (red == 231 && green == 103 && blue == 119) ||
				 (red == 231 && green == 104 && blue == 119) ||
				 (red == 231 && green == 105 && blue == 120) ||
				 (red == 231 && green == 106 && blue == 121) ||
				 (red == 231 && green == 107 && blue == 121) ||
				 (red == 231 && green == 107 && blue == 122) ||
				 (red == 231 && green == 108 && blue == 122) ||
				 (red == 232 && green == 107 && blue == 122) ||
				 (red == 232 && green == 108 && blue == 123) ||
				 (red == 232 && green == 109 && blue == 124) ||
				 (red == 232 && green == 110 && blue == 124) ||
				 (red == 232 && green == 110 && blue == 125) ||
				 (red == 232 && green == 111 && blue == 126) ||
				 (red == 232 && green == 112 && blue == 126) ||
				 (red == 232 && green == 112 && blue == 127) ||
				 (red == 232 && green == 113 && blue == 127) ||
				 (red == 232 && green == 113 && blue == 128) ||
				 (red == 232 && green == 114 && blue == 128) ||
				 (red == 232 && green == 114 && blue == 129) ||
				 (red == 232 && green == 115 && blue == 129) ||
				 (red == 233 && green == 114 && blue == 128) ||
				 (red == 233 && green == 114 && blue == 129) ||
				 (red == 233 && green == 115 && blue == 129) ||
				 (red == 233 && green == 115 && blue == 130) ||
				 (red == 233 && green == 116 && blue == 130) ||
				 (red == 233 && green == 117 && blue == 131) ||
				 (red == 233 && green == 117 && blue == 132) ||
				 (red == 233 && green == 118 && blue == 132) ||
				 (red == 233 && green == 119 && blue == 133) ||
				 (red == 233 && green == 120 && blue == 133) ||
				 (red == 233 && green == 120 && blue == 134) ||
				 (red == 233 && green == 121 && blue == 134) ||
				 (red == 233 && green == 121 && blue == 135) ||
				 (red == 234 && green == 120 && blue == 134) ||
				 (red == 234 && green == 121 && blue == 134) ||
				 (red == 234 && green == 121 && blue == 135) ||
				 (red == 234 && green == 122 && blue == 135) ||
				 (red == 234 && green == 122 && blue == 136) ||
				 (red == 234 && green == 123 && blue == 136) ||
				 (red == 234 && green == 123 && blue == 137) ||
				 (red == 234 && green == 124 && blue == 137) ||
				 (red == 234 && green == 124 && blue == 138) ||
				 (red == 234 && green == 125 && blue == 138) ||
				 (red == 234 && green == 125 && blue == 139) ||
				 (red == 234 && green == 126 && blue == 139) ||
				 (red == 234 && green == 127 && blue == 140) ||
				 (red == 235 && green == 126 && blue == 140) ||
				 (red == 235 && green == 127 && blue == 140) ||
				 (red == 235 && green == 127 && blue == 141) ||
				 (red == 235 && green == 128 && blue == 141) ||
				 (red == 235 && green == 129 && blue == 142) ||
				 (red == 235 && green == 129 && blue == 143) ||
				 (red == 235 && green == 130 && blue == 143) ||
				 (red == 235 && green == 131 && blue == 143) ||
				 (red == 235 && green == 131 && blue == 144) ||
				 (red == 235 && green == 132 && blue == 144) ||
				 (red == 235 && green == 132 && blue == 145) ||
				 (red == 235 && green == 133 && blue == 145) ||
				 (red == 235 && green == 133 && blue == 146) ||
				 (red == 235 && green == 134 && blue == 147) ||
				 (red == 236 && green == 133 && blue == 146) ||
				 (red == 236 && green == 134 && blue == 147) ||
				 (red == 236 && green == 135 && blue == 148) ||
				 (red == 236 && green == 136 && blue == 148) ||
				 (red == 236 && green == 136 && blue == 149) ||
				 (red == 236 && green == 137 && blue == 149) ||
				 (red == 236 && green == 137 && blue == 150) ||
				 (red == 236 && green == 138 && blue == 150) ||
				 (red == 236 && green == 138 && blue == 151) ||
				 (red == 236 && green == 139 && blue == 151) ||
				 (red == 236 && green == 139 && blue == 152) ||
				 (red == 236 && green == 140 && blue == 152) ||
				 (red == 236 && green == 141 && blue == 153) ||
				 (red == 237 && green == 141 && blue == 153) ||
				 (red == 237 && green == 141 && blue == 154) ||
				 (red == 237 && green == 142 && blue == 154) ||
				 (red == 237 && green == 143 && blue == 155) ||
				 (red == 237 && green == 144 && blue == 156) ||
				 (red == 237 && green == 145 && blue == 157) ||
				 (red == 237 && green == 146 && blue == 157) ||
				 (red == 237 && green == 146 && blue == 158) ||
				 (red == 237 && green == 147 && blue == 158) ||
				 (red == 237 && green == 147 && blue == 159) ||
				 (red == 237 && green == 148 && blue == 160) ||
				 (red == 238 && green == 146 && blue == 158) ||
				 (red == 238 && green == 147 && blue == 159) ||
				 (red == 238 && green == 148 && blue == 159) ||
				 (red == 238 && green == 148 && blue == 160) ||
				 (red == 238 && green == 149 && blue == 160) ||
				 (red == 238 && green == 149 && blue == 161) ||
				 (red == 238 && green == 150 && blue == 161) ||
				 (red == 238 && green == 150 && blue == 162) ||
				 (red == 238 && green == 151 && blue == 162) ||
				 (red == 238 && green == 151 && blue == 163) ||
				 (red == 238 && green == 152 && blue == 163) ||
				 (red == 238 && green == 152 && blue == 164) ||
				 (red == 238 && green == 153 && blue == 164) ||
				 (red == 239 && green == 152 && blue == 163) ||
				 (red == 239 && green == 153 && blue == 164) ||
				 (red == 239 && green == 154 && blue == 165) ||
				 (red == 239 && green == 155 && blue == 166) ||
				 (red == 239 && green == 156 && blue == 166) ||
				 (red == 239 && green == 156 && blue == 167) ||
				 (red == 239 && green == 157 && blue == 167) ||
				 (red == 239 && green == 157 && blue == 168) ||
				 (red == 239 && green == 158 && blue == 168) ||
				 (red == 239 && green == 158 && blue == 169) ||
				 (red == 239 && green == 159 && blue == 170) ||
				 (red == 239 && green == 160 && blue == 171) ||
				 (red == 239 && green == 161 && blue == 171) ||
				 (red == 239 && green == 161 && blue == 172) ||
				 (red == 240 && green == 159 && blue == 170) ||
				 (red == 240 && green == 160 && blue == 170) ||
				 (red == 240 && green == 160 && blue == 171) ||
				 (red == 240 && green == 161 && blue == 171) ||
				 (red == 240 && green == 161 && blue == 172) ||
				 (red == 240 && green == 162 && blue == 172) ||
				 (red == 240 && green == 162 && blue == 173) ||
				 (red == 240 && green == 163 && blue == 173) ||
				 (red == 240 && green == 163 && blue == 174) ||
				 (red == 240 && green == 164 && blue == 174) ||
				 (red == 240 && green == 164 && blue == 175) ||
				 (red == 240 && green == 165 && blue == 175) ||
				 (red == 240 && green == 166 && blue == 176) ||
				 (red == 241 && green == 165 && blue == 175) ||
				 (red == 241 && green == 166 && blue == 176) ||
				 (red == 241 && green == 166 && blue == 177) ||
				 (red == 241 && green == 167 && blue == 177) ||
				 (red == 241 && green == 167 && blue == 178) ||
				 (red == 241 && green == 168 && blue == 178) ||
				 (red == 241 && green == 169 && blue == 178) ||
				 (red == 241 && green == 169 && blue == 179) ||
				 (red == 241 && green == 170 && blue == 180) ||
				 (red == 241 && green == 171 && blue == 180) ||
				 (red == 241 && green == 171 && blue == 181) ||
				 (red == 241 && green == 172 && blue == 181) ||
				 (red == 241 && green == 172 && blue == 182) ||
				 (red == 241 && green == 173 && blue == 182) ||
				 (red == 241 && green == 173 && blue == 183) ||
				 (red == 241 && green == 174 && blue == 184) ||
				 (red == 242 && green == 172 && blue == 182) ||
				 (red == 242 && green == 173 && blue == 183) ||
				 (red == 242 && green == 174 && blue == 183) ||
				 (red == 242 && green == 174 && blue == 184) ||
				 (red == 242 && green == 175 && blue == 184) ||
				 (red == 242 && green == 175 && blue == 185) ||
				 (red == 242 && green == 176 && blue == 185) ||
				 (red == 242 && green == 176 && blue == 186) ||
				 (red == 242 && green == 177 && blue == 186) ||
				 (red == 242 && green == 177 && blue == 187) ||
				 (red == 242 && green == 178 && blue == 187) ||
				 (red == 242 && green == 178 && blue == 188) ||
				 (red == 242 && green == 179 && blue == 188) ||
				 (red == 242 && green == 180 && blue == 188) ||
				 (red == 242 && green == 180 && blue == 189) ||
				 (red == 242 && green == 181 && blue == 190) ||
				 (red == 243 && green == 178 && blue == 187) ||
				 (red == 243 && green == 178 && blue == 188) ||
				 (red == 243 && green == 179 && blue == 188) ||
				 (red == 243 && green == 179 && blue == 189) ||
				 (red == 243 && green == 180 && blue == 189) ||
				 (red == 243 && green == 181 && blue == 189) ||
				 (red == 243 && green == 181 && blue == 190) ||
				 (red == 243 && green == 181 && blue == 191) ||
				 (red == 243 && green == 182 && blue == 190) ||
				 (red == 243 && green == 182 && blue == 191) ||
				 (red == 243 && green == 183 && blue == 191) ||
				 (red == 243 && green == 183 && blue == 192) ||
				 (red == 243 && green == 184 && blue == 192) ||
				 (red == 243 && green == 184 && blue == 193) ||
				 (red == 243 && green == 185 && blue == 193) ||
				 (red == 243 && green == 185 && blue == 194) ||
				 (red == 243 && green == 186 && blue == 194) ||
				 (red == 243 && green == 187 && blue == 195) ||
				 (red == 244 && green == 185 && blue == 194) ||
				 (red == 244 && green == 186 && blue == 194) ||
				 (red == 244 && green == 186 && blue == 195) ||
				 (red == 244 && green == 187 && blue == 195) ||
				 (red == 244 && green == 187 && blue == 196) ||
				 (red == 244 && green == 188 && blue == 196) ||
				 (red == 244 && green == 188 && blue == 197) ||
				 (red == 244 && green == 189 && blue == 197) ||
				 (red == 244 && green == 189 && blue == 198) ||
				 (red == 244 && green == 190 && blue == 198) ||
				 (red == 244 && green == 191 && blue == 199) ||
				 (red == 244 && green == 192 && blue == 200) ||
				 (red == 245 && green == 192 && blue == 200) ||
				 (red == 245 && green == 192 && blue == 201) ||
				 (red == 245 && green == 193 && blue == 201) ||
				 (red == 245 && green == 194 && blue == 202) ||
				 (red == 245 && green == 195 && blue == 203) ||
				 (red == 245 && green == 196 && blue == 203) ||
				 (red == 245 && green == 196 && blue == 204) ||
				 (red == 245 && green == 197 && blue == 204) ||
				 (red == 245 && green == 197 && blue == 205) ||
				 (red == 245 && green == 198 && blue == 205) ||
				 (red == 245 && green == 198 && blue == 206) ||
				 (red == 246 && green == 198 && blue == 206) ||
				 (red == 246 && green == 199 && blue == 207) ||
				 (red == 246 && green == 200 && blue == 207) ||
				 (red == 246 && green == 200 && blue == 208) ||
				 (red == 246 && green == 201 && blue == 208) ||
				 (red == 246 && green == 201 && blue == 209) ||
				 (red == 246 && green == 202 && blue == 209) ||
				 (red == 246 && green == 202 && blue == 210) ||
				 (red == 246 && green == 203 && blue == 210) ||
				 (red == 246 && green == 203 && blue == 211) ||
				 (red == 246 && green == 204 && blue == 211) ||
				 (red == 246 && green == 207 && blue == 214) ||
				 (red == 247 && green == 204 && blue == 211) ||
				 (red == 247 && green == 204 && blue == 212) ||
				 (red == 247 && green == 205 && blue == 212) ||
				 (red == 247 && green == 206 && blue == 213)*/)
			return 2;
		return 0;
	}
	auto IsGameXY(int startX, int startY, int width, int height)
	{
		int countX = 0;
		for (int x = startX; x < width; x++)
			if (IsGameColor(GetPixelColor(hDC, std::make_tuple(x, startY))) == 1)
				countX++;
			else
				break;
		int countY = 0;
		for (int y = startY; y < height; y++)
			if (IsGameColor(GetPixelColor(hDC, std::make_tuple(startX, y))) == 1)
				countY++;
			else
				break;

		return std::make_tuple(countX, countY);
	}
	auto GetStartAndEndPoint(int width, int height)
	{
		std::tuple<int, int> startPoint;
		std::tuple<int, int> endPoint;
		bool hasFound = false;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if (IsGameColor(GetPixelColor(hDC, std::make_tuple(x, y))) == 1)
				{
					std::tuple<int, int> check = IsGameXY(x, y, width, height);
					if (std::get<0>(check) > 200 && std::get<1>(check) > 200)
					{
						startPoint = std::make_tuple(x, y);
						endPoint = std::make_tuple(x + std::get<0>(check), y + std::get<1>(check));
						hasFound = true;
						break;
					}
				}
			}
			if (hasFound)
				break;
		}
		return std::make_tuple(startPoint, endPoint);
	}
	void GetAllNumberColors()
	{
		std::vector<std::tuple<int, int, int>> colors;
		for (int y = 0; y < height; y++)
			for (int x = 0; x < width; x++)
			{
				COLORREF color = GetPixelColor(hDC, std::make_tuple(x, y));
				int r = GetRValue(color);
				int g = GetGValue(color);
				int b = GetBValue(color);
				bool okay = true;
				//Border
				if (!(//(r == 153 && g == 163 && b == 176) ||
					  //(r == 52 && g == 72 && b == 97) ||
					  //(r == 0 && g == 44 && b == 78) ||
					  //(r == 98 && g == 130 && b == 157) ||
					  //(r == 115 && g == 127 && b == 149) ||
					  //Inner Border
					  (r == 198 && g == 195 && b == 204) ||
					  (r == 218 && g == 226 && b == 236) ||
					  //Background
					  (r == 196 && g == 218 && b == 240) ||
					  (r == 191 && g == 221 && b == 247) ||
					  (r == 230 && g == 224 && b == 254) ||
					  (r == 187 && g == 223 && b == 252) ||
					  (r == 187 && g == 222 && b == 251) ||
					  (r == 185 && g == 223 && b == 253) ||
					  (r == 184 && g == 224 && b == 255) ||
					  (r == 191 && g == 209 && b == 229) ||
					  (r == 226 && g == 202 && b == 209) ||
					  (r == 198 && g == 195 && b == 204) ||
					  (r == 255 && g == 209 && b == 214) ||
					  (r == 251 && g == 208 && b == 214) ||
					  (r == 255 && g == 255 && b == 255) ||
					  (r == 247 && g == 207 && b == 214)))
					colors.push_back(std::make_tuple(r, g, b));
			}

		std::sort(colors.begin(), colors.end());
		colors.erase(std::unique(colors.begin(), colors.end()), colors.end());

		for (std::tuple<int, int, int> col : colors)
			std::cout << std::get<0>(col) << "," << std::get<1>(col) << "," << std::get<2>(col) << std::endl;
	}
	std::vector<std::vector<bool>> GetGame()
	{
		if (matchColor == 2)
			GetScreenshot(width, height, std::make_tuple(startX, startY));
		width = bitmap.bmWidth;
		height = bitmap.bmHeight;

		auto startAndEndPoint = GetStartAndEndPoint(width, height);
		width = std::get<0>(std::get<1>(startAndEndPoint)) - std::get<0>(std::get<0>(startAndEndPoint));
		height = std::get<1>(std::get<1>(startAndEndPoint)) - std::get<1>(std::get<0>(startAndEndPoint));
		startX += std::get<0>(std::get<0>(startAndEndPoint));
		startY += std::get<1>(std::get<0>(startAndEndPoint));

		//Solving
		GetScreenshot(width, height, std::make_tuple(startX, startY));
		//GetAllNumberColors();
		std::cout << "StartingPoint = " << startX << "," << startY << std::endl;
		std::cout << "Width/Height = " << width << "/" << height << std::endl;
		std::vector<std::vector<bool>> game;
		game.reserve(height);
		for (int y = 0; y < height; y++)
		{
			std::vector<bool> tempGame;
			tempGame.reserve(width);
			for (int x = 0; x < width; x++)
				tempGame.push_back(IsGameColor(GetPixelColor(hDC, std::make_tuple(x, y))) == matchColor);
			game.push_back(std::move(tempGame));
		}

			
		
		


		//Remove lines
		std::vector<std::vector<bool>> onlyGameNumbers;
		int removedX = 0;
		for (int y = 0; y < game.size(); y++)
		{
			int countX = 0;
			for (int x = 0; x < game[y].size(); x++)
				if (game[y][x])
					countX++;
			if (width / 2 > countX)
				onlyGameNumbers.push_back(game[y]);
			else
				removedX++;
		}
		for (int x = 0; x < onlyGameNumbers[0].size(); x++)
		{
			int countY = 0;
			for (int y = 0; y < onlyGameNumbers.size(); y++)
				if (onlyGameNumbers[y][x])
					countY++;

			if (onlyGameNumbers.size() / 1.2 - removedX < countY)
				for (int y = 0; y < onlyGameNumbers.size(); y++)
					onlyGameNumbers[y][x] = false;
		}
		

		return onlyGameNumbers;
	}
#pragma endregion
#pragma region GetGameCheckers
	POINT button{};
	std::tuple<std::tuple<int, int>, std::tuple<int, int>> startEndPoint;
	void Calibrate()
	{
		char test;
		std::cout << "Change these values, until you get an image with a white margin around, and the sudoku game in the middle" << std::endl;
		std::cout << "To see image go to a word document, and press 'CTRL V'" << std::endl;
		std::cout << "When youre ready, you will have 2 seconds to press on the sudoku table!" << std::endl;

		do
		{
			std::cin.ignore();
			std::string val;
			std::cout << "------------------------" << std::endl;
			std::cout << "Width = ";
			std::getline(std::cin, val); std::stringstream vWIDTH; vWIDTH << val; vWIDTH >> width;
			std::cout << "Height = ";
			std::getline(std::cin, val); std::stringstream vHEIGHT; vHEIGHT << val; vHEIGHT >> height;
			std::cout << "X = ";
			std::getline(std::cin, val); std::stringstream vStartX; vStartX << val; vStartX >> startX;
			std::cout << "Y = ";
			std::getline(std::cin, val); std::stringstream vStartY; vStartY << val; vStartY >> startY;
			TestScreenshot();
			std::cout << "Ready (y/n)? ";
			std::cin >> test;
			std::cout << std::endl;
			if (test == 'y')
			{
				test = 'n';
				system("cls");
				std::cout << "Hover the mouse over the 'Classic' button" << std::endl;
				do
				{
					std::cout << "Ready (y/n)? ";
					std::cin >> test;
					if (test == 'y')
						GetCursorPos(&button);
				} while (test == 'n');
			}
		} while (test == 'n');
		SetCursorPos(startX - 100, startY - 100);
		PressMouse(true);
		for (int i = 0; i < 9; i++)
		{
			std::vector<std::vector<std::tuple<std::string, int>>> innerList;
			for (int j = 0; j < 9; j++)
			{
				std::vector<std::tuple<std::string, int>> innerList2;
				innerList.push_back(innerList2);
			}
			MatchCodes.push_back(innerList);
		}

		int count = 1;
		matchColor = 2;
		TimeDelay = 1;
		//Making sure the users mouse is in the top left corner
		for (byte y = 0; y < 9; y++)
		{
			PressKey(VK_UP, INPUT_KEYBOARD);
			PressKey(VK_LEFT, INPUT_KEYBOARD);
		}
		for (byte i = 1; i <= 9; i++)
		{
			for (byte y = 0; y < 5; y++)
			{
				for (byte x = 0; x < 9; x++)
				{
					PressKey(0x30 + i, INPUT_KEYBOARD);
					PressKey(VK_RIGHT, INPUT_KEYBOARD);
				}
				if (y < 4)
				{
					PressKey(VK_DOWN, INPUT_KEYBOARD);
					for (byte x = 0; x < 9; x++)
					{
						PressKey(0x30 + i, INPUT_KEYBOARD);
						PressKey(VK_LEFT, INPUT_KEYBOARD);
					}
					PressKey(VK_DOWN, INPUT_KEYBOARD);
				}
			}
			for (byte y = 0; y < 9; y++)
			{
				PressKey(VK_LEFT, INPUT_KEYBOARD);
				PressKey(VK_UP, INPUT_KEYBOARD);
			}

			auto numberCode = GetIndividualNumbers();
			for (int y = 0; y < numberCode.size(); y++)
				for (int x = 0; x < numberCode[y].size(); x++)
					MatchCodes[y][x].push_back(std::make_tuple(numberCode[y][x], i));
			count++;
		}
	}
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
		std::vector<std::vector<int>> SudokuValues;
		matchColor = 1;
		while (true)
		{
			SetCursorPos(button.x, button.y);
			PressMouse(true);
			Sleep(800);
			system("cls");
			SudokuValues.clear();
#pragma region Solve & Print
			auto numberCode = GetIndividualNumbers();
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
			std::cout << std::endl;

			CalculateSudokuWithValues(SudokuValues);
#pragma endregion
			std::cout << std::endl << std::endl;

			Sleep(2000);
		}
	}
	std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> MatchCodes;
#pragma endregion
#pragma region Universal
	int width{};
	int height{};
	int startX{};
	int startY{};
	int TimeDelay = 0;
	void PressMouse(bool left)
	{
		INPUT input{};
		input.type = INPUT_MOUSE;
		if (left)
			input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		else
			input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		SendInput(1, &input, sizeof(input));
		ZeroMemory(&input, sizeof(input));
		input.type = INPUT_MOUSE;
		if (left)
			input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		else
			input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		SendInput(1, &input, sizeof(input));
	}
	void PressKey(WORD virtualKey, DWORD keyType)
	{
		//Very basic way of sending keys and mouse events to the computer
		if (virtualKey == 0)
			return;
		INPUT ip{};
		// ...
			// Set up a generic keyboard event.
		ip.type = keyType;
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
};
int main()
{
	ManipulateSudoku sudoku;
	sudoku.ExecuteOrder66();

	///Show
	//for (std::vector<bool> item : game)
	//{
	//	for (bool value : item)
	//		if (value)
	//			std::cout << "X";
	//		else
	//			std::cout << " ";
	//	std::cout << std::endl;
	//}
	///Show

}