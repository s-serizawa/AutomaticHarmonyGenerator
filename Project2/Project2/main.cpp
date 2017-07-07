#pragma warning(disable:4996)

#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include "main.h"


#ifdef _DEBUG
#pragma comment(lib, "libtesseract302d.lib")
#pragma comment(lib, "liblept168d.lib")
#else
#pragma comment(lib, "libtesseract302.lib")
#pragma comment(lib, "liblept168.lib")
#endif

//#include 
//#include 

std::string UTF8toSJIS(const char* src) {

	// UTF8 -> UTF16
	int lenghtUnicode = MultiByteToWideChar(CP_UTF8, 0, src, strlen(src) + 1, NULL, 0);
	wchar_t* bufUnicode = new wchar_t[lenghtUnicode];
	MultiByteToWideChar(CP_UTF8, 0, src, strlen(src) + 1, bufUnicode, lenghtUnicode);

	// UTF16 -> ShiftJis
	int lengthSJis = WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, -1, NULL, 0, NULL, NULL);
	char* bufShiftJis = new char[lengthSJis];
	WideCharToMultiByte(CP_THREAD_ACP, 0, bufUnicode, lenghtUnicode + 1, bufShiftJis, lengthSJis, NULL, NULL);

	std::string strSJis(bufShiftJis);

	delete bufUnicode;
	delete bufShiftJis;

	return strSJis;
}

void detectLines(cv::Mat image, cv::Mat original_image) {
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(image, lines, 4, CV_PI / 180.0 * 90, 1000, 400, 10);
		
	//Draw detected segments on the original image.
	if(!lines.empty()){
		for (auto it = lines.begin(); it != lines.end(); ++it){
			cv::Vec4i pt = *it;
			cv::line(original_image, cv::Point(pt[0], pt[1]), cv::Point(pt[2], pt[3]), cv::Scalar(0, 255, 0), 1);
			std::cout << "(" << pt[0] << "," << pt[1] << ")-(" << pt[2] << "," << pt[3] << ")" << std::endl;
		}
	}
}

int main(int argc, char* argv[])
{
	tesseract::TessBaseAPI tess;
	tess.Init("C:/Users/MEIP-users/Documents/tesseract-3.02.02-win32-lib-include-dirs/tessdata", "eng");

	//コード認識
	STRING text_out;
	tess.ProcessPages("C:/Users/MEIP-users/Documents/score_3.png", NULL, 0, &text_out);
	tesseract::ResultIterator* ri = tess.GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	
	//コード格納
	std::vector<detectedText> dictionary;//コード保管用
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			if (word == NULL || strlen(word) == 0) {
				continue;
			}

			int x1, y1, x2, y2;//x:横軸(右ほど大) y:縦軸(下ほど大)　
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			if (ri->Confidence(level) > 70.0f) {
				detectedText code(x1, y1, x2, y2, ri->Confidence(level), UTF8toSJIS(word));
				printf("(%d, %d)-(%d, %d) : %.1f%% : %s \n", code.getX1(), code.getY1(), code.getX2(), code.getY2(), code.getConf(), code.getText().c_str());
				dictionary.push_back(code);
			}
		} while (ri->Next(level));
	}

	//opencvによる楽譜認識
	cv::Mat score = cv::imread("C:/Users/MEIP-users/Documents/score_3.png");
	cv::imshow("score", score);
	cv::Mat gray_score;
	cv::Mat binarized;
	cv::cvtColor(score, gray_score, CV_BGR2GRAY);//一応グレースケールに
	cv::imshow("gray_score", gray_score);
	cv::threshold(gray_score, binarized,224, 255, cv::THRESH_BINARY_INV);
	cv::imshow("binarized", binarized);

	//五線の認識
	detectLines(binarized,score);
	cv::imshow("score", score);
	cv::waitKey();
	return 0;
}
