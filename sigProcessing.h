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

//Array of noteInfo elements to allow keys
//on the computer keyboard to play certain notes
noteInfo **keyMap;

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
    float delayLen;
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

void setDelayLen (float delayLen);

void mixDelaySig (float pctWet, float pctDry);

/**** Amplitude and Frequency Modulation effects ****/ 

void AMmodulate (float modFreq);

void FMmodulate (float modFreq);

/*** Allow user to play notes using the computer keyboard ***/
void initKeyMap();

void freeKeyMap();
