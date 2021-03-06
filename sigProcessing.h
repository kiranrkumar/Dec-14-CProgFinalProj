/*
 * =====================================================================================
 *
 *       Filename:  sigProcessing.h
 *
 *    Description:  Signal processing functions
 *
 *        Version:  1.0
 *        Created:  12/06/2014 19:19:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kiran Kumar (), kk2916@nyu.edu
 *   Organization:  Steinhardt Music Technology M.M.
 *
 * =====================================================================================
 */

#include <portaudio.h>
#include <sndfile.h>
#include <stdio.h>
#include <stdlib.h>

/********************************************************
 ******************** Structs ***************************
 ********************************************************/

//Data for a single keyboard note
typedef struct {
    float frequency;
    char *noteName;
} noteInfo;

//Audio signal source
typedef enum signalSource {
    OSCILLATOR,
    SOUNDFILE,
    MICROPHONE
} sigSource;

//Oscillator wave type
// (applies only to oscillator mode) 
typedef enum wave {
    SINE,
    SAWTOOTH,
    SQUARE,
    TRIANGLE
} waveType;

//PortAudio Data
typedef struct {
    float frequency;
    float volume;
    float pitchShift;
    float amModFreq;
    float fmModFreq;
    int delayLen; //delay length in samples
    int delayLenMs; //delay length in milliseconds
    int prevDelayLen;
    float *delayBuffer;
    float delayPctDry;
    float delayPctWet;
    sigSource sigSrc;
    waveType sigWaveType;
    SNDFILE *audioFile;
    SF_INFO audioData;
} paData;



/********************************************************
 *************** Function Prototypes ********************
 ********************************************************/

/****** Delay effects ******/

void setDelayLen (float delayLen, paData *data, float sampleRate);

void addDelayLen (float delayLen, paData *data, float sampleRate);

void mixDelaySig (float pctWet, float pctDry);

void createDelayBuffer (float *buffer, int bufferLen);

void freeDelayBuffer (float *buffer);

/**** Amplitude and Frequency Modulation effects ****/ 

void AMmodulate (float modfreq, float *AMbuffer, int numSamples, float sampleRate, float *phase, float *prevPhase);

void FMmodulate (float *FMbuffer, float carrFreq /* make global */, float harmRatio, float modIn, float sampleRate, float numSamples, float *phase, float *prevPhase);

void createSineWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase);

void createTriangleWave (float freq, float *tribuffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, int direction);

void createSawWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase);

void createSquareWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase);

void createFinalWave(float *finalBuffer1, float *finalBuffer2, float *finalBuffer3, float *finalBuffer4, float ampFinal, int numSamples);

