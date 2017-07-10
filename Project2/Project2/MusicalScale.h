#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "notes.h"

class MusicalScale {
private:
	int tonic;			  //---- 主音、ド
	int supertonic;      //---- 上主音(2度上)、レ
	int mediant;        //---- 中音(3度上)、ミ
	int subdominant;   //---- 下属音(完全4度上)、ファ
	int dominant;     //---- 属音(完全5度上)、ソ
	int submediant;  //---- 下中音(6度上)、ラ
	int leadingtone;//----　導音(短2度下)、シ
	std::vector<int> scalenotes; //---- 配列管理したほうがよさそう

	bool is_chordreading_success;

public:
	//---- コンストラクタ
	MusicalScale();

	//---- デストラクタ
	~MusicalScale();

	int get_tonic();
	int get_supertonic();
	int get_mediant();
	int get_subdominant();
	int get_dominant();
	int get_submediant();
	int get_leadingtone();

	//---- コードから，コードの乗っているスケールを判定し，クラスMusicalScaleのメンバ変数に代入
	void setScaleAccordingToChord(const std::string& chord);

	//---- 与えられたコードネーム，音高，ハモリの度数から，よさげなハモリ音を返す関数
	int returnHarmonicTone(const std::string& chord, const int& note, const int& degree);

};