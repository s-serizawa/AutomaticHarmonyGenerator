#pragma warning(disable:4996)

#include <tesseract/baseapi.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <sstream>
#include <iostream>
#include "main.h"
#include "notes.h"


#ifdef _DEBUG
#pragma comment(lib, "libtesseract302d.lib")
#pragma comment(lib, "liblept168d.lib")
#else
#pragma comment(lib, "libtesseract302.lib")
#pragma comment(lib, "liblept168.lib")
#endif

//#include 
//#include 
/*
std::string itos(int i) {
	ostringstream s;
	s << i;
	return s.str();
}
*/

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
	}else{
		std::cout << "line not detected" << std::endl;
		cv::waitKey();
	}

	//とりあえず誤認識なしとする
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
	int lines_interval = 0;
	for (int i = 0; i < steps_num; i++) {
		lines_interval += ((lines_y[i * 5 + 4] - lines_y[i * 5]) / 4);
	}
	lines_interval /= steps_num;
	std::cout << "left: "<< lines_left << " right:"  << lines_right << " interval:" << lines_interval <<std::endl;
	return linesInfo::linesInfo(lines_y, lines_left, lines_right,lines_interval,steps_num);
}

void drawingNotesInfo(cv::Mat original_image, int scale, int x, int y) {
	int rect_width = 10;
	int rect_height = 10;
	int delta_text_pos_y = 30;
	/*cv::rectangle(original_image, cv::Point(x, y), cv::Point(x + rect_width, y + rect_height)
		, cv::Scalar(0, 0, 0), 1);
	char buffer[3];
	itoa(scale, buffer, 10);
	cv::putText(original_image, buffer, cv::Point(x, y + delta_text_pos_y), 
		cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1, CV_AA);*/
}

std::vector<noteInfo> detectNotes(cv::Mat image, cv::Mat original_image, linesInfo lines_info) {
	int interval = lines_info.getLinesInterval();
	int ignorance_left = 34;//五線の左端から右へ何ピクセルまで無視するか
	int ignorance_right = 10;//五線の右端から左へ何ピクセルまで無視するか
	int ignorance_y = interval * 3;//五線から上下何ピクセル無視するか
	cv::Mat ell = cv::Mat::zeros(9, 9, CV_8UC1);
	cv::Point ell_center = cv::Point(4, 4);
	cv::ellipse(ell, ell_center, cv::Size(3, 5), 60, 0, 360, 255, -1, CV_AA);
	//cv::ellipse(ell, ell_center, cv::Size(3, 5), 55, 0, 360, 255, 2, 8);// CV_AA);
	cv::imshow("ellipse", ell);
	
	cv::Mat matches; 
	cv::matchTemplate(image, ell, matches, CV_TM_CCORR_NORMED);
	
	//音階の認識
	float threshold = 0.7f;
	
	//多重認識の防止　近傍でマッチ度が極大になる位置を記憶
	std::vector<std::vector<int>> matches_pos;
	for (int x = 0; x < matches.cols; x++) {
		for (int y = 0; y < matches.rows; y++) {
			if (matches.at<float>(y, x) > threshold && x > lines_info.getLinesLeft() + ignorance_left && x < lines_info.getLinesRight() - ignorance_right) {
				std::vector<int> pos = { x, y };
				if (matches_pos.empty()) {
					matches_pos.push_back(pos);
				}
				for (int i = 0; i < matches_pos.size(); i++) {
					if (pow(pow(x - matches_pos[i][0], 2) + pow(y - matches_pos[i][1], 2), 0.5) < 10) {//十分近いものが存在
						if (matches.at<float>(y, x) > matches.at<float>(matches_pos[i][1], matches_pos[i][0])) {
							matches_pos[i][0] = x;
							matches_pos[i][1] = y;
						}
						break;
					}
					if (i == matches_pos.size() - 1) {
						matches_pos.push_back(pos);
					}
				}
			}
		}
	}
	std::cout << matches_pos.size() << std::endl;

	std::vector<int> lines_y = lines_info.getLinesY();
	std::vector<noteInfo> notes_info;
	int scale;
	int step;
	//半音上下はなし
	for (int i = 0; i < matches_pos.size(); i++) {
		int x = matches_pos[i][0] + ell_center.x;
		int y = matches_pos[i][1] + ell_center.y;
		if (y > lines_y[lines_y.size() - 1]) {//一番下の線より下
			scale = (y - lines_y[lines_y.size() - 1]) / (interval / 2) * (-2);
			if ((y - lines_y[lines_y.size() - 1]) % (interval / 2) > (interval / 2) / 2.0) {
				scale -= 2;
			}
			scale += NOTE_MI;
			step = lines_info.getStepsNum() - 1;
			drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
			notes_info.push_back(noteInfo(x,y, scale, step));
		}else{
			for (int i = 0; i < lines_y.size(); i++) {
				if (y < lines_y[i]) {
					if (i == 0) { // 一番上の線より上
						if ((lines_y[0] - y) < ignorance_y) {
							scale = (lines_y[i] - y) / (interval / 2) * 2;
							if ((lines_y[i] - y) % (interval / 2) > (interval / 2) / 2.0) {
								scale += 2;
							}
							scale += NOTE_DO + 20;
							step = 0;
							drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
							notes_info.push_back(noteInfo(x, y, scale,step));
						}
						break;
					}else if (i % 5 == 0) { //五線同士の間
						if (y - lines_y[i - 1] < ignorance_y) {//上の五線に属する
							scale = (y - lines_y[i - 1]) / (interval / 2) * (-2);
							if ((y - lines_y[i - 1]) % (interval / 2) > (interval / 2) / 2.0) {
								scale -= 2;
							}
							scale += NOTE_MI;
							step = i / 5 - 1;
							drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
							notes_info.push_back(noteInfo(x, y, scale,step ));
						}else if (lines_y[i] - y < ignorance_y) {//下の五線に属する
							scale = (lines_y[i] - y) / (interval / 2) * 2;
							if ((lines_y[i] - y) % (interval / 2) > (interval / 2) / 2.0) {
								scale += 2;
							}
							std::cout << lines_y[0] - y << std::endl;
							scale += NOTE_DO + 20;
							step = i / 5;
							drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
							notes_info.push_back(noteInfo(x, y, scale, step));
						}
						break;
					}else { //線の間
						std::vector<int> v = { std::abs(y - lines_y[i]), std::abs(y - (lines_y[i - 1] + lines_y[i]) / 2), std::abs(y - lines_y[i - 1])};
						scale = (std::min_element(v.begin(), v.end()) - v.begin()) * 2 + (4 - i % 5) * 4 + NOTE_MI;
						drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
						step = i / 5;
						notes_info.push_back(noteInfo(x, y, scale, step));
						break;
					}
				}
			}
		}
	}
	return notes_info;
}

/*
void drawNote(cv::Point pos, int note_type){//描く位置を直接指定 補助線は書かない
	if (note_type == 4) {

	}
	else if (note_type == 2) {

	}
	else {
		return;
	}
}
*/

void drawNoteFromScale(cv::Mat original_image, int x, int y, int scale, int step, linesInfo lines_info, int note_type) {
	int interval = lines_info.getLinesInterval();
	//int y_from_scale = (NOTE_MI -scale) * (interval / 2) + lines_info.getLinesY()[step * 5 + 4];
	int mi_y = +lines_info.getLinesY()[step * 5 + 4];
	//符頭を描く位置は直接指定
	if (note_type == 4) {
		cv::ellipse(original_image, cv::Point(x,y), cv::Size(3, 5), 60, 0, 360, cv::Scalar(0,0,0), -1, CV_AA);
	}else if (note_type == 2) {
		cv::ellipse(original_image, cv::Point(x,y), cv::Size(3, 5), 55, 0, 360, cv::Scalar(0,0,0), 2, 8);
	}else {
		return;
	}

	//補助線はscale見て決める
	//とりあえず下二本まで
	int hojo_l = 12;
	if (scale < NOTE_RE) {//面倒なのでめっちゃ高音を考えない
		cv::line(original_image, cv::Point(x - hojo_l / 2, mi_y + interval), cv::Point(x + hojo_l / 2, mi_y + interval), cv::Scalar(0, 0, 0), 1, CV_AA);
		if (scale < NOTE_SI_L) {
			cv::line(original_image, cv::Point(x - hojo_l / 2, mi_y + interval* 2), cv::Point(x + hojo_l / 2, mi_y + interval * 2), cv::Scalar(0, 0, 0), 1, CV_AA);
		}
	
	}

}


int main(int argc, char* argv[])
{	
	tesseract::TessBaseAPI tess;
	tess.Init("C:/Users/MEIP-users/Documents/tesseract-3.02.02-win32-lib-include-dirs/tessdata", "eng");

	//コード認識
	STRING text_out;
	char* data = "C:/Users/MEIP-users/Documents/flog.png";
	tess.ProcessPages(data, NULL, 0, &text_out);
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
			if (ri->Confidence(level) > 7.0f) {
				detectedText code(x1, y1, x2, y2, ri->Confidence(level), UTF8toSJIS(word));
				printf("(%d, %d)-(%d, %d) : %.1f%% : %s \n", code.getX1(), code.getY1(), code.getX2(), code.getY2(), code.getConf(), code.getText().c_str());
				dictionary.push_back(code);
			}
		} while (ri->Next(level));
	}

	

	//opencvによる楽譜認識
	cv::Mat score = cv::imread(data);
	//cv::imshow("score", score);
	cv::Mat gray_score;
	cv::Mat binarized;
	cv::cvtColor(score, gray_score, CV_BGR2GRAY);//一応グレースケールに
	//cv::imshow("gray_score", gray_score);
	cv::threshold(gray_score, binarized,224, 255, cv::THRESH_BINARY_INV);
	//cv::imshow("binarized", binarized);

	//五線の認識
	linesInfo lines_info = detectLines(binarized,score);
	//cv::imshow("score", score);
	
	//符頭の認識
	std::vector<noteInfo> notes_info = detectNotes(binarized, score,lines_info);
	//cv::imshow("score", score);

	//ハモリ生成
	for (int i = 0; i < notes_info.size(); i++) {
		int scale_change = -4;
		drawNoteFromScale(score, notes_info[i].getPosX(), notes_info[i].getPosY() - scale_change * lines_info.getLinesInterval() / 4, 
			notes_info[i].getScale() + scale_change, notes_info[i].getStep(), lines_info, 4);
	}
	cv::imshow("score", score);

	cv::waitKey();
	return 0;
}
