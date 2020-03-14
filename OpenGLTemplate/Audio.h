#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include "CircularBuffer.h"
#include "Camera.h"
#include <cstdlib>
#include "Game.h"

class CAudio
{
public:
	CAudio();
	~CAudio();
	bool Initialise(CCamera* camera);
	bool LoadEventSound(char *filename);
	bool PlayEventSound();
	bool LoadMusicStream(char *filename);
	bool PlayMusicStream();
	void Update();

	
private:

	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD
	FMOD::Sound *m_eventSound;

	FMOD::Sound *m_music;
	FMOD::Channel *m_musicChannel;

	FMOD::DSP *m_dsp;

};

void RecordInputSignal(unsigned int length, int* outchannels, float* inbuffer, int inchannels, float* outbuffer);
