/*
 * udpServer.h
 *
 *  Created on: Mar 24, 2018
 *      Author: rtfm
 */

#ifndef UDPSERVER_H_
#define UDPSERVER_H_

#include "def.h"

extern stampdata reciveddata[NOMBRECAM];

void* udpserverThread(void* t);

#endif /* UDPSERVER_H_ */
