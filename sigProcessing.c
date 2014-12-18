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
 *        Authors:  Kiran Kumar and Ryan Edwards
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

/**** Volume ****/ 

float dbToAmplitude (float decibel)
{
    return (float)pow(10, (decibel/10));
}

/****** Delay effects ******/

void setDelayLen (float delayLen, paData *data, float sampleRate)
{
    int delayInSamples = (int)((delayLen / 1000) * sampleRate);
    data->delayLen = delayInSamples;
    data->delayLenMs = delayLen;
}

void addDelayLen (float delayLen, paData *data, float sampleRate)
{
    int delayInSamples = (int)((delayLen / 1000) * sampleRate);
    data->delayLen += delayInSamples;
    data->delayLenMs += delayLen;
}

void createDelayBuffer (float *buffer, int bufferLen)
{
        buffer = (float *)malloc(bufferLen * sizeof(float));
        for (int i = 0; i < bufferLen; i++)
        {
            buffer[i] = 0;
        }
}

void freeDelayBuffer (float *buffer)
{
    free(buffer);
}

/**** Wave generation ****/ 

void FMmodulate (float carrFreq, float harmRatio, float modIn)
{
    //float freq1 = carrFreq * harmRatio;
    //float scaleVal = freq1 * modIn;
    //float freq2 = scaleVal + carrFreq
    
    //*phase = 2 * M_PI * freq1 / sampleRate + *prevPhase;
    //sample = sin(*phase);
    
    //float freqBuff = freq2 + sample;
    
    //return freqBuff;
}

//return a buffer of frequency values used to FM modulate a wave
void createFMBuffer (float carrFreq, float modFreq, float modIndex, float *buffer, int numSamples, float sampleRate, float *phase, float *prevPhase)
{
    int i;
    float sample;
    
    for (i = 0; i < numSamples; i ++)
    {
        *phase = 2 * M_PI * modFreq / sampleRate + *prevPhase;
        sample = modIndex * sin(*phase);
        if (*phase > 2 * M_PI)
        {
            *phase -= 2 * M_PI;
        }
        *prevPhase = *phase;

        buffer[i] = sample + carrFreq;

    }

}


void createSineWave(float freq, float *buffer, int numSamples, float sampleRate, float *phase, float *prevPhase, float *FMbuffer)
//Hey won't every create_wave function going to need to take the Harmonicity Ratio and Modulation Index? - Ryan
//[Kiran] - Maybe just the createSineWave function. To keep things simple, we should let the modulating waves just be sine waves
{
    int i;
    float sample;
    float actingFrequency;

    for (i = 0; i < numSamples; i ++)
    {
        //float finalFreq = FMmodulate(freq);

        actingFrequency = ((FMbuffer == NULL) ? freq : FMbuffer[i]);
        
        *phase = 2 * M_PI * actingFrequency / sampleRate + *prevPhase;
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
        float sampleRate, float *phase, float *prevPhase, int *direction, float *FMbuffer)
{
        int i;
        float sample;
        float actingFrequency;

        for (i = 0; i < numSamples; i++)
        {
            //float finalFreq = FMmodulate(freq);
            actingFrequency = ((FMbuffer == NULL) ? freq : FMbuffer[i]);
            int periodInsamples = sampleRate / actingFrequency;
            
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
        float sampleRate, float *phase, float *prevPhase, float *FMbuffer)
{
    int i;
    float sample;
    float actingFrequency;

    for (i = 0; i < numSamples; i++)
    {
        actingFrequency = ((FMbuffer == NULL) ? freq : FMbuffer[i]);
        int periodInSamples = sampleRate / actingFrequency;

        *phase = 2. / periodInSamples + *prevPhase;
        sample = *phase;

        //move the *phase back down to -1 once it crosses 1
        if (*phase >= 1)
        {
            *phase -= 1;
        }
        *prevPhase = *phase;

        buffer[i] = sample;
    }
    
}

void createSquareWave (float freq, float *buffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, float *FMbuffer)
{
    int periodInSamples;
    int i;
    float sample, actingFrequency;

    for (i = 0; i < numSamples; i++)
    {
        actingFrequency = ((FMbuffer == NULL) ? freq : FMbuffer[i]);
        periodInSamples = sampleRate / actingFrequency;
        *phase = *prevPhase + 1;

        if ( ( ((int) *phase) % periodInSamples ) < (periodInSamples / 2) )
            sample = 1;
        else
            sample = -1;

        *prevPhase = *phase;

        buffer[i] = sample;
    }
}



