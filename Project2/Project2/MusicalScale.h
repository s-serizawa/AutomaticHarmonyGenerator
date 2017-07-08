#pragma once
#include <vector>
#include <iostream>
#include <string>
#include "notes.h"

class MusicalScale {
private:
	int tonic;			  //---- �剹�A�h
	int supertonic;      //---- ��剹(2�x��)�A��
	int mediant;        //---- ����(3�x��)�A�~
	int subdominant;   //---- ������(���S4�x��)�A�t�@
	int dominant;     //---- ����(���S5�x��)�A�\
	int submediant;  //---- ������(���S6�x��)�A��
	int leadingtone;//----�@����(�Z2�x��)�A�V

	bool is_chordreading_success;

public:
	//---- �R���X�g���N�^
	MusicalScale();

	//---- �f�X�g���N�^
	~MusicalScale();

	int get_tonic();
	int get_supertonic();
	int get_mediant();
	int get_subdominant();
	int get_dominant();
	int get_submediant();
	int get_leadingtone();

	//---- �R�[�h����C�R�[�h�̏���Ă���X�P�[���𔻒肵�C�N���XMusicalScale�̃����o�ϐ��ɑ��
	void setScaleAccordingToChord(const std::string& chord);

};