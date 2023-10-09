#include <iostream>
#include <vector>
#include <windows.h>
#include <string>
using namespace std;

class Sudoku
{
public:
	//Simple method to just check if everything was typed right
	void CalculateSudoku()
	{
		char response = 'y';

		do
		{
			string time;

			if (response == 'y')
			{
				cout << "How many ms/number? ";
				cin.ignore();
				getline(cin, time);

				TimeDelay = stoi(time);

				//
				GetValues();
				Sleep(10);
				if (grid.size() >= 9)
					Solve();
				else
					cout << "Try again. You fucked up!";
				//
			}
			cout << endl << "Another sudoku (y/n)? ";
			cin >> response;
			grid.clear();
			system("cls");
		} while (response == 'y');
	}
private:
	//Set the time delay for each PressKey 0 is the fastest
	int TimeDelay = 100;
	//The main grid that holds the numbers just like matrix
	vector<vector<int>> grid;
	void GetValues()
	{
		string row;
		do
		{
			vector<int> ints;
			cout << "Row " << grid.size() + 1 << ": ";
			getline(cin, row);
			if (row.size() != 9)
			{
				cout << "Der skal være 9 tal!" << endl;
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
	void Print(vector<vector<int>> matrix)
	{
		vector<int> list;
		//Here we add all of the rows into one big list
		for (vector<int> row : matrix)
			list.insert(list.end(), row.begin(), row.end());
		cout << endl;
		for (byte i_1 = 0; i_1 < matrix.size(); i_1++)
		{
			if (i_1 % 3 == 0 && i_1 != 0)
				cout << "-----------------------" << endl;
			for (byte i_2 = 0; i_2 < matrix[i_1].size(); i_2++)
				if (i_2 % 3 == 0 && i_2 != 0)
					cout << " | " << matrix[i_1][i_2];
				else
					cout << " " << matrix[i_1][i_2];
			cout << endl;
		}

		cout << endl << "Do you want to print to the Sudoku website (y/n)? ";
		char print;
		cin >> print;
		if (print == 'y')
		{
			cout << endl << "Move the mouse to the upper right corner of the sudoku game." << endl <<
							"When you have moved the mouse, press 'y' and then 'Enter'" << endl <<
							"You will have 3 seconds to press 'left mousebutton'" << endl <<
							"So click 'left mousebutton', right after you have clicked 'Enter'!" << endl;
			char ready;
			cout << "Ready (y/n)? ";
			cin >> ready;
			if (ready == 'y')
			{
				int counterGoBack = 1;
				//Here we wait for the user to do the thing described above in the strings
				Sleep(3000);
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
};
class GetSudoku
{
public:
	vector<vector<int>> GetSudokuValues()
	{
		return vector<vector<int>>();
	}
};

int main()
{
	/*GetSudoku getSudoku;
	getSudoku.GetSudokuValues();*/
	Sudoku sudoku;
	sudoku.CalculateSudoku();
}