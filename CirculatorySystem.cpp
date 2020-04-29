#include <ncurses.h>
#include <stdio.h>
#include <thread>
#include <mutex>
#include <chrono>
#include <random>
using namespace std;

const int ESC = 27;

const int WIN_LINES = 8;
const int WIN_COLS = 20;

const int DEFAULT_COLOR = 0;

const int TASK_TIME_LB = 2500;
const int TASK_TIME_UB = 3500;

mutex printMtx;		//do synchronizacji pisania na ekran

default_random_engine generator;

int randomTime(int a, int b) {		//zwraca liczbe calkowita losowa z przedzialu [a,b]
	uniform_int_distribution<int> distribution(a, b);
	return distribution(generator);
}

void initColors() {					//inicjalizuje pary kolorow
	init_pair(DEFAULT_COLOR, COLOR_WHITE, COLOR_BLACK);
	for (int i = 1; i < 8; ++i)
		init_pair(i, 0, i);
	init_pair(EATING_COLOR, COLOR_GREEN, COLOR_BLACK);
	init_pair(SLEEPING_COLOR, COLOR_CYAN, COLOR_BLACK);
	init_pair(THINKING_COLOR, COLOR_MAGENTA, COLOR_BLACK);
}

/* wClearLine, mvwprintw, mvwaddch synchronizowane przez blokowanie mutex printMtx */
void synch_wClearLine(WINDOW* win, int line, int start = 1, int stop = WIN_COLS - 1) {
	lock_guard<mutex> lck {printMtx};
	for (int i = start; i < stop; ++i) {
		mvwaddch(win, line, i, ' ');
	}
}

void synch_mvwprintw(WINDOW* win, int line, int col, int color, const char* fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char buffer[30];
	vsnprintf(buffer, 30, fmt, args);

	lock_guard<mutex> lck {printMtx};
	wattron(win, COLOR_PAIR(color));
	mvwprintw(win, line, col, buffer);
	wattroff(win, COLOR_PAIR(color));
}

void synch_mvwaddch(WINDOW* win, int line, int col, char ch) {
	lock_guard<mutex> lck {printMtx};
	mvwaddch(win, line, col, ch);
}
/* synchronizowane wClearLine, mvwprintw, mvwaddch koniec */



int main(int argc, char* argv[])
{
	initscr();						//inicjalizacja terminala curses
	start_color();					//uruchomienie obslugi kolorow
	noecho();						//nie wyswietlaj wciskanych klawiszy
	curs_set(0);					//nie wyswietlaj kursora
	keypad(stdscr, true);			//obsluga klawiszy specjalnych
	timeout(25);					//timeout dla getch()

	initColors();



	endwin();

	return 0;
}
