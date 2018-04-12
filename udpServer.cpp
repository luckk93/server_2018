
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
cat_info cat_data_save[NOMBRECAM];

stampdata reciveddata[NOMBRECAM];

#define ROBOT_IP "192.168.0.255"
#define ROBOT_PORT 5000
#define LOCAL_SERVER_PORT 8888

void sendPosition(int udpSocket, struct sockaddr_in si_robot, char robotId, short x, short y);
void sendCatData();

void *udpserverThread(void *t){
    struct data buffer;
    struct sockaddr_in si_me, si_other, si_robot;
    int udpSocket, recv_len ; 
    socklen_t slen = sizeof(si_other);
    memset((char *) &si_robot, 0, sizeof(si_robot));
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
    int broadcast_enable=1; 
    
    if(setsockopt(udpSocket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        sprintf(upd_err_msg, "Could not set timeout on socket.");
    }
    if(setsockopt(udpSocket, SOL_SOCKET, SO_BROADCAST, &broadcast_enable, sizeof(broadcast_enable)) <0) {
        sprintf(upd_err_msg, "Could not set broadcast on socket.");
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
    
    memset(cat_data_save, 0, sizeof(cat_data_save));

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
        if((recv_len = recvfrom(udpSocket, &buffer, BUFFER_SIZE, 0, (struct sockaddr *)&si_other, &slen)) == -1) {
            sprintf(upd_err_msg, "Data reception failed %d.", errno);
        }
        else{

            if(!((buffer.pattern[0]==0)||(buffer.pattern[0]==0)||(buffer.pattern[0]==0))){
            	if((patternData[0]!=buffer.pattern[0])||(patternData[1]!=buffer.pattern[1])||(patternData[2]!=buffer.pattern[2])){
            		patternData[0] = buffer.pattern[0];
                    patternData[1] = buffer.pattern[1];
                    patternData[2] = buffer.pattern[2];
            		newPattern=true;
            	}
            }
                    
            camMsgId = buffer.camera_id-1;
            if(camMsgId < 0 || camMsgId > NOMBRECAM) {
                sprintf(upd_err_msg,"wrong camMsgId.");
            }
            else{

                if(buffer.cat_data.red==0){
                    sendCatData();
                }
                else{
                    memcpy( &(cat_data_save[camMsgId]), &(buffer.cat_data), sizeof(cat_data));
                    new_cat_pos=true;
                    new_cat_pos_array[camMsgId]=true;
                }
                sendto(udpSocket, &cat_data, cat_data_size, 0,(struct sockaddr *)&si_other,slen);
                clock_gettime(CLOCK_REALTIME, &messagetime);
                
                pthread_mutex_lock(&mutex_udpin);
                memcpy(&(reciveddata[camMsgId].buffer), &buffer, sizeof(buffer));
                reciveddata[camMsgId].modified = true;
                reciveddata[camMsgId].expire = messagetime;
                pthread_mutex_unlock(&mutex_udpin);  
            }          
            
            
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
            //sendPosition(udpSocket, si_robot, msntorobot[0], msntorobot[1], msntorobot[2]);		//1° ally
            //sendPosition(udpSocket, si_robot, msntorobot[4], msntorobot[5], msntorobot[6]);		//2° ally
            sendPosition(udpSocket, si_robot, msntorobot[8], msntorobot[9], msntorobot[10]);		//1° enemy
            sendPosition(udpSocket, si_robot, msntorobot[12], msntorobot[13], msntorobot[14]);		//2° enemy
        }
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
    int red_average=0;
    int blue_average=0;
    int count=0;
    for(int i=0; i<NOMBRECAM; i++){
        if(cat_data_save[i].red!=0){
            count++;
            red_average+=cat_data_save[i].red;
            blue_average+=cat_data_save[i].blue;
        }
    }
    if(count>=1){
        cat_data.red=red_average/count;
        cat_data.blue=blue_average/count;
    }
    else{
        cat_data.red=1300;
        cat_data.blue=2000;
    }
}