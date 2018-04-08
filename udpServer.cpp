
#include "def.h"
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>
#include "tcpClient.h"

int patternData[3]={0,0,0};
bool newPattern=false;

char upd_err_msg[100];

cat_info cat_data;

stampdata reciveddata[NOMBRECAM];

#define ROBOT_IP "192.168.0.39"
#define ROBOT_PORT 5000
#define LOCAL_SERVER_PORT 8888

void sendPosition(int udpSocket, struct sockaddr_in si_robot, char robotId, short x, short y);
void sendCatData();
void getCatData();

void *udpserverThread(void *t){
    struct sockaddr_in si_me, si_other, si_robot;
    int udpSocket, slen = sizeof(si_other), recv_len;
    memset((char *) &si_robot, 0, sizeof(si_robot));
    struct data buffer;
    timespec messagetime, nextdisplay;
    int camMsgId = 0;
    int cat_data_size = sizeof(cat_info);
    
    // Define robot address and port
    memset((char *)&si_robot, 0, sizeof(si_robot));
    si_robot.sin_family = AF_INET;
    si_robot.sin_port = htons(ROBOT_PORT);
    if(inet_aton(ROBOT_IP, &si_robot.sin_addr) == 0) {
        sprintf(upd_err_msg, "inet_aton() failed\n");
    }
    
    // Create a UDP socket
    if((udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        sprintf(upd_err_msg, "Socket init failed.");
    }
    
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 100000;
    
    if(setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        sprintf(upd_err_msg, "Could not set timeout on socket.");
    }
    
    // zero out the structure
    memset((char *)&si_me, 0, sizeof(si_me));
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(LOCAL_SERVER_PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind socket to port
    if(bind(udpSocket, (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
        sprintf(upd_err_msg, "Binding socket failed.");
    }
    
    udpinit = true;
    
    //data backup file initialisation**********************************
    
    FILE* datalog = NULL;
    
    datalog = fopen("data1.txt", "w");
    
    fprintf(datalog,"\n\n\n\n\n\n*** New Data serie ***\n");
    
    //*****************************************************************
    
    printf("\033[4;1H**Ready to receive**\n");
    
    clock_gettime(CLOCK_REALTIME,&nextdisplay);
    
    //keep listening for data
    while(!quitServer) {
        fflush(stdout); 
        memset(&buffer, 0, sizeof(buffer)); //clean buffer
        
        //try to receive some data, this is a blocking call
        if((recv_len = recvfrom(udpSocket, &buffer, BUFFER_SIZE, 0, (struct sockaddr *)&si_other, (socklen_t*)&slen)) == -1) {
            sprintf(upd_err_msg, "Data reception failed %d.", errno);
        }
        else{

            if(buffer.cat_data.red==0){
                sendCatData();
                sendto(udpSocket, cat_data, cat_data_size, 0,(struct sockaddr *)&si_other,(socklen_t*)&slen);
            }
            else{
                getCatData();
            }

            if(!((buffer.pattern[0]==0)||(buffer.pattern[0]==0)||(buffer.pattern[0]==0))){
            	if((patternData[0]!=buffer.pattern[0])||(patternData[1]!=buffer.pattern[1])||(patternData[2]!=buffer.pattern[2])){
            		patternData = buffer.pattern;
            		newPattern=true;
            	}
            }
            //patternData[0] = buffer.pattern[0];
            //patternData[1] = buffer.pattern[1];
            //patternData[2] = buffer.pattern[2];
                    
            camMsgId = buffer.camera_id-1;
            if(camMsgId < 0 || camMsgId > NOMBRECAM) {
                sprintf(upd_err_msg,"wrong camMsgId.");
            }
    
            clock_gettime(CLOCK_REALTIME, &messagetime);
            
            pthread_mutex_lock(&mutex_udpin);
            memcpy(&(reciveddata[camMsgId].buffer), &buffer, sizeof(buffer));
            reciveddata[camMsgId].modified = true;
            reciveddata[camMsgId].expire = messagetime;
            pthread_mutex_unlock(&mutex_udpin);            
            
            
            //begin write backupo data on file*****************************
            fprintf(datalog, "%d ", buffer.camera_id);
            for(int i1 = 0; (i1 < NOMBREBALLS); i1++) {
                if(buffer.boules[i1].boule_id != 0) {
                    for(int i2 = 0; (i2 < NOMBREDATA); i2++) {
                        fprintf(datalog, "%d ", buffer.boules[i1].boule_data[i2]);
                    }
                }
            }
            fputc('\n', datalog);
            //end write backup of data on file*****************************

        }

        //Input terminal printing-----------------------------------
        clock_gettime(CLOCK_REALTIME, &messagetime);
        
        if(((messagetime.tv_sec > nextdisplay.tv_sec) || ((messagetime.tv_nsec > nextdisplay.tv_nsec) && (messagetime.tv_sec >= nextdisplay.tv_sec)))) {
            nextdisplay.tv_nsec = messagetime.tv_nsec + DISPLAYRATE;
            nextdisplay.tv_sec = messagetime.tv_sec;
            if(nextdisplay.tv_nsec > NTOSECOND) {
                nextdisplay.tv_nsec = nextdisplay.tv_nsec - NTOSECOND;
                nextdisplay.tv_sec++;
            }
            //displayData(buffer);
            printterminal();
        }

        sendPosition(udpSocket, si_robot, msntorobot[0], msntorobot[1], msntorobot[2]);
        sendPosition(udpSocket, si_robot, msntorobot[4], msntorobot[5], msntorobot[6]);
    }
    fclose(datalog);
    // fermeture du socket
    close(udpSocket);
    pthread_exit(NULL);
}

void sendPosition(int udpSocket, struct sockaddr_in si_robot, char robotId, short x, short y)
{
    char data[6];
    data[0] = 4;
    data[1] = robotId;
    *((short*)(data + 2)) = x;
    *((short*)(data + 4)) = y;
    
    if(sendto(udpSocket, data, sizeof(data), 0, (struct sockaddr *)&si_robot, sizeof(si_robot)) == -1) {
        sprintf(upd_err_msg, "Sending robot position failed %d.", errno);
    }
}

void sendCatData(){
    
}

void getCatData(){

}