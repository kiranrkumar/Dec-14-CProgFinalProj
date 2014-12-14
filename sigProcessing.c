/*
 * =====================================================================================
 *
 *       Filename:  sigProcessing.c
 *
 *    Description:  Implement the functions in signal Processing.h
 *
 *        Version:  1.0
 *        Created:  12/06/2014 19:32:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Kiran Kumar (), kk2916@nyu.edu
 *   Organization:  Steinhardt Music Technology M.M.
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <portaudio.h>
#include "sigProcessing.h"

#define PI 3.14159265358979323846264338327950288 //for synthesizing sine waves

#define TABLE_SIZE 123 //cover ASCII values to allow for computer keyboard note generation

void setDelayLen (float delayLen)
{
    //TODO
}

void mixDelaySig (float pctWet, float pctDry)
{
    //TODO
}

void AMmodulate (float modFreq)
{
    //TODO
}

void FMmodulate (float modFreq)
{
    //TODO
}

void createSineWave(float freq, float *buffer, int numSamples, float sampleRate, float *phase, float *prevPhase)
{
    int i;
    float sample;

    for (i = 0; i < numSamples; i ++)
    {

        *phase = 2 * M_PI * freq / sampleRate + *prevPhase;
        sample = sin(*phase);
        if (*phase > 2 * M_PI)
        {
            *phase -= 2 * M_PI;
        }
        *prevPhase = *phase;

        buffer[i] = sample;

    }
}

void createTriangleWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, int *direction)
{
        int periodInsamples = sampleRate / freq;
        int i;
        float sample;

        for (i = 0; i < numSamples; i++)
        {
            //calculate the phase using the current direction of the slope
            *phase = *direction * (4. / periodInsamples) + *prevPhase;
            sample = *phase;
            *prevPhase = *phase;

            //Flip the slope's sign if it crosses 1 or -1
            if (abs(*phase) >= 1)
            {
                //Cap the maximum of the absolute value of the phase at 1
                *prevPhase = *direction;
                *phase = *prevPhase;
                sample = *phase;
                *direction *= -1;
            }

            buffer[i] = sample;
        }

}

void createSawWave(float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase)
{
    int periodInSamples = sampleRate / freq;
    int i;
    float sample;

    for (i = 0; i < numSamples; i++)
    {
        *phase = 2. / periodInSamples + *prevPhase;
        sample = *phase;

        //move the *phase back down to -1 once it crosses 1
        if (*phase >= 1)
        {
            *phase = -1;
        }
        *prevPhase = *phase;

        buffer[i] = sample;
    }
    
}

void createSquareWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase)
{
    int periodInSamples = sampleRate / freq;
    int i;
    float sample;

    for (i = 0; i < numSamples; i++)
    {
        *phase = *prevPhase + 1;

        if ( (int) *phase % periodInSamples < (periodInSamples / 2))
            sample = 1;
        else
            sample = -1;

        *prevPhase = *phase;

        buffer[i] = sample;
    }
}


void freeKeyMap(noteInfo **keyMap)
{
    //loop index
    int i;

    //free each non-null element, 
    for (i = 0; i < TABLE_SIZE; i++)
    {
        if (keyMap[i] != NULL)
        {
            free(keyMap[i]);
        }
    }

    //free the array itself
    free(keyMap);
}
