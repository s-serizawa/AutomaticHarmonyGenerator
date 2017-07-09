#pragma once

//�ǂݍ��񂾕����̃N���X
class detectedText
{
private:
	int x1;
	int x2;
	int y1;
	int y2;
	float conf;
	std::string text;
public:
	detectedText(int x1, int y1, int x2, int y2, float conf, std::string text);
	//void setPos(int x1, int y1, int x2, int y2);
	int getX1();
	int getX2();
	int getY1();
	int getY2();
	float getConf();
	std::string getText();
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

float detectedText::getConf() {
	return detectedText::conf;
}

std::string detectedText::getText() {
	return detectedText::text;
}

//�ǂݍ��񂾌ܐ��̃N���X
class linesInfo {
private:
	std::vector<int> lines_y; //�ܐ��̍���
	int lines_left;//�ܐ��̍��[
	int lines_right;//�ܐ��̉E�[
	int lines_interval;//�ܐ��̐����m�̊Ԋu
	int  steps_num;//�ܐ������i���邩
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

class noteInfo {
private:
	int pos_x;
	int pos_y;
	int scale;
	int step;//���i�ڂ̌ܐ��� �ŏ��̌ܐ���0�Ƃ���
public:
	noteInfo(int x, int y, int sc, int st);
	int getPosX();
	int getPosY();
	int getScale();
	int getStep();
};

noteInfo::noteInfo(int x, int y, int sc, int st) {
	noteInfo::pos_x = x;
	noteInfo::pos_y = y;
	noteInfo::scale = sc;
	noteInfo::step = st;
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