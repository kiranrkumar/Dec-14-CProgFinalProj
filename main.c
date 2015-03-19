/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  Main driver and helper functions for audio synthesizer
 *
 *        Version:  1.0
 *        Created:  12/11/2014 08:13:32
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
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <SOIL/SOIL.h>
#include <portaudio.h>
#include <sndfile.h>
#include "sigProcessing.h"

// OpenGL
#ifdef __MACOSX_CORE__
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

// Platform-dependent sleep routines.
#if defined( __WINDOWS_ASIO__ ) || defined( __WINDOWS_DS__ )
#include <windows.h>
#define SLEEP( milliseconds ) Sleep( (DWORD) milliseconds ) 
#else // Unix variants
#include <unistd.h>
#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )
#endif

/**************************************
 ************* '#define's *************
 **************************************/

#define TABLE_SIZE              123

//PortAudio Values
#define FORMAT                  paFloat32
#define BUFFER_SIZE             1024
#define SAMPLE                  float
#define SAMPLING_RATE           44100
#define MONO                    1
#define STEREO                  2
#define INIT_FREQ               0
#define INIT_MOD_FREQ           0
#define INIT_MOD_AMT            10
#define DEL_BUF_SIZE            (BUFFER_SIZE + SAMPLING_RATE)
#define VOLUME                  1//amplitude level
#define VOLUME_INC              .5//in DECIBELS
#define VOLUME_MIN              (.01)//amplitude level
#define VOLUME_MAX              1.5//amplitude level

//OpenGL Values
#define INIT_WIDTH              800
#define INIT_HEIGHT             600

//Signal Thresholds
#define NUM_MODES               2

#define DELAY_LEN_MIN           0
#define DELAY_LEN_MAX           1000//1000 ms = 1 second
#define DELAY_FX_INC            5

#define AM_FREQ_MAX             400
#define AM_FREQ_MIN             0
#define AM_FREQ_INC             5

#define FM_FREQ_MAX             400
#define FM_FREQ_MIN             0
#define FM_FREQ_INC             5

#define FM_MOD_MAX              100
#define FM_MOD_MIN              0
#define FM_MOD_INC              5

/**************************************
 *********** </'#define's> ************
 **************************************/


/**************************************
 **********Global variables************
 **************************************/


typedef double  MY_TYPE;
typedef char BYTE;   // 8-bit unsigned entity.

//PortAudio - structs
PaStream *g_stream;
paData data;
noteInfo **keyMap;

//PortAudio - additional variables
GLint g_buffer_size = BUFFER_SIZE;
SAMPLE g_buffer[BUFFER_SIZE];
unsigned int g_channels = STEREO;
float *delayBuffer;

bool VOICE_TWO_ON = false;

//OpenGL - structs
typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    GLuint texture_id;
    Pos center;
    int width;
    int height;
} Texture;

//OpenGL - width and height of window
GLsizei g_width = INIT_WIDTH;
GLsizei g_height = INIT_HEIGHT;
GLsizei g_last_width = INIT_WIDTH;
GLsizei g_last_height = INIT_HEIGHT;


//OpenGL - Threads Management
GLboolean g_ready = false;

//OpenGL - fill mode
GLenum g_fillmode = GL_FILL;

//OpenGL - light 0 position
GLfloat g_light0_pos[4] = { 2.0f, 1.2f, 4.0f, 1.0f };

//OpenGL - light 1 parameters
GLfloat g_light1_ambient[] = { .2f, .2f, .2f, 1.0f };
GLfloat g_light1_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat g_light1_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat g_light1_pos[4] = { -2.0f, 0.0f, -4.0f, 1.0f };

//OpenGL - fullscreen
GLboolean g_fullscreen = false;

//OpenGL - modelview stuff
GLfloat g_linewidth = 2.0f;

//Texture
Texture g_texture;

bool g_key_rotate_x = false;
bool g_key_rotate_y = false;
GLfloat g_inc_x = 0.0;
GLfloat g_inc_y = 0.0;

//Translation
bool g_translate = false;
Pos g_tex_init_pos = {0,0};
Pos g_tex_incr = {0,0};


/**************************************
 **********</Global variables>*********
 **************************************/



/**************************************
 *********Function Prototypes**********
 **************************************/

void initMapOfKeys(noteInfo **map);
void freeMapOfKeys(noteInfo **map);

//PortAudio Functions
void initialize_audio();
void stop_portAudio();

//OpenGL Functions
void idleFunc( );
void displayFunc( );
void reshapeFunc( int width, int height );
void keyboardFunc( unsigned char, int, int );
void initialize_graphics( );
void initialize_glut(int argc, char *argv[]);
void drawTimeDomainSignal(SAMPLE *buffer, float yCoord, float yDiv, float red, float green, float blue);
void drawScreen(SAMPLE *buffer);

/**************************************
 *********</Function Prototypes>*******
 **************************************/

void
bitmap_output(int x, int y, char *string, void *font)
{
    int len, i;
    
    glRasterPos2f(x, y);
    len = (int) strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}



//-----------------------------------------------------------------------------
// name: help()
// desc: display help window to assist with user control of the program
//-----------------------------------------------------------------------------
void help()
{
    printf( "----------------------------------------------------\n" );
    printf( "A Pretty Nifty Audio Synthesizer\n" );
    printf( "by Ryan Edwards & Kiran Kumar, 2014\n" );
    printf( "M.M. Music Technology\nNew York University - Class of 2016\n\n" );
    printf( "----------------------------------------------------\n" );
    printf( "'H' - print this help message\n" );
    printf( "'F' - toggle fullscreen\n" );
    printf( "'q' - quit\n" );
    printf( "----------------------------------------------------\n" );
    printf( "See the OpenGL window for synthesizer controls\n");
    printf( "----------------------------------------------------\n" );
    printf( "\n" );
}


//-----------------------------------------------------------------------------
// Name: paCallback( )
// Desc: callback from portAudio
//-----------------------------------------------------------------------------
static int paCallback( const void *inputBuffer,
        void *outputBuffer, unsigned long framesPerBuffer,
        const PaStreamCallbackTimeInfo* timeInfo,
        PaStreamCallbackFlags statusFlags, void *userData ) 
{
    int i;

    //Cast void pointers
    float *out = (float *)outputBuffer;
    float *in = (float *)inputBuffer;
    paData *audioData = (paData *)userData;

    //Phase information for oscillators

    //Voice 1
    static float phase = 0;
    static float prevPhase = 0;

    //Voice 2
    static float phase2 = 0;
    static float prevPhase2 = 0;

    static float AMphase = 0;
    static float AMprevPhase = 0;

    static float FMphase = 0;
    static float FMprevPhase = 0;
    static float FMphase2 = 0;
    static float FMprevPhase2 = 0;
    
    static int triDirection = 1;

    /***** Audio buffers *****/

    //tmp buffer to hold signal
    float *tmp = (float *)malloc(framesPerBuffer * sizeof(float)); //voice 1
    float *tmp2; //voice 2 - don't initialize unless it's on

    //AM buffer
    float *AMbuffer = (float *)malloc(framesPerBuffer * sizeof(float));

    //FM buffers
    float *FMbuffer1, *FMbuffer2;

    //--FM buffer for voice 1
    if (audioData->fmModFreq > 0)
    {
        FMbuffer1 = (float *)malloc(framesPerBuffer * sizeof(float));
        createFMBuffer(audioData->frequency, audioData->fmModFreq, audioData->fmModAmt, FMbuffer1, framesPerBuffer, SAMPLING_RATE, &FMphase, &FMprevPhase);
    }
    else
    {
        FMbuffer1 = NULL;
    }

    //--FM buffer for voice 2
    if (VOICE_TWO_ON && (audioData->fmModFreq2 > 0))
    {
        FMbuffer2 = (float *)malloc(framesPerBuffer * sizeof(float));
        createFMBuffer(audioData->frequency, audioData->fmModFreq2, audioData->fmModAmt2, FMbuffer2, framesPerBuffer, SAMPLING_RATE, &FMphase2, &FMprevPhase2);
    }
    else
    {
        FMbuffer2 = NULL;
    }


    //delay
    static int delayReader = 0;
    static int delayWriter = 0;

    //Get audio data if audio file or frequency if oscillator (based on mode)
    
    if (audioData->sigSrc == MICROPHONE)
    {
        //Read microphone input into buffer
        memcpy(tmp, in, framesPerBuffer * sizeof(float));
    }
    else
    {
        //generate proper wave type
        switch (audioData->sigWaveType)
        {
            case SINE:
                //create sine wave
                createSineWave(audioData->frequency, tmp, framesPerBuffer, 
                        SAMPLING_RATE, &phase, &prevPhase, FMbuffer1);
                break;
            case SAWTOOTH:
                //create sawtooth wave
                createSawWave(audioData->frequency, tmp, framesPerBuffer, 
                        SAMPLING_RATE, &phase, &prevPhase, FMbuffer1);
                break;
            case SQUARE:
                //create square wave
                createSquareWave(audioData->frequency, tmp, framesPerBuffer, 
                        SAMPLING_RATE, &phase, &prevPhase, FMbuffer1);
                break;
            case TRIANGLE:
                //create triangle wave
                createTriangleWave(audioData->frequency, tmp, framesPerBuffer, 
                        SAMPLING_RATE, &phase, &prevPhase, &triDirection, FMbuffer1);

                break;
        }
    }

    //Intialize voice 2 if necessary
    if (VOICE_TWO_ON)
    {
        tmp2 = (float *)malloc(framesPerBuffer * sizeof(float));
        switch (audioData->sigWaveType2)
        {
            case SINE:
                //create sine wave
                createSineWave(audioData->frequency, tmp2, framesPerBuffer, 
                        SAMPLING_RATE, &phase2, &prevPhase2, FMbuffer2);
                break;
            case SAWTOOTH:
                //create sawtooth wave
                createSawWave(audioData->frequency, tmp2, framesPerBuffer,
                        SAMPLING_RATE, &phase2, &prevPhase2, FMbuffer2);
                break;
            case SQUARE:
                //create square wave
                createSquareWave(audioData->frequency, tmp2, framesPerBuffer, 
                        SAMPLING_RATE, &phase2, &prevPhase2, FMbuffer2);
                break;
            case TRIANGLE:
                //create triangle wave
                createTriangleWave(audioData->frequency, tmp2, framesPerBuffer, 
                        SAMPLING_RATE, &phase2, &prevPhase2, &triDirection, FMbuffer2);
                break;
        }

    }
    
    //Initialize Amplitude Modulation buffer if AM frequency is not 0
    if (audioData->amModFreq != 0)
    {
        createSineWave(audioData->amModFreq, AMbuffer, framesPerBuffer, 
                SAMPLING_RATE, &AMphase, &AMprevPhase, NULL);
    }

    //Shift delayWriter if the delay length has changed
    delayWriter += (audioData->delayLen - audioData->prevDelayLen);

    for (i = 0; i < framesPerBuffer; i++)
    {
        //Set AM buffer to all 1s if the AM frequency is 0
        if (audioData->amModFreq == 0)
        {
            AMbuffer[i] = 1;
        }
        
        //Apply Volume to both voices
        tmp[i] *= data.volume;
        if (VOICE_TWO_ON)
            tmp2[i] *= data.volume2;
        
        //store voice2 values into voice1 buffer if voice2 is on
        if (VOICE_TWO_ON)
        {
            tmp[i] = (tmp[i] + tmp2[i])/2.;
        }

        //Write to the delay buffer
        delayBuffer[delayWriter++] = tmp[i];

        //Add the appropriate mix of dry and wet signals
        tmp[i] = tmp[i] * .01 * audioData->delayPctDry + delayBuffer[delayReader++] * .01 * audioData->delayPctWet;

        //Wrap the delay indices around as necessary
        delayWriter %= DEL_BUF_SIZE;
        delayReader %= DEL_BUF_SIZE;


        //Apply Amplitude Modulation
        tmp[i] *= AMbuffer[i];

    }

    //set the previous delay equal to the delay so that the delay writer pointer doesn't shift more the necessary
    audioData->prevDelayLen = audioData->delayLen;

    //Copy contents of temporary buffer into output buffer
    memcpy(out, tmp, framesPerBuffer * sizeof(float));

    //Free dynamically allocated buffers
    free(tmp);
    if (VOICE_TWO_ON)
        free(tmp2);
    if (FMbuffer1 != NULL)
        free(FMbuffer1);
    if (FMbuffer2 != NULL)
        free(FMbuffer2);
    
    g_ready = true;
    return 0;
}

//-----------------------------------------------------------------------------
// Name: initialize_glut( )
// Desc: Initializes Glut with the global vars
//-----------------------------------------------------------------------------
void initialize_glut(int argc, char *argv[]) {
    // initialize GLUT
    glutInit( &argc, argv );
    // double buffer, use rgb color, enable depth buffer
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    // initialize the window size
    glutInitWindowSize( g_width, g_height );
    // set the window postion
    glutInitWindowPosition( 400, 100 );
    // create the window
    glutCreateWindow("Ryan Edwards and Kiran Kumar - Final Project");
    // full screen
    if( g_fullscreen )
        glutFullScreen();

    // set the idle function - called when idle
    glutIdleFunc( idleFunc );
    // set the display function - called when redrawing
    glutDisplayFunc( displayFunc );
    // set the reshape function - called when client area changes
    glutReshapeFunc( reshapeFunc );
    // set the keyboard function - called on keyboard events
    glutKeyboardFunc( keyboardFunc );

    // Load Texture
    g_texture.texture_id = SOIL_load_OGL_texture
        (
         argv[1],
         SOIL_LOAD_AUTO,
         SOIL_CREATE_NEW_ID,
         SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y
        );
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glEnable( GL_TEXTURE_2D );

    // Get Width and Height
    unsigned char *image = SOIL_load_image(argv[1], &g_texture.width, &g_texture.height, NULL, 0);
    SOIL_free_image_data(image);
    
    // do our own initialization
    initialize_graphics( );  
}

//-----------------------------------------------------------------------------
// Name: initialize_audio( RtAudio *dac )
// Desc: Initializes PortAudio with the global vars and the stream
//-----------------------------------------------------------------------------
void initialize_audio() {

    PaStreamParameters outputParameters;
    PaStreamParameters inputParameters;
    PaError err;

    /* Initialize PortAudio */
    Pa_Initialize();

    /* Set input stream parameters */
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = MONO;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = 
        Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    /* Set output stream parameters */
    outputParameters.device = Pa_GetDefaultOutputDevice();
    outputParameters.channelCount = MONO; //g_channels;
    outputParameters.sampleFormat = paFloat32;
    outputParameters.suggestedLatency = 
        Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    /* Open audio stream */
    err = Pa_OpenStream( &g_stream,
            &inputParameters,
            &outputParameters,
            SAMPLING_RATE, g_buffer_size, paNoFlag, 
            paCallback, &data);

    if (err != paNoError) {
        printf("PortAudio error: open stream: %s\n", Pa_GetErrorText(err));
    }

    /* Start audio stream */
    err = Pa_StartStream( g_stream );
    if (err != paNoError) {
        printf(  "PortAudio error: start stream: %s\n", Pa_GetErrorText(err));
    }
}

void stop_portAudio() {
    PaError err;

    /* Stop audio stream */
    err = Pa_StopStream( g_stream );
    if (err != paNoError) {
        printf(  "PortAudio error: stop stream: %s\n", Pa_GetErrorText(err));
    }
    /* Close audio stream */
    err = Pa_CloseStream(g_stream);
    if (err != paNoError) {
        printf("PortAudio error: close stream: %s\n", Pa_GetErrorText(err));
    }
    /* Terminate audio stream */
    err = Pa_Terminate();
    if (err != paNoError) {
        printf("PortAudio error: terminate: %s\n", Pa_GetErrorText(err));
    }
}

//=============================================================================
//=============================================================================
//====================== MAIN FUNCTION ========================================
//=============================================================================
//=============================================================================
// Name: main
// Desc: Main function for the synthesizer
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================
int main( int argc, char *argv[] )
{
    //Zero out global texture
    memset(&g_texture, 0, sizeof(Texture));

    //Initialize keyboard mapping
    initMapOfKeys(keyMap);
    
    // Initialize PA data struct values
    data.volume = 1.;
    data.volume2 = 1.;
    data.frequency = INIT_FREQ;
    data.sigSrc = OSCILLATOR;
    data.fmModFreq = INIT_MOD_FREQ;
    data.fmModFreq2 = INIT_MOD_FREQ;
    data.fmModAmt = INIT_MOD_AMT;
    data.fmModAmt2 = INIT_MOD_AMT;
    data.amModFreq = INIT_MOD_FREQ;
    data.sigWaveType = SINE;
    data.sigWaveType2 = SINE;
    data.delayLen = 0;
    data.prevDelayLen = 0;
    delayBuffer = (float *)malloc(DEL_BUF_SIZE * sizeof(float));
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        delayBuffer[i] = 0;
    }
    data.delayPctDry = 100;
    data.delayPctWet = 0;

    data.delayReader = 0;
    data.delayWriter = 0;
    
    // Initialize Glut
    initialize_glut(argc, argv);

    // Initialize PortAudio
    initialize_audio();

    // Print help
    help();

    // Wait until 'q' is pressed to stop the process
    glutMainLoop();

    // This will never get executed

    return EXIT_SUCCESS;
}

//-----------------------------------------------------------------------------
// Name: idleFunc( )
// Desc: callback from GLUT
//-----------------------------------------------------------------------------
void idleFunc( )
{
    // render the scene
    glutPostRedisplay( );
}

//-----------------------------------------------------------------------------
// Name: keyboardFunc( )
// Desc: key event
//-----------------------------------------------------------------------------
void keyboardFunc( unsigned char key, int x, int y )
{
    //Check if the key was one of the notes on the synthesizer
    if (keyMap[key] != NULL)
    {
        data.frequency = keyMap[key]->frequency;
        printf("[synthesizer]: Note - %s\n", keyMap[key]->noteName);
    }
    else
    {
        switch( key )
        {
            // Print Help
            case 'H':
                help();
                break;

                // Fullscreen
            case 'F':
                if( !g_fullscreen )
                {
                    g_last_width = g_width;
                    g_last_height = g_height;
                    glutFullScreen();
                }
                else
                    glutReshapeWindow( g_last_width, g_last_height );
                g_fullscreen = !g_fullscreen;
                printf("[synthesizer]: fullscreen: %s\n", g_fullscreen ? "ON" : "OFF" );
                break;
            //Select mode
            case 'B':
                data.sigSrc = (data.sigSrc + 1) % NUM_MODES;
                printf("[synthesizer]: MODE = %d\n", data.sigSrc);
                break;
            //Decrease volume - Voice 1
            case 'z':
                if ( (data.volume * dbToAmplitude(-VOLUME_INC) ) > VOLUME_MIN)
                {
                    data.volume *= dbToAmplitude(-VOLUME_INC);
                    printf("[synthesizer]: Voice 1 Volume: %f\n", data.volume);
                }
                else
                {
                    data.volume = VOLUME_MIN;
                    printf("[synthesizer]: Voice 1 Volume: %f\n", data.volume);
                }
                break;
            //Increase volume - Voice 1
            case 'x':
                if ( (data.volume * dbToAmplitude(VOLUME_INC) ) < VOLUME_MAX)
                {
                    data.volume *= dbToAmplitude(VOLUME_INC);
                    printf("[synthesizer]: Voice 1 Volume: %f\n", data.volume);
                }
                else
                {
                    data.volume = VOLUME_MAX;
                    printf("[synthesizer]: Voice 1 Volume: %f\n", data.volume);
                }
                break;
            //Decrease volume - Voice 2
            if (VOICE_TWO_ON)
            {
                case '1':
                if ( (data.volume2 * dbToAmplitude(-VOLUME_INC) ) > VOLUME_MIN)
                    {
                        data.volume2 *= dbToAmplitude(-VOLUME_INC);
                        printf("[synthesizer]: Voice 2 Volume: %f\n", data.volume2);
                    }
                    else
                    {
                        data.volume2 = VOLUME_MIN;
                        printf("[synthesizer]: Voice 2 Volume: %f\n", data.volume2);
                    }
            }
                    break;
            //Increase volume - Voice 2
            case '2':
            if (VOICE_TWO_ON)
            {
                if ( (data.volume2 * dbToAmplitude(VOLUME_INC) ) < VOLUME_MAX)
                    {
                        data.volume2 *= dbToAmplitude(VOLUME_INC);
                        printf("[synthesizer]: Voice 2 Volume: %f\n", data.volume2);
                    }
                    else
                    {
                        data.volume2 = VOLUME_MAX;
                        printf("[synthesizer]: Voice 2 Volume: %f\n", data.volume2);
                    }
            }
                break;
            //Increase delay
            case '5':
                if ((data.delayLenMs + DELAY_FX_INC) > DELAY_LEN_MAX)
                    setDelayLen(DELAY_LEN_MAX, &data, SAMPLING_RATE);
                else
                    addDelayLen(DELAY_FX_INC, &data, SAMPLING_RATE);
                printf("[synthesizer]: Delay: %d ms\n", data.delayLenMs);
                break;
            //Decrease delay
            case '4':
                if ((data.delayLenMs - DELAY_FX_INC) < DELAY_LEN_MIN)
                    setDelayLen(DELAY_LEN_MIN, &data, SAMPLING_RATE);
                else
                    addDelayLen(-DELAY_FX_INC, &data, SAMPLING_RATE);
                printf("[synthesizer]: Delay: %d ms\n", data.delayLenMs);
                break;
            //Increase wet (delayed) signal percentage
            case '7':
                if ((data.delayPctWet + DELAY_FX_INC) > 100)
                {
                    data.delayPctWet = 100;
                    data.delayPctDry = 0;
                }
                else
                {
                    data.delayPctWet += DELAY_FX_INC;
                    data.delayPctDry -= DELAY_FX_INC;
                }
                printf("[synthesizer]: Delay - Dry: %f  Wet: %f\n", data.delayPctDry, data.delayPctWet);
                break;
            //Decrease wet (delayed) signal percentage
            case '6':
                if ((data.delayPctWet - DELAY_FX_INC) < 0)
                {
                    data.delayPctWet = 0;
                    data.delayPctDry = 100;
                }
                else
                {
                    data.delayPctWet -= DELAY_FX_INC;
                    data.delayPctDry += DELAY_FX_INC;
                }
                printf("[synthesizer]: Delay - Dry: %f  Wet: %f\n", data.delayPctDry, data.delayPctWet);
                break;
            //Change wave type - Voice 1
            case 'c':
                data.sigWaveType = (data.sigWaveType + 1) % 4;
                break;
            //Change wave type - Voice 2
            case '3':
                data.sigWaveType2 = (data.sigWaveType2 + 1) % 4;
                break;
            //Turn Voice 2 On/Off
            case 'p':
                VOICE_TWO_ON = !VOICE_TWO_ON;
                break;
            //Subtract AM Modulation
            case 'Z':
                if (data.amModFreq <= (AM_FREQ_INC + AM_FREQ_MIN))
                    data.amModFreq = AM_FREQ_MIN;
                else
                    data.amModFreq -= AM_FREQ_INC;
                printf("[synthesizer]: AM Modulator Freq: %f\n", data.amModFreq);
                break;
            //Add AM Modulation
            case 'X':
                if (data.amModFreq >= (AM_FREQ_MAX - AM_FREQ_INC))
                    data.amModFreq = AM_FREQ_MAX;
                else
                    data.amModFreq += AM_FREQ_INC;
                printf("[synthesizer]: AM Modulator Freq: %f\n", data.amModFreq);
                break;
            //Subtract FM Modulation Frequency - Voice 1
            case 'n':
                if (data.fmModFreq <= (FM_FREQ_INC + FM_FREQ_MIN))
                    data.fmModFreq = FM_FREQ_MIN;
                else
                    data.fmModFreq -= FM_FREQ_INC;
                printf("[synthesizer]: FM Modulator Freq - Voice 1: %f\n", data.fmModFreq);
                break;
            //Add FM Modulation Frequency - Voice 1
            case 'm':
                if (data.fmModFreq >= (FM_FREQ_MAX - FM_FREQ_INC))
                    data.fmModFreq = FM_FREQ_MAX;
                else
                    data.fmModFreq += FM_FREQ_INC;
                printf("[synthesizer]: FM Modulator Freq - Voice 1: %f\n", data.fmModFreq);
                break;
            //Subtract FM Modulation Amount - Voice 1
            case 'N':
                if (data.fmModAmt <= (FM_MOD_MIN + FM_MOD_INC))
                    data.fmModAmt = FM_MOD_MIN;
                else
                    data.fmModAmt -= FM_MOD_INC;
                printf("[synthesizer]: FM Modulator Amount - Voice 1: %f\n", data.fmModAmt);
                break;
            //Add FM Modulation Amount - Voice 1
            case 'M':
                if (data.fmModAmt >= (FM_MOD_MAX - FM_MOD_INC))
                    data.fmModAmt = FM_MOD_MAX;
                else
                    data.fmModAmt += FM_MOD_INC;
                printf("[synthesizer]: FM Modulator Amount - Voice 1: %f\n", data.fmModAmt);
                break;
            //Subtract FM Modulation - Voice 2
            case '8':
                if (data.fmModFreq2 <= (FM_FREQ_INC + FM_FREQ_MIN))
                    data.fmModFreq2 = FM_FREQ_MIN;
                else
                    data.fmModFreq2 -= FM_FREQ_INC;
                printf("[synthesizer]: FM Modulator Freq - Voice 2: %f\n", data.fmModFreq2);
                break;
            //Add FM Modulation - Voice 2
            case '9':
                if (data.fmModFreq2 >= (FM_FREQ_MAX - FM_FREQ_INC))
                    data.fmModFreq2 = FM_FREQ_MAX;
                else
                    data.fmModFreq2 += FM_FREQ_INC;
                printf("[synthesizer]: FM Modulator Freq - Voice 2: %f\n", data.fmModFreq2);
                break;
            //Subtract FM Modulation Amount - Voice 2
            case '*':
                if (data.fmModAmt2 <= (FM_MOD_MIN + FM_MOD_INC))
                    data.fmModAmt2 = FM_MOD_MIN;
                else
                    data.fmModAmt2 -= FM_MOD_INC;
                printf("[synthesizer]: FM Modulator Amount - Voice 2: %f\n", data.fmModAmt2);
                break;
            //Add FM Modulation Amount - Voice 2
            case '(':
                if (data.fmModAmt2 >= (FM_MOD_MAX - FM_MOD_INC))
                    data.fmModAmt2 = FM_MOD_MAX;
                else
                    data.fmModAmt2 += FM_MOD_INC;
                printf("[synthesizer]: FM Modulator Amount - Voice 2: %f\n", data.fmModAmt2);
                break;
            //Quit the program
            case 'q':
                // Close Stream before exiting
                stop_portAudio();
                freeMapOfKeys(keyMap);
                free(delayBuffer);
                printf("\n[synthesizer]: Goodbye!\n");
                exit( 0 );
            break;
        }
    }
}

//-----------------------------------------------------------------------------
// Name: reshapeFunc( )
// Desc: called when window size changes
//-----------------------------------------------------------------------------
void reshapeFunc( int w, int h )
{
    // save the new window size
    g_width = w; g_height = h;
    // set the matrix mode to project
    glMatrixMode( GL_PROJECTION );
    // load the identity matrix
    glLoadIdentity( );
    // map the view port to the client area
    glViewport( 0, 0, w, h );
    // create the viewing frustum
    GLfloat fovy = 45.0f;
    GLfloat zNear = .05f;
    GLfloat zFar = 2500.0f;
    gluPerspective( fovy, (GLfloat) w / (GLfloat) h, zNear, zFar);
    // set the matrix mode to modelview
    glMatrixMode( GL_MODELVIEW );
    // load the identity matrix
    glLoadIdentity( );

    // gluLookAt( 0.0f, 0.0f, 30.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
    gluLookAt( 0.0f, 0.0f, h / 2.0f / tanf(fovy/2.0f), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f );
}


//-----------------------------------------------------------------------------
// Name: initialize_graphics( )
// Desc: sets initial OpenGL states and initializes any application data
//-----------------------------------------------------------------------------
void initialize_graphics()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);                 //Black Background
    // enable depth
    glEnable( GL_DEPTH_TEST );
    // set fill mode
    glPolygonMode( GL_FRONT_AND_BACK, g_fillmode );
    // line width
    glLineWidth( g_linewidth );
}

//-----------------------------------------------------------------------------
// Name: displayFunc( )
// Desc: callback function invoked to draw the client area
//-----------------------------------------------------------------------------
void displayFunc( )
{
    
    // clear the color and depth buffers
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    // local variables
    SAMPLE buffer[g_buffer_size];

    // wait for data
    while( !g_ready ) usleep( 1000 );

    // Hand off to audio callback thread
    g_ready = false;

    drawScreen(buffer);

    // flush gl commands
    glFlush();

    // swap the buffers
    glutSwapBuffers( );
}

void drawScreen(SAMPLE *buffer)
{

    float x = 0, y = 0;
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    {
        // Translate
        glTranslatef(x + g_texture.center.x + g_tex_incr.x, y + g_texture.center.y + g_tex_incr.y, -200.0f);

        glPushMatrix();
        //draw Picture
        GLfloat halfX = g_width / 2.0f;
        GLfloat halfY = g_height / 2.0f;
        
        bitmap_output(0, 0, "Helvetica is yet another bitmap font.", GLUT_BITMAP_HELVETICA_18);


        // Use the texture
        glBindTexture(GL_TEXTURE_2D, g_texture.texture_id);

        glBegin( GL_QUADS );
        glTexCoord2d(0.0,0.0); 
        glVertex3d(-halfX, -halfY, 0);
        glTexCoord2d(1.0,0.0); 
        glVertex3d(halfX, -halfY, 0);
        glTexCoord2d(1.0,1.0); 
        glVertex3d(halfX, halfY, 0);
        glTexCoord2d(0.0,1.0); 
        glVertex3d(-halfX, halfY, 0);
        glEnd();
        glPopMatrix();

       
    }
    
    glPopMatrix();
    
}

void initMapOfKeys(noteInfo **map)
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

    //now set frequency values for the appropriate keys
    

    keyMap['a'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['a']->noteName = "A3";
    keyMap['a']->frequency = 220.00;

    keyMap['w'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['w']->noteName = "A#/Bb3";
    keyMap['w']->frequency = 233.08;

    keyMap['s'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['s']->noteName = "B3";
    keyMap['s']->frequency = 246.94;

    keyMap['d'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['d']->noteName = "C4";
    keyMap['d']->frequency = 261.63;

    keyMap['r'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['r']->noteName = "C#/Db4";
    keyMap['r']->frequency = 277.18;

    keyMap['f'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['f']->noteName = "D4";
    keyMap['f']->frequency = 293.66;

    keyMap['t'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['t']->noteName = "D#/Eb4";
    keyMap['t']->frequency = 311.13;
    
    keyMap['g'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['g']->noteName = "E4";
    keyMap['g']->frequency = 329.63;

    keyMap['h'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['h']->noteName = "F4";
    keyMap['h']->frequency = 349.23;

    keyMap['u'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['u']->noteName = "F#/Gb4";
    keyMap['u']->frequency = 369.9;

    keyMap['j'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['j']->noteName = "G4";
    keyMap['j']->frequency = 392.00;

    keyMap['i'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['i']->noteName = "G#/Ab4";
    keyMap['i']->frequency = 415.30;
    
    keyMap['k'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['k']->noteName = "A4";
    keyMap['k']->frequency = 440.00;

    keyMap['o'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['o']->noteName = "A#/Bb4";
    keyMap['o']->frequency = 466.16;

    keyMap['l'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap['l']->noteName = "B4";
    keyMap['l']->frequency = 493.88;

    keyMap[';'] = (noteInfo*)malloc(sizeof(noteInfo));
    keyMap[';']->noteName = "C5";
    keyMap[';']->frequency = 523.25;

}

void freeMapOfKeys(noteInfo **map)
{
    //loop index
    int i;

    //free each non-null element, 
    for (i = 0; i < TABLE_SIZE; i++)
    {
        if (map[i] != NULL)
        {
            free(map[i]);
        }
    }

    //free the array itself
    free(map);

}

