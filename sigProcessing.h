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
    float volume2;
    float amModFreq;
    float fmModFreq;
    float fmModFreq2;
    int delayLen; //delay length in samples
    int delayLenMs; //delay length in milliseconds
    int prevDelayLen;
    int delayReader;
    int delayWriter;
    float *delayBuffer;
    float delayPctDry;
    float delayPctWet;
    sigSource sigSrc;
    waveType sigWaveType;
    waveType sigWaveType2;
    SNDFILE *audioFile;
    SF_INFO audioData;
} paData;



/********************************************************
 *************** Function Prototypes ********************
 ********************************************************/

/**** Volume ****/ 

float dbToAmplitude(float decibel);

/****** Delay effects ******/

void setDelayLen (float delayLen, paData *data, float sampleRate);

void addDelayLen (float delayLen, paData *data, float sampleRate);

void createDelayBuffer (float *buffer, int bufferLen);

void freeDelayBuffer (float *buffer);

/**** Wave generation ****/ 

void FMmodulate (float carrFreq, float harmRatio, float modIn);

void createFMBuffer (float carrFreq, float modFreq, float modIndex, float *buffer, int numSamples, float sampleRate, float *phase, float *prevPhase);

void createSineWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, float *FMbuffer);

void createTriangleWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, int *direction, float *FMbuffer);

void createSawWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, float *FMbuffer);

void createSquareWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, float *FMbuffer);


