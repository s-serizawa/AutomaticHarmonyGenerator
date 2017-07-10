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
	//std::cout<< "image_size:" << original_image.cols << std::endl;
	cv::HoughLinesP(image, lines, 4, CV_PI / 180.0 * 90, original_image.cols * 19 / 18, 40 * original_image.cols / 77, 10);
		
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


std::vector<noteInfo> matchToPatern(cv::Mat original_image, cv::Mat image, std::vector<noteInfo>& notes_info, cv::Mat pattern, linesInfo lines_info, float threshold, NoteType note_type) {
	
	int interval = lines_info.getLinesInterval();
	//intervalから楽譜の大きさに合わせてパラメータいじる
	int ignorance_left = interval * 5;//五線の左端から右へ何ピクセルまで無視するか
	int ignorance_right = interval * 2;//五線の右端から左へ何ピクセルまで無視するか
	int ignorance_y = interval * 3;//五線から上下何ピクセル無視するか
	cv::Point ell_center = cv::Point(interval * 3 / 4, interval * 3 / 4);
	
	cv::Mat matches;
	cv::matchTemplate(image, pattern, matches, CV_TM_CCORR_NORMED);
	

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
			notes_info.push_back(noteInfo(x, y, scale, step, note_type));
		}
		else {
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
							notes_info.push_back(noteInfo(x, y, scale, step, note_type));
						}
						break;
					}
					else if (i % 5 == 0) { //五線同士の間
						if (y - lines_y[i - 1] < ignorance_y) {//上の五線に属する
							scale = (y - lines_y[i - 1]) / (interval / 2) * (-2);
							if ((y - lines_y[i - 1]) % (interval / 2) > (interval / 2) / 2.0) {
								scale -= 2;
							}
							scale += NOTE_MI;
							step = i / 5 - 1;
							drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
							notes_info.push_back(noteInfo(x, y, scale, step, note_type));
						}
						else if (lines_y[i] - y < ignorance_y) {//下の五線に属する
							scale = (lines_y[i] - y) / (interval / 2) * 2;
							if ((lines_y[i] - y) % (interval / 2) >(interval / 2) / 2.0) {
								scale += 2;
							}
							std::cout << lines_y[0] - y << std::endl;
							scale += NOTE_DO + 20;
							step = i / 5;
							drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
							notes_info.push_back(noteInfo(x, y, scale, step, note_type));
						}
						break;
					}
					else { //線の間
						std::vector<int> v = { std::abs(y - lines_y[i]), std::abs(y - (lines_y[i - 1] + lines_y[i]) / 2), std::abs(y - lines_y[i - 1]) };
						scale = (std::min_element(v.begin(), v.end()) - v.begin()) * 2 + (4 - i % 5) * 4 + NOTE_MI;
						drawingNotesInfo(original_image, scale, x - ell_center.x, y - ell_center.y);
						step = i / 5;
						notes_info.push_back(noteInfo(x, y, scale, step, note_type));
						break;
					}
				}
			}
		}
	}
	return notes_info;
}

//---- 各音符に対し、コードネームを割り当てる
void AssignChordnameToNote(noteInfo _note_info, std::vector<detectedText> dictionary, linesInfo _line_info) {
	int margin = _line_info.getLinesInterval();
	for (int chord = 0; chord < dictionary.size(); ++chord) //---- chord番目のやつに対して
	{
		if (_note_info.getStep() == dictionary[chord].getStep()) //---- 同じ段にいる
		{
			if (_note_info.getPosX() >= dictionary[chord].getX1() - margin) //---- X座標が左側のコードよりも右
			{
				if (chord != dictionary.size()-1) { //---- 
					if (dictionary[chord].getStep() == dictionary[chord+1].getStep()) //---- 同じ段にいたら
					{
						if (dictionary[chord + 1].getX1() > _note_info.getPosX() + margin)
						{
							std::cout << dictionary[chord].getText() << std::endl;
							_note_info.setChordname(dictionary[chord].getText());
						}
					}
					else //---- 同じ段にいなかったら
					{
						_note_info.setChordname(dictionary[chord].getText());
					}
				}
			}
		}
	}
}

//---- 与えられたコードネームに対し、スケールを返す
std::vector<int> determineScale(std::string _chordname) {
	std::vector<int> _Scalenotes;
		std::vector<int> scalenotes(15, 0); //---- scalenotes[7] が真ん中の主音
		if (_chordname.length() == 1){
			switch (_chordname[0])
			{
			case 'A': scalenotes[7] = NOTE_RA;
				break;
			case 'B': scalenotes[7] = NOTE_SI;
				break;
			case 'C':
			case 'c':
				scalenotes[7] = NOTE_DO;
				break;
			case 'D': scalenotes[7] = NOTE_RE;
				break;
			case 'E': scalenotes[7] = NOTE_MI;
				break;
			case 'F':
			case 'f':
				scalenotes[7] = NOTE_FA;
				break;
			case 'G': scalenotes[7] = NOTE_SO;
				break;
			default:
				std::cout << "Chord reading error!" << std::endl;
				getchar();
				break;
			}
			// 長調
			scalenotes[0] = scalenotes[7] - 12; //---- ココ
			scalenotes[1] = scalenotes[7] - 10;
			scalenotes[2] = scalenotes[7] - 8; //---- ココ
			scalenotes[3] = scalenotes[7] - 7;
			scalenotes[4] = scalenotes[7] - 5; //---- ココ
			scalenotes[5] = scalenotes[7] - 3;
			scalenotes[6] = scalenotes[7] - 1;
												//---- ココ
			scalenotes[8] = scalenotes[7] + 2;
			scalenotes[9] = scalenotes[7] + 4; //---- ココ
			scalenotes[10] = scalenotes[7] + 5;
			scalenotes[11] = scalenotes[7] + 7; //---- ココ
			scalenotes[12] = scalenotes[7] + 9;
			scalenotes[13] = scalenotes[7] + 11;
			scalenotes[14] = scalenotes[7] + 12; //---- ココ
		}
		else if (_chordname[1] == 'm') { //一応短調チェック
			switch (_chordname[0])
			{
			case 'A': scalenotes[7] = NOTE_RA;
				break;
			case 'B': scalenotes[7] = NOTE_SI;
				break;
			case 'C':
			case 'c':
				scalenotes[7] = NOTE_DO;
				break;
			case 'D': scalenotes[7] = NOTE_RE;
				break;
			case 'E': scalenotes[7] = NOTE_MI;
				break;
			case 'F':
			case 'f':
				scalenotes[7] = NOTE_FA;
				break;
			case 'G': scalenotes[7] = NOTE_SO;
				break;
			default:
				std::cout << "Chord reading error!" << std::endl;
				getchar();
				break;
			}
			// 短調
			scalenotes[0] = scalenotes[7] - 12; //---- ココ
			scalenotes[1] = scalenotes[7] - 10;
			scalenotes[2] = scalenotes[7] - 9; //---- ココ
			scalenotes[3] = scalenotes[7] - 7;
			scalenotes[4] = scalenotes[7] - 5; //---- ココ
			scalenotes[5] = scalenotes[7] - 4;
			scalenotes[6] = scalenotes[7] - 2;
												//---- ココ
			scalenotes[8] = scalenotes[7] + 2;
			scalenotes[9] = scalenotes[7] + 3; //---- ココ
			scalenotes[10] = scalenotes[7] + 5;
			scalenotes[11] = scalenotes[7] + 7; //---- ココ
			scalenotes[12] = scalenotes[7] + 8;
			scalenotes[13] = scalenotes[7] + 10;
			scalenotes[14] = scalenotes[7] + 12; //---- ココ
		}

	return _Scalenotes;
}


//---- 実際の音階、（コードネーム、スケール情報、）何度下情報を与えると、よさげなハモり音を返してくれる
void getHarmonicNotes(std::vector<noteInfo> _note_info, int degree, std::vector<int>& harmony_tones)
{
	std::string main_code = _note_info[_note_info.size() - 1].getChordname();
	//---- 最後の音符が乗ってるコードがまあ主調でしょという仮定
	std::cout << "fuck" << std::endl;

	if (degree > 0) { --degree; }
	else { ++degree; }

	for (int i = 0; i < _note_info.size(); i++) //---- 各音符についてみていく
	{
		std::string cn = _note_info[i].getChordname();        //---- コードネームを見る
		std::vector<int> sn = _note_info[i].getScalenotes(); //---- スケールを見る
		int nt = _note_info[i].getScale();					//---- 音階を見る

		for (int i = 0; i < sn.size(); ++i) {
			std::cout << sn[i] << std::endl;
		}

		bool isNoteOnTheChord = (nt == sn[0] || nt == sn[2] || nt == sn[4] || nt == sn[7] || nt == sn[9] || nt == sn[11] || nt == sn[14]);
		if (cn == main_code && isNoteOnTheChord) { //---- コードネームが主調と一緒で、かつ音階がそのコードに乗ってたら
			for (int j = 0; j < sn.size(); ++j) {
				if (nt == sn[j]) //---- ノートが一致してる音に対して
				{
					int m = (std::min({ abs(j + degree - 0), abs(j + degree - 2), abs(j + degree - 4),  abs(j + degree - 7),  abs(j + degree - 9),  abs(j + degree - 11), abs(j + degree - 14) }));
					//---- コード上でよさげなハモり音探す
					harmony_tones.emplace_back(sn[m]);
					break;
				}
			}
		}
		else { //---- そうでない場合
			for (int j = 0; j < sn.size(); ++j) { //---- それ以外の場合
				if (sn[j] == nt) //---- スケール上にノートが一致してる音があったら
				{
					harmony_tones.emplace_back(sn[j + degree]); //---- degree度上の音を返す
					break;
				}
				if (sn[j] == nt + 1) //---- それ以外の場合、とりあえず半音ずらしたやつの下の音を見る
				{
					harmony_tones.emplace_back(sn[j + degree]); //---- んでdegree度上の音を返す
					break;
				}
			}
		}
	}
}

std::vector<noteInfo> detectNotes(cv::Mat image, cv::Mat original_image, linesInfo lines_info){
	int interval = lines_info.getLinesInterval();
	
	//とりあえず4分と2分のみ
	cv::Mat ell_q = cv::Mat::zeros(interval * 3 / 2, interval * 3 / 2, CV_8UC1);
	cv::Mat ell_h = cv::Mat::zeros(interval * 3 / 2, interval * 3 / 2, CV_8UC1);
	cv::Point ell_center = cv::Point(interval * 3 / 4, interval * 3 / 4);
	cv::ellipse(ell_q, ell_center, cv::Size(interval / 2, interval * 2 / 3), 60, 0, 360, 255, -1, CV_AA);
	cv::ellipse(ell_h, ell_center, cv::Size(interval * 2 / 5, interval * 3 / 4), 60, 0, 360, 255, 2, 8);
	cv::imshow("ellipse_q", ell_q);
	cv::imshow("ellipse_h", ell_h);

	
	//音階の認識
	float threshold_q = 0.8f;
	float threshold_h = 0.77f;
	
	std::vector<noteInfo> notes_info;
	matchToPatern(original_image, image, notes_info, ell_q, lines_info, threshold_q, QUARTER);
	matchToPatern(original_image, image, notes_info, ell_h, lines_info, threshold_h, HALF);

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

void drawNoteFromScale(cv::Mat original_image, int x, int y, int scale, int step, linesInfo lines_info, NoteType note_type) {
	int interval = lines_info.getLinesInterval();
	//int y_from_scale = (NOTE_MI -scale) * (interval / 2) + lines_info.getLinesY()[step * 5 + 4];
	int mi_y = +lines_info.getLinesY()[step * 5 + 4];
	//符頭と棒を描く位置は直接指定
	if (note_type == QUARTER) {
		cv::ellipse(original_image, cv::Point(x,y), cv::Size(interval / 2, interval * 2 / 3), 60, 0, 360, cv::Scalar(0,0,0), -1, CV_AA);
		cv::line(original_image, cv::Point(x + interval  * 2 / 3, y - interval * 7 / 2), cv::Point(x + interval * 2 / 3, y), cv::Scalar(0, 0, 0), 1, CV_AA);
	}else if (note_type == HALF) {
		cv::ellipse(original_image, cv::Point(x,y), cv::Size(interval * 2 / 5, interval * 3 / 4), 60, 0, 360, cv::Scalar(0,0,0), 2, 8);
		cv::line(original_image, cv::Point(x + interval * 2 / 3, y - interval * 7 / 2), cv::Point(x + interval * 2 / 3, y), cv::Scalar(0, 0, 0), 1, CV_AA);
	}else {
		return;
	}

	//補助線はscale見て決める
	//とりあえず下二本まで
	int hojo_l = 16 * interval / 6;
	if (scale < NOTE_RE) {//面倒なのでめっちゃ高音を考えない
		cv::line(original_image, cv::Point(x - hojo_l / 2, mi_y + interval), cv::Point(x + hojo_l / 2, mi_y + interval), cv::Scalar(0, 0, 0), 1, CV_AA);
		if (scale < NOTE_SI_L) {
			cv::line(original_image, cv::Point(x - hojo_l / 2, mi_y + interval* 2), cv::Point(x + hojo_l / 2, mi_y + interval * 2), cv::Scalar(0, 0, 0), 1, CV_AA);
		}
	
	}

}


//--- main関数
int main(int argc, char* argv[])
{

	char* data = "C:/Users/MEIP-users/Documents/flog_l.png";
	//---- 何度上、あるいは下か
	int degree = -3;

	

	//opencvによる楽譜認識
	cv::Mat score = cv::imread(data);
	//cv::imshow("score", score);
	cv::Mat gray_score;
	cv::Mat binarized;
	cv::cvtColor(score, gray_score, CV_BGR2GRAY);//一応グレースケールに
	//cv::imshow("gray_score", gray_score);
	cv::threshold(gray_score, binarized, 240, 255, cv::THRESH_BINARY_INV);
	cv::imshow("binarized", binarized);

	//五線の認識
	linesInfo lines_info = detectLines(binarized,score);
	//cv::imshow("score", score);
	
	tesseract::TessBaseAPI tess;
	tess.Init("C:/Users/MEIP-users/Documents/tesseract-3.02.02-win32-lib-include-dirs/tessdata", "eng");

	//コード認識
	STRING text_out;
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
				detectedText chord(x1, y1, x2, y2, ri->Confidence(level), UTF8toSJIS(word));
				std::vector<int> lines_y = lines_info.getLinesY();
				for (int i = 0; i < lines_y.size(); ++i) {
					if (i == 0) { //---- 1段目
						if (chord.getY2() < lines_y[i]) {
							chord.setStep(0);
						}
					}
					else if ((i % 5 == 4) && ((i+1) % 5 == 0) ) { //---- 段の間
						chord.setStep(1);
					}
					else {
						chord.setStep(-1);
					}
				}
				//printf("(%d, %d)-(%d, %d) : %.1f%% : %s \n", chord.getX1(), chord.getY1(), chord.getX2(), chord.getY2(), chord.getConf(), chord.getText().c_str());
				std::cout << "(" << chord.getX1() << ", " << chord.getY1() << ")-(" << chord.getX2() << ", " << chord.getY2() << ") : " << chord.getConf()  << "%" << chord.getText().c_str() << ", "<< chord.getStep() << "段目" <<std::endl;
				dictionary.push_back(chord);
			}
		} while (ri->Next(level));
	}

	//符頭の認識
	std::vector<noteInfo> notes_info = detectNotes(binarized, score, lines_info);
	//cv::imshow("score", score);

	//---- 符頭の乗ってるコードネームをdictionary使って決定
	for (int note = 0; note < notes_info.size(); ++note) {
		std::string chord_name;
		AssignChordnameToNote(notes_info[note], dictionary, lines_info);
	}

	//---- コードネームからScale認識
	for (int note = 0; note < notes_info.size(); ++note) {
		std::vector<int> scale_notes;
		std::string cn = notes_info[note].getChordname();
		scale_notes = determineScale(cn);
		notes_info[note].setScalenotes(scale_notes);
	}

	//---- 実際の音階、コードネーム、何度下情報を与えると、よさげなハモり音を返してくれる
	std::vector<int> harmony_tones;
	getHarmonicNotes(notes_info, degree, harmony_tones);//---- harmony_tonesによさげなハモり音を格納
	std::cout << "unko" << std::endl;

	//ハモリ生成
	for (int i = 0; i < notes_info.size(); i++) {
		int scale_change = -4;
		drawNoteFromScale(score, notes_info[i].getPosX(), notes_info[i].getPosY() - scale_change * lines_info.getLinesInterval() / 4, 
			notes_info[i].getScale() + scale_change, notes_info[i].getStep(), lines_info, notes_info[i].getNoteType());
	}
	cv::imshow("score", score);

	cv::waitKey();
	return 0;
}
