#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
#include <tuple>
#include <sstream>

class ImageAnalyser
{
public:
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
};
class Sudoku
{
public:
	//Simple method to just check if everything was typed right
	void CalculateSudokuWithValues(std::vector<std::vector<int>> Grid)
	{
		grid = Grid;
		if (grid.size() >= 9)
			Solve();
	}
	int TimeDelay = 100;
private:
#pragma region Calculator
	//Set the time delay for each PressKey 0 is the fastest
	//The main grid that holds the numbers just like matrix
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
		SendInput(1, &ip, sizeof(INPUT));

		//Sleep for the desired time, can be changed by the user.
		Sleep(TimeDelay);
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
};
class GetGamePlate
{
private:
	ImageAnalyser imageAnalyser;
	int IsGameColor(COLORREF color)
	{
		int red = GetRValue(color);
		int green = GetGValue(color);
		int blue = GetBValue(color);

		if (red == 52 && green == 72 && blue == 97)
			return 1;
		else if (red == 229 && green == 92 && blue == 108)
			return 2;
		return 0;
	}
	auto IsGameXY(int startX, int startY, int width, int height)
	{
		int countX = 0;
		for (int x = startX; x < width; x++)
			if (IsGameColor(imageAnalyser.GetPixelColor(imageAnalyser.hDC, std::make_tuple(x, startY))) == 1)
				countX++;
			else
				break;
		int countY = 0;
		for (int y = startY; y < height; y++)
			if (IsGameColor(imageAnalyser.GetPixelColor(imageAnalyser.hDC, std::make_tuple(startX, y))) == 1)
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
				if (IsGameColor(imageAnalyser.GetPixelColor(imageAnalyser.hDC, std::make_tuple(x, y))) == 1)
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

	std::vector<std::vector<bool>> GetGame()
	{
		if (matchColor == 1)
			imageAnalyser.GetScreenshot(width, height, std::make_tuple(startX, startY));
		width = imageAnalyser.bitmap.bmWidth;
		height = imageAnalyser.bitmap.bmHeight;


		auto startAndEndPoint = GetStartAndEndPoint(width, height);
		width = std::get<0>(std::get<1>(startAndEndPoint)) - std::get<0>(std::get<0>(startAndEndPoint));
		height = std::get<1>(std::get<1>(startAndEndPoint)) - std::get<1>(std::get<0>(startAndEndPoint));
		startX += std::get<0>(std::get<0>(startAndEndPoint));
		startY += std::get<1>(std::get<0>(startAndEndPoint));

		//Solving
		imageAnalyser.GetScreenshot(width, height, std::make_tuple(startX, startY));
		std::vector<std::vector<bool>> game;
		for (int y = 0; y < height; y++)
		{
			std::vector<bool> tempGame;
			for (int x = 0; x < width; x++)
				tempGame.push_back(IsGameColor(imageAnalyser.GetPixelColor(imageAnalyser.hDC, std::make_tuple(x, y))) == matchColor);
			game.push_back(tempGame);
		}


		//Remove lines
		std::vector<std::vector<bool>> onlyGameNumbers;
		int countX;
		for (int y = 0; y < game.size(); y++)
		{
			countX = 0;
			for (int x = 0; x < game[y].size(); x++)
				if (game[y][x])
					countX++;
				else
					break;
			if (width > countX)
				onlyGameNumbers.push_back(game[y]);
		}
		int countY;
		for (int x = 0; x < onlyGameNumbers[0].size(); x++)
		{
			countY = 0;
			for (int y = 0; y < onlyGameNumbers.size(); y++)
				if (onlyGameNumbers[y][x])
					countY++;
			if (onlyGameNumbers.size() == countY)
				for (int y = 0; y < onlyGameNumbers.size(); y++)
					onlyGameNumbers[y][x] = false;
		}

		return onlyGameNumbers;
	}
public:
	int width{};
	int height{};
	int startX{};
	int startY{};
	void TestScreenshot()
	{
		imageAnalyser.GetScreenshot(width, height, std::make_tuple(startX, startY));
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
};
class GetGameCheckers
{
private:
	int WIDTH = 0;
	int HEIGHT = 0;
	int STARTX = 0;
	int STARTY = 0;
	POINT button{};
	float MatchPercentage = 0.10f;
	int TimeDelay = 10;
	std::tuple<std::tuple<int, int>, std::tuple<int, int>> startEndPoint;
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
	float GetMatchPercentage(std::string a, std::string b)
	{
		int count = 0;
		float returnValue;
		if (a.size() > b.size())
		{
			for (int i = 0; i < a.size(); i++)
				if (b.size() > i)
					if (a[i] == b[i])
						count++;
					else
						count--;
				else
					count--;
			returnValue = count / static_cast<float>(a.size());
		}
		else
		{
			for (int i = 0; i < b.size(); i++)
				if (a.size() > i)
					if (a[i] == b[i])
						count++;
					else
						count--;
				else
					count--;
			returnValue = count / static_cast<float>(b.size());
		}

		if (isinf(returnValue))
			return -1;
		return returnValue;

	}
	void Calibrate()
	{
		GetGamePlate getGamePlate;

		char test;
		std::cout << "Change these values, until you get an image with a white margin around, and the sudoku game in the middle" << std::endl;
		std::cout << "To see image go to a word document, and press 'CTRL V'" << std::endl;
		std::cout << "When youre ready, you will have 2 seconds to press on the sudoku table!" << std::endl;

		do
		{
			std::string val;
			std::cout << "------------------------" << std::endl;
			std::cout << "Width = ";
			std::getline(std::cin, val); std::stringstream vWIDTH; vWIDTH << val; vWIDTH >> WIDTH;
			std::cout << "Height = ";
			std::getline(std::cin, val); std::stringstream vHEIGHT; vHEIGHT << val; vHEIGHT >> HEIGHT;
			std::cout << "X = ";
			std::getline(std::cin, val); std::stringstream vSTARTX; vSTARTX << val; vSTARTX >> STARTX;
			std::cout << "Y = ";
			std::getline(std::cin, val); std::stringstream vSTARTY; vSTARTY << val; vSTARTY >> STARTY;
			getGamePlate.width = WIDTH;
			getGamePlate.height = HEIGHT;
			getGamePlate.startX = STARTX;
			getGamePlate.startY = STARTY;
			Sleep(3000);
			getGamePlate.TestScreenshot();
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
		SetCursorPos(STARTX - 100, STARTY - 100);
		PressMouse(true);
		Sleep(500);
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
		getGamePlate.matchColor = 2;
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

			auto numberCode = getGamePlate.GetIndividualNumbers();
			for (int y = 0; y < numberCode.size(); y++)
				for (int x = 0; x < numberCode[y].size(); x++)
					MatchCodes[y][x].push_back(std::make_tuple(numberCode[y][x], i));
			count++;
		}
	}
	void SolveSudoku()
	{
		Sudoku sudoku;
		sudoku.TimeDelay = 10;
		std::vector<std::vector<int>> SudokuValues;
		GetGamePlate getGamePlate;
		getGamePlate.width = WIDTH;
		getGamePlate.height = HEIGHT;
		getGamePlate.startX = STARTX;
		getGamePlate.startY = STARTY;
		getGamePlate.matchColor = 1;
		while (true)
		{
			SetCursorPos(button.x, button.y);
			PressMouse(true);
			Sleep(1000);
			system("cls");
			SudokuValues.clear();
#pragma region Solve & Print
			auto numberCode = getGamePlate.GetIndividualNumbers();
			for (int y = 0; y < MatchCodes.size(); y++)
			{
				std::vector<int> row;
				for (int x = 0; x < MatchCodes[y].size(); x++)
				{
					float oldNum = -1;
					int num = 0;
					for (std::tuple<std::string, int> item : MatchCodes[y][x])
					{
						if (numberCode[y][x] != "")
						{
							float testMatch = GetMatchPercentage(std::get<0>(item), numberCode[y][x]);
							//std::cout << std::get<1>(item) << ": " << std::get<0>(item) << " and " << numberCode[y][x] << " = " << testMatch << std::endl;
							if (testMatch > oldNum)
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

			sudoku.CalculateSudokuWithValues(SudokuValues);
#pragma endregion
			std::cout << std::endl << std::endl;

			Sleep(2000);
		}
	}
public:
	std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> MatchCodes;

	void ExecuteOrder()
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
};
int main()
{
	GetGameCheckers getGameCheckers;
	getGameCheckers.ExecuteOrder();
}