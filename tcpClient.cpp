#include <stdbool.h>
#include "def.h"
#include "tcpClient.h"

#define ROBOT_IP "192.168.0.39"
#define ROBOT_PORT 5000

extern int patternData[3];

bool isTcpClientConnected = false;

void* tcpClientThread(void* t)
{
    int sockfd;
    struct hostent *he;
    struct sockaddr_in si_robot; /* connector's address information */
    
    if((he = gethostbyname(ROBOT_IP)) == NULL) { /* get the host info */
        herror("gethostbyname");
    }
    
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
    }
    
    si_robot.sin_family = AF_INET; /* host byte order */
    si_robot.sin_port = htons(ROBOT_PORT); /* short, network byte order */
    si_robot.sin_addr = *((struct in_addr *)he->h_addr);
    bzero(&(si_robot.sin_zero), 8); /* zero the rest of the struct */

    int isError = -1;
    do {
        isError = connect(sockfd, (struct sockaddr *)&si_robot, sizeof(struct sockaddr)); 
        /*if(isError == -1) {
            perror("Impossible to join server.");
        }*/
        sleep(3);
    } while(!quitServer && isError == -1);
    while(!quitServer)
    {
        if(patternData[0] != 0 && patternData[1] != 0 && patternData[2] != 0)
        sendPattern(sockfd, patternData);
        sleep(3);
    }
    
    close(sockfd);
    
    return 0;
}

//Bleu=1; Orange=2; Vert=3; Jaune=4; Noir=5
//NoData = 0
void sendPattern(int sockfd, int pattern[3]) {
    //Envoyer message identification (voir fichier excel)
    char data2[3];
    
    data2[0] = 2;
    data2[1] = 0;
    data2[2] = 5;
    int size = sizeof(data2);
    
    if(send(sockfd, data2, size, 0) == -1) {
        perror("send");
    }
    
    /*
     * 1er byte : 29
     * 2e byte : 7
     * 3e byte : 3
     * 4e-10e byte : "P" "A" "T" "T" "E" "R" "N"
     * 11e byte : pattern[0]
     * 12e byte : pattern[1]
     * 13e byte : pattern[2]
     */

    char data[14];
    int tcpsize = sizeof(data);
    data[0] = 13;
    data[1] = 29;
    data[2] = 7;
    data[3] = 3;
    data[4] = 'p';
    data[5] = 'a';
    data[6] = 't';
    data[7] = 't';
    data[8] = 'e';
    data[9] = 'r';
    data[10] = 'n';
    data[11] = pattern[0];
    data[12] = pattern[1];
    data[13] = pattern[2];
    
    if(send(sockfd, data, tcpsize, 0) == -1) {
        perror("send");
    }
}
