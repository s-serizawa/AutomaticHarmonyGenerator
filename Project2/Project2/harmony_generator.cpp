#include "MusicalScale.h"

MusicalScale::MusicalScale()
	: tonic(0),
	supertonic(0),
	mediant(0),
	subdominant(0),
	dominant(0),
	submediant(0),
	leadingtone(0),
	scalenotes(7,0),
	is_chordreading_success(true)
{
}

MusicalScale::~MusicalScale()
{
}

int MusicalScale::get_tonic() { return tonic; }
int MusicalScale::get_supertonic() { return supertonic; }
int MusicalScale::get_mediant() { return mediant; }
int MusicalScale::get_subdominant() { return subdominant; }
int MusicalScale::get_dominant() { return dominant; }
int MusicalScale::get_submediant() { return submediant; }
int MusicalScale::get_leadingtone() { return leadingtone; }

void MusicalScale::setScaleAccordingToChord(const std::string& chord) {
	if (chord.length() == 1) //---- #とか♭はない前提で書いてる
	{
		switch (chord[0])
		{
		case 'A': tonic = NOTE_RA; scalenotes[0] = NOTE_RA;
			break;
		case 'B': tonic = NOTE_SI; scalenotes[0] = NOTE_SI;
			break;
		case 'C': tonic = NOTE_DO; scalenotes[0] = NOTE_DO;
			break;
		case 'D': tonic = NOTE_RE; scalenotes[0] = NOTE_RE;
			break;
		case 'E': tonic = NOTE_MI; scalenotes[0] = NOTE_MI;
			break;
		case 'F': tonic = NOTE_FA; scalenotes[0] = NOTE_FA;
			break;
		case 'G': tonic = NOTE_SO; scalenotes[0] = NOTE_SO;
			break;
		default:
			is_chordreading_success = false;
			std::cout << "Chord Reading Error: This is not an alphabet" << std::endl;
			break;
		}
		if (is_chordreading_success)
		{
			//---- 長調
			supertonic = tonic + 2;
			mediant = tonic + 4;
			subdominant = tonic + 5;
			dominant = tonic + 7;
			submediant = tonic + 9;
			leadingtone = tonic + 11;
			scalenotes[1] = scalenotes[0] + 2;
			scalenotes[2] = scalenotes[0] + 4;
			scalenotes[3] = scalenotes[0] + 5;
			scalenotes[4] = scalenotes[0] + 7;
			scalenotes[5] = scalenotes[0] + 9;
			scalenotes[6] = scalenotes[0] + 11;
		}
	}
	else
		if (chord.length() == 2) //---- 絶対に短調になる前提で書いてる
			switch (chord[0])
			{
			case 'A': tonic = NOTE_RA; scalenotes[0] = NOTE_RA;
				break;
			case 'B': tonic = NOTE_SI; scalenotes[0] = NOTE_SI;
				break;
			case 'C': tonic = NOTE_DO; scalenotes[0] = NOTE_DO;
				break;
			case 'D': tonic = NOTE_RE; scalenotes[0] = NOTE_RE;
				break;
			case 'E': tonic = NOTE_MI; scalenotes[0] = NOTE_MI;
				break;
			case 'F': tonic = NOTE_FA; scalenotes[0] = NOTE_FA;
				break;
			case 'G': tonic = NOTE_SO; scalenotes[0] = NOTE_SO;
				break;
			default:
				is_chordreading_success = false;
				std::cout << "Chord Reading Error: Letters include some non-alphabets." << std::endl;
				break;
			}

	if (is_chordreading_success)
	{
		//---- 一応短調の条件チェック
		if (chord[1] == 'm') {
		   //---- 自然短音階で書いてます
			supertonic = tonic + 2;
			mediant = tonic + 3;
			subdominant = tonic + 5;
			dominant = tonic + 7;
			submediant = tonic + 8;
			leadingtone = tonic + 10;
			scalenotes[1] = scalenotes[0] + 2;
			scalenotes[2] = scalenotes[0] + 3;
			scalenotes[3] = scalenotes[0] + 5;
			scalenotes[4] = scalenotes[0] + 7;
			scalenotes[5] = scalenotes[0] + 8;
			scalenotes[6] = scalenotes[0] + 10;
		}
		else
		{
			std::cout << "the process for this chord name is undefined." << std::endl;
		}
	}
}

int MusicalScale::returnHarmonicTone(const std::string& chord, const int& note, const int& degree) {
	setScaleAccordingToChord(chord);
	for (int i = 0; i < scalenotes.size(); ++i)
	{
		if (scalenotes[i] == note) //---- スケールに乗っているとき。どっちかには合致するはず
		{

		}
		else if (scalenotes[i] == note + 1) //---- スケールに乗っていないとき。どっちかには合致するはず
		{

		}
	}
};