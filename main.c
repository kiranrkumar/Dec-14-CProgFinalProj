/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Synthesizer main driver for C Final Project
 *
 *        Version:  1.0
 *        Created:  12/07/2014 09:47:52
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kiran Kumar (), kk2916@nyu.edu
 *   Organization:  Steinhardt Music Technology M.M.
 *
 * =====================================================================================
 */

//This is a new comment

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <portaudio.h>
#include "sigProcessing.h"

#define MONO 1
#define STEREO 2
#define NUM_CHANNELS STEREO

#define INIT_FREQ 0
#define SAMPLE_RATE 44100
#define FRAMES_PER_BUFFER 1024

/**** PA callback prototype *****/
static int paCallback( const void *inputBuffer,
        void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void *userData );


/*=================================================*
****************************************************
***************** MAIN FUNCTION ********************
****************************************************
===================================================*/
int main ()
{

    /******************* SETUP  ***********************/
    
    //Set up keyboard mapping
    initKeyMap();
    
    //declare PortAudio variables
    PaStream *stream;
    PaStreamParameters outputParameters;
    PaError err;
    paData data;

    //Initialize PortAudio
    Pa_Initialize();

    //Set output stream parameters
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = NUM_CHANNELS;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = 
        Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    //Open audio stream and check for error
    err = Pa_OpenStream(&stream, NULL /* no input */,
            &outputParameters,
            SAMPLE_RATE, FRAMES_PER_BUFFER, paNoFlag,
            paCallback, &data);

    if (err != paNoError)
    {
        printf("Error in PortAudio: Pa_OpenStream: %s\n", Pa_GetErrorText(err));
    }

    //Start audio stream and check for error
    err = Pa_StartStream(stream);

    if (err != paNoError)
    {
        printf("Error in PortAudio: Pa_StartStream: %s\n", Pa_GetErrorText(err));
    }
    return 0;
}

/**** PA callback function *****/
static int paCallback( const void *inputBuffer,
        void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void *userData )
{

    //Output buffer
    float *out = (float *)outputBuffer;

    //phase and period information for generating tones
    static float phase = 0, prevPhase = 0;
    static int periodInSamples = 0;

    //loop index
    int i;

    return paContinue;
}

