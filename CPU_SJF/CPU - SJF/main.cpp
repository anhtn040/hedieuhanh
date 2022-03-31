#include <graphics.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#define INP "input.txt"
#define dong 20
using namespace std;

struct process {
	char name[10];
	int timeRL, timeCPU;
	int timeOUT, timeIN, timewait, timesave;
	int index; 
};
double avg = 0;
typedef process *ListP;
typedef process TT;

void output(TT t[], TT tt[], int n); // out KQ ra console
void outputGRP(TT tt[], TT t[], int n); //out KQ ra graaphics
void table_input(TT t[], int n, int x1, int y1, int x2, int y2); //Bang intput
void table_output(TT t[], int n, int x1, int y1, int x2, int y2); //bang output
void inittable(int n, int x1, int y1, int x2, int y2); //tao bang
void veKhungNutmode(bool Left, bool reset);

void About(){
	MessageBox(NULL, 
	"Thuat toan SJF\n"
	"Sinh vien: Tham Ngoc Anh \n"
	"MSSV: N19DCCN012 \n"
	"Di chuyen: Phim ->, <- \n"
	"Thoat: ESC",
	"About", MB_APPLMODAL | MB_ICONINFORMATION);
}

void input(ListP &pr, int &n, int &timeOUT) {
	timeOUT = 0;
	ifstream in(INP);
	if (in == NULL) {
		cout << "Not input file !";
		return;
	}
	in >> n;
	pr = new process[n];
	for (int i = 0; i < n; i++) {
		in >> pr[i].name;
		in >> pr[i].timeRL;
		in >> pr[i].timeCPU;
		if (timeOUT < pr[i].timeRL)
			timeOUT = pr[i].timeRL + pr[i].timeCPU;
		else
			timeOUT += pr[i].timeCPU;
		pr[i].index = i;
	}
}

void output_input(ListP pr, int n) {
	cout << endl << "---------------INPUT---------------" << endl << endl;
	cout << "Name" << setw(10) << "TimeRL" << setw(10) << "TimeCPU"<<endl;
	for (int i = 0; i < n; i++)
		cout << pr[i].name << setw(10) << pr[i].timeRL << setw(10)
				<< pr[i].timeCPU <<  endl;
	cout << endl << "---------------OUTPUT---------------" << endl << endl;
}

void process_SJF_nopreemptive(ListP &pr, ListP &RL, int n, int &m, int timeOUT) {
	RL = new process;
	ListP pr1 = pr; 
	process temp;
	int j = 0;
	m = 0;
	int temptime = 0;
	for (int t = 0; t <= timeOUT; t++) {
		if (m > 0 && j < m) {
			if (temptime < RL[j].timeCPU)
				temptime++;
			if (temptime == RL[j].timeCPU) {
				RL[j].timeIN = t - RL[j].timeCPU;
				RL[j].timeOUT = RL[j].timeIN + RL[j].timeCPU;
				RL[j].timewait = RL[j].timeOUT - (RL[j].timeRL + RL[j].timeCPU);
				RL[j].timesave = RL[j].timeOUT - RL[j].timeRL;

				pr1[RL[j].index].timeOUT = t;
				pr1[RL[j].index].timewait = pr1[RL[j].index].timeOUT
						- (pr1[RL[j].index].timeRL + pr1[RL[j].index].timeCPU);
				pr1[RL[j].index].timesave = pr1[RL[j].index].timeOUT - pr1[RL[j].index].timeRL;

				temptime = 0;
				j++;
			}
		}
		for (int i = 0; i < n; i++)
			if (t == pr1[i].timeRL) {
				m++;
				int k = m - 1;
				RL = (process *) realloc(RL, m * sizeof(process));
				if (temptime > 0 && pr1[i].timeCPU < RL[j].timeCPU - temptime) {
					m++;
					k = m - 1;
					RL = (process *) realloc(RL, m * sizeof(process));
					for (k = m - 1; k > j + 1; k--)
						RL[k] = RL[k - 2];
					RL[j + 1] = pr1[i];

					RL[j + 2] = RL[j];
					RL[j + 2].timeCPU -= temptime;

					RL[j].timeIN = t - temptime;
					RL[j].timeOUT = t;
					RL[j].timeCPU = temptime;
					temptime = 0;
					j++;
				} else {
					while (k > j + 1 && pr1[i].timeCPU < RL[k - 1].timeCPU) {
						RL[k] = RL[k - 1];
						k--;
						if (k == j + 1
								&& pr1[i].timeCPU < RL[k - 1].timeCPU - temptime) {
							RL[k] = RL[k - 1];
							k--;
						}
					}
					RL[k] = pr1[i];
				}
			}
	}
	double tmp;
	for(int i = 0; i < n; i++){
		tmp += pr[i].timewait;
	}
	avg = tmp/n;
}

void process_SJF_preemptive(ListP &pr, int n, int timeOUT) {
	ListP RL = new process[n];
	ListP pr1 = pr; 
	int j = 0, m = 0;
	int temptime = 0;
	for (int t = 0; t <= timeOUT; t++) {
		if (m > 0 && j < m) {
			if (temptime < RL[j].timeCPU)
				temptime++;
			if (temptime == RL[j].timeCPU) {
				RL[j].timeIN = t - RL[j].timeCPU;
				RL[j].timeOUT = RL[j].timeIN + RL[j].timeCPU;
				RL[j].timewait = RL[j].timeOUT - (RL[j].timeRL + RL[j].timeCPU);
				RL[j].timesave = RL[j].timeOUT - RL[j].timeRL;
				temptime = 0;
				j++;
			}
		}
		for (int i = 0; i < n; i++)
			if (t == pr1[i].timeRL) {
				int k = m;
				while (k > j + 1 && pr1[i].timeCPU < RL[k - 1].timeCPU) {
					RL[k] = RL[k - 1];
					k--;
				}
				RL[k] = pr1[i];
				m++;
			}
	}
	pr = RL;
	double tmp;
	for(int i = 0; i < n; i++){
		tmp += pr[i].timewait;
	}
	avg = tmp/n;
}

void output_SJF_nopreemptive(ListP pr, ListP RL, int n, int m, int timeOUT) {
	cout << "SJF nopreemptive" << endl << endl << "OUTPUT" << endl << endl;
	cout << "Name" << setw(10) << "TimeOUT" << setw(10) << "Timewait"
			<< setw(10) << "Timesave" << endl;
	for (int i = 0; i < n; i++)
		cout << pr[i].name << setw(10) << pr[i].timeOUT << setw(10)
				<< pr[i].timewait << setw(10) << pr[i].timesave << endl;
	cout<<"Time average: "<<avg;

	cout << endl << endl << "---PROCESS---" << endl << endl;

	cout << "Name" << setw(10) << "TimeRL" << setw(10) << "TimeCPU" << setw(10)
			 << "TimeIN" << setw(10) << "TimeOUT" << endl;
	for (int i = 0; i < m; i++)
		cout << RL[i].name << setw(10) << RL[i].timeRL << setw(10)
				<< RL[i].timeCPU << setw(10) 
				<< RL[i].timeIN << setw(10) << RL[i].timeOUT << endl;
	
}

void output_SJF_preemptive(ListP pr, int n, int timeOUT) {
	cout << "SJF preemptive" << endl << endl << "PROCESS" << endl << endl;
	cout << "Name" << setw(10) << "TimeRL" << setw(10) << "TimeCPU" << setw(10) << "TimeIN" << setw(10) << "TimeOUT"
			<< setw(10) << "Timewait" << setw(10) << "Timesave" << endl;
	for (int i = 0; i < n; i++)
		cout << pr[i].name << setw(10) << pr[i].timeRL << setw(10)
				<< pr[i].timeCPU << setw(10) << pr[i].timeIN << setw(10) << pr[i].timeOUT << setw(10)
				<< pr[i].timewait << setw(10) << pr[i].timesave << endl;
}

void inittable(int n, int x1, int y1, int x2, int y2) {
	setfillstyle(1,8);
	settextjustify(1, 1);
	int zx = (x2 - x1) / 3;
	int zy;
	if (n < 3)
		zy = dong * 2;
	else
		zy = (y2 - y1) / (n + 1);

	bar(x1 + 2, y1 - zy, x2 - 2, y1 - 2);

	for (int i = 0; i <= n; i++)
		for (int j = 0; j < 3; j++)
			bar(x1 + j * zx + 2, y1 + i * zy + 2, x1 + (j + 1) * zx,
					y1 + (i + 1) * zy);
}

void table_input(TT t[], int n, int x1, int y1, int x2, int y2) {
	setfillstyle(1, 8);
	settextjustify(1, 1);
	setbkcolor(8);
	int zx = (x2 - x1) / 3;
	int zy;
	if (n < 3)
		zy = dong * 2;
	else
		zy = (y2 - y1) / (n + 1);

	inittable(n, x1, y1, x2, y2);

	outtextxy(x1 + 1 * zx + 2 + zx / 2, y1 - zy / 2, "INPUT");

	for (int i = 0; i <= n; i++)
		for (int j = 0; j < 3; j++) {
			char *s = (char*) malloc(50 * sizeof(char));
			switch (j) {
			case 0:
				if (i == 0)
					strcpy(s, "Process");
				else
					sprintf(s, "%s", t[i - 1].name);
				break;
			case 1:
				if (i == 0)
					strcpy(s, "Time RL");
				else
					sprintf(s, "%d", t[i - 1].timeRL);
				break;
			case 2:
				if (i == 0)
					strcpy(s, "Time CPU");
				else
					sprintf(s, "%d", t[i - 1].timeCPU);
				break;
			}
			outtextxy(x1 + j * zx + 2 + zx / 2, y1 + i * zy + 5 + zy / 2, s);
		}
}

void inittable2(int n, int x1, int y1, int x2, int y2) {
	setfillstyle(1, 8);
	settextjustify(1, 1);
	int zx = (x2 - x1) / 3;
	int zy;
	if (n < 4)
		zy = dong * 2;
	else
		zy = (y2 - y1) / (n + 1);

	bar(x1 + 2, y1 - zy, x2 + 165, y1 - 2);

	for (int i = 0; i <= n; i++)
		for (int j = 0; j < 4; j++)
			bar(x1 + j * zx + 2, y1 + i * zy + 2, x1 + (j + 1) * zx,
					y1 + (i + 1) * zy);
}

void table_output2(TT t[], int n, int x1, int y1, int x2, int y2) {
	setfillstyle(1, 8);
	settextjustify(1, 1);
	setbkcolor(8);
	int zx = (x2 - x1) / 3;
	int zy;
	if (n < 3)
		zy = dong * 2;
	else
		zy = (y2 - y1) / (n + 1);

	inittable2(n, x1, y1, x2, y2);

	outtextxy(x1 + 1 * zx + 95 + zx / 2, y1 - zy / 2, "PROCESS");
	int i,j;
	for (i = 0; i <= n; i++)
		for (j = 0; j < 4; j++) {
			char *s = (char*) malloc(50 * sizeof(char));
			switch (j) {
			case 0:
				if (i == 0)
					strcpy(s, "Process");
				else
					sprintf(s, "%s", t[i - 1].name);
				break;
			case 1:
				if (i == 0)
					strcpy(s, "Time IN");
				else
					sprintf(s, "%d", t[i - 1].timeIN);
				break;
			case 2:
				if (i == 0)
					strcpy(s, "Time OUT");
				else
					sprintf(s, "%d", t[i - 1].timeOUT);
				break;
			case 3:
				if (i == 0)
					strcpy(s, "Time WAIT");
				else
					sprintf(s, "%d", t[i - 1].timewait);
				break;
			}
			outtextxy(x1 + j * zx + 2 + zx / 2, y1 + i * zy + 5 + zy / 2, s);
		}
	char *c = (char*) malloc(50 * sizeof(char));
	sprintf(c,"Time Average: %0.2f",avg);

	bar(x1 + 0 * zx + 2, y1 + i * zy + 2, x1 + (j-2) * zx, y1 + (i + 1) * zy);
	outtextxy(x1 + 0 * zx + 80 + zx / 2, y1 + i * zy + 5 + zy / 2, c);
	setbkcolor(0);
}

void table_output(TT t[], int n, int x1, int y1, int x2, int y2) {
	setfillstyle(1, 8);
	settextjustify(1, 1);
	setbkcolor(8);
	int zx = (x2 - x1) / 3;
	int zy;
	if (n < 3)
		zy = dong * 2;
	else
		zy = (y2 - y1) / (n + 1);

	inittable(n, x1, y1, x2, y2);
	outtextxy(x1 + 1 * zx + 2 + zx / 2, y1 - zy / 2, "PROCESS");
	
	int i,j;
	for (i = 0; i <= n ; i++)
		for (j = 0; j < 3; j++) {
			char *s = (char*) malloc(50 * sizeof(char));
			switch (j) {
			case 0:
				if (i == 0)
					strcpy(s, "Process");
				else
					sprintf(s, "%s", t[i - 1].name);
				break;
			case 1:
				if (i == 0)
					strcpy(s, "Time OUT");
				else
					sprintf(s, "%d", t[i - 1].timeOUT);
				break;
			case 2:
				if (i == 0)
					strcpy(s, "Time WAIT");
				else
					sprintf(s, "%d", t[i - 1].timewait);
				break;
			}
			outtextxy(x1 + j * zx + 2 + zx / 2, y1 + i * zy + 5 + zy / 2, s);
		}

	char *c = (char*) malloc(50 * sizeof(char));
	sprintf(c,"Time Average: %0.2f",avg);

	bar(x1 + 0 * zx + 2, y1 + i * zy + 2, x1 + (j-1) * zx, y1 + (i + 1) * zy);
	outtextxy(x1 + 0 * zx + 80 + zx / 2, y1 + i * zy + 5 + zy / 2, c);
	setbkcolor(0);	
}

void outputGRP(TT tt[], TT t[] , int n, int m, bool mode) {
	if(mode == 1) {
		int x = 200, y = 200, khoangcach = 15;
		int tile = 1100 / t[m - 1].timeOUT;
		int z = tile * t[m - 1].timeOUT;
		int xI[m], xO[m], xRL[m];
		int yI = y - 2 * dong;
		int yO = y + 5 * dong;
		int yT = y - dong;
		int yRL = y + 4 * dong;
		for (int i = 0; i < m; i++)
			xRL[i] = x + t[i].timeRL * tile;
	
		setbkcolor(0);
		
		setcolor(15);
		setfillstyle(1, WHITE);
		settextstyle(8, HORIZ_DIR, 2);
		settextjustify(1, 1);
	
		setcolor(14);
		for (int i = 0; i < 3; i++)
			rectangle(x - i, y - i, x + z + i, y + 2 * dong + i);
		bar(x, y, x + z, y + 2 * dong);
	
		
		settextstyle(8, HORIZ_DIR, 2);
		int xt1 = 200, yt1 = 400, dai = 500, rong = 250;
		table_input(tt, n, xt1, yt1, xt1 + dai, yt1 + rong);
		table_output(tt, n, xt1 + 30 + dai, yt1, xt1 + 30 + 2 * dai, yt1 + rong);
		settextstyle(8, HORIZ_DIR, 2);
		outtextxy(x - 80, yI, "Input --->");
		outtextxy(x - 80, yO, "Output --->");
		outtextxy(x - 80, yT, "Time --->");
		outtextxy(x - 80, yRL, "Time RL --->");
		outtextxy(x - 80, y + dong, "Process --->");
	
		settextstyle(8, HORIZ_DIR, 2);
		delay(1000);
	
		char *s = (char*) malloc(sizeof(char));
	
		xI[0] = x + t[0].timeIN * tile;
		xO[0] = x + t[0].timeOUT * tile;
		int yI1 = yI, yO1 = yO, yT1 = yT, xm = xI[0];
	
	
		for (int i = 0; i < m; i++) {
			if (i > 0) {
				if (xRL[i] < xO[i - 1])
					xI[i] = xO[i - 1];
				else
					xI[i] = xRL[i];
				xO[i] = xI[i] + t[i].timeCPU * tile;
			}
	
			if (i > 0 && xO[i - 1] - xm >= khoangcach)
				xm = xO[i - 1];
			if (i > 0 && xO[i - 1] - xm != 0) {
				yI1 = yI1 - 2 * dong;
				yT1 = yT1 - 2 * dong;
			} else {
				yI1 = yI;
				yT1 = yT;
			}
	
			outtextxy(xI[i], yI1, t[i].name);
			sprintf(s, "%d", t[i].timeIN);
			outtextxy(xI[i], yT1, s);
			outtextxy(xRL[i], yRL, t[i].name);
	
			int j;
			setfillstyle(1, i + 2);
			for (j = 0; j < t[i].timeCPU; j++) {
				delay(500);
				bar(xI[i], y, xI[i] + (j + 1) * tile, y + 2 * dong);
				for (int k = 0; k < n; k++)
					if (xI[i] + (j + 1) * tile > xRL[k]) {
						sprintf(s, "%d", t[k].timeRL);
						outtextxy(xRL[k], yT, s);
	
						outtextxy(xRL[k], yRL, t[k].name);
						line(xRL[k], y + 2 * dong, xRL[k], y + 3 * dong);
					}
			}
			sprintf(s, "%d", t[i].timeOUT);
			outtextxy(xO[i], yT, s);
			line(xO[i], y, xO[i], y + 2 * dong);
			if (xO[i] - xm <= khoangcach)
				yO1 = yO1 + dong;
			else
				yO1 = yO;
	
			outtextxy(xO[i], yO1, t[i].name);
		}
		sprintf(s, "%d", t[n - 1].timeOUT);
		outtextxy(xO[n - 1], yT, s);
	} else if (mode == 0){
		int x = 200, y = 200, khoangcach = 15;
	int tile = 1100 / tt[n - 1].timeOUT;
	int z = tile * tt[n - 1].timeOUT;
	int xI[n], xO[n], xRL[n];
	int yI = y - 2 * dong;
	int yO = y + 5 * dong;
	int yT = y - dong;
	int yRL = y + 4 * dong;
	for (int i = 0; i < n; i++)
		xRL[i] = x + tt[i].timeRL * tile;

	setbkcolor(0);

	setcolor(15);
	setfillstyle(1, WHITE);
	settextstyle(8, HORIZ_DIR, 2);

	settextjustify(1, 1);

	setcolor(14);
	for (int i = 0; i < 3; i++)
		rectangle(x - i, y - i, x + z + i, y + 2 * dong + i);
	bar(x, y, x + z, y + 2 * dong);

	settextstyle(8, HORIZ_DIR, 2);

	int xt1 = 200, yt1 = 400, dai = 500, rong = 250;
	table_input(tt, n, xt1, yt1, xt1 + dai, yt1 + rong);
	table_output2(tt, n, xt1 + 30 + dai, yt1, xt1 + 30 + 2 * dai, yt1 + rong);
    settextstyle(8, HORIZ_DIR, 2);
	outtextxy(x - 100, yI, "Input -->");
	outtextxy(x - 100, yO, "Output -->");
	outtextxy(x - 100, yT, "Time-->");
	outtextxy(x - 100, yRL, "Time RL -->");
	outtextxy(x - 100, y + dong, "Process -->");

	settextstyle(8, HORIZ_DIR, 2);
	delay(1000);

	char *s = (char*) malloc(sizeof(char));

	xI[0] = x + tt[0].timeIN * tile;
	xO[0] = x + tt[0].timeOUT * tile;
	int yI1 = yI, yO1 = yO, yT1 = yT, xm = xI[0];
	for (int i = 0; i < n; i++) {
		if (i > 0) {
			if (xRL[i] < xO[i - 1])
				xI[i] = xO[i - 1];
			else
				xI[i] = xRL[i];
			xO[i] = xI[i] + tt[i].timeCPU * tile;
		}

		if (i > 0 && xO[i - 1] - xm >= khoangcach)
			xm = xO[i - 1];
		if (i > 0 && xO[i - 1] - xm != 0) {
			yI1 = yI1 - 2 * dong;
			yT1 = yT1 - 2 * dong;
		} else {
			yI1 = yI;
			yT1 = yT;
		}

		outtextxy(xI[i], yI1, tt[i].name);
		sprintf(s, "%d", tt[i].timeIN);
		outtextxy(xI[i], yT1, s);
		outtextxy(xRL[i], yRL, tt[i].name);

		int j;
		setfillstyle(1, 3 + (9 + i) % 12);
		for (j = 0; j < tt[i].timeCPU; j++) {
			delay(500);
			bar(xI[i], y, xI[i] + (j + 1) * tile, y + 2 * dong);
			for (int k = 0; k < n; k++)
				if (xI[i] + (j + 1) * tile > xRL[k]) {
					sprintf(s, "%d", tt[k].timeRL);
					outtextxy(xRL[k], yT, s);

					outtextxy(xRL[k], yRL, tt[k].name);
					line(xRL[k], y + 2 * dong, xRL[k], y + 3 * dong);
				}
		}
		sprintf(s, "%d", tt[i].timeOUT);
		outtextxy(xO[i], yT, s);
		line(xO[i], y, xO[i], y + 2 * dong);
		if (xO[i] - xm <= khoangcach)
			yO1 = yO1 + dong;
		else
			yO1 = yO;

		outtextxy(xO[i], yO1, tt[i].name);
	}
	sprintf(s, "%d", tt[n - 1].timeOUT);
	outtextxy(xO[n - 1], yT, s);
	}
	
}

void veKhungNutmode(bool Left, bool reset){
	int kichThuocSTT = 30;
	int kichThuocNut = 40;
	//tinh kich thuoc khung
	int L =	250;
	int U = 5;
	int R =	1300;
	int D =	55;
	//ve khung duoi
	setfillstyle(SOLID_FILL, 8);
	bar (L, D-kichThuocNut, R, D);
	// In info khung
	char a[20];
	char b[20];
	settextstyle(8, HORIZ_DIR, 2);
	settextjustify(1, 2);
	strcpy(a,"SJF preemptive");
	strcpy(b,"SJF nonpreemptive");
	// Vi trí 2 nut cuoi
	int midText1 = ((R-L)/2 - textwidth(a))/2 + 80;
	int midText2 = ((R-L)/2 - textwidth(b))/2 + 140;
	int midTextH = (kichThuocNut - textheight(a))/2 + textheight(b);
	setcolor(WHITE);
	// In 2 nut cuoi
	int MAU_NUT_HL = 15;
	if (reset)
			MAU_NUT_HL = 8;
		
	setfillstyle(SOLID_FILL, MAU_NUT_HL);
	if (Left){
		bar (L, D-kichThuocNut, (L+R)/2, D);
		setbkcolor(MAU_NUT_HL);
		setcolor(WHITE);
		outtextxy(L+midText1, D-midTextH, a);
		setbkcolor(8);
		outtextxy((L+R)/2+midText2, D-midTextH, b);
	} else{
		bar ((L+R)/2, D-kichThuocNut, R, D);
		setcolor(WHITE);
		setbkcolor(8);
		outtextxy(L+midText1, D-midTextH, a);
		setbkcolor(MAU_NUT_HL);
		outtextxy((L+R)/2+midText2, D-midTextH, b);
	}
	setcolor(BLACK);
	rectangle(L,D-kichThuocNut,(L+R)/2,D);
	rectangle((L+R)/2,D-kichThuocNut,R,D);					
	setbkcolor(0);
}

void PROCESS(ListP &pr, ListP &RL, int n, int &m, int timeOUT, bool select) {
	switch (select) {
	case 0:
		process_SJF_preemptive(pr, n, timeOUT);
		break;
	case 1:
		process_SJF_nopreemptive(pr, RL, n, m, timeOUT);
		break;
	}
}

void Chontumenu(ListP pr, ListP RL, int n, int &m, int timeOUT){	
	initwindow(1600, 900);
	cleardevice();
	About();
	bool mode = 1;
	veKhungNutmode(1, 0);
	PROCESS(pr, RL, n, m, timeOUT, mode);
	outputGRP(pr, RL, n, m, mode);
	while(1){
		if(kbhit())	{
			char key = getch();
			if (key == 0){
				char nextK = getch();
				switch(nextK){
					case KEY_LEFT:{
							mode ^= 1;
							cleardevice();
							veKhungNutmode(mode, 0);
							input(pr, n, timeOUT);
							PROCESS(pr, RL, n, m, timeOUT, mode);
							outputGRP(pr, RL, n, m, mode);
							break;
					}
					case KEY_RIGHT: {
							mode ^= 1;
							cleardevice();
							veKhungNutmode(mode, 0);
							input(pr, n, timeOUT);
							PROCESS(pr, RL, n, m, timeOUT, mode);
							outputGRP(pr, RL, n, m, mode);
							break;
					}
				}
			}
			else if (key == 27){ 
				closegraph();
			}
			else if (key == '\r'){
				return;
				
			}
		}
	}
}

int main() {
	ListP pr, RL;
	int n, m, timeOUT;
	input(pr, n, timeOUT);
	output_input(pr, n);
	Chontumenu(pr, RL, n, m, timeOUT);
	return 0;
}


