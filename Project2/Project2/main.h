#pragma once

//“Ç‚İ‚ñ‚¾•¶š‚ÌƒNƒ‰ƒX
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

