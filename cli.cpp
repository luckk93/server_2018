#include "def.h"
#include "triangulation.h"

void printterminal(void){
    char tempbuffer[20];
    char terminalbuffer[2000];
    bzero(terminalbuffer, sizeof(terminalbuffer));

  for(int camNbr = 0; camNbr < NOMBRECAM; camNbr++) {
      switch(camNbr){
        case 0: sprintf(tempbuffer,"\033[8;1H\033[2K"); break;
        case 1: sprintf(tempbuffer,"\033[9;1H\033[2K"); break;
        case 2: sprintf(tempbuffer,"\033[10;1H\033[2K"); break;
        case 3: sprintf(tempbuffer,"\033[11;1H\033[2K"); break;
        case 4: sprintf(tempbuffer,"\033[12;1H\033[2K"); break;
        case 5: sprintf(tempbuffer,"\033[13;1H\033[2K"); break;
        case 6: sprintf(tempbuffer,"\033[14;1H\033[2K"); break;
        case 7: sprintf(tempbuffer,"\033[15;1H\033[2K"); break;
        default: 
      }
  
      strcat(terminalbuffer,tempbuffer);
      sprintf(tempbuffer, "%d", reciveddatabackup[camNbr].buffer.camera_id);
      strcat(terminalbuffer,tempbuffer);
      for (int i1 = 0;(i1<NOMBREBALLS); i1++){
        if(reciveddatabackup[camNbr].buffer.boules[i1].boule_id!=0){
          sprintf(tempbuffer, "   ball %d:", reciveddatabackup[camNbr].buffer.boules[i1].boule_id);
          strcat(terminalbuffer,tempbuffer);
          for(int i2 = 0;(i2<NOMBREDATA-4); i2++){
              sprintf(tempbuffer, " %d", reciveddatabackup[camNbr].buffer.boules[i1].boule_data[i2]);
              strcat(terminalbuffer,tempbuffer);
          }
        }
      }
    }
    sprintf(tempbuffer, "\033[17;1H\033[Jtic %d", tic);				//clean terminal line
    strcat(terminalbuffer, tempbuffer);
    
    sprintf(tempbuffer, "\033[19;1H%d %d %d\n%d %d %d\n%d %d %d\n%d %d %d", msntorobot[0], msntorobot[1], msntorobot[2], msntorobot[4], msntorobot[5], msntorobot[6], msntorobot[8], msntorobot[9],
        msntorobot[10], msntorobot[12], msntorobot[13], msntorobot[14]);
    strcat(terminalbuffer, tempbuffer);

     sprintf(tempbuffer, "\033[24;1H%s", upd_err_msg);
    strcat(terminalbuffer, tempbuffer);
    
    printf("%s\n", terminalbuffer);
}
