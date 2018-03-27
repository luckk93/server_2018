#include "def.h"
#include <stdbool.h>
#include "tcpClient.h"
#include "udpServer.h"
#include "gui.h"

#ifdef DISPLAY
#define NUM_THREAD 4
#else
#define NUM_THREAD 3
#endif

pos_t posRobot[NOMBREBALLS]; // in mm

bool quitServer = false;

int tic = 0;

int msntorobot[ROBOTINFONUMBER];
char stringtorobot[STRINGTOROBOTSIZE];

bool udpinit = false;

int buffer[BUFFERSIZE];

pthread_mutex_t mutex_udpin;

/*End of Variable declaration*********************************************************************************************/

void intHandler(int something) {
    quitServer = true;
}

/*Begin main code***************************************************************/
int main(int argc, char *argv[])
{
    signal(SIGINT, intHandler);
    
    //thread variable
    long t1 = 1, t3 = 3, t4 = 4;
#ifdef DISPLAY
    long t2 = 2; 
#endif
    
    pthread_t threads[NUM_THREAD];
    pthread_attr_t threadattr;
    
    //mutex initialisation
    pthread_mutex_init(&mutex_udpin, NULL);
    
    printf("\033[2J\033[2;1H\n### Starting the server soft ###\n");
    
    /*Variable initialisation***********************************/
    for(int i = 0; i < NOMBREBALLS; i++)
    {
        posRobot[i].x = 0;
        posRobot[i].y = 0;
    }
    
    for(int i = 0; i < ROBOTINFONUMBER; i++)
        msntorobot[i] = 0;
    
    //thread init adn creation*******************************************
    pthread_attr_init(&threadattr);
    pthread_attr_setdetachstate(&threadattr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &threadattr, udpserverThread, (void *)t1);
    pthread_create(&threads[2], &threadattr, calcPosVect, (void *)t3);
    pthread_create(&threads[3], &threadattr, tcpClientThread, (void *)t4);
#ifdef DISPLAY
    pthread_create(&threads[1], &threadattr, interfaceThread, (void *)t2);
#endif
    
    for(int i = 0; i < NUM_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_attr_destroy(&threadattr);
    pthread_mutex_destroy(&mutex_udpin);
    pthread_exit(NULL);
    
    return 0;
}
