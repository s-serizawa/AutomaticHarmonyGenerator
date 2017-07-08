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

linesInfo detectLines(cv::Mat image, cv::Mat original_image) {
	std::vector<cv::Vec4i> lines;
	cv::HoughLinesP(image, lines, 4, CV_PI / 180.0 * 90, 1000, 400, 10);
		
	//Draw detected segments on the original image.
	if(!lines.empty()){
		for (auto it = lines.begin(); it != lines.end(); ++it){
			cv::Vec4i pt = *it;
			cv::line(original_image, cv::Point(pt[0], pt[1]), cv::Point(pt[2], pt[3]), cv::Scalar(0, 255, 0), 1);
			std::cout << "(" << pt[0] << "," << pt[1] << ")-(" << pt[2] << "," << pt[3] << ")" << std::endl;
		}
	}else {
		std::cout << "line not detected" << std::endl;
		cv::waitKey();
	}

	//�Ƃ肠������F���Ȃ��Ƃ���
	int steps_num = lines.size() / 5;
	std::cout << "steps:" << steps_num << std::endl;
	std::vector<int> lines_y(lines.size(),0);
	int lines_left = 0;
	int lines_right = 0;
	for (int i = 0; i < lines.size(); i++) {
		lines_y[i] = (lines[i][1] + lines[i][3]) / 2;
		lines_left += lines[i][0];
		lines_right += lines[i][2];
	}
	lines_left /= lines.size();
	lines_right /= lines.size();
	std::sort(lines_y.begin(), lines_y.end());
	std::cout << "left: "<< lines_left << " right:"  << lines_right << std::endl;
	return linesInfo::linesInfo(lines_y, lines_left, lines_right);
}

void detectNotes(cv::Mat image, cv::Mat original_image) {
	int template_width = 15;
	int template_height = 15;
	cv::Mat ell = cv::Mat::zeros(9, 9, CV_8UC1);
	cv::ellipse(ell, cv::Point(4, 4), cv::Size(3, 5), 80, 0, 360, 255, -1, CV_AA);
	cv::imshow("ellipse", ell);
	cv::Mat matches; 
	cv::matchTemplate(image, ell, matches, CV_TM_CCORR_NORMED);
	float threshold = 0.815f;
	for (int y = 0; y < matches.rows; y++) {
		for (int x = 0; x < matches.cols; x++){
			if(matches.at<float>(y,x) > threshold){
				cv::rectangle(original_image, cv::Point(x, y), cv::Point(x + template_width, y + template_height), cv::Scalar(255, 0, 0), 1);
			}
		}
	}
}

int main(int argc, char* argv[])
{	
	tesseract::TessBaseAPI tess;
	tess.Init("C:/Users/MEIP-users/Documents/tesseract-3.02.02-win32-lib-include-dirs/tessdata", "eng");

	//�R�[�h�F��
	STRING text_out;
	tess.ProcessPages("C:/Users/MEIP-users/Documents/flog.png", NULL, 0, &text_out);
	tesseract::ResultIterator* ri = tess.GetIterator();
	tesseract::PageIteratorLevel level = tesseract::RIL_WORD;
	
	//�R�[�h�i�[
	std::vector<detectedText> dictionary;//�R�[�h�ۊǗp
	if (ri != 0) {
		do {
			const char* word = ri->GetUTF8Text(level);
			if (word == NULL || strlen(word) == 0) {
				continue;
			}

			int x1, y1, x2, y2;//x:����(�E�قǑ�) y:�c��(���قǑ�)�@
			ri->BoundingBox(level, &x1, &y1, &x2, &y2);
			if (ri->Confidence(level) > 7.0f) {
				detectedText code(x1, y1, x2, y2, ri->Confidence(level), UTF8toSJIS(word));
				printf("(%d, %d)-(%d, %d) : %.1f%% : %s \n", code.getX1(), code.getY1(), code.getX2(), code.getY2(), code.getConf(), code.getText().c_str());
				dictionary.push_back(code);
			}
		} while (ri->Next(level));
	}

	

	//opencv�ɂ��y���F��
	cv::Mat score = cv::imread("C:/Users/MEIP-users/Documents/flog.png");
	cv::imshow("score", score);
	cv::Mat gray_score;
	cv::Mat binarized;
	cv::cvtColor(score, gray_score, CV_BGR2GRAY);//�ꉞ�O���[�X�P�[����
	cv::imshow("gray_score", gray_score);
	cv::threshold(gray_score, binarized,224, 255, cv::THRESH_BINARY_INV);
	cv::imshow("binarized", binarized);

	//�ܐ��̔F��
	linesInfo lines_info = detectLines(binarized,score);
	cv::imshow("score", score);
	
	//�����̔F��
	detectNotes(binarized, score);
	cv::imshow("score", score);
	
	cv::waitKey();
	return 0;
}
