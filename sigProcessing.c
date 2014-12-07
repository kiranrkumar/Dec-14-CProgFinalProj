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

void initKeyMap()
{
    //initialize array of noteInfo pointers
    keyMap = (noteInfo**) malloc(TABLE_SIZE * sizeof(noteInfo*)); 
    
    //loop index
    int i;

    //start by initializing everything to NULL for simplicity's sake
    for (i = 0; i < TABLE_SIZE; i++)
    {
        keyMap[i] = NULL;
    }

    //now set frequency and channel values for the appropriate keys
    

    keyMap['a'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['a']->noteName = "A3";
    keyMap['a']->frequency = 220.0;

    keyMap['s'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['s']->noteName = "B3";
    keyMap['s']->frequency = 246.9;

    keyMap['d'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['d']->noteName = "C#4";
    keyMap['d']->frequency = 277.2;

    keyMap['f'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['f']->noteName = "D4";
    keyMap['f']->frequency = 293.7;

    keyMap['q'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['q']->noteName = "E4";
    keyMap['q']->frequency = 329.6;

    keyMap['w'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['w']->noteName = "F#4";
    keyMap['w']->frequency = 370.0;

    keyMap['e'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['e']->noteName = "G#4";
    keyMap['e']->frequency = 415.3;

    keyMap['r'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['r']->noteName = "A4";
    keyMap['r']->frequency = 440.0;

    keyMap['j'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['j']->noteName = "A4";
    keyMap['j']->frequency = 440.0;

    keyMap['k'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['k']->noteName = "B4";
    keyMap['k']->frequency = 493.9;

    keyMap['l'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['l']->noteName = "C#5";
    keyMap['l']->frequency = 554.4;

    keyMap[';'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap[';']->noteName = "D5";
    keyMap[';']->frequency = 587.3;

    keyMap['u'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['u']->noteName = "E5";
    keyMap['u']->frequency = 659.3;

    keyMap['i'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['i']->noteName = "F#5";
    keyMap['i']->frequency = 740.0;

    keyMap['o'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['o']->noteName = "G#5";
    keyMap['o']->frequency = 830.6;

    keyMap['p'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['p']->noteName = "A5";
    keyMap['p']->frequency = 880;

}

void freeKeyMap()
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
