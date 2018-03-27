/*
 * triangulation.h
 *
 *  Created on: Mar 24, 2018
 *      Author: rtfm
 */

#ifndef TRIANGULATION_H_
#define TRIANGULATION_H_

#include "def.h"

typedef struct {
    int x;
    int y;
    timespec expire;
    bool active;
    bool modify;
    float inclinx;
    float incliny;
} stampvector;

extern stampdata reciveddatabackup[NOMBRECAM];
extern stampdata reciveddata[NOMBRECAM];
extern stampvector lastdata[NOMBREBALLS][NOMBRECAM];

#endif /* TRIANGULATION_H_ */
