#include <iostream>
#include <windows.h>
#include <thread>
#include <cstdlib>
#include <string>
#include <chrono>
#include <algorithm>
#include <vector>
#include <array>
#include <bitset>

//Auto Solver
class SudokuAutoGameSolver
{
public: //Methods
	/// <summary>
	/// Execute the sudoku destroyer
	/// </summary>
	void ExecuteOrder66()
	{
		//Handling values, so that the user doesnt choose something that is not possible
		readSudokuSpeed = readSudokuSpeed > 9 ? 9 : readSudokuSpeed;
		int tempTimeDelay = inputDelay;
		inputDelay = inputDelay == 0 ? 1 : inputDelay;

		//Making sure the userr can stop at any time
		std::thread keyPressThread([&]
		{
			checkForKeyPress();
		});
		std::cout << "Go to https://sudoku.com/sudoku-solver" << std::endl;
		PromptForKeyAndMessage("Hold CTRL and left click on the link above", VK_LBUTTON, 2000);
		GetGameCopyPos();
		Calibrate();
		PromptForKeyAndMessage("Left click on 'Classic'", VK_LBUTTON, 1000);
		PromptForKeyAndMessage("Select difficulty", VK_LBUTTON, 0, &difficulty);
		std::cout << "Starting..." << std::endl;
		inputDelay = tempTimeDelay;
		while (true)
		{
			SetCursorPos(difficulty.x, difficulty.y);
			PressMouse(VK_LBUTTON);
			Sleep(newGameWait);
			SolveSudoku();
			Sleep(showScore);
		}
	}
public: //Objects
	static enum PrintingMethods
	{
		NoPrinting,
		Print,
		OnlyGoToNULL,
		Humanize,
		SuperHumanize,
	};
	/// <summary>
	/// The delay in which every key is pressed. Unit: Milliseconds. Standard: 1
	/// </summary>
	int inputDelay = 1;
	/// <summary>
	/// Speed from 0-9.
	/// 0 has the best precision but is the slowest.
	/// 9 has bad precision but is the fastest. 
	/// Standard: 9. 
	/// </summary>
	int readSudokuSpeed = 9;
	/// <summary>
	/// The delay in which the calibration keys are executed. Unit: Milliseconds. Standard: 1
	/// </summary>
	int calibrationSpeed = 1;
	/// <summary>
	/// How much time should the rating be shown (Where you can see your score). Unit: Milliseconds. Standard: 2000
	/// </summary>
	int showScore = 2000;
	/// <summary>
	/// When printing the sudoku to the website, you can toggle between it 
	/// snapping to a place that is null or simply go slowly to a place that 
	/// is null. Works only for PrintingMethods higher than 'Print' and at a 
	/// slow inputDelay
	/// </summary>
	bool snapToNull = false;
	/// <summary>
	/// When the sudoku game is being copied, theres a time between the 2 clicks. This changes that time. Unit: Milliseconds. Default: 100
	/// </summary>
	int clickCopyTime = 50;
	/// <summary>
	/// How many milliseconds should the solver wait, to copy after "new game is clicked". Default: 1000
	/// </summary>
	int newGameWait = 1000;
	/// <summary>
	/// Print the solved sudoku to the website. 0 = No Printing. 1 = Print to the website. 2 = Print randomly to the website. Default: NoPrinting
	/// </summary>
	PrintingMethods printWebsite = NoPrinting;
private: //Methods
#pragma region Stop when the user want to
	/// <summary>
	/// Stop the application the moment we hit 's'
	/// </summary>
	void checkForKeyPress()
	{
		while (true)
		{
			if (GetKeyState('S') & 0x8000)
			{
				std::cout << "Stopping the solver..." << std::endl;
				exit(0);
			}
			Sleep(10);
		}
	}
#pragma endregion
#pragma region Calibrating
	void Calibrate()
	{
		int tempInputDelay = inputDelay;
		inputDelay = calibrationSpeed;
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
		inputDelay = tempInputDelay;
	}
#pragma endregion
#pragma region Solving
	int LevenshteinDistance(const char* s1, const char* s2) const
	{
#define MIN(x,y) (x < y ? x : y) //calculate minimum between two values
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
		std::chrono::steady_clock::time_point start_time, end_time;

		start_time = std::chrono::high_resolution_clock::now();
		int i = 1;
		auto numberCode = GetGameCodes();
		std::vector<std::vector<std::size_t>> SudokuValues;
		for (int y = 0; y < MatchCodes.size(); y++)
		{
			std::vector<std::size_t> row;
			for (int x = 0; x < MatchCodes[y].size(); x++)
			{
				if (numberCode[y][x] != "")
				{
					std::cout << "Number: " << i << std::endl;
					i++;
				}

				float oldNum = 100;
				std::size_t num = 0;
				for (std::tuple<std::string, int> item : MatchCodes[y][x])
					if (numberCode[y][x] != "")
					{
						float testMatch = (double)LevenshteinDistance(std::get<0>(item).c_str(), numberCode[y][x].c_str()) / std::get<0>(item).size();
						
						std::cout << std::get<1>(item) << ": " << std::get<0>(item) << " and " << numberCode[y][x] << " = " << testMatch << std::endl;
						if (testMatch < oldNum)
						{
							num = std::get<1>(item);
							oldNum = testMatch;

							if (testMatch == 0)
								break;
						}
					}
				row.push_back(num);
			}
			SudokuValues.push_back(row);
		}
		end_time = std::chrono::high_resolution_clock::now();
		std::cout << std::endl;
		std::cout << "Got game in: " << std::chrono::duration<double>(end_time - start_time).count() * 1000 << " milliseconds:" << std::endl;
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

		start_time = std::chrono::high_resolution_clock::now();
		solveSudoku(SudokuValues);
		end_time = std::chrono::high_resolution_clock::now();
		std::cout << "Solved game in: " << std::chrono::duration<double>(end_time - start_time).count() * 1000 << " milliseconds:" << std::endl;

		start_time = std::chrono::high_resolution_clock::now();
		PrintSudoku(SudokuValues);
		end_time = std::chrono::high_resolution_clock::now();
		std::cout << std::endl;
		std::cout << "Printed game in: " << std::chrono::duration<double>(end_time - start_time).count() * 1000 << " milliseconds" << std::endl;
		std::cout << std::endl;

	}
#pragma region Actual Sudoku Solver
	void solveSudoku(std::vector<std::vector<std::size_t>>& board) const noexcept
	{
		std::array<std::bitset<9>, 9> row_contains = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		std::array<std::bitset<9>, 9> col_contains = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		std::array<std::bitset<9>, 9> cell_contains = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		for (std::size_t row = 0; row < 9; row++)
			for (std::size_t col = 0; col < 9; col++)
			{
				std::size_t digit;
				if ((digit = board[row][col]) != 0)
				{
					std::size_t digit_idx = digit - 1;
					row_contains[row].set(digit_idx);
					col_contains[col].set(digit_idx);
					std::size_t cell = (row / 3) * 3 + col / 3;
					cell_contains[cell].set(digit_idx);
				}
			}
		solve(board, 0, 0, row_contains, col_contains, cell_contains);
	}
	static constexpr std::pair<std::size_t, std::size_t>
		next_empty_position(std::vector<std::vector<std::size_t>>& board, std::size_t row, std::size_t col) noexcept
	{
		while (row != 9)
		{
			if (board[row][col] == 0)
				return { row, col };
			row = row + (col + 1) / 9;
			col = (col + 1) % 9;
		}
		return { 9, 0 };
	}

	static bool
		solve(std::vector<std::vector<std::size_t>>& board, std::size_t const row_start, std::size_t const col_start,
			  std::array<std::bitset<9>, 9>& row_contains,
			  std::array<std::bitset<9>, 9>& col_contains,
			  std::array<std::bitset<9>, 9>& cell_contains) noexcept
	{

		std::pair<size_t, size_t> rowCol = next_empty_position(board, row_start, col_start);

		if (rowCol.first == 9) // end of board
			return true;

		std::size_t const cell = (rowCol.first / 3) * 3 + rowCol.second / 3;
		std::bitset<9> const contains = row_contains[rowCol.first] | col_contains[rowCol.second] | cell_contains[cell];
		if (contains.all())
			return false;

		for (std::size_t digit_idx = 0; digit_idx < 9; ++digit_idx)
			if (!contains[digit_idx])
			{
				board[rowCol.first][rowCol.second] = digit_idx + 1;
				row_contains[rowCol.first].set(digit_idx);
				col_contains[rowCol.second].set(digit_idx);
				cell_contains[cell].set(digit_idx);
				if (solve(board, rowCol.first, rowCol.second, row_contains, col_contains, cell_contains))
					return true;

				row_contains[rowCol.first].reset(digit_idx);
				col_contains[rowCol.second].reset(digit_idx);
				cell_contains[cell].reset(digit_idx);
			}
		board[rowCol.first][rowCol.second] = 0;
		return false;
	}
#pragma endregion
#pragma region Printing sudoku
	void PrintSudoku(std::vector<std::vector<std::size_t>> matrix)
	{
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

		int tempInputDelay = inputDelay;
		int X = 0;
		int Y = 0;
		std::vector<std::tuple<POINT, std::size_t>> values;
		switch (printWebsite)
		{
			case Print:
				for (byte y = 0; y < 5; y++)
				{
					X = 0;
					for (byte x = 0; x < 9; x++)
					{
						PressKey(0x30 + matrix[Y][X]);
						PressKey(VK_RIGHT);
						X++;
					}
					Y++;
					if (y < 4)
					{
						X = 0;
						std::reverse(matrix[Y].begin(), matrix[Y].end());
						PressKey(VK_DOWN);
						for (byte x = 0; x < 9; x++)
						{
							PressKey(0x30 + matrix[Y][X]);
							PressKey(VK_LEFT);
							X++;
						}
						PressKey(VK_DOWN);
						Y++;
					}
				}
				if (snapToNull)
					inputDelay = 0;
				for (byte y = 0; y < 9; y++)
				{
					PressKey(VK_LEFT);
					PressKey(VK_UP);
				}
				if (snapToNull)
					inputDelay = tempInputDelay;
				break;
			case OnlyGoToNULL:
				for (byte y = 0; y < originalGrid.size(); y++)
					for (byte x = 0; x < originalGrid[y].size(); x++)
						if (originalGrid[y][x] == 0)
						{
							if (snapToNull)
								inputDelay = 0;
							int valueX = x - X;
							int valueY = y - Y;
							for (byte i = 0; i < std::abs(valueX); i++)
								PressKey((valueX < 0 ? true : false) ? VK_LEFT : VK_RIGHT);
							for (byte i = 0; i < std::abs(valueY); i++)
								PressKey((valueY < 0 ? true : false) ? VK_UP : VK_DOWN);
							if (snapToNull)
								inputDelay = tempInputDelay;

							PressKey(0x30 + matrix[y][x]);

							X = x;
							Y = y;
						}
				break;
			case Humanize:
				for (byte y = 0; y < originalGrid.size(); y++)
					for (byte x = 0; x < originalGrid[y].size(); x++)
						if (originalGrid[y][x] == 0)
							values.emplace_back(POINT{ x,y }, matrix[y][x]);
				std::random_shuffle(values.begin(), values.end());
				for (byte i = 0; i < values.size(); i++)
				{
					if (snapToNull)
						inputDelay = 0;
					int valueX = std::get<0>(values[i]).x - X;
					int valueY = std::get<0>(values[i]).y - Y;
					for (int i = 0; i < std::abs(valueX); i++)
						PressKey((valueX < 0 ? true : false) ? VK_LEFT : VK_RIGHT);
					for (int i = 0; i < std::abs(valueY); i++)
						PressKey((valueY < 0 ? true : false) ? VK_UP : VK_DOWN);
					if (snapToNull)
						inputDelay = tempInputDelay;

					PressKey(0x30 + std::get<1>(values[i]));

					X = std::get<0>(values[i]).x;
					Y = std::get<0>(values[i]).y;
				}
				break;
			case SuperHumanize:
				//Do every 3x3 first
				break;
		}
	}
#pragma endregion
#pragma endregion
#pragma region Inputs
	/// <summary>
	/// Very basic way of telling the user what to do, and wait for them to do it
	/// </summary>
	void PromptForKeyAndMessage(std::string message, WORD virtKey, int wait, POINT* point = nullptr)
	{
		std::cout << message << std::endl;
		while (true)
		{
			if (GetKeyState(virtKey) & 0x8000)
			{
				GetCursorPos(point);
				break;
			}
			Sleep(10);
		}
		Sleep(wait);
	}
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
		Sleep(inputDelay);
	}
#pragma endregion
#pragma region Converters
	static HDC ConvertHandleToHDC(HANDLE hHandle)
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
	static HBITMAP ConvertHandleToHBitmap(HANDLE hHandle)
	{
		HBITMAP hBitmap = NULL;

		// Copy the image using CopyImage
		hBitmap = (HBITMAP)CopyImage(hHandle, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);

		return hBitmap;
	}
#pragma endregion
#pragma region Value Checkers
	static int IsGameColor(COLORREF color)
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
#pragma endregion
#pragma region Get Game
	//Get "Copy Points"
	void GetGameCopyPos()
	{
		//Copy image
		PromptForKeyAndMessage("Right click on game", VK_RBUTTON, 0, &rightClick);
		PromptForKeyAndMessage("Left click on copy", VK_LBUTTON, 0, &leftClick);
		EmptyClipboard();
	}

	//Get Game
	std::vector<std::vector<std::string>> GetGameCodes()
	{
		EmptyClipboard();
		//Copy Game
		SetCursorPos(rightClick.x, rightClick.y);
		PressMouse(VK_RBUTTON, 0);
		Sleep(clickCopyTime);
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
			if (y % readSudokuSpeed == 0)
			{
				std::vector<bool> tempGame;
				for (int x = 0; x < bitmap.bmWidth; x++)
					if (x % readSudokuSpeed == 0)
						tempGame.push_back(IsGameColor(GetPixel(hdc, x, y)) > 0);
				game.push_back(tempGame);
			}

		//Remove lines - I failed so fucking much with this one
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
private: //Objects
#pragma region Get Game
	POINT rightClick{};
	POINT leftClick{};
	POINT difficulty{};
#pragma endregion
#pragma region Solving
#pragma region Printing sudoku
	std::vector<std::vector<int>> originalGrid;
#pragma endregion
#pragma endregion
#pragma region Calibrating
	std::vector<std::vector<std::vector<std::tuple<std::string, int>>>> MatchCodes;
#pragma endregion
};

//Main sudoku solver engine
class SudokuSolver
{
public:
	void solveSudoku(std::vector<std::vector<std::size_t>>& board) const noexcept
	{
		std::array<std::bitset<9>, 9> row_contains = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		std::array<std::bitset<9>, 9> col_contains = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		std::array<std::bitset<9>, 9> cell_contains = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };

		for (std::size_t row = 0; row < 9; row++)
			for (std::size_t col = 0; col < 9; col++)
			{
				std::size_t digit;
				if ((digit = board[row][col]) != 0)
				{
					std::size_t digit_idx = digit - 1;
					row_contains[row].set(digit_idx);
					col_contains[col].set(digit_idx);
					std::size_t cell = (row / 3) * 3 + col / 3;
					cell_contains[cell].set(digit_idx);
				}
			}
		solve(board, 0, 0, row_contains, col_contains, cell_contains);
	}

private:
	static constexpr 
		std::pair<std::size_t, std::size_t>next_empty_position(std::vector<std::vector<std::size_t>>& board, std::size_t row, std::size_t col) noexcept
	{
		while (row != 9)
		{
			if (board[row][col] == 0)
				return { row, col };
			row = row + (col + 1) / 9;
			col = (col + 1) % 9;
		}
		return { 9, 0 };
	}

	static bool
		solve(std::vector<std::vector<std::size_t>>& board, std::size_t const row_start, std::size_t const col_start,
			  std::array<std::bitset<9>, 9>& row_contains,
			  std::array<std::bitset<9>, 9>& col_contains,
			  std::array<std::bitset<9>, 9>& cell_contains) noexcept
	{
		
		std::pair<size_t, size_t> rowCol = next_empty_position(board, row_start, col_start);

		if (rowCol.first == 9) // end of board
			return true;

		std::size_t const cell = (rowCol.first / 3) * 3 + rowCol.second / 3;
		std::bitset<9> const contains = row_contains[rowCol.first] | col_contains[rowCol.second] | cell_contains[cell];
		if (contains.all())
			return false;

		for (std::size_t digit_idx = 0; digit_idx < 9; ++digit_idx)
			if (!contains[digit_idx])
			{
				board[rowCol.first][rowCol.second] = digit_idx + 1;
				row_contains[rowCol.first].set(digit_idx);
				col_contains[rowCol.second].set(digit_idx);
				cell_contains[cell].set(digit_idx);

				if (solve(board, rowCol.first, rowCol.second, row_contains, col_contains, cell_contains))
					return true;

				row_contains[rowCol.first].reset(digit_idx);
				col_contains[rowCol.second].reset(digit_idx);
				cell_contains[cell].reset(digit_idx);
			}
		board[rowCol.first][rowCol.second] = 0;
		return false;
	}
};


int main()
{
	//To stop the code press 's'
	SudokuAutoGameSolver gameSolver;
	gameSolver.readSudokuSpeed = 9;
	gameSolver.inputDelay = 0;
	gameSolver.calibrationSpeed = 1;
	gameSolver.showScore = 2000;
	gameSolver.printWebsite = gameSolver.Print;
	gameSolver.clickCopyTime = 100;
	gameSolver.newGameWait = 1000;
	//gameSolver.snapToNull = false; //Trying to make it work
	gameSolver.ExecuteOrder66();


	
	//Take time if needed :)
	/*
	auto start_time = std::chrono::high_resolution_clock::now();
	auto end_time = std::chrono::high_resolution_clock::now();
	std::cout << "Execution time: " << std::chrono::duration<double>(end_time - start_time).count() * 1000 << " milliseconds" << std::endl;
	*/

}

