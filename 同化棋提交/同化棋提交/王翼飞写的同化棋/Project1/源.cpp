//同化棋小程序
//2100017408 王翼飞
//更新日志
//2021.12.02 双人对战基本框架 已保存
//2021.12.10 棋盘优化及胜负判断 已保存
//2021.12.21 图形界面 已保存
//2021.12.27 图形界面优化 设置背景图片 已保存
//2022.1.2 AI行棋 已保存
//2022.1.3 悔棋一步 已保存
//2022.1.6 存档功能 已保存

#include <iostream>
#include <cstring>
#include<cmath>
#include<string>
#include<cstdlib>
#include<graphics.h>
#include<conio.h>
#include<stdio.h>
#include<windows.h>
#include<fstream>
using namespace std;

#include<MMSystem.h>
#pragma comment(lib,"winmm.lib")

int gridinfo[7][7] = { 0 };
int savegridblack[7][7] = { 0 };//黑棋悔棋用
int savegridwhite[7][7] = { 0 };//白棋悔棋用
int blackpiececount = 2, whitepiececount = 2;
int currbotcolor = 0;
static int delta[24][2] = { { 1,1 },{ 0,1 },{ -1,1 },{ -1,0 },
{ -1,-1 },{ 0,-1 },{ 1,-1 },{ 1,0 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };


int startX, startY, resultX, resultY;
static int del[24][2] = { { 1,1 },{ -1,-1 } ,{ 1,-1 },{ -1,1 },{ 1,0 },{ 0,-1 },
{ -1,0 },{ 0,1 },
{ 2,0 },{ 2,1 },{ 2,2 },{ 1,2 },
{ 0,2 },{ -1,2 },{ -2,2 },{ -2,1 },
{ -2,0 },{ -2,-1 },{ -2,-2 },{ -1,-2 },
{ 0,-2 },{ 1,-2 },{ 2,-2 },{ 2,-1 } };
int dx[24] = { 0, 0, 1, -1, -1, 1, -1, 1, 2, 2, 2, 2,
		2, -2, -2, -2, -2, -2, 1, 0, -1, 1, 0, -1 };
int dy[24] = { 1, -1, 0, 0, 1, -1, -1, 1, 2, -2,1, 0,
		-1, 2,  -2, 1,  0,  -1,  2, 2, 2,  -2, -2,-2 };

int searchgrid[7][7][7];
int nowbeginPos[7][1000][2] = { 0 }, nowpossiblePos[7][1000][2] = { 0 }, nowposCount[7] = { 0 };
struct chose
{
	int ax;
	int ay;
	int ad;
};

MOUSEMSG SBmsg;
HWND hwnd;//窗口地址
IMAGE bk;

void Menu();//菜单栏
void operation1();//双人对战
void operation11();//双人对战图形界面
void operation2();//读档游戏
void inigrid();//初始化
void printgrid();//输出棋盘（不成熟版）现不使用
void printboard();//制表符输出棋盘
void drawboard();//最终版
bool judgecondition(int color);//判断局面能否继续
void whowin(int color);//判断谁赢
inline bool inmap(int x, int y);//检查是否在棋盘内
inline bool MoveStep(int x, int y, int Direction);//向direction移动，并检查是否越界
bool judgemove(int x0, int y0, int x1, int y1, int color);//检查落子是否合法
void move(int x0, int y0, int x1, int y1, int color);//落子并统计黑白子数量
void AIgo(int color);//AI行棋
int evaluate(int depth, int color);//估值函数
void makemove(int movex1, int movey1, int color, int depth);//AI虚拟下棋
int minmax(int d, int fx, int fy, int color, int depth, int alpha, int beta);//最大最小搜索
chose AI();

int main()
{
	mciSendString(L"open 乡野幽居.mp3 alias music4", 0, 0, 0);
	mciSendString(L"play music4 repeat", 0, 0, 0);
	Menu();
	mciSendString(L"close music4", 0, 0, 0);
	return 0;
}

void Menu()
{
	hwnd = initgraph(1000, 600);
	loadimage(&bk, _T(".\\无标题.jpg"));
	setfillcolor(RGB(0, 0, 0));
	while (1)
	{
		putimage(0, 0, &bk);
		fillrectangle(450, 420, 550, 460);
		outtextxy(475, 430, L"新游戏");
		fillrectangle(450, 480, 550, 520);
		outtextxy(470, 490, L"读取存档");
		fillrectangle(450, 540, 550, 580);
		outtextxy(480, 550, L"退出");
        FlushBatchDraw();
		//putimage(0, 0, &bk);
		FlushMouseMsgBuffer();
		while (1)
		{
			SBmsg = GetMouseMsg();
			if (SBmsg.uMsg == WM_LBUTTONDOWN)
			{
				if (SBmsg.x >= 450 && SBmsg.x <= 550 && SBmsg.y >= 420 && SBmsg.y <= 460)
				{
					operation11();
					return;
				}
				if (SBmsg.x >= 450 && SBmsg.x <= 550 && SBmsg.y >= 480 && SBmsg.y <= 520)
				{
					operation2();
					return;
				}
				if (SBmsg.x >= 450 && SBmsg.x <= 550 && SBmsg.y >= 540 && SBmsg.y <= 580)
				{
					return;
				}
			}
		}
	}
	
	int order;
	cout << "欢迎来到同化棋小游戏，输入1进行双人对战";
	cin >> order;
	if (order == 1)
		operation11();
}
//菜单

void operation1()
{
	inigrid();
	int x0, x1, y0, y1,round=0,color;
	while (1)
	{
		system("cls");
		printboard();
		round++;
		if (round % 2 == 1)
		{
			color = 1;
			for (int i = 0; i < 7; i++)
				for (int j = 0; j < 7; j++)
					savegridblack[i][j] = gridinfo[i][j];
		}
		else
		{
			color = -1;
			for (int i = 0; i < 7; i++)
				for (int j = 0; j < 7; j++)
					savegridwhite[i][j] = gridinfo[i][j];
		}
		if (judgecondition(color) == 0)
			break;
		if (round % 2 == 1)
			cout << "请黑方输入落子坐标" << endl;
		else
			cout << "请白方输入落子坐标" << endl;
		cin >> y0 >> x0 >> y1 >> x1;
		x0--;
		y0--;
		x1--;
		y1--;
		if (judgemove(x0, y0, x1, y1, color) == 0)
		{
			cout << "输入违规，请重新输入" << endl;
			round--;
			system("pause");
			continue;
		}
		if (judgemove(x0, y0, x1, y1, color) == 1)
		{
			move(x0, y0, x1, y1, color);
			system("cls");
			printboard();
		}

	}
	whowin(color);
	system("cls");
}
//双人对战（不再使用）

void operation11()
{
	hwnd=initgraph(1000, 600);
	loadimage(&bk, _T(".\\bk1.jpg"));
	inigrid();
	int x0 = 0, x1 = 0, y0 = 0, y1 = 0,round=0;
	bool flagAI = 0;
	int flaghq = 0;
	while (1)
	{
		flagAI = 0;
		flaghq = 0;
		putimage(0, 0, &bk);
		drawboard();
		round++;
		if (round % 2 == 1)
		{
			currbotcolor = 1;
		}
		else
		{
			currbotcolor = -1;
		}
		if (judgecondition(currbotcolor) == 0)
			break;
		if (round % 2 == 1)
		{
			outtextxy(10, 20, L"BLACK");
		}
		else
		{
			outtextxy(10, 20, L"WHITE");
		}
		FlushMouseMsgBuffer();
		while (1)
		{
			SBmsg = GetMouseMsg();
			if (SBmsg.uMsg == WM_LBUTTONDOWN)
			{
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 380 && SBmsg.y <= 420)
				{
					return;
				}
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 110 && SBmsg.y <= 150)
				{
					ofstream fout("file.txt");
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							fout << gridinfo[i][j] << " ";

					fout << round << " ";
					fout << blackpiececount <<" " << whitepiececount<<" ";
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							fout << savegridwhite[i][j] << " " << savegridblack[i][j] << " ";
					fout.close();
					return;
				}
				if ((round == 1 && SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 290 && SBmsg.y <= 330) || (round == 2 && SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 290 && SBmsg.y <= 330))
				{
					MessageBox(hwnd, L"无法悔棋！", L"违规提示", MB_OK);
					flaghq = 2;
					break;
				}
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 290 && SBmsg.y <= 330)
				{
					if (currbotcolor == 1)
					{
						for (int i = 0; i < 7; i++)
							for (int j = 0; j < 7; j++)
								gridinfo[i][j] = savegridblack[i][j];
					}
					else
					{
						for (int i = 0; i < 7; i++)
							for (int j = 0; j < 7; j++)
								gridinfo[i][j] = savegridwhite[i][j];
					}
					flaghq = 1;
				}
				if (currbotcolor == 1)
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							savegridblack[i][j] = gridinfo[i][j];
				else
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							savegridwhite[i][j] = gridinfo[i][j];
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 200 && SBmsg.y <= 240)
				{
					//此处引入ai行棋
					AIgo(currbotcolor);
					move(startX, startY, resultX, resultY, currbotcolor);





					flagAI = 1;
					break;
				}
				x0 = (SBmsg.x - 100) / 50;
				y0 = (SBmsg.y - 100) / 50;
				if (SBmsg.x < 100 || SBmsg.y < 100)
				{
					x0 = -1;
					y0 = -1;
				}
				//gridinfo[(SBmsg.x - 100) / 50][(SBmsg.y - 100) / 50] = color;
				break;
			}
		}
		if (flagAI)
		{
			continue;
		}
		else if (flaghq==1)
		{
			round-=3;
			continue;
		}
		else if (flaghq == 2)
		{
			round --;
			continue;
		}
		FlushMouseMsgBuffer();
		while (1)
		{
			SBmsg = GetMouseMsg();
			if (SBmsg.uMsg == WM_LBUTTONDOWN)
			{
				x1 = (SBmsg.x - 100) / 50;
				y1 = (SBmsg.y - 100) / 50;
				if (SBmsg.x < 100 || SBmsg.y < 100)
				{
					x1 = -1;
					y1 = -1;
				}
				break;
			}
		}
		
		if (judgemove(x0, y0, x1, y1, currbotcolor) == 0)
		{
			MessageBox(hwnd,L"非法操作\n请重新输入！",L"违规提示",MB_OK);
			round--;
			continue;
		}
		if (judgemove(x0, y0, x1, y1, currbotcolor) == 1)
		{
			move(x0, y0, x1, y1, currbotcolor);
			//system("cls");
			drawboard();
		}

	}
	whowin(currbotcolor);
	//system("cls");
	
}

void operation2()
{
	hwnd=initgraph(1000, 600);
	loadimage(&bk, _T(".\\bk1.jpg"));
	inigrid();
	int x0 = 0, x1 = 0, y0 = 0, y1 = 0,round=0;
	fstream fin("file.txt");
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
			fin >> gridinfo[i][j];
	fin >> round;
	fin >> blackpiececount >> whitepiececount;
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
			fin >> savegridwhite[i][j] >> savegridblack[i][j];
	fin.close();
	bool flagAI = 0;
	int flaghq = 0;
	while (1)
	{
		flagAI = 0;
		flaghq = 0;
		putimage(0, 0, &bk);
		drawboard();
		round++;
		if (round % 2 == 1)
		{
			currbotcolor = 1;
		}
		else
		{
			currbotcolor = -1;
		}
		if (judgecondition(currbotcolor) == 0)
			break;
		if (round % 2 == 1)
		{
			outtextxy(10, 20, L"BLACK");
		}
		else
		{
			outtextxy(10, 20, L"WHITE");
		}
		FlushMouseMsgBuffer();
		while (1)
		{
			SBmsg = GetMouseMsg();
			if (SBmsg.uMsg == WM_LBUTTONDOWN)
			{
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 380 && SBmsg.y <= 420)
				{
					return;
				}
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 110 && SBmsg.y <= 150)
				{
					ofstream fout("file.txt");
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							fout << gridinfo[i][j] << " ";
					fout << round << " ";
					fout << blackpiececount << " " << whitepiececount << " ";
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							fout << savegridwhite[i][j] <<" "<< savegridblack[i][j] << " ";
					fout.close();
					return;
				}
				if ((round == 1 && SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 290 && SBmsg.y <= 330) || (round == 2 && SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 290 && SBmsg.y <= 330))
				{
					MessageBox(hwnd, L"无法悔棋！", L"违规提示", MB_OK);
					flaghq = 2;
					break;
				}
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 290 && SBmsg.y <= 330)
				{
					if (currbotcolor == 1)
					{
						for (int i = 0; i < 7; i++)
							for (int j = 0; j < 7; j++)
								gridinfo[i][j] = savegridblack[i][j];
					}
					else
					{
						for (int i = 0; i < 7; i++)
							for (int j = 0; j < 7; j++)
								gridinfo[i][j] = savegridwhite[i][j];
					}
					flaghq = 1;
				}
				if (currbotcolor == 1)
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							savegridblack[i][j] = gridinfo[i][j];
				else
					for (int i = 0; i < 7; i++)
						for (int j = 0; j < 7; j++)
							savegridwhite[i][j] = gridinfo[i][j];
				if (SBmsg.x >= 680 && SBmsg.x <= 800 && SBmsg.y >= 200 && SBmsg.y <= 240)
				{
					//此处引入ai行棋
					AIgo(currbotcolor);
					move(startX, startY, resultX, resultY, currbotcolor);





					flagAI = 1;
					break;
				}
				x0 = (SBmsg.x - 100) / 50;
				y0 = (SBmsg.y - 100) / 50;
				if (SBmsg.x < 100 || SBmsg.y < 100)
				{
					x0 = -1;
					y0 = -1;
				}
				//gridinfo[(SBmsg.x - 100) / 50][(SBmsg.y - 100) / 50] = color;
				break;
			}
		}
		if (flagAI)
		{
			continue;
		}
		else if (flaghq==1)
		{
			round-=3;
			continue;
		}
		else if (flaghq == 2)
		{
			round --;
			continue;
		}
		FlushMouseMsgBuffer();
		while (1)
		{
			SBmsg = GetMouseMsg();
			if (SBmsg.uMsg == WM_LBUTTONDOWN)
			{
				x1 = (SBmsg.x - 100) / 50;
				y1 = (SBmsg.y - 100) / 50;
				if (SBmsg.x < 100 || SBmsg.y < 100)
				{
					x1 = -1;
					y1 = -1;
				}
				break;
			}
		}
		
		if (judgemove(x0, y0, x1, y1, currbotcolor) == 0)
		{
			MessageBox(hwnd,L"非法操作\n请重新输入！",L"违规提示",MB_OK);
			round--;
			continue;
		}
		if (judgemove(x0, y0, x1, y1, currbotcolor) == 1)
		{
			move(x0, y0, x1, y1, currbotcolor);
			//system("cls");
			drawboard();
		}

	}
	whowin(currbotcolor);
	//system("cls");
	
}

void inigrid()
{
	memset(gridinfo, 0, sizeof(gridinfo));
	gridinfo[0][0] = gridinfo[6][6] = 1;
	gridinfo[6][0] = gridinfo[0][6] = -1;
	memset(savegridwhite, 0, sizeof(savegridwhite));
	savegridwhite[0][0] = savegridwhite[6][6] = 1;
	savegridwhite[6][0] = savegridwhite[0][6] = -1;
	memset(savegridblack, 0, sizeof(savegridblack));
	savegridblack[0][0] = savegridblack[6][6] = 1;
	savegridblack[6][0] = savegridblack[0][6] = -1;
}
//初始化

void printgrid()
{
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
			cout << gridinfo[j][i];
		cout << endl;
	}
}
//输出棋盘（不成熟版）现不使用

void printboard()
{
	cout << "    1   2   3   4   5   6   7"<<endl;
	cout << " ┏━━━┳━━━┳━━━┳━━━┳━━━┳━━━┳━━━┓" << endl;
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (j == 0)
			{
				if (gridinfo[i][j] == 0)
					cout <<i+1<< "┃   ┃";
				if (gridinfo[i][j] == 1)
					cout <<i+1<< "┃ ○┃";
				if (gridinfo[i][j] == -1)
					cout <<i+1<<"┃ ●┃";
			}
			else
			{
				if (gridinfo[i][j] == 0)
					cout << "   ┃";
				if (gridinfo[i][j] == 1)
					cout << " ○┃";
				if (gridinfo[i][j] == -1)
					cout << " ●┃";
				if (j == 6)
					cout << endl;
			}
			
			
		}
		if(i==6)
			cout << " ┗━━━┻━━━┻━━━┻━━━┻━━━┻━━━┻━━━┛" << endl;
		else
			cout << " ┣━━━╋━━━╋━━━╋━━━╋━━━╋━━━╋━━━┫" << endl;
	}
	cout << "  黑子数：" << blackpiececount << "     " << "白子数：" << whitepiececount << endl;
	
	
}

void drawboard()
{
	//putimage(0, 0, &bk);
	//setbkcolor(RGB(0, 0, 0));
	//cleardevice();
	setfillcolor(RGB(245, 204, 132));
	fillrectangle(100, 100, 450, 450);
	setlinecolor(RGB(0, 0, 0));
	for (int i = 100; i <= 450; i += 50)
	{
		line(i, 100, i, 450);
		line(100, i, 450, i);
	}
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (gridinfo[i][j] == 1)
			{
				setlinecolor(RGB(0, 0, 0));
				setfillcolor(RGB(0, 0, 0));
				fillcircle(i * 50 + 125, j * 50 + 125, 20);
			}
			if (gridinfo[i][j] == -1)
			{
				setfillcolor(RGB(255, 255, 255));
				fillcircle(i * 50 + 125, j * 50 + 125, 20);
			}
		}
	}
	//setfillcolor(RGB(232, 232, 236));
	//settextcolor(BLACK);
	fillrectangle(680, 110, 800, 150);
	outtextxy(720,125, L"存档");
	fillrectangle(680,200,800,240);
	outtextxy(715, 215, L"AI代行");
	fillrectangle(680, 290, 800, 330);
	outtextxy(710, 305, L"悔棋一步");
	fillrectangle(680, 380, 800, 420);
	outtextxy(720, 395, L"退出");

	FlushBatchDraw();
}
//输出棋盘最终版

bool judgecondition(int color)
{
	if (blackpiececount == 0)
		return 0;
	else if (whitepiececount == 0)
		return 0;
	
    int all = 1;
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
			all *= gridinfo[i][j];
	if (all)
	    return 0;
	bool cancolormove = 0; 
	int x, y;
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
		{
			if (gridinfo[i][j] == 0||gridinfo[i][j]==-color)
				continue;
			
			for (int s = 0; s < 24; s++)
			{
				x = i + delta[s][0];
				y = j + delta[s][1];
				if (!inmap(x, y))
					continue;
				if (gridinfo[x][y] == 0)
					cancolormove = 1;
			}
		}
	if (!cancolormove)
		return 0;
	return 1;
	
}
//判断局面能否继续

void whowin(int color)
{
	if (blackpiececount == 0)
	{
		MessageBox(hwnd, L"White win!", L"系统消息", MB_OK);
		return;
	}
	if (whitepiececount == 0)
	{
		MessageBox(hwnd, L"Black win!", L"系统消息", MB_OK);
		return;
	}
    int all = 1;
    for (int i = 0; i < 7; i++)
	    for (int j = 0; j < 7; j++)
			all *= gridinfo[i][j];
	if (all)
	{
		if (blackpiececount > whitepiececount)
		{
			MessageBox(hwnd, L"Black win!", L"系统消息", MB_OK);
			return;
		}
		else if (blackpiececount < whitepiececount)
		{
			MessageBox(hwnd, L"White win！", L"系统消息", MB_OK);
			return;
		}
	}
	bool cancolormove = 0;
	int x, y;
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
		{
			if (gridinfo[i][j] == 0 || gridinfo[i][j] == -color)
				continue;
			for (int s = 0; s < 24; s++)
			{
				x = i + delta[s][0];
				y = j + delta[s][1];
				if (!inmap(x, y))
					continue;
				if (gridinfo[x][y] == 0)
					cancolormove = 1;
			}
		}
	if (!cancolormove)
	{
		if (color == 1)
		{
			MessageBox(hwnd, L"无法落子，黑棋输了！", L"系统消息", MB_OK);
			return;
		}
		else
		{
			MessageBox(hwnd, L"无法落子，白棋输了！", L"系统消息", MB_OK);
			return;
		}
	}
}
//判断谁赢

inline bool inmap(int x, int y)
{
	if (x < 0 || x>6 || y < 0 || y>6)
		return false;
	return true;
}
//检查是否在棋盘内

inline bool MoveStep(int x, int y, int Direction)
{
	x = x + delta[Direction][0];
	y = y + delta[Direction][1];
	return inmap(x, y);
}//向direction移动，并检查是否越界

bool judgemove(int x0, int y0, int x1, int y1, int color)
{
	//if (color == 0)
		//return false;
	//if (x1 == -1)//无路可走就跳过
		//return true;
	if (!inmap(x0, y0) || !inmap(x1, y1))
		return false;
	if (gridinfo[x0][y0] != color)
		return false;
	if (gridinfo[x1][y1] != 0)
		return false;
	int dx, dy;
	dx = fabs(x0 - x1), dy = fabs(y0 - y1);
	if ((dx == 0 && dy == 0) || dx > 2 || dy > 2)
		return false;
	return true;
}
//检查落子是否合法

void move(int x0, int y0, int x1, int y1, int color)
{
	int dx, dy, x, y, dir, currcount = 0;
	dx = fabs(x0 - x1), dy = fabs(y0 - y1);
	if (dx == 2 || dy == 2)
		gridinfo[x0][y0] = 0;
	else
	{
		if (color == 1)
			blackpiececount++;
		else
			whitepiececount++;
	}
	gridinfo[x1][y1] = color;
	for (dir = 0; dir < 8; dir++)
	{
		x = x1 + delta[dir][0];
		y = y1 + delta[dir][1];
		if (!inmap(x, y))
			continue;
		if (gridinfo[x][y] == -color)
		{
			currcount++;
			gridinfo[x][y] = color;
		}
	}
	if (currcount)
	{
		if (color == 1)
		{
			blackpiececount += currcount;
			whitepiececount -= currcount;
		}
		else
		{
			blackpiececount -= currcount;
			whitepiececount += currcount;
		}
	}
}
//落子操作

void AIgo(int color)
{
	int beginPos[1000][2], possiblePos[1000][2], posCount = 0, choice, dir;
	int x0, y0, x1, y1;
	for (y0 = 0; y0 < 7; y0++)
		for (x0 = 0; x0 < 7; x0++)
		{
			if (gridinfo[x0][y0] != currbotcolor)
				continue;
			for (dir = 0; dir < 24; dir++)
			{
				x1 = x0 + del[dir][0];
				y1 = y0 + del[dir][1];
				if (!inmap(x1, y1))
					continue;
				if (gridinfo[x1][y1] != 0)
					continue;
				beginPos[posCount][0] = x0;
				beginPos[posCount][1] = y0;
				possiblePos[posCount][0] = x1;
				possiblePos[posCount][1] = y1;
				posCount++;
			}
		}
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
			searchgrid[0][i][j] = gridinfo[i][j];
	chose ch;
	ch = AI();
	startX = ch.ax;
	startY = ch.ay;
	resultX = startX + dx[ch.ad];
	resultY = startY + dy[ch.ad];

	if (!judgemove(startX, startY, resultX, resultY, currbotcolor))
	{
		int maxpiece = 0;
		int mark = 0;
		for (int i = 0; i < posCount; i++)
		{
			startX = beginPos[i][0];
			startY = beginPos[i][1];
			resultX = possiblePos[i][0];
			resultY = possiblePos[i][1];
			if (currbotcolor == 1)
			{
				int sum = 0;
				int x, y;
				for (int s = 0; s < 8; s++)
				{
					x = resultX + delta[s][0];
					y = resultY + delta[s][1];
					if (!inmap(x, y))
						continue;
					if (gridinfo[x][y] == -1)
						sum++;
				}
				if (sum > maxpiece)
				{
					maxpiece = sum;
					mark = i;
				}
			}
			else
			{
				int sum = 0;
				int x, y;
				for (int s = 0; s < 8; s++)
				{
					x = resultX + delta[s][0];
					y = resultY + delta[s][1];
					if (!inmap(x, y))
						continue;
					if (gridinfo[x][y] == 1)
						sum++;
				}
				if (sum > maxpiece)
				{
					maxpiece = sum;
					mark = i;
				}
			}
		}
		startX = beginPos[mark][0];
		startY = beginPos[mark][1];
		resultX = possiblePos[mark][0];
		resultY = possiblePos[mark][1];
	}
	//如果结果是非法输出选择贪心




	for (int i = 0; i < posCount; i++)
	{
		int X = beginPos[i][0];
		int Y = beginPos[i][1];
		int X1 = possiblePos[i][0];
		int Y1 = possiblePos[i][1];
		for (int i = 0; i < 7; i++)
			for (int j = 0; j < 7; j++)
				searchgrid[0][i][j] = gridinfo[i][j];
		makemove(X1, Y1, currbotcolor, 0);
		int sumpiece = 0;
		for (int i = 0; i < 7; i++)
			for (int j = 0; j < 7; j++)
			{
				if (searchgrid[0][i][j] == -currbotcolor)
					sumpiece++;
			}
		if (sumpiece == 0)
		{
			startX = X;
			startY = Y;
			resultX = X1;
			resultY = Y1;
		}
	}
	//当然，如果能一步都吃了最好
}
//AI行棋

int evaluate(int depth, int color)
{
	int ans1 = 0, ans2 = 0;
	for (int i = 0; i < 7; ++i)
		for (int j = 0; j < 7; ++j)
		{
			if (searchgrid[depth][i][j] == 0)
				continue;
			else if (searchgrid[depth][i][j] == color)
				ans1++;
			else
				ans2++;
		}
	if (ans2 == 0)
		return 1000000;
	if (ans1 > 24)
		return 1000000 + ans1 - ans2;
	if (ans2 > 24)
		return -1000000 + ans1 - ans2;
	return ans1 - ans2;
}
//估值函数

void makemove(int movex1, int movey1, int color, int depth)
{
	int movex2, movey2;
	for (int i = 0; i < 8; i++)
	{
		movex2 = movex1 + del[i][0];
		movey2 = movey1 + del[i][1];
		if (!inmap(movex2, movey2))
			continue;
		if (searchgrid[depth][movex2][movey2] == -color)
			searchgrid[depth][movex2][movey2] = color;
	}
}

int minmax(int d, int fx, int fy, int color, int depth, int alpha, int beta)
{
	for (int i = 0; i < 7; i++)
		for (int j = 0; j < 7; j++)
			searchgrid[depth][i][j] = searchgrid[depth - 1][i][j];
	int x = fx + dx[d];
	int y = fy + dy[d];
	makemove(x, y, -color, depth);
	if (d > 7)
		searchgrid[depth][fx][fy] = 0;
	int value = 0;
	int max = -10000000;
	int min = 10000000;

	if (depth <= 3)
	{
		for (int i = 0; i < 7; ++i)
		{
			for (int j = 0; j < 7; ++j)
			{
				if (searchgrid[depth][i][j] == color)
				{
					for (int k = 0; k < 24; ++k)
					{
						int tx = i + dx[k];
						int ty = j + dy[k];
						if (!inmap(tx, ty))
							continue;
						if (searchgrid[depth][tx][ty] != 0)
							continue;
						value = minmax(k, i, j, -color, depth + 1, min, max);
						if (value > max)
							max = value;
						if (value < min)
							min = value;
						if (color == currbotcolor)
						{
							if (max >= alpha)
								return max;
						}
						else
						{
							if (min <= beta)
								return min;
						}
					}
				}
			}
		}
		if (color == currbotcolor)
			return max; // 最大层给出最大值
		else
			return min; // 最小层给出最小值
	}
	else
	{
		value = evaluate(depth, currbotcolor); // 评估当前方局面
		return value; // 搜索到限定层后给出估值
	}
}

chose AI()
{
	int value = 0;
	int max = -10000000;
	int min = 10000000;
	int fd = 0, fx = 0, fy = 0;
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 7; j++)
		{
			if (gridinfo[i][j] == currbotcolor)
			{
				for (int d = 0; d < 24; ++d)
				{
					int tx = i + dx[d];
					int ty = j + dy[d];
					if (!inmap(tx, ty))
						continue;
					if (gridinfo[tx][ty] != 0)
						continue;

					value = minmax(d, i, j, -currbotcolor, 1, min, max);
					if (value > max)
					{
						max = value;
						fd = d;
						fx = i;
						fy = j;
					}
					if (value < min)
						min = value;

				}
			}
		}
	}
	chose ans;
	ans.ax = fx;
	ans.ay = fy;
	ans.ad = fd;
	return ans;
}