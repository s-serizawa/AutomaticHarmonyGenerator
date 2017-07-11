#pragma once

//読み込んだ文字のクラス
class detectedText
{
private:
	int x1;
	int x2;
	int y1;
	int y2;
	int step; //---- 五線の何段目か
	float conf;
	std::string text;
public:
	detectedText(int x1, int y1, int x2, int y2, float conf, std::string text);
	//void setPos(int x1, int y1, int x2, int y2);
	int getX1();
	int getX2();
	int getY1();
	int getY2();
	int getStep();
	float getConf();
	std::string getText();
	void setStep(int _step);
	//~detectedText();
};

detectedText::detectedText(int x1, int y1, int x2, int y2, float conf, std::string text) {
	detectedText::x1 = x1;
	detectedText::y1 = y1;
	detectedText::x2 = x2;
	detectedText::y2 = y2;
	detectedText::conf = conf;
	detectedText::text = text;
}

int detectedText::getX1() {
	return detectedText::x1;
}

int detectedText::getX2() {
	return detectedText::x2;
}

int detectedText::getY1() {
	return detectedText::y1;
}

int detectedText::getY2() {
	return detectedText::y2;
}

int detectedText::getStep() {
	return detectedText::step;
}

float detectedText::getConf() {
	return detectedText::conf;
}

std::string detectedText::getText() {
	return detectedText::text;
}

void detectedText::setStep(int _step){
	this->step = _step;
}

//読み込んだ五線のクラス
class linesInfo {
private:
	std::vector<int> lines_y; //五線の高さ
	int lines_left;//五線の左端
	int lines_right;//五線の右端
	int lines_interval;//五線の線同士の間隔
	int steps_num;//五線が何段あるか
public:
	linesInfo(std::vector<int> y, int left, int right, int interval, int steps_num);
	std::vector<int> getLinesY();
	int getLinesLeft();
	int getLinesRight();
	int getLinesInterval();
	int getStepsNum();
};

linesInfo::linesInfo(std::vector<int> y, int left, int right, int interval, int steps_num) {
	linesInfo::lines_y = y;
	linesInfo::lines_left = left;
	linesInfo::lines_right = right;
	linesInfo::lines_interval = interval;
	linesInfo::steps_num = steps_num;
}

std::vector<int> linesInfo::getLinesY(){
	return linesInfo::lines_y;
}

int linesInfo::getLinesLeft() {
	return linesInfo::lines_left;
}

int linesInfo::getLinesRight() {
	return linesInfo::lines_right;
}

int linesInfo::getLinesInterval() {
	return linesInfo::lines_interval;
}

int linesInfo::getStepsNum() {
	return linesInfo::steps_num;
}

enum NoteType
{
	QUARTER = 0,
	HALF = 1,
};

class noteInfo {
private:
	int pos_x;
	int pos_y;
	int scale; //---- 音高
	int step;//何段目の五線か 最初の五線を0とする
	std::string chordname = ""; //---- その音符の乗ってるコードネーム
	std::vector<int> scalenotes; //---- そのコードネームに対応するスケール
	NoteType note_type;
	int harmonic_tone;

public:
	noteInfo(int x, int y, int sc, int st, NoteType nt);
	int getPosX();
	int getPosY();
	int getScale();
	int getStep();
	std::string getChordname();
	std::vector<int> getScalenotes();
	void setChordname(std::string _chord_name);
	void setScalenotes(std::vector<int> _scale_notes);
	int getHarmonicTone();
	void setHarmonicTone(int h_tone);
	NoteType getNoteType();

	static bool noteInfo::compareNote(noteInfo left, noteInfo right) {
		if (left.step < right.step) { return true; }
		else if (left.step > right.step) { return false; }
		else if (left.pos_x < right.pos_x) { return true; }
		else return false;
	};
};

noteInfo::noteInfo(int x, int y, int sc, int st, NoteType nt) {
	noteInfo::pos_x = x;
	noteInfo::pos_y = y;
	noteInfo::scale = sc;
	noteInfo::step = st;
	noteInfo::note_type = nt;
}

int noteInfo::getPosX(){
	return pos_x;
}

int noteInfo::getPosY(){
	return pos_y;
}

int noteInfo::getScale(){
	return scale;
}

int noteInfo::getStep() {
	return step;
}

std::string noteInfo::getChordname() {
	return chordname;
}

std::vector<int> noteInfo::getScalenotes() {
	return scalenotes;
}

int noteInfo::getHarmonicTone() {
	return harmonic_tone;
}

void noteInfo::setChordname(std::string _chord_name) {
	this->chordname = _chord_name;
}

void noteInfo::setScalenotes(std::vector<int> _scale_notes) {
	this->scalenotes = _scale_notes;

}

void noteInfo::setHarmonicTone(int h_tone) {
	this->harmonic_tone = h_tone;
};

NoteType noteInfo::getNoteType() {
	return note_type;
}

