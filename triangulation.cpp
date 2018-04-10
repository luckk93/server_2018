#include "triangulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <cmath>
#include <chrono>
#include "def.h"

#define EXPTIME 10 

#define HALFWIDTH 1296

robotvector_t ballvct[NOMBREBALLS][NOMBRECAM + 1];
robotpos_t ballpst[NOMBREBALLS][NOMBRECAM][NOMBRECAM];
int vectnumbr[NOMBREBALLS] = {0};
int posnumbr[NOMBREBALLS] = {0};
campos_t camPos[NOMBRECAM] = {
    { 72, -49, 0.3859, 0.9225 },    	// 1	22.7°
    { 95, -72, 0.9135, 0.4067 },    	// 2	66°
    { 1905, -72, -0.9156, 0.4019 }, 	// 3	-66.3°
    { 1928, -49, -0.4146, 0.9099 },  	// 4	-24.5°
    { 1048, 3057, 0.939, -0.342 },		// 5	110°
    { 1018, 3049, 0.342, -0.939 },  	// 6	160°
    { 982, 3049, -0.3502, -0.9366 },  	// 7	-159.5°
    { 952, 3057, -0.939, -0.342 },		// 8	-110°
};
stampdata reciveddatabackup[NOMBRECAM];

stampvector lastdata[NOMBREBALLS][NOMBRECAM];

/***********************************************************************************************/
/* get vector origin and inclination   
 * save data and erase them after a certain time 
 * update at every modification the number of vector for every ball
 * calculate all interception
 * put interception in the right output
 *************************************************************************************************/

void *calcPosVect(void *t) {
//vector array most have expirement time,  camera number, origin, inclination and a utilisation number
//2 dimension first for color ball, second for the differente vector
//the utilisation number is a cyclic number generated at position calculation, a nombre from 1 to 255 (0 mean not used yet)
//the utilisation number is gived to vector and to postion array
//if all the vector have the same number of the position no new calculation is made   
    //static robotvector_t ballvct[NOMBREBALLS][NOMBRECAM+1];
    float rotcos = 0;
    float rotsin = 0;
    
    static float camMaxSideAngle = ANGLECAM/2;
    static float tanValue = tan(camMaxSideAngle * 3.1415 / 180);
    static float camInclX = 1296 / tanValue;
    float camInclY = 0;
    
    //array with the ball position, utiliation number and expiration time 
    //1 dimension color ball
    
    //array with nomber of vector for each ball
    
    int ballident = 0;
    timespec start;
    timespec stop;
    timespec diff;
    timespec now;
    //std::chrono::system_clock::time_point now_11;
    //std::chrono::system_clock::time_point nowdiff_11;
    timespec nowdiff;
    
    int xTemp = 0, yTemp = 0;
    
    bool vectmodify[NOMBREBALLS][NOMBRECAM];
    
    bzero(ballvct, sizeof(ballvct));
    bzero(ballpst, sizeof(ballpst));
    bzero(reciveddata, sizeof(reciveddata));
    bzero(lastdata, sizeof(lastdata));
    bzero(vectmodify, sizeof(vectmodify));
    bzero(msntorobot, sizeof(msntorobot));
    
    while(!udpinit)
        sleep(1);
    
    clock_gettime(CLOCK_REALTIME, &start);
    
    while(!quitServer) {
        
        pthread_mutex_lock(&mutex_udpin);
        for(int i1 = 0; i1 < NOMBRECAM; i1++) {
            if(reciveddata[i1].modified){
                memcpy(&(reciveddatabackup[i1]), &(reciveddata[i1]), sizeof(reciveddata[i1]));
                memset(&(reciveddata[i1]), 0, sizeof(reciveddata[i1]));
                reciveddata[i1].modified = false;
            }
        }
        pthread_mutex_unlock(&mutex_udpin);
        
        //now_11 = std::chrono::system_clock::now();
        //nowdiff_11 = std::chrono::seconds;
        clock_gettime(CLOCK_REALTIME, &now);
        nowdiff.tv_sec = now.tv_sec;
        nowdiff.tv_nsec = now.tv_nsec - NSECEXP;
        if(nowdiff.tv_nsec < 0) {
            nowdiff.tv_nsec = NTOSECOND + nowdiff.tv_nsec;
            nowdiff.tv_sec--;
        }
        
        for(int ball = 0; ball < NOMBREBALLS; ball++) {
            for(int camera = 0; camera < NOMBRECAM; camera++) {
                if(((nowdiff.tv_sec > lastdata[ball][camera].expire.tv_sec) ||
                ((nowdiff.tv_nsec > lastdata[ball][camera].expire.tv_nsec) && 
                (nowdiff.tv_sec >= lastdata[ball][camera].expire.tv_sec))))
                    lastdata[ball][camera].active = false;
                else
                    lastdata[ball][camera].active = true;
            }
        }
        
        //begin message reading loop
        /********************************************************************************************/
        for(int i1 = 0; i1 < NOMBRECAM; i1++) {
            if(reciveddatabackup[i1].modified) {
                if(((nowdiff.tv_sec < reciveddatabackup[i1].expire.tv_sec) || ((nowdiff.tv_nsec < reciveddatabackup[i1].expire.tv_nsec) && (nowdiff.tv_sec <= reciveddatabackup[i1].expire.tv_sec)))) {
                    for(int i2 = 0; i2 < NOMBREBALLS; i2++) {
                        if((reciveddatabackup[i1].buffer.boules[i2].boule_id>0)&&(reciveddatabackup[i1].buffer.boules[i2].boule_id<=NOMBREBALLS)) {
                            ballident = reciveddatabackup[i1].buffer.boules[i2].boule_id - 1;
                        }
                        else{
                            ballident = 99;
                        }
                        
                        if((ballident >= 0) && (ballident < NOMBREBALLS)) {
                            rotcos = camPos[i1].camcos;
                            rotsin = camPos[i1].camsin;
                            
                            camInclY = (float)(HALFWIDTH - (((int)reciveddatabackup[i1].buffer.boules[i2].boule_data[3] + (int)reciveddatabackup[i1].buffer.boules[i2].boule_data[1]) >> 1));
                            lastdata[ballident][i1].x = camPos[i1].x;
                            lastdata[ballident][i1].y = camPos[i1].y;
                            lastdata[ballident][i1].expire = reciveddatabackup[i1].expire;
                            lastdata[ballident][i1].inclinx = rotcos * camInclX - rotsin * camInclY;
                            lastdata[ballident][i1].incliny = rotsin * camInclX + rotcos * camInclY;
                            lastdata[ballident][i1].active = true;
                            lastdata[ballident][i1].modify = true;
                        }
                    }
                }
            }
        }
        // end message reading looping
        /*************************************************************************************************************/

        bool modflag = false;
        int P21x = 0, P21y = 0;
        float crossV2 = 0, crossV1 = 0, factor = 0;
        
        for(int i1 = 0; i1 < NOMBREBALLS; i1++) {
            for(int i2 = 0; i2 < NOMBRECAM; i2++) {
                if(lastdata[i1][i2].modify)
                    modflag = true;
                if(lastdata[i1][i2].active) {
                    for(int i3 = i2 + 1; i3 < NOMBRECAM; i3++) {
                        if(((modflag) && (lastdata[i1][i3].active)) || (lastdata[i1][i3].modify)) {
                            P21x = lastdata[i1][i2].x - lastdata[i1][i3].x;
                            P21y = lastdata[i1][i2].y - lastdata[i1][i3].y;
                            crossV1 = (lastdata[i1][i3].inclinx * lastdata[i1][i2].incliny) - (lastdata[i1][i3].incliny * lastdata[i1][i2].inclinx);
                            crossV2 = (P21x * lastdata[i1][i2].incliny) - (P21y * lastdata[i1][i2].inclinx);
                            factor = crossV2 / crossV1;
                            ballpst[i1][i2][i3].x = round(lastdata[i1][i3].x + (factor * lastdata[i1][i3].inclinx));
                            ballpst[i1][i2][i3].y = round(lastdata[i1][i3].y + (factor * lastdata[i1][i3].incliny));
                        }
                    }
                }
            }
        }
        /************************************************************************************************************************************************************************/
        int valuenumber = 0;
        // if vector change  position change
        for(int i1 = 0; i1 < NOMBREBALLS; i1++) {
            xTemp = 0;
            yTemp = 0;
            valuenumber = 0;
            for(int i2 = 0; i2 < NOMBRECAM; i2++) {
                lastdata[i1][i2].modify = false;
                if(lastdata[i1][i2].active) {
                    for(int i3 = i2 + 1; i3 < NOMBRECAM; i3++) {
                        if(lastdata[i1][i3].active) {
                            xTemp = xTemp + ballpst[i1][i2][i3].x;
                            yTemp = yTemp + ballpst[i1][i2][i3].y;
                            valuenumber++;
                        }
                    }
                }
            }
            if(valuenumber >= 1) {
                posRobot[i1].x = round(xTemp / valuenumber);
                posRobot[i1].y = round(yTemp / valuenumber);
            }
            else{
                posRobot[i1].x = 0;
                posRobot[i1].y = 0;
            }
        }
        
        for(int i1 =0; i1<NOMBREBALLS; i1++){
        	if(posRobot[i1].x<0||posRobot[i1].x>XWIDTH||posRobot[i1].y<0||posRobot[i1].y>YWIDTH){
    			ofstream ofs;
    			ofs.open ("outball.info", std::ofstream::out);
    			ofs << "result " << posRobot[i1].x << " " << posRobot[i1].y << endl;
    			for(int i2 = 0; i2 < NOMBRECAM; i2++) {
    				if(lastdata[i1][i2].active){
    					ofs << i1 << "\t" << i2 << "\t";
    					ofs << reciveddatabackup[i2].buffer.boules[i1].boule_data[1] << "\t";
    					ofs << reciveddatabackup[i2].buffer.boules[i1].boule_data[3] << endl;
    				}
    			}
    			ofs.close();
        	}
        }

        //  grand robot ennemi
        msntorobot[0] = 1;
        msntorobot[1] = posRobot[0].x;
        msntorobot[2] = posRobot[0].y;
        //msntorobot[3]=0;
        
        // petit robot ennemi
        msntorobot[4] = 2;
        msntorobot[5] = posRobot[1].x;
        msntorobot[6] = posRobot[1].y;
        //msntorobot[7]=0;
        
        // grand robot allié
        msntorobot[8] = 3;
        msntorobot[9] = posRobot[2].x;
        msntorobot[10] = posRobot[2].y;
        
        // petit robot allié
        msntorobot[12] = 4;
        msntorobot[13] = posRobot[3].x;
        msntorobot[14] = posRobot[3].y;

        
        /**************************************************************************************/
        clock_gettime(CLOCK_REALTIME, &stop);
        diff.tv_sec = stop.tv_sec - start.tv_sec;
        diff.tv_nsec = stop.tv_nsec - start.tv_nsec;
        if(diff.tv_nsec < 0) {
            diff.tv_nsec = NTOSECOND - diff.tv_nsec;
            diff.tv_sec--;
        }
        tic = diff.tv_nsec;
    }
    pthread_exit(NULL);
}
