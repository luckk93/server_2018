#ifndef def_H
#define def_H

#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <cmath>
#include <time.h> 
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdbool.h>
#include <fstream>

using namespace std;

#define NOMBRECAM     8
#define NOMBREDATA    8
#define NOMBREBALLS   4

#define PIXELWIDTH 2592 // largeur en pixel de l'image d'une camera
#define ANGLECAM 53.5 // angle de vue total en largeur  //maybe 54.6
#define PI 3.1415926535

#define ONEDATASIZE 8 // taille des donnée d'une camera pour une seule balle

#define DISPLAYEDBALL 5 // est utile pour le fichier triangulation.c. Sert à afficher les angles pour une balle

#define ROBOTINFONUMBER 16
#define STRINGTOROBOTSIZE 4*(4+6+6+8)

#define BUFFERSIZE 56

#define NTOSECOND 1000000000
#define NSECEXP   500000000
#define DISPLAYRATE 500000000

#define XWIDTH 2000
#define YWIDTH 3000

#define BUFFER_SIZE sizeof(struct data) // taille du buffer 8 unsigned int

// position x et y des balles

typedef struct
{
    float x;
    float y;
} pos_t;

void printterminal();

void computeAbsAbgle(int beaconNum, int ballNum, int camNum, float** angleBalls, float** angleAbs);
void calcPos(int *in, pos_t *out);

void *calcPosVect(void *t);

void interfaceMainLoop();

void interfaceInit();

void closeInterface();

typedef struct
{
    int x;
    int y;
    int exptime;
    int camnbr;
    int identNbr;
    float inclinx;
    float incliny;
} robotvector_t;

typedef struct
{
    int x;
    int y;
} robotpos_t;

typedef struct
{
    int x;
    int y;
    float angle;
    float max_angle;
    int cat_x;
    int cat_y;
} camdata_t;

typedef struct
{
    float camcos;
    float camsin;
    float camInclX;
} camincl_t;

struct boule {
    unsigned int boule_id;
    unsigned int boule_data[8];
};

struct cat_info{
    int x;
    int y;
    int red;
    int blue;
  } ;
  
  struct data {
  unsigned int camera_id;
  boule boules[6];
  int pattern[3];
  cat_info cat_data;
  };

typedef struct {
    data buffer;
    timespec expire;
    bool modified;
} stampdata;

extern robotvector_t ballvct[NOMBREBALLS][NOMBRECAM + 1];
extern robotpos_t ballpst[NOMBREBALLS][NOMBRECAM][NOMBRECAM];
extern int vectnumbr[NOMBREBALLS];
extern int posnumbr[NOMBREBALLS];
extern camdata_t camData[NOMBRECAM];
extern camincl_t camIncl[NOMBRECAM];
extern pos_t posRobot[NOMBREBALLS];
extern bool quitServer;
extern bool new_cat_pos;
extern bool new_cat_pos_array[NOMBRECAM];
extern bool cam_init_flag;
extern long int packet_counter;


extern int msntorobot[ROBOTINFONUMBER];
extern char stringtorobot[STRINGTOROBOTSIZE];
extern int tic;
//extern int buffer[BUFFERSIZE];
extern bool udpinit;

extern pthread_mutex_t mutex_udpin;

extern char upd_err_msg[100];
extern cat_info cat_data_save[NOMBRECAM];

extern int patternData[3];
extern bool newPattern;

#endif
