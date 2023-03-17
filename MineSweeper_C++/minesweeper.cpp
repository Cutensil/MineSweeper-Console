#include<iostream>
#include<fstream>
#include<windows.h>
#include<conio.h>
using namespace std;

HANDLE output;
char gameover;
int open_count;
int flag_count;
int height;
int width;
int mines;
int active_x;
int active_y;

struct MAP
{
	char mine = 1;
	char open = 0;
	char flag = 0;
	char error = 0;
	char correct = 0;
}map[30][24], map_init[30][24];
class CONFIG
{
public:
	char open = 'D';
	char flag = 'F';
	char retry = 'R';
	int height = 9;
	int width = 9;
	int mines = 10;
public:
	void Save();
public:
	void Load();
}config;

void CONFIG::Save()
{
	ofstream config_file;
	config_file.open("config.dat", ios::binary);
	if (!config_file.is_open())
	{
		SetConsoleTextAttribute(output, FOREGROUND_RED);
		cout << "Error:Failed to save config data! Exiting in 5 seconds." << endl;
		SetConsoleTextAttribute(output, 7);
		Sleep(5000);
	}
	else
		config_file.write((const char*)&config, sizeof(config));
}
void CONFIG::Load()
{
	ifstream config_file;
	config_file.open("config.dat", ios::binary);
	if (!config_file.is_open())
	{
		SetConsoleTextAttribute(output, FOREGROUND_RED);
		cout << "Error:Cannot find config data! Now using the default settings." << endl;
		cout << "(If it is the first time you run this program, please ignore.)" << endl;
		SetConsoleTextAttribute(output, 7);
		ofstream config_file;
		config_file.open("config.dat", ios::binary);
		if (!config_file.is_open())
		{
			SetConsoleTextAttribute(output, FOREGROUND_RED);
			cout << "Error:Failed to create config data!" << endl;
			SetConsoleTextAttribute(output, 7);
		}
		else
			config_file.write((const char*)&config, sizeof(config));
	}
	else
		while (config_file.read((char*)&config, sizeof(config)));
}
void CursorJump(int x, int y)
{
	COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(output, pos);
}
void HideCursor()
{
	CONSOLE_CURSOR_INFO curInfo;
	curInfo.dwSize = 1;
	curInfo.bVisible = FALSE;
	SetConsoleCursorInfo(output, &curInfo);
}
void Print()
{
	HideCursor();
	CursorJump(0, 0);
	for (int y = 0;y < height;y++)
	{
		for (int x = 0;x < width;x++)
		{
			if (map[x][y].open == 0)
			{
				if (map[x][y].error == 1)
					SetConsoleTextAttribute(output, 0 | BACKGROUND_RED | BACKGROUND_INTENSITY);
				else if (map[x][y].correct == 1)
					SetConsoleTextAttribute(output, 0 | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
				else if (x == active_x && y == active_y)
					SetConsoleTextAttribute(output, 0 | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
				else
					SetConsoleTextAttribute(output, 0 | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
				if (gameover == 1 && map[x][y].mine == 'M')
					cout << "¡Á";
				else
				{
					switch (map[x][y].flag)
					{
					case 1:
						cout << "¡ò";
						break;
					default:
						cout << "¡¡";
					}
				}
			}
			else
			{
				if (map[x][y].error == 1)
					SetConsoleTextAttribute(output, 7 | BACKGROUND_RED | BACKGROUND_INTENSITY);
				else if (map[x][y].correct == 1)
					SetConsoleTextAttribute(output, 0 | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
				else if (x == active_x && y == active_y)
					SetConsoleTextAttribute(output, 7 | BACKGROUND_BLUE | BACKGROUND_INTENSITY);
				else
					SetConsoleTextAttribute(output, 7);
				switch (map[x][y].mine)
				{
				case 0:
					cout << "¡¡";
					break;
				case 1:
					cout << "£±";
					break;
				case 2:
					cout << "£²";
					break;
				case 3:
					cout << "£³";
					break;
				case 4:
					cout << "£´";
					break;
				case 5:
					cout << "£µ";
					break;
				case 6:
					cout << "£¶";
					break;
				case 7:
					cout << "£·";
					break;
				case 8:
					cout << "£¸";
					break;
				case 'M':
					cout << "¡Á";
				}
			}
		}
		cout << endl;
	}
	SetConsoleTextAttribute(output, 7);
	cout << "Mines:" << mines - flag_count << "          " << endl;
}
void OpenAround(int x, int y)
{
	for (int i = -1;i <= 1;i++)
	{
		for (int j = -1;j <= 1;j++)
		{
			if ((i == 0 && j == 0) || x + i < 0 || x + i >= width || y + j < 0 || y + j >= height)
				continue;
			if (map[x + i][y + j].open == 0 && map[x + i][y + j].flag == 0)
			{
				map[x + i][y + j].open = 1;
				open_count++;
				if (map[x + i][y + j].mine == 0)
					OpenAround(x + i, y + j);
				else if (map[x + i][y + j].mine == 'M')
					gameover = 1;
			}
		}
	}
}
void Open(int x, int y)
{
	if (map[x][y].open == 0 && map[x][y].flag == 0)
	{
		map[x][y].open = 1;
		open_count++;
		if (map[x][y].mine == 0)
			OpenAround(x, y);
		else if (map[x][y].mine == 'M')
			gameover = 1;
	}
	else if (map[x][y].open == 1 && map[x][y].mine != 0)
	{
		int count = 0;
		for (int i = -1;i <= 1;i++)
		{
			for (int j = -1;j <= 1;j++)
			{
				if (map[x + i][y + j].flag == 1)
					count++;
			}
		}
		if (count == map[x][y].mine)
			OpenAround(x, y);
	}
}
char GetInput()
{
	int temp;
	temp = _getch();
	if (temp == 224)
	{
		temp = _getch();
		switch (temp)
		{
		case 72:
			active_y = (active_y + height - 1) % height;
			break;
		case 75:
			active_x = (active_x + width - 1) % width;
			break;
		case 77:
			active_x = (active_x + 1) % width;
			break;
		case 80:
			active_y = (active_y + 1) % height;
			break;
		}
	}
	else
	{
		if (temp == config.open || temp == config.open + 32)
		{
			Open(active_x, active_y);
			return 1;
		}
		else if (temp == config.flag || temp == config.flag + 32)
		{
			if (map[active_x][active_y].open == 0)
			{
				if (map[active_x][active_y].flag == 0)
				{
					map[active_x][active_y].flag = 1;
					flag_count++;
				}
				else
				{
					map[active_x][active_y].flag = 0;
					flag_count--;
				}
			}
		}
		else if (temp == 27)
			return 2;
		else if (temp == config.retry || temp == config.retry + 32)
			return 3;
	}
	return 0;
}
void Initialize()
{
	gameover = 0;
	open_count = 0;
	flag_count = 0;
	int x, y;
	srand((unsigned int)time(NULL));
	memset(&map, 0, sizeof(map));
	for (int count = 0;count < mines;)
	{
		x = rand() % width;
		y = rand() % height;
		if (map[x][y].mine != 'M' && ((x<active_x - 1 || x>active_x + 1) || (y<active_y - 1 || y>active_y + 1)))
		{
			map[x][y].mine = 'M';
			count++;
			for (int i = -1;i <= 1;i++)
			{
				for (int j = -1;j <= 1;j++)
				{
					if (i == 0 && j == 0)
						continue;
					if (map[x + i][y + j].mine != 'M' && x + i >= 0 && x + i < width && y + j >= 0 && y + j < height)
						map[x + i][y + j].mine++;
				}
			}
		}
	}
	Open(active_x, active_y);
}

int main()
{
	output = GetStdHandle(STD_OUTPUT_HANDLE);
	config.Load();
retry0:
	system("cls");
	SetConsoleTextAttribute(output, 7);
	cout << "--------MineSweeper in C++--------" << endl;
	cout << "        1.Beginner" << endl;
	cout << "        2.Intermediate" << endl;
	cout << "        3.Expert" << endl;
	cout << "        4.Custom" << endl;
	cout << "        5.Settings" << endl;
	cout << "----------------------------------" << endl;
	char error_flag = 0;
	char input;
	int input_temp = 0;
	input = _getch();
	switch (input)
	{
	case '1':
		width = 9;
		height = 9;
		mines = 10;
		break;
	case '2':
		width = 16;
		height = 16;
		mines = 40;
		break;
	case '3':
		width = 30;
		height = 16;
		mines = 99;
		break;
	case '4':
		width = config.width;
		height = config.height;
		mines = config.mines;
		break;
	case '5':
	retry5:
		system("cls");
		cout << "--------KeyBoard Setting--------" << endl;
		cout << "        1.Open:" << config.open << endl;
		cout << "        2.Flag:" << config.flag << endl;
		cout << "        3.Retry:" << config.retry << endl;
		cout << "---------Custom Setting---------" << endl;
		cout << "         4.Width:" << config.width << endl;
		cout << "         5.Height:" << config.height << endl;
		cout << "         6.Mines:" << config.mines << endl;
		cout << "--------------------------------" << endl;
		cout << "         0.Save and Exit" << endl;
		input = _getch();
		switch (input)
		{
		case '1':case '2':case '3':
		retry51:
			cout << "Input a key" << endl;
			if (error_flag == 1)
			{
				SetConsoleTextAttribute(output, FOREGROUND_RED);
				error_flag = 0;
			}
			cout << "(Must be A-Z)" << endl;
			SetConsoleTextAttribute(output, 7);
			input_temp = _getch();
			if (input_temp >= 'A' && input_temp <= 'Z')
			{
				if (input == '1')
					config.open = input_temp;
				else if (input == '2')
					config.flag = input_temp;
				else
					config.retry = input_temp;
				goto retry5;
			}
			else if (input_temp >= 'a' && input_temp <= 'z')
			{
				if (input == '1')
					config.open = input_temp - 32;
				else if (input == '2')
					config.flag = input_temp - 32;
				else
					config.retry = input_temp - 32;
				goto retry5;
			}
			else
			{
				error_flag = 1;
				goto retry51;
			}
		case '4':case '5':case '6':
		retry53:
			cout << "Input a number, then press Enter" << endl;
			if (error_flag == 1)
			{
				SetConsoleTextAttribute(output, FOREGROUND_RED);
				error_flag = 0;
			}
			if (input == '4')
				cout << "(Must be 9-30)" << endl;
			else if (input == '5')
				cout << "(Must be 9-24)" << endl;
			else
				cout << "(Must be 10-" << config.width * config.height - 10 << " )" << endl;
			SetConsoleTextAttribute(output, 7);
			cin >> input_temp;
			if (input == '4')
			{
				if (input_temp > 30 || input_temp < 9)
				{
					error_flag = 1;
					goto retry53;
				}
				else
				{
					config.width = input_temp;
					goto retry5;
				}
			}
			else if (input == '5')
			{
				if (input_temp > 24 || input_temp < 9)
				{
					error_flag = 1;
					goto retry53;
				}
				else
				{
					config.height = input_temp;
					goto retry5;
				}
			}
			else
			{
				if (input_temp > config.width * config.height - 10 || input_temp < 10)
				{
					error_flag = 1;
					goto retry53;
				}
				else
				{
					config.mines = input_temp;
					goto retry5;
				}
			}
		case '0':
			config.Save();
			goto retry0;
		default:
			goto retry5;
		}
	}
retry:
	memcpy(&map, &map_init, sizeof(map));
	active_x = width / 2 - 1;
	active_y = height / 2 - 1;
	flag_count = 0;
	system("cls");
	while (1)
	{
		Print();
		input = GetInput();
		if (input == 1)
		{
			Initialize();
			break;
		}
		else if (input == 2)
			goto retry0;
	}
	while (!gameover)
	{
		if (open_count == width * height - mines)
		{
			gameover = 1;
			for (int x = 0;x < width;x++)
			{
				for (int y = 0;y < height;y++)
				{
					if ((map[x][y].open == 1 && map[x][y].mine == 'M') || (map[x][y].flag == 1 && map[x][y].mine != 'M'))
						map[x][y].error = 1;
					else if (map[x][y].mine == 'M' && map[x][y].flag == 1)
						map[x][y].correct = 1;
				}
			}
			Print();
			SetConsoleTextAttribute(output, FOREGROUND_GREEN);
			cout << "You Win!" << endl;
			cout << "1.Retry" << endl;
			cout << "2.Menu" << endl;
			cout << "3.Exit" << endl;
		retry_win:
			input = _getch();
			switch (input)
			{
			case '1':
				goto retry;
			case '2':
				goto retry0;
			case '3':
				return 0;
			default:
				goto retry_win;
			}
		}
		Print();
		input = GetInput();
		if (input == 2)
			goto retry0;
		else if (input == 3)
			goto retry;
	}
	for (int x = 0;x < width;x++)
	{
		for (int y = 0;y < height;y++)
		{
			if ((map[x][y].open == 1 && map[x][y].mine == 'M') || (map[x][y].flag == 1 && map[x][y].mine != 'M'))
				map[x][y].error = 1;
			else if (map[x][y].mine == 'M' && map[x][y].flag == 1)
				map[x][y].correct = 1;
		}
	}
	Print();
	SetConsoleTextAttribute(output, FOREGROUND_RED);
	cout << "Game Over!" << endl;
	cout << "1.Retry" << endl;
	cout << "2.Menu" << endl;
	cout << "3.Exit" << endl;
retry_over:
	input = _getch();
	switch (input)
	{
	case '1':
		goto retry;
	case '2':
		goto retry0;
	case '3':
		return 0;
	default:
		goto retry_over;
	}
}