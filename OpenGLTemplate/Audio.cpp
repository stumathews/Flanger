#include "Audio.h"
#include <iostream>
#include "CircularBuffer.h"
#include "ConvolutionHelper.h"
#include "Game.h"


#pragma comment(lib, "lib/fmod_vc.lib")

// Left and Right channel buffers
cbuf<float> cbuffLeft(1024);
cbuf<float> cbuffRight(1024);

// historic buffer of samples for n-1.. calculation
cbuf<float> prevBuf(1024);

// Quick and easy access to the camera
CCamera* _camera;
Game* _game;
double dt = 0;

// Lo-pass filter passes freqs 0-1000 Hz and removes those > 1000 Hz
// Run using python filter design:
// Arbitary num_taps used trial and error too find the most suitable looking frequency response without
// excessive band pass ripple or band cut attenuation
// bCoefficients = signal.firls(167, [0, 1000, 4000, 44000/2], [1, 1, 0, 0], fs=44000)
float bCoefficients[167] = { 
	1.32727108e-09,	7.07530209e-09,	2.35903552e-08,	6.13798167e-08,
	1.35025413e-07,	2.61011214e-07,	4.52468561e-07,	7.10288692e-07,
	1.01105268e-06, 1.29375661e-06,	1.44915107e-06, 1.31721749e-06,
	6.99215233e-07, -6.09896576e-07, -2.76567778e-06, -5.79738729e-06
    -9.51992478e-06 -1.34528799e-05 -1.67727103e-05 -1.83283097e-05
    -1.67482831e-05 -1.06578868e-05, 9.96105124e-07,1.85421958e-05,
	4.11644680e-05,6.65585613e-05, 9.07756300e-05,1.08367774e-04,
	1.12923355e-04,9.80256825e-05, 5.85860953e-05,-7.59809191e-06,
	-9.83121612e-05,-2.05751240e-04,-3.16073463e-04,-4.10013742e-04
    -4.64779102e-04 -4.57251857e-04 -3.68268824e-04 -1.87459443e-04,
	8.21355058e-05,	4.20619816e-04,	7.89962302e-04,	1.13572036e-03,
	1.39225130e-03,1.49135940e-03, 1.37366812e-03,1.00132456e-03,
	3.70063355e-04, -4.81689159e-04, -1.46898551e-03, -2.46368958e-03,
	-3.30599838e-03, -3.82395788e-03, -3.85934242e-03, -3.29683533e-03,
	-2.09227443e-03 -2.95061462e-04, 1.94011546e-03, 4.35622133e-03,
	6.61376299e-03, 8.32757109e-03, 9.11771253e-03, 8.66909448e-03, 
	6.79211876e-03, 3.47543813e-03, -1.07821923e-03, -6.44088973e-03,
	-1.19779649e-02, -1.68978896e-02, -2.03299780e-02, -2.14229946e-02
	-1.94534396e-02, -1.39300362e-02, -4.68021905e-03, 8.09429657e-03,
	2.38025752e-02, 4.14950657e-02, 5.99388933e-02, 7.77310134e-02,
	9.34370982e-02, 1.05740158e-01, 1.13581088e-01, 1.16273874e-01,
	1.13581088e-01, 1.05740158e-01, 9.34370982e-02, 7.77310134e-02,
	5.99388933e-02, 4.14950657e-02, 2.38025752e-02, 8.09429657e-03
	-4.68021905e-03, -1.39300362e-02, -1.94534396e-02, -2.14229946e-02,
	-2.03299780e-02, -1.68978896e-02, -1.19779649e-02, -6.44088973e-03,
	-1.07821923e-03, 3.47543813e-03, 6.79211876e-03, 8.66909448e-03,
	9.11771253e-03, 8.32757109e-03, 6.61376299e-03, 4.35622133e-03, 
	1.94011546e-03, -2.95061462e-04, -2.09227443e-03, -3.29683533e-03, 
	-3.85934242e-03, -3.82395788e-03, -3.30599838e-03, -2.46368958e-03,
	-1.46898551e-03, -4.81689159e-04, 3.70063355e-04, 1.00132456e-03,
	1.37366812e-03, 1.49135940e-03, 1.39225130e-03, 1.13572036e-03,
	7.89962302e-04, 4.20619816e-04, 8.21355058e-05, -1.87459443e-04, 
	-3.68268824e-04, -4.57251857e-04, -4.64779102e-04, -4.10013742e-04,
	-3.16073463e-04, -2.05751240e-04 -9.83121612e-05, -7.59809191e-06, 
	5.85860953e-05, 9.80256825e-05, 1.12923355e-04, 1.08367774e-04, 
	9.07756300e-05, 6.65585613e-05, 4.11644680e-05, 1.85421958e-05, 
	9.96105124e-07, -1.06578868e-05, -1.67482831e-05, -1.83283097e-05,
	-1.67727103e-05, -1.34528799e-05, -9.51992478e-06, -5.79738729e-06, 
	-2.76567778e-06, -6.09896576e-07, 6.99215233e-07, 1.31721749e-06,
	1.44915107e-06, 1.29375661e-06, 1.01105268e-06, 7.10288692e-07,
	4.52468561e-07, 2.61011214e-07, 1.35025413e-07,6.13798167e-08,
	2.35903552e-08,	7.07530209e-09, 1.32727108e-09 };

/*
I've made these two functions non-member functions
*/

// Check for error
void FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

// DSP callback
FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE *dsp_state, float *inbuffer, float *outbuffer, unsigned int length, int inchannels, int *outchannels)
{	
	for (unsigned int n = 0; n < length; n++)
	{
		for (int chan = 0; chan < *outchannels; chan++)
		{
			const auto x = &inbuffer[(n * inchannels) + chan];
			auto y = &outbuffer[(n * *outchannels) + chan];
		}
		
		static float time = 0;
		const auto depth = 1.0f;

		// Sinusoid modulation of delay parameter
		auto M = [](const unsigned sample_n)->float
		{
			const auto vol_factor = 100;
			const auto cycles_per_second = 0.1f;
			const float val = 1 + sin(2 * M_PI * cycles_per_second * sample_n * time) * vol_factor;
			time += static_cast<float>(1) / 44100;
			return val;
		};

		const auto modulated_delay = static_cast<int>(M(n)); // See implementation of M(n)

		outbuffer[(n * *outchannels) + 0] = inbuffer[(n * inchannels) + 0] + (depth * cbuffLeft.ReadN(modulated_delay));
		outbuffer[(n * *outchannels) + 1] = inbuffer[(n * inchannels) + 1] + (depth * cbuffRight.ReadN(modulated_delay));
		
		cbuffLeft.Put(inbuffer[(n * inchannels) + 0]);
		cbuffRight.Put(inbuffer[(n * inchannels) + 1]);
		
	}
	return FMOD_OK;
}

CAudio::CAudio()
{

}

CAudio::~CAudio()
{
	m_FmodSystem->release();
}

bool CAudio::Initialise(CCamera* camera)
{
	_camera = camera;

	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// Initialise the system
	result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	// Create the DSP effect
	{
		FMOD_DSP_DESCRIPTION dspdesc;
		memset(&dspdesc, 0, sizeof(dspdesc));

		strncpy_s(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
		dspdesc.numinputbuffers = 1;
		dspdesc.numoutputbuffers = 1;
		dspdesc.read = DSPCallback;

		result = m_FmodSystem->createDSP(&dspdesc, &m_dsp);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	return true;
	
}

// Load an event sound
bool CAudio::LoadEventSound(char *filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_LOOP_OFF, 0, &m_eventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play an event sound
bool CAudio::PlayEventSound()
{
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, &m_soundChannel);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// Inject a custom DSP unit into the channel
	//m_soundChannel->addDSP(0, m_dsp);
	return true;
}


// Load a music stream
bool CAudio::LoadMusicStream(char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);

	if (result != FMOD_OK) 
		return false;

	return true;
	

}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	//// Inject a custom DSP unit into the channel
	m_musicChannel->addDSP(0, m_dsp);

	return true;
}

void CAudio::Update(double t)
{
	dt = t;
	m_FmodSystem->update();
}