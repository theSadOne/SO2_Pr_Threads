#pragma once

#include "global.hpp"
#include "Destination.hpp"
#include "Vein.hpp"
#include "Erythrocyte.hpp"
#include "Leukocyte.hpp"

class Heart : public Destination {
	Coords pos;
	const int WIN_LINES {6};
	const int WIN_COLS {17};
	WINDOW* win {nullptr};
	Vein* inUpV {nullptr};
	Vein* inDownV {nullptr};
	Vein* outUpV {nullptr};
	Vein* outDownV {nullptr};
	mutex accessMtx;
	const int PERIOD {500};

public:
	Heart(Coords pos);
	~Heart();
	void setVeins(Vein* inUpV, Vein* inDownV, Vein* outUpV, Vein* rightDownV);
	void refresh();
	void addBloodCell(Erythrocyte& erythrocyte);
	void addBloodCell(Leukocyte& leukocyte);
	void interact(Erythrocyte& erythrocyte);
	void interact(Leukocyte& leukocyte);
	void operator()(forward_list<Erythrocyte>* erythrocytes, mutex* erListMtx,
					forward_list<Leukocyte>* leukocytes, mutex* leukListMtx);
	Coords outUpVPos();
	Coords outDownVPos();
};

Heart::Heart(Coords pos): pos(pos) {
	win = newwin(WIN_LINES, WIN_COLS, pos.line, pos.col);
	box(win, 0, 0);
	mvwprintw(win, 0, 0, "Heart");
}

Heart::~Heart() {
	delwin(win);
}

void Heart::setVeins(Vein* inUpV, Vein* inDownV, Vein* outUpV, Vein* outDownV){
	lock_guard<mutex> lckm{accessMtx};
	this->inUpV = inUpV;
	this->inDownV = inDownV;
	this->outUpV = outUpV;
	this->outDownV = outDownV;
}

void Heart::refresh() {
	box(win, 0, 0);
	mvwprintw(win, 0, 0, "Heart");
	wrefresh(win);
}

void Heart::addBloodCell(Erythrocyte& erythrocyte) {
	lock_guard<mutex> lckm{accessMtx};
	erythrocyte.setVein(outUpV);
}

void Heart::addBloodCell(Leukocyte& leukocyte) {
	lock_guard<mutex> lckm{accessMtx};
	leukocyte.setVein(outUpV);
}

void Heart::interact(Erythrocyte& erythrocyte) {
	lock_guard<mutex> lckm{accessMtx};
	Vein* vein = erythrocyte.getVein();
	if (vein == inUpV) erythrocyte.setVein(outDownV);
	else erythrocyte.setVein(outUpV);
}

void Heart::interact(Leukocyte& leukocyte) {
	lock_guard<mutex> lckm{accessMtx};
	Vein* vein = leukocyte.getVein();
	if (vein == inUpV) leukocyte.setVein(outDownV);
	else leukocyte.setVein(outUpV);
}

void Heart::operator()(forward_list<Erythrocyte>* erythrocytes, mutex* erListMtx,
						forward_list<Leukocyte>* leukocytes, mutex* leukListMtx){
	synch_mvwprintw(win, 1, 1, Color::DEFAULT, "loading blood");

	double erProb = 1;
	if (ER_COUNT > 0 && LEUK_COUNT > 0) {
		erProb = (double) ER_COUNT / (ER_COUNT + LEUK_COUNT);
	} else if (ER_COUNT > 0) {
		erProb = 1;
	} else {
		erProb = 0;
	}

	unique_lock<mutex> lckErL{*erListMtx};
	auto ite = erythrocytes->begin();
	unique_lock<mutex> lckLeukL{*leukListMtx};
	auto itl = leukocytes->begin();
	while(true) {	//petla dodawania krwinek
		if (outUpV->isAvailable()) {	//jezeli zyla wyjsciowa jest wolna - mozna dodac krwinke
			if (!lckErL) lckErL.lock();
			if (!lckLeukL) lckLeukL.lock();
			if (ite == erythrocytes->end() && itl == leukocytes->end()) break;	//jezeli koniec tablic krwinek
																				//zakoncz etap dodawania

			if ((random01() <= erProb || itl == leukocytes->end()) && ite != erythrocytes->end()) {
				Erythrocyte* er = &(*ite);
				++ite;
				erProb -= (double) 1 / (ER_COUNT + LEUK_COUNT);
				lckErL.unlock();
				lckLeukL.unlock();
				addBloodCell(*er);
			} else if (itl != leukocytes->end()) {
				Leukocyte* leuk = &(*itl);
				++itl;
				erProb += (double) 1 / (ER_COUNT + LEUK_COUNT);
				lckErL.unlock();
				lckLeukL.unlock();
				addBloodCell(*leuk);
			}
		}

		if (lckErL) lckErL.unlock();
		if (lckLeukL) lckLeukL.unlock();

		beatcv.notify_all();	//sygnal ruchu dla krwinek
		this_thread::sleep_for(chrono::milliseconds(PERIOD));

		{
			lock_guard<mutex> lcke {endThreadsMtx};
			if (endThreads) break;
		}
		
	}
	
	if (lckErL) lckErL.unlock();
	if (lckLeukL) lckLeukL.unlock();

	synch_wClearLine(win, 1, 1, WIN_COLS - 1);
	synch_mvwprintw(win, 1, 1, Color::DEFAULT, "beating");
	
	while(true) {	//petla bicia serca
		{
			lock_guard<mutex> lcke {endThreadsMtx};
			if (endThreads) break;
		}
		beatcv.notify_all();	//sygnal ruchu dla krwinek

		synch_mvwprintw(win, 3, WIN_COLS / 2 - 1, Color::DEFAULT, "***");
		this_thread::sleep_for(chrono::milliseconds(PERIOD / 2));
		synch_wClearLine(win, 3, 1, WIN_COLS - 1);
		this_thread::sleep_for(chrono::milliseconds(PERIOD / 2));
	}

	beatcv.notify_all();		//sygnal ruchu dla krwinek

	synch_wClearLine(win, 1, 1, WIN_COLS - 1);
	synch_mvwprintw(win, 1, 1, Color::DEFAULT, "ended");
}

Coords Heart::outUpVPos() {
	lock_guard<mutex> lckm{accessMtx};
	return Coords{pos.line + 1, pos.col + WIN_COLS};
}

Coords Heart::outDownVPos() {
	lock_guard<mutex> lckm{accessMtx};
	return Coords{pos.line + 4, pos.col + WIN_COLS};
}