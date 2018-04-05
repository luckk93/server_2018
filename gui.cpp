#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <math.h>
#include "def.h"
#include <stdbool.h>
#include "triangulation.h"

#define SCREEN_WIDTH 1700
#define SCREEN_HEIGHT 1200

bool quitInterface = false;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
    
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

//The image we will load and show on the screen

SDL_Renderer* renderer = NULL;
SDL_Texture *newTexture=NULL;
SDL_Texture *gTexture=NULL;

int ballcolor[NOMBREBALLS][3] = { { 255, 255, 0 }, { 255, 51, 153 }, { 51, 204, 51 }, { 255, 51, 0 } };

SDL_Event e;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "W§indow could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			renderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( renderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}


SDL_Texture* loadTexture(std::string path )
{
	//The final texture
	SDL_Texture* newfTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
	}
	else
	{
		//Create texture from surface pixels
        newfTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
		if( newfTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newfTexture;
} 

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Load PNG texture
	newTexture = loadTexture( "Screenshot.png" );
	if( newTexture == NULL )
	{
		printf( "Failed to load texture image!\n" );
		success = false;
	}

	return success;
}

void closeInterface()
{
    //Deallocate surface
    //SDL_FreeSurface( gHelloWorld );
    //gHelloWorld = NULL;

    //Destroy window
    SDL_DestroyWindow( gWindow );
    gWindow = NULL;

    //Quit SDL subsystems
    SDL_Quit();
}

void myDrawLine(float x1,float y1,float x2, float y2){
  float deltax=float(x2-x1);
  float deltay=float(y2-y1);
  int i=0;
  float prop;
  if(abs(deltax)>=abs(deltay)){
    prop=deltay/deltax;
    if(deltax>=0)for(i=x1;i<=x2;i++)SDL_RenderDrawPoint(renderer,i,round(y1+prop*(i-x1)));
    else for(i=x1;i>=x2;i--)SDL_RenderDrawPoint(renderer,i,round(y1+prop*(i-x1)));
  }
  else{
    prop=deltax/deltay;
    if(deltay>=0)for(i=y1;i<=y2;i++)SDL_RenderDrawPoint(renderer,round(x1+prop*(i-y1)),i);
    else for(i=y1;i>=y2;i--)SDL_RenderDrawPoint(renderer,round(x1+prop*(i-y1)),i);
  }
}

int drawCamera(float x, float y ,float coscam,float sincam){
  
  float x1,y1,x2,y2;
  int xf1,yf1,xf2,yf2,errorcam=0;
  SDL_SetRenderDrawColor(renderer,0,0,255,255);
  
  /* big camera valeu
  x1=-10;y1=-10;x2=-10;y2=10;
  x1=-10;y1=-10;x2=-5;y2=-10;
  x1=-10;y1=10;x2=-5;y2=10;
  x1=-5;y1=-10;x2=-5;y2=-3;
  x1=-5;y1=10;x2=-5;y2=3;
  x1=-5;y1=-3;x2=5;y2=-7;
  x1=-5;y1=3;x2=5;y2=7;
  x1=5;y1=-7;x2=5;y2=7;
  */
  
  x1=-5;y1=-5;x2=-5;y2=5;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i |  %i  %i  %i  %i\n",errorline,xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);
  
  x1=-5;y1=-5;x2=-2;y2=-5;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i |  %i  %i  %i  %i\n",errorline,xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);
  
  x1=-5;y1=5;x2=-2;y2=5;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i |  %i  %i  %i  %i\n",errorline,xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);

  
  x1=-2;y1=-5;x2=-2;y2=-2;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i  %i  %i  %i\n",xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);
  
  x1=-2;y1=5;x2=-2;y2=2;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i  %i  %i  %i\n",xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);
  
  x1=-2;y1=-2;x2=2;y2=-3;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i  %i  %i  %i\n",xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);
  
  x1=-2;y1=2;x2=2;y2=3;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i  %i  %i  %i\n",xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);
  
  x1=2;y1=-3;x2=2;y2=3;
  xf1=round(x+(x1*coscam-y1*sincam));yf1=round(y-(x1*sincam+y1*coscam));
  xf2=round(x+(x2*coscam-y2*sincam));yf2=round(y-(x2*sincam+y2*coscam));
  //printf("float %i  %i  %i  %i\n",xf1,yf1,xf2,yf2);
  SDL_RenderDrawLine(renderer,xf1,yf1,xf2,yf2);
  SDL_RenderPresent(renderer);
  return errorcam;
  
}

void myDrawPoint(int x,int y){
     SDL_RenderDrawLine(renderer,x-2,y-1,x-2,y+1);
     SDL_RenderDrawLine(renderer,x-1,y-2,x-1,y+2);
     SDL_RenderDrawLine(renderer,x,y-2,x,y+2);
     SDL_RenderDrawLine(renderer,x+1,y-2,x+1,y+2);
     SDL_RenderDrawLine(renderer,x+2,y-1,x+2,y+1);
  }
  
void myDrawBall(int x, int y){
  SDL_RenderDrawLine(renderer,x-2,y+5,x+2,y+5);
  SDL_RenderDrawLine(renderer,x-3,y+4,x+3,y+4);
  SDL_RenderDrawLine(renderer,x-2,y-5,x+2,y-5);
  SDL_RenderDrawLine(renderer,x-3,y-4,x+3,y-4);
  SDL_RenderDrawLine(renderer,x+5,y-2,x+5,y+2);
  SDL_RenderDrawLine(renderer,x+4,y-3,x+4,y+3);
  SDL_RenderDrawLine(renderer,x-5,y-2,x-5,y+2);
  SDL_RenderDrawLine(renderer,x-4,y-3,x-4,y+3);
  SDL_RenderDrawPoint(renderer,x-3,y-3);
  SDL_RenderDrawPoint(renderer,x-3,y+3);
  SDL_RenderDrawPoint(renderer,x+3,y-3);
  SDL_RenderDrawPoint(renderer,x+3,y+3);
  SDL_RenderDrawPoint(renderer,x,y);

}

void interfaceInit(){
    
//The image we will load and show on the screen
//SDL_Surface* gHelloWorld = NULL;

//Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL_Error: %s\n", SDL_GetError() );
    }
    else
    {
        //Create window
        gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL_Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
        }
    }

    renderer=SDL_CreateRenderer(gWindow,-1,0);
    SDL_SetRenderDrawColor(renderer,255,255,255,50);
    SDL_RenderClear(renderer);
    SDL_Rect fieldLine={100,100,1500,1000};
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    SDL_RenderDrawRect(renderer,&fieldLine);

    fieldLine.w=200;fieldLine.h=325;
    fieldLine.x=100;fieldLine.y=100;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.x=1400;
    SDL_RenderDrawRect(renderer,&fieldLine);

    fieldLine.w=280;fieldLine.h=90;
    fieldLine.x=299;fieldLine.y=100;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.x=1121;
    SDL_RenderDrawRect(renderer,&fieldLine);

    fieldLine.w=50;fieldLine.h=27;
    fieldLine.x=100;fieldLine.y=517;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.x=1550;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.w=27;fieldLine.h=50;
    fieldLine.x=392;fieldLine.y=1050;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.x=1281;
    SDL_RenderDrawRect(renderer,&fieldLine);

    fieldLine.w=50;fieldLine.h=50;
    fieldLine.x=28;fieldLine.y=100;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.y=575;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.y=1050;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.x=1622;fieldLine.y=100;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.y=575;
    SDL_RenderDrawRect(renderer,&fieldLine);
    fieldLine.y=1050;
    SDL_RenderDrawRect(renderer,&fieldLine);
    
    int c1;
    for(c1=0;c1<8;c1++){
      drawCamera(100+(camPos[c1].y>>1), 100+(camPos[c1].x>>1) ,camPos[c1].camsin,-camPos[c1].camcos);
    }

    
    SDL_RenderPresent(renderer);
    SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 1024, 768);
   
    // Create an empty RGB surface that will be used to create the screenshot bmp file 
              
    SDL_Surface* pScreenShot = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);  

    if(pScreenShot) 
    { 
      // Read the pixels from the current render target and save them onto the surface 
      SDL_RenderReadPixels(renderer, NULL, SDL_GetWindowPixelFormat(gWindow), pScreenShot->pixels, pScreenShot->pitch); 

      // Create the bmp screenshot file 
      
      SDL_SaveBMP(pScreenShot, "Screenshot.bmp");     
      SDL_Surface * image = SDL_LoadBMP("Screenshot.bmp");
      IMG_SavePNG(image, "Screenshot.png");
      /*
      gHelloWorld = SDL_LoadBMP( "Screenshot.bmp" );
      if( gHelloWorld == NULL )
      {
          printf( "Unable to load image %s! SDL Error: %s\n", "02_getting_an_image_on_the_screen/hello_world.bmp", SDL_GetError() );
          success = false;
      }
      */
      
      //newTexture = SDL_CreateTextureFromSurface( renderer, pScreenShot );
       
      if( !loadMedia() )
      {
        printf( "Failed to load media!\n" );
      }
       
      SDL_SetRenderDrawColor(renderer,255,255,255,255);
      SDL_RenderClear(renderer);
      SDL_RenderCopy( renderer, newTexture, NULL, NULL );
      SDL_RenderPresent(renderer);

      // Destroy the screenshot surface 
      SDL_FreeSurface(pScreenShot); 
      SDL_SetRenderDrawColor(renderer,0,0,0,255);
    } 
    //close();
    if(!success)printf( "error 1\n");


}

void interfaceMainLoop(){
        //Handle events on queue
    while( SDL_PollEvent( &e ) != 0 )
    {
      //User requests quit
      if( e.type == SDL_QUIT )
      {
        quitInterface = true;
        quitServer=true;
      }
    }
    
    SDL_RenderClear(renderer);
    SDL_RenderCopy( renderer, newTexture, NULL, NULL );
    
    float tempvect;
    int vectx,vecty;
    int i1,i2;
    for(i1=0;i1<NOMBREBALLS;i1++){
      SDL_SetRenderDrawColor(renderer,ballcolor[i1][0],ballcolor[i1][1],ballcolor[i1][2],0);
      for(i2=0;i2<NOMBRECAM;i2++){
        if(lastdata[i1][i2].active){
          if((lastdata[i1][i2].x<100)&&(lastdata[i1][i2].y<0)){
            tempvect=(lastdata[i1][i2].incliny/lastdata[i1][i2].inclinx)*(2000-lastdata[i1][i2].x)+lastdata[i1][i2].y;
            if((tempvect>=0)&&(tempvect<=3000)){
              vectx=2000;
              vecty=round(tempvect);
            }
            else{
              vectx=round((3000-lastdata[i1][i2].y)*(lastdata[i1][i2].inclinx/lastdata[i1][i2].incliny)+lastdata[i1][i2].x);
              vecty=3000;
            }
            SDL_RenderDrawLine(renderer,100+(lastdata[i1][i2].y>>1),100+(lastdata[i1][i2].x>>1),100+(vecty>>1),100+(vectx>>1));
          }
          else if((lastdata[i1][i2].y<0)&&(lastdata[i1][i2].x>1900)){
            tempvect=(lastdata[i1][i2].incliny/lastdata[i1][i2].inclinx)*(-lastdata[i1][i2].x)+lastdata[i1][i2].y;
            if((tempvect>=0)&&(tempvect<=3000)){
              vectx=0;
              vecty=round(tempvect);
            }
            else{
              vectx=round((3000-lastdata[i1][i2].y)*(lastdata[i1][i2].inclinx/lastdata[i1][i2].incliny)+lastdata[i1][i2].x);
              vecty=3000;
            }
            SDL_RenderDrawLine(renderer,100+(lastdata[i1][i2].y>>1),100+(lastdata[i1][i2].x>>1),100+(vecty>>1),100+(vectx>>1));
          }
          else if (lastdata[i1][i2].x>3000){
            tempvect=(-lastdata[i1][i2].y)*(lastdata[i1][i2].inclinx/lastdata[i1][i2].incliny)+lastdata[i1][i2].x;
            if(tempvect<0){
              vectx=0;
              vecty=round((lastdata[i1][i2].incliny/lastdata[i1][i2].inclinx)*(-lastdata[i1][i2].x)+lastdata[i1][i2].y);
            }
            else if(tempvect>2000){
              vectx=2000;
              vecty=round((lastdata[i1][i2].incliny/lastdata[i1][i2].inclinx)*(2000-lastdata[i1][i2].x)+lastdata[i1][i2].y);
            }
            else{
              vectx=round(tempvect);
              vecty=0;
            }
            SDL_RenderDrawLine(renderer,100+(lastdata[i1][i2].y>>1),100+(lastdata[i1][i2].x>>1),100+(vecty>>1),100+(vectx>>1));
          } 
        }
      }
      
      for(int i2=0;i2<NOMBRECAM;i2++){
        if(lastdata[i1][i2].active){
          for(int i3=i2+1;i3<NOMBRECAM;i3++){
            if(lastdata[i1][i3].active){
              myDrawPoint(100+(ballpst[i1][i2][i3].y>>1),100+(ballpst[i1][i2][i3].x>>1));
            }
          }
        }
      }
      SDL_SetRenderDrawColor(renderer,0,0,0,0);
      if((posRobot[i1].x!=0)&&(posRobot[i1].y!=0))
        myDrawBall(100+(int(round(posRobot[i1].y))>>1),100+(int(round(posRobot[i1].x))>>1));
    }
    SDL_RenderPresent(renderer);

}
  

void *interfaceThread(void *t){
  interfaceInit();
  while(!quitServer)interfaceMainLoop();
  closeInterface();
  pthread_exit(NULL);
  }
