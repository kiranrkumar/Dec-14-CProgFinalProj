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
 *         Author:  Ryan Edwards, rce243@nyu.edu, Kiran Kumar (), kk2916@nyu.edu
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

/*float AMmodulate (float modfreq, float *AMbuffer, int numSamples, float sampleRate, float *phase, float *prevPhase)
{
    int i;
    float sample;

    phase = 2.f * (float)M_PI * modfreq / sampleRate + prevPhase;
    sample = sin(*phase);
    if (*phase > 2 * M_PI)
    {
        *phase -= 2 * M_PI;
    }
    prevPhase = phase;

    return sample;
}*/

void FMmodulate (float carrFreq /* make global */, float harmRatio, float modIn, float sampleRate, float *phase, float *prevPhase)
{
    float freq1 = carrFreq * harmRatio;
    float scaleVal = freq1 * modIn;
    float freq2 = scaleVal + carrFreq;
    
    *phase = 2 * M_PI * freq1 / sampleRate + *prevPhase;

    float sample = sin(*phase);
    float freqFM = freq2 + sample;
}

void createSineWave(float freq, float *sinebuffer, int numSamples, float sampleRate, float *phase, float *prevPhase)
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

        sinebuffer[i] = sample;

    }
}

void createTriangleWave (float freq, float *tribuffer, int numSamples, 
        float sampleRate, float *phase, float *prevPhase, int direction)
{
        int i;
        float sample;

        for (i = 0; i < numSamples; i++)
        {
            //float finalFreq = FMmodulate(freq);
            //int periodInsamples = sampleRate / finalFreq;
            int periodInsamples = sampleRate / freq;
            
            //calculate the phase using the current direction of the slope
            *phase = (float)direction * (4.f / periodInsamples) + *prevPhase;
            sample = *phase;
            *prevPhase = *phase;

            //Flip the slope's sign if it crosses 1 or -1
            if (abs(*phase) >= 1)
            {
                //Cap the maximum of the absolute value of the phase at 1
                *prevPhase = direction;
                *phase = *prevPhase;
                sample = *phase;
                direction *= -1;
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
        //float finalFreq = FMmodulate(freq);
        //int periodInsamples = sampleRate / finalFreq;
        int periodInsamples = sampleRate / freq;

        *phase = 2.f / (float)periodInsamples + *prevPhase;
        sample = *phase;

        //move the *phase back down to -1 once it crosses 1
        if (*phase >= 1)
        {
            *phase = -1;
        }
        prevPhase = phase;

        sawbuffer[i] = sample;
    }
    
}

void createSquareWave (float freq, float *squarebuffer, int numSamples, 
        float sampleRate, float *phase, float* prevPhase)
{
    int periodInSamples = sampleRate / freq;
    int i;
    float sample;

    for (i = 0; i < numSamples; i++)
    {
        //float finalFreq = FMmodulate(freq);
        //int periodInsamples = sampleRate / finalFreq;

        *phase = *prevPhase + 1;

        if ( (int) *phase % periodInSamples < (periodInSamples / 2))
            sample = 1;
        else
            sample = -1;

        prevPhase = phase;

        squarebuffer[i] = sample;
    }
}

void createVoice1(float freq, float AMfreq1, float harmRatio, float modIn, float *sinebuffer, float *tribuffer, float *sawbuffer, float *squarebuffer, float *AMbuffer, int numSamples, float amp1)
{
    int i;
    float finalBuffer1[numSamples];
    float AMbuffer1[numSamples];

   // AMbuffer1[] = AMbuffer(AMfreq1);

    if (/*Sine condition here.*/)
    {
        createSineWave(freq, harmRatio, modIn);
        for (i = 0; i < numSamples; i++)  
        {
            finalBuffer1[i] = sinebuffer[i] /* * AMbuffer1[i]*/;
            finalBuffer1[i] = finalBuffer1[i] * amp1;
        }
    }
        
    if (/*Tri condition here.*/)
    {
        createTriWave(freq);
        for (i = 0; i < numSamples; i++)  
        {
            //AMbuffer1[i] = AMbuffer(AMfreq1);
            finalBuffer1[i] = tribuffer[i] /*  * AMbuffer1[i]*/;
            finalBuffer1[i] = finalBuffer1[i] * amp1;
        }
    } 
    
    if (/*Saw condition here.*/)
    {
        createSawWave(freq);
        for (i = 0; i < numSamples; i++)  
        {
            //AMbuffer1[i] = AMbuffer(AMfreq1);
            finalBuffer1[i] = sawbuffer[i] /*  * AMbuffer1[i]*/;
            finalBuffer1[i] = finalBuffer1[i] * amp1;
        }
    } 
    
    if (/*Square condition here.*/)
    {
        createSquareWave(freq);
        for (i = 0; i < numSamples; i++)  
        {
            //AMbuffer1[i] = AMbuffer(AMfreq1);
            finalBuffer1[i] = squarebuffer[i] /*   * AMbuffer1[i]*/;
            finalBuffer1[i] = finalBuffer1[i] * amp1;
        }
    } 
}

void createFinalWave(float *finalBuffer1, float *finalBuffer2, float *finalBuffer3, float *finalBuffer4, float ampFinal, int numSamples)
{
    float finalBuffer[numSamples];
    
    for (int i = 0; i < numSamples; i++)
    {
        finalBuffer[i] = (finalBuffer1[i] + finalBuffer2[i] + finalBuffer3[i] + finalBuffer4[i] / 4) * ampFinal;
        out[2*i] = finalBuffer[i];
        out[2*i+1] = finalBuffer[i];
    }
}



