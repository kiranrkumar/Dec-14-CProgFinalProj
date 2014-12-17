/*
 * =====================================================================================
 *
 *       Filename:  main2.c
 *
 *    Description:  main driver and helper functions for the C Programming final project
 *                  for Ryan Edwards and Kiran Kumar
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
#define DEL_BUF_SIZE            (BUFFER_SIZE + SAMPLING_RATE)
#define VOLUME                  1//amplitude level
#define VOLUME_INC              .5//in DECIBELS
#define VOLUME_MIN              (.01)//amplitude level
#define VOLUME_MAX              1.5//amplitude level

//ADSR Values
#define ATTACK_TIME             100
#define DECAY_TIME              800
#define RELEASE_TIME            900

//OpenGL Values
#define INIT_WIDTH              800
#define INIT_HEIGHT             600
#define ROTATION_INCR           .75f

//Signal Thresholds
#define NUM_MODES               3
#define DELAY_LEN_MIN           0
#define DELAY_LEN_MAX           1000//1000 ms = 1 second
#define DELAY_FX_INC            5
#define AM_FREQ_MAX             400
#define AM_FREQ_MIN             0
#define AM_FREQ_INC             5
#define FM_FREQ_MAX             400
#define FM_FREQ_MIN             0
#define FM_FREQ_INC             5

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

bool NOTE_PRESSED = false;
bool NOTE_RELEASED = false;
bool NOTE_ON = false;

float curVolume = .6;

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



//SAMPLE g_buffer_sine[BUFFER_SIZE];
//SAMPLE g_buffer_tri[BUFFER_SIZE];
//SAMPLE g_buffer_saw[BUFFER_SIZE];
//SAMPLE g_buffer_imp[BUFFER_SIZE];
bool g_key_rotate_x = false;
bool g_key_rotate_y = false;
GLfloat g_inc_x = 0.0;
GLfloat g_inc_y = 0.0;

//For mouse movement

int posX = 0;
int posY = 0;
int incrX = 0;
int incrY = 0;

//Translation
bool g_translate = false;
Pos g_tex_init_pos = {0,0};
Pos g_tex_incr = {0,0};


/**************************************
 **********</Global variables>*********
 **************************************/

void initDelayBuffer(float *buffer, int bufferLen)
{

}

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
void keyboardUpFunc (unsigned char, int, int);
void specialKey( int key, int x, int y );
void specialUpKey( int key, int x, int y);
void initialize_graphics( );
void initialize_glut(int argc, char *argv[]);
void drawTimeDomainSignal(SAMPLE *buffer, float yCoord, float yDiv, float red, float green, float blue);
void drawScreen(SAMPLE *buffer);
void rotateView();
void mouseFunc(int button, int state, int x, int y);
void mouseMotionFunc(int x, int y);

/**************************************
 *********</Function Prototypes>*******
 **************************************/



//-----------------------------------------------------------------------------
// name: help()
// desc: ...
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

    //TODO - Implement audio processing
    
    int i, j;

    //Cast void pointers
    float *out = (float *)outputBuffer;
    float *in = (float *)inputBuffer;
    paData *audioData = (paData *)userData;

    //Temporary audio buffer
    float *tmp = (float *)malloc(framesPerBuffer * sizeof(float));

    //Phase information for oscillators
    static float phase = 0;
    static float prevPhase = 0;
    static int triDirection = 1;

    //delay
    static int delayReader = 0;
    static int delayWriter = 0;

    //attack, decay, and release values
    static float attackSlope = VOLUME / ATTACK_TIME;
    static float decaySlope = - (VOLUME / DECAY_TIME);
    static float releaseSlope = - (VOLUME / RELEASE_TIME);

    //1. Get audio data if audio file or frequency if oscillator (based on mode)
    
    if (audioData->sigSrc == SOUNDFILE)
    {
        //TODO - read data from sound file into buffer
    }
    else if (audioData->sigSrc == MICROPHONE)
    {
        //(FINISHED) read microphone input into buffer
        memcpy(tmp, in, framesPerBuffer * sizeof(float));
    }
    else
    {
        //(FINISHED) generate proper wave type
        switch (audioData->sigWaveType)
        {
            case SINE:
                //create sine wave
                createSineWave(audioData->frequency, tmp, framesPerBuffer, SAMPLING_RATE, &phase, &prevPhase);
                break;
            case SAWTOOTH:
                //create sawtooth wave
                createSawWave(audioData->frequency, tmp, framesPerBuffer, SAMPLING_RATE, &phase, &prevPhase);
                break;
            case SQUARE:
                //create square wave
                createSquareWave(audioData->frequency, tmp, framesPerBuffer, SAMPLING_RATE, &phase, &prevPhase);
                break;
            case TRIANGLE:
                //create triangle wave
                createTriangleWave(audioData->frequency, tmp, framesPerBuffer, 
                        SAMPLING_RATE, &phase, &prevPhase, &triDirection);

                break;
        }
    }
    

    //2. Perform FM Modulation

    //3. Perform AM Modulation
    
    //4. Perform delay based on delay length. Store delayed signal in separate tmp buffer

    delayWriter += (audioData->delayLen - audioData->prevDelayLen);
    j = delayReader;
    
    //printf("Delay: %d  Previous Delay: %d  Readpointer: %d\n", audioData->delayLen, audioData->prevDelayLen, readPtr);
    for (i = 0; i < framesPerBuffer; i++)
    {
        delayBuffer[delayWriter++] = tmp[i];
        tmp[i] = tmp[i] * .01 * audioData->delayPctDry + delayBuffer[delayReader++] * .01 * audioData->delayPctWet;
        //printf("tmp[i]: %f\n", tmp[i]);
        //printf("delayBuffer[i]: %f\n", delayBuffer[i]);
        //printf("j: %d\n", j);
        
        delayWriter %= DEL_BUF_SIZE;
        delayReader %= DEL_BUF_SIZE;



        /*
        if (audioData->sigSrc == OSCILLATOR)
        //Apply Attack/Decay or Release
        {

            if (NOTE_PRESSED)
            {
                if (i < ATTACK_TIME)
                {
                    tmp[i] *= (VOLUME * 1.5) * (i/ATTACK_TIME);
                }
                else if ((i >= ATTACK_TIME) && (i < ATTACK_TIME + DECAY_TIME)) 
                {
                    tmp[i] -= (0.5 * ((i - ATTACK_TIME) / DECAY_TIME));
                }
                
                NOTE_PRESSED = false;
                NOTE_ON = true;
            }
            else if (NOTE_RELEASED)
            {
                if (i > (framesPerBuffer - RELEASE_TIME))
                {
                    tmp[i] *= ((framesPerBuffer - i - RELEASE_TIME) / RELEASE_TIME);
                }
    
                NOTE_RELEASED = false;
                NOTE_ON = false;
            }
            else if (!NOTE_ON)
            {
                tmp[i] = 0;
            }
        }
        */


        //Apply Volume
        tmp[i] *= data.volume;

    }

    audioData->prevDelayLen = audioData->delayLen;

    //6. Scale levels appropriately
    
    //7. Apply volume
    
    //8. Apply ADSR envelope

    //(FINISHED) 9. Copy contents of temporary buffer into output buffer
    memcpy(out, tmp, framesPerBuffer * sizeof(float));
    
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
    // set the keyboard function - called on releasing keyboard keys
    glutKeyboardUpFunc( keyboardUpFunc );
    // set window's to specialKey callback
    glutSpecialFunc( specialKey );
    // set window's to specialUpKey callback (when the key is up is called)
    glutSpecialUpFunc( specialUpKey );
    // set the mouse function for new clicks
    glutMouseFunc( mouseFunc );
    // set the mouse function for motion when a button is pressed
    glutMotionFunc( mouseMotionFunc ); 

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
// Desc: ...
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
    data.frequency = INIT_FREQ;
    data.sigSrc = OSCILLATOR;
    data.fmModFreq = INIT_MOD_FREQ;
    data.amModFreq = INIT_MOD_FREQ;
    data.sigWaveType = SINE;
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
        if (!NOTE_PRESSED)
        {
            NOTE_PRESSED = true; //direct PortAudio to apply attack and decay time
            printf("Pressing %c\n", key);
        }
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
            //Decrease volume
            case 'z':
                data.volume++;
                break;
            //Increase volume
            case 'x':
                data.volume--;
                break;
            //Increase delay
            case 'b':
                if ((data.delayLenMs + DELAY_FX_INC) > DELAY_LEN_MAX)
                    setDelayLen(DELAY_LEN_MAX, &data, SAMPLING_RATE);
                else
                    addDelayLen(DELAY_FX_INC, &data, SAMPLING_RATE);
                printf("[synthesizer]: Delay: %d ms\n", data.delayLenMs);
                break;
            //Decrease delay
            case 'v':
                if ((data.delayLenMs - DELAY_FX_INC) < DELAY_LEN_MIN)
                    setDelayLen(DELAY_LEN_MIN, &data, SAMPLING_RATE);
                else
                    addDelayLen(-DELAY_FX_INC, &data, SAMPLING_RATE);
                printf("[synthesizer]: Delay: %d ms\n", data.delayLenMs);
                break;
            //Increase wet (delayed) signal percentage
            case 'm':
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
            case 'n':
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
            //Change wave type
            case 'c':
                data.sigWaveType = (data.sigWaveType + 1) % 4;
                break;
            //Add AM Modulation
            case 'Z':
                if (data.amModFreq <= (AM_FREQ_INC + AM_FREQ_MIN))
                    data.amModFreq = AM_FREQ_MIN;
                else
                    data.amModFreq -= AM_FREQ_INC;
                break;
            //Subtract AM Modulation
            case 'X':
                if (data.amModFreq >= (AM_FREQ_MAX - AM_FREQ_INC))
                    data.amModFreq = AM_FREQ_MAX;
                else
                    data.amModFreq -= AM_FREQ_INC;
                break;
            //Add FM Modulation
            case 'N':
                if (data.fmModFreq <= (FM_FREQ_INC + FM_FREQ_MIN))
                    data.fmModFreq = FM_FREQ_MIN;
                else
                    data.fmModFreq -= FM_FREQ_INC;
                break;
            //Subtract AM Modulation
            case 'M':
                if (data.fmModFreq >= (FM_FREQ_MAX - FM_FREQ_INC))
                    data.fmModFreq = FM_FREQ_MAX;
                else
                    data.fmModFreq -= FM_FREQ_INC;
                break;
    
            case 'q':
                // Close Stream before exiting
                stop_portAudio();
                freeMapOfKeys(keyMap);
                free(delayBuffer);
                printf("\n[syntheizer]: Goodbye!\n");
                exit( 0 );
            break;
        }
    }
}

void keyboardUpFunc (unsigned char key, int x, int y)
{
    //Check if the key was one of the notes on the synthesizer
    if (keyMap[key] != NULL)
    {
        NOTE_RELEASED = true; //direct PortAudio to apply release time
    }
    printf("Released %c\n", key);

}

//-----------------------------------------------------------------------------
// Name: specialKey( int key, int x, int y)
// Desc: Callback to know when a special key is pressed
//-----------------------------------------------------------------------------
void specialKey(int key, int x, int y) { 
    // Check which (arrow) key is pressed
    /*  switch(key) {
        case GLUT_KEY_LEFT : // Arrow key left is pressed
            g_key_rotate_y = true;
            g_inc_y = -ROTATION_INCR;
            break;
        case GLUT_KEY_RIGHT :    // Arrow key right is pressed
            g_key_rotate_y = true;
            g_inc_y = ROTATION_INCR;
            break;
        case GLUT_KEY_UP :        // Arrow key up is pressed
            g_key_rotate_x = true;
            g_inc_x = -ROTATION_INCR;
            break;
        case GLUT_KEY_DOWN :    // Arrow key down is pressed
            g_key_rotate_x = true;
            g_inc_x = ROTATION_INCR;
            break;   
    }*/

    switch(key) 
    {
        case GLUT_KEY_UP: //Increase volume via up arrow
            if (data.volume >= VOLUME_MAX)
                data.volume *= dbToAmplitude(VOLUME_INC);
            break;
        case GLUT_KEY_DOWN: //Decrease volume via down arrow
            if (data.volume <= VOLUME_MIN)
                data.volume *= dbToAmplitude(-VOLUME_INC);
            break;
    }
}  

//-----------------------------------------------------------------------------
// Name: specialUpKey( int key, int x, int y)
// Desc: Callback to know when a special key is up
//-----------------------------------------------------------------------------
void specialUpKey( int key, int x, int y) {
    // Check which (arrow) key is unpressed
    /*  switch(key) {
        case GLUT_KEY_LEFT : // Arrow key left is unpressed
            printf("[synthesizer]: LEFT\n");
            g_key_rotate_y = false;
            break;
        case GLUT_KEY_RIGHT :    // Arrow key right is unpressed
            printf("[synthesizer]: RIGHT\n");
            g_key_rotate_y = false;
            break;
        case GLUT_KEY_UP :        // Arrow key up is unpressed
            printf("[synthesizer]: UP\n");
            g_key_rotate_x = false;
            break;
        case GLUT_KEY_DOWN :    // Arrow key down is unpressed
            printf("[synthesizer]: DOWN\n");
            g_key_rotate_x = false;
            break;   
    }*/
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

    // copy currently playing audio into buffer
    //memcpy( buffer, g_buffer, g_buffer_size * sizeof(SAMPLE) );

    // Hand off to audio callback thread
    g_ready = false;

    drawScreen(buffer);

    // flush gl commands
    glFlush();

    // swap the buffers
    glutSwapBuffers( );
}

/*********************************
 **********<Kiran's Code>*********
 *********************************/

void drawTimeDomainSignal(SAMPLE *buffer, float yCoord, float yDiv, float red, float green, float blue)
{
    int i;
    // Initialize initial x
    GLfloat x = -5.0;

    // Calculate increment x
    GLfloat xinc = fabs((2*x)/g_buffer_size);

    glPushMatrix();
    {
        rotateView();
        glColor3f(red, green, blue);

        glBegin(GL_LINE_STRIP);

        // Draw Full Windowed Time Domain Signal
        glPushMatrix();
        glTranslatef(0, yCoord, 0);
        for (i = 0; i < g_buffer_size; i++)
        {
            glVertex3f(x, (2*buffer[i] / yDiv) + yCoord, 0.0f);
            x += xinc;
        }

        glEnd();

    }
    glPopMatrix();
}

//-----------------------------------------------------------------------------
// Name: void rotateView ()
// Desc: Rotates the current view by the angle specified in the globals
//-----------------------------------------------------------------------------
void rotateView () 
{
    static GLfloat angle_x = 0;
    static GLfloat angle_y = 0;
    if (g_key_rotate_y) {
        glRotatef ( angle_y += g_inc_y, 0.0f, 1.0f, 0.0f );
    }
    else {
        glRotatef (angle_y, 0.0f, 1.0f, 0.0f );
    }

    if (g_key_rotate_x) {
        glRotatef ( angle_x += g_inc_x, 1.0f, 0.0f, 0.0f );
    }
    else {
        glRotatef (angle_x, 1.0f, 0.0f, 0.0f );
    }
}

//-----------------------------------------------------------------------------
// Name: mouseFunc(int button, int state, int x, int y)
// Desc: Callback to manage the mouse input when click new button
//-----------------------------------------------------------------------------
void mouseFunc(int button, int state, int x, int y)
{
    
    printf("Mouse: %d, %d, x:%d, y:%d\n", button, state, x, y);
    if (state == 0) {
        // start Translation
        g_translate = true;
        posX = x + g_width/2.f;
        posY = y + g_height/2.f;
    }
    else {
        // Stop Translation
        g_translate = false;
        posX += incrX;
        posY += incrY;
        incrX = 0;
        incrY = 0;
    }
    
}

//-----------------------------------------------------------------------------
// Name: mouseMotionFunc(int x, int y)
// Desc: Callback to manage the mouse motion
//-----------------------------------------------------------------------------
void mouseMotionFunc(int x, int y)
{
    
    printf("Mouse Moving: %d, %d\n", x, y);
    if (g_translate) {
        incrX = x + g_width/2.0f - posX;
        incrY = y + g_height/2.0f - posY;
        printf("Position %d, %d\n", posX, posY);
        //scale down the translation values so you can move the mouse more
        printf("Translating %f, %f, %f\n", ((float)incrX) * .01, (-(float)incrY) * .01, 0.0);
        glTranslatef((float)incrX * .01, -(float)(incrY) * .01, 0.0);
        posX += incrX;
        posY += incrY;
    }
    
}

void drawScreen(SAMPLE *buffer)
{
    //GLfloat half = side / 2.0f;

    // Get the actual volume
    //GLfloat rms = computeRMS(buffer);

    // Get the value to scale the speaker based on the rms volume
    //GLfloat scale = (rms * 0.5 + 0.5); 

    float x = 0, y = 0;
    
    glPushMatrix();
    {
        // Translate
        glTranslatef(x + g_texture.center.x + g_tex_incr.x, y + g_texture.center.y + g_tex_incr.y, -250.0f);

        //Rotate
        rotateView();

        glPushMatrix();
        //draw Picture
        GLfloat halfX = g_width / 2.0f;
        GLfloat halfY = g_height / 2.0f;
        
        // Use the texture
        glBindTexture(GL_TEXTURE_2D, g_texture.texture_id);

        glBegin( GL_QUADS );
        //glColor3f( 1f, 1.0f, .7f );
        glTexCoord2d(0.0,0.0); 
        glVertex3d(-halfX, -halfY, 0);
        //glColor3f( 0.8f, 0.2f, .4f );
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

/*********************************
 **********</Kiran's Code>********
 *********************************/

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

    //now set frequency and channel values for the appropriate keys
    

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

