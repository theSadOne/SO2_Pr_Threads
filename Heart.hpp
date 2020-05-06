#pragma once

#include "global.hpp"
#include "Destination.hpp"
#include "Vein.hpp"

class Heart : public Destination {
	Coords pos;
	const int WIN_LINES {6};
	const int WIN_COLS {17};
	WINDOW* win {nullptr};
	Vein* inUpV {nullptr};
	Vein* inDownV {nullptr};
	Vein* outUpV {nullptr};
	Vein* outDownV {nullptr};
	mutex modifyableMtx;

public:
	Heart(Coords pos);
	~Heart();
	void setVeins(Vein* inUpV, Vein* inDownV, Vein* outUpV, Vein* rightDownV);
	void refresh();
	void addErythrocyte(Erythrocyte& erythrocyte);
	void interact(Erythrocyte& erythrocyte);
	void operator()(forward_list<Erythrocyte>* erythrocytes, mutex* erListMtx);
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
	lock_guard<mutex> lckm{modifyableMtx};
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

void Heart::addErythrocyte(Erythrocyte& erythrocyte) {
	lock_guard<mutex> lckm{modifyableMtx};
	erythrocyte.setVein(outUpV);
}

void Heart::interact(Erythrocyte& erythrocyte) {
	lock_guard<mutex> lckm{modifyableMtx};
	Vein* vein = erythrocyte.getVein();
	if (vein->getId() == inUpV->getId()) erythrocyte.setVein(outDownV);
	else erythrocyte.setVein(outUpV);
}

void Heart::operator()(forward_list<Erythrocyte>* erythrocytes, mutex* erListMtx){
	synch_mvwprintw(win, 1, 1, Color::DEFAULT, "loading blood");

	unique_lock<mutex> lckErL{*erListMtx};
	auto it = erythrocytes->begin();
	while(true) {
		if (!lckErL) lckErL.lock();
		if (it == erythrocytes->end()) break;
		Erythrocyte* er = &(*it);
		++it;
		lckErL.unlock();

		addErythrocyte(*er);

		{
			lock_guard<mutex> lck {endThreadsMtx};
			if (endThreads) break;
		}
	}
	
	if (lckErL) lckErL.unlock();

	synch_wClearLine(win, 1, 1, WIN_COLS - 1);
	synch_mvwprintw(win, 1, 1, Color::DEFAULT, "loading ended");
}

Coords Heart::outUpVPos() {
	lock_guard<mutex> lckm{modifyableMtx};
	return Coords{pos.line + 1, pos.col + WIN_COLS};
}

Coords Heart::outDownVPos() {
	lock_guard<mutex> lckm{modifyableMtx};
	return Coords{pos.line + 4, pos.col + WIN_COLS};
}