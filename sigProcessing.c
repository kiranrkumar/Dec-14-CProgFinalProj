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

void mixDelaySig (float pctWet, float pctDry)
{
    //TODO
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

void AMmodulate (float modfreq, float *AMbuffer, int numSamples, float sampleRate, float *phase, float *prevPhase)
{
    int i;
    float sample;

    for (i = 0; i < numSamples; i ++)
    {
        *phase = 2 * M_PI * modfreq / sampleRate + *prevPhase;
        sample = sin(*phase);
        if (*phase > 2 * M_PI)
        {
            *phase -= 2 * M_PI;
        }
        *prevPhase = *phase;

        AMbuffer[i] = sample;
    }
}

float FMmodulate (float carrFreq, float harmRatio, float modIn, float sampleRate, float phase, float prevPhase)
{
    float freq1 = carrFreq * harmRatio;
    float scaleVal = freq1 * modIn;
    float freq2 = scaleVal + carrFreq
    
    *phase = 2 * M_PI * freq1 / sampleRate + *prevPhase;

    float sample = sin(*phase);
    float freqBuff = freq2 + sample;
    
    return freqBuff;
}

void createSineWave(float freq, float *sinebuffer, int numSamples, float sampleRate, float *phase, float *prevPhase)
{
    int i;
    float sample;

    for (i = 0; i < numSamples; i ++)
    {
        float finalFreq = FMmodulate(freq);
        
        *phase = 2 * M_PI * finalFreq / sampleRate + *prevPhase;
        sample = sin(*phase);
        if (*phase > 2 * M_PI)
        {
            *phase -= 2 * M_PI;
        }
        *prevPhase = *phase;

        sinebuffer[i] = sample;

    }
}

void createTriangleWave (float freq, float *tribuffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, int *direction)
{
        int i;
        float sample;

        for (i = 0; i < numSamples; i++)
        {
            float finalFreq = FMmodulate(freq);
            int periodInsamples = sampleRate / finalFreq;
            
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

            tribuffer[i] = sample;
        }

}

void createSawWave(float freq, float *sawbuffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase)
{
    int i;
    float sample;

    for (i = 0; i < numSamples; i++)
    {        
        float finalFreq = FMmodulate(freq);
        int periodInsamples = sampleRate / finalFreq;
        
        *phase = 2. / periodInSamples + *prevPhase;
        sample = *phase;

        //move the *phase back down to -1 once it crosses 1
        if (*phase >= 1)
        {
            *phase = -1;
        }
        *prevPhase = *phase;

        sawbuffer[i] = sample;
    }
    
}

void createSquareWave (float freq, float *squarebuffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase)
{
<<<<<<< HEAD

    int periodInSamples = sampleRate / freq;
=======
>>>>>>> FETCH_HEAD
    int i;
    float sample;

    for (i = 0; i < numSamples; i++)
    {
        float finalFreq = FMmodulate(freq);
        int periodInsamples = sampleRate / finalFreq;
        
        *phase = *prevPhase + 1;

        if ( (int) *phase % periodInSamples < (periodInSamples / 2))
            sample = 1;
        else
            sample = -1;

        *prevPhase = *phase;

        squarebuffer[i] = sample;
    }
}

void createFinalWave(float *sinebuffer, float *tribuffer, float *sawbuffer, float *squarebuffer, float *AMbuffer, int numSamples, 
        float amp;)
{
    for (i = 0; i < numSamples; i++)  
    {
        if (/*Sine condition here.*/)
        {
            finalBuffer[i] = sineBuffer[i] * AMbuffer[i] / 2;
        }
        
        if (/*Tri condition here.*/)
        {
            finalBuffer[i] = triBuffer[i] * AMbuffer[i] / 2;
        }
        
        if (/*Saw condition here.*/)
        {
            finalBuffer[i] = sawBuffer[i] * AMbuffer[i] / 2;
        }
        
        if (/*Square condition here.*/)
        {
            finalBuffer[i] = squareBuffer[i] * AMbuffer[i] / 2;
        }
        
        finalBuffer[i] = finalBuffer[i] * amp;
        
        out[2*i] = finalBuffer[i];
        out[2*i+1] = finalBuffer[i];
    }
}

