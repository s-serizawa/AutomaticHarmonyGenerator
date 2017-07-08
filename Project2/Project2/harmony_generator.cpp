#include "MusicalScale.h"

MusicalScale::MusicalScale()
	: tonic(0),
	supertonic(0),
	mediant(0),
	subdominant(0),
	dominant(0),
	submediant(0),
	leadingtone(0),
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
	if (chord.length() == 1) //---- #Ç∆Ç©ÅÛÇÕÇ»Ç¢ëOíÒÇ≈èëÇ¢ÇƒÇÈ
	{
		switch (chord[0])
		{
		case 'A': tonic = NOTE_RA;
			break;
		case 'B': tonic = NOTE_SI;
			break;
		case 'C': tonic = NOTE_DO;
			break;
		case 'D': tonic = NOTE_RE;
			break;
		case 'E': tonic = NOTE_MI;
			break;
		case 'F': tonic = NOTE_FA;
			break;
		case 'G': tonic = NOTE_SO;
			break;
		default:
			is_chordreading_success = false;
			std::cout << "Chord Reading Error: This is not an alphabet" << std::endl;
			break;
		}
		if (is_chordreading_success)
		{
			//---- í∑í≤
			supertonic = tonic + 2;
			mediant = tonic + 4;
			subdominant = tonic + 5;
			dominant = tonic + 7;
			submediant = tonic + 9;
			leadingtone = tonic + 11;
		}
	}
	else
		if (chord.length() == 2) //---- ê‚ëŒÇ…íZí≤Ç…Ç»ÇÈëOíÒÇ≈èëÇ¢ÇƒÇÈ
			switch (chord[0])
			{
			case 'A': tonic = NOTE_RA;
				break;
			case 'B': tonic = NOTE_SI;
				break;
			case 'C': tonic = NOTE_DO;
				break;
			case 'D': tonic = NOTE_RE;
				break;
			case 'E': tonic = NOTE_MI;
				break;
			case 'F': tonic = NOTE_FA;
				break;
			case 'G': tonic = NOTE_SO;
				break;
			default:
				is_chordreading_success = false;
				std::cout << "Chord Reading Error: Letters include some non-alphabets." << std::endl;
				break;
			}

	if (is_chordreading_success)
	{
		//---- àÍâûíZí≤ÇÃèåèÉ`ÉFÉbÉN
		if (chord[1] == 'm') {
		   //---- é©ëRíZâπäKÇ≈èëÇ¢ÇƒÇ‹Ç∑
			supertonic = tonic + 2;
			mediant = tonic + 3;
			subdominant = tonic + 4;
			dominant = tonic + 7;
			submediant = tonic + 8;
			leadingtone = tonic + 10;
		}
		else
		{
			std::cout << "the process for this chord name is undefined." << std::endl;
		}
	}

};

