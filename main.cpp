

#include "SDL.h"
#include <stdio.h>
#include <math.h>

const int mapWidth = 24;
const int mapHeight= 24;

long long framecount=0;
double vscale=1;




//change to gridunits/second; rotations/second

double speed=.02;
double rotspeed=.005;

const int width= 1024;
const int height = 768;


double fovconst=.66;

double fov = atan(fovconst)*2.0;

const double twopi = asin(1.0) *4.0;

int pixfov = int (double(width) * twopi / fov);

	SDL_Rect screenslice;
	SDL_Rect screenslice2;



double playerx = 2.01;
double playery = 16.0;
double playerangle = 0.0;


struct inputstate
{
	bool up;
	bool down;
	bool left;
	bool right;
	bool leftstrafe;
	bool rightstrafe;
	bool run;
};

inputstate playerstate;



//prototypes
SDL_Surface* renderFrame(SDL_Surface* prescreen, double rposx, double rposy, double directionx, double directiony);
SDL_Surface* hello = NULL; 
void drawbackground(double viewdir, SDL_Surface* backimg);

SDL_Surface* prescreen = NULL; 

SDL_Surface* t = NULL;
SDL_Surface* pano = NULL; 
int worldMap[mapWidth][mapHeight]=
{
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,2,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,3,0,0,0,3,0,0,0,1},
	{1,0,0,0,0,0,2,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,2,2,0,2,2,0,0,0,0,3,0,3,0,3,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,5,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
	{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};







int main( int argc, char* args[] ) 
{ 
	//start sdl
	SDL_Surface* screen = NULL;
	SDL_Init( SDL_INIT_EVERYTHING );
	screen = SDL_SetVideoMode( width, height, 32, SDL_SWSURFACE );
	SDL_WM_SetCaption( "raycast", NULL );


	//load textures
	printf("loading textures...");
	
//	hello = SDL_LoadBMP( "checker3.bmp" ); //wall texture
	
	hello = SDL_LoadBMP( "greystone.bmp" );


	pano = SDL_LoadBMP( "pano.bmp" ); //background
	prescreen = SDL_LoadBMP( "t.bmp" ); //now used to set up a canvas of the right size, fix later


//  getinputstate()
//	updateworld()

	int frames = 3;

	struct inputstate curstate = {false, false, false, false, false, false, false};
	struct inputstate oldstate = curstate;
	struct inputstate nextstate = curstate;

	bool mainloopdone = false;

	while (!mainloopdone)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch(event.type){  /* Process the appropiate event type */
			case SDL_KEYDOWN:
				{
					if (event.key.keysym.sym  ==  SDLK_ESCAPE){
						mainloopdone = true;}

					if ((event.key.keysym.sym  == SDLK_w) | (event.key.keysym.sym  == SDLK_UP)){
						curstate.up= true;
					}
					if ((event.key.keysym.sym  == SDLK_s) | (event.key.keysym.sym  == SDLK_DOWN)){
						curstate.down= true;}

					if (event.key.keysym.sym  == SDLK_a){
						curstate.leftstrafe= true;}

					if (event.key.keysym.sym  == SDLK_d){
						curstate.rightstrafe= true;}
					if (event.key.keysym.sym  == SDLK_LEFT){
					curstate.left= true;
						
					}					
					if (event.key.keysym.sym  == SDLK_RIGHT){
				curstate.right= true;
					}
					break;
				}

				case SDL_KEYUP:
				{

					if ((event.key.keysym.sym  == SDLK_w) | (event.key.keysym.sym  == SDLK_UP)){
						curstate.up= false;
					}
					if ((event.key.keysym.sym  == SDLK_s) | (event.key.keysym.sym  == SDLK_DOWN)){
						curstate.down= false;}

					if (event.key.keysym.sym  == SDLK_a){
						curstate.leftstrafe= false;}

					if (event.key.keysym.sym  == SDLK_d){
						curstate.rightstrafe= false;}
					if (event.key.keysym.sym  == SDLK_LEFT){
					curstate.left= false;
						
					}					
					if (event.key.keysym.sym  == SDLK_RIGHT){
				curstate.right= false;
					}
					break;
				}


			case SDL_QUIT:
				mainloopdone= true;
				break;
			default:
				break;
			}											
		}



		if (curstate.up== true){
			playerx += speed * cos(playerangle);
			playery += speed * sin(playerangle);}


		if (curstate.down== true){
			playerx -= speed * cos(playerangle);
			playery -= speed * sin(playerangle);}

		if (curstate.leftstrafe== true){
			playerx += speed * sin(playerangle);
			playery -= speed * cos(playerangle);}

		if (curstate.rightstrafe== true){
			playerx -= speed * sin(playerangle);
			playery += speed * cos(playerangle);}
		if (curstate.left== true){
			playerangle-=rotspeed;
			if (playerangle<0.0){
				playerangle += twopi;
			}
		}					
		if (curstate.right== true){
			playerangle+=rotspeed;
			if (playerangle>=twopi){
				playerangle -= twopi;
			}
		}

			// change!!!!!
//	curstate= oldstate;




		drawbackground(playerangle, pano);
		long long a = SDL_GetTicks(); //time frame
		prescreen = renderFrame(prescreen, playerx , playery, cos(playerangle) , sin(playerangle));
		printf("%d, %d, %d\n", (SDL_GetTicks()-a), frames ); //time frame

		//finally, draw screen
		SDL_BlitSurface(prescreen , NULL, screen, NULL );
		SDL_Flip( screen );
		frames++;
	}
	SDL_FreeSurface( prescreen );
	SDL_Quit(); 
	return 0; 
}

void drawbackground(double viewdir, SDL_Surface* backimg){

	if ( viewdir + fov < twopi){
		screenslice.x =int(width*viewdir/fov);
		screenslice.y =0;
		screenslice.w = width;
		screenslice.h = height;
		SDL_BlitSurface(backimg , &screenslice   , prescreen, NULL );
	}
	else
	{
		screenslice.x =int(width*viewdir/fov);
		screenslice.y =0;
		screenslice.w = int(width*(twopi-viewdir)/fov);
		screenslice.h = height;

		SDL_BlitSurface(backimg , &screenslice   , prescreen, NULL );

		screenslice2.x =int(width*(twopi-viewdir)/fov);
		screenslice2.y =0;
		screenslice2.w = width -int(width*(twopi-viewdir)/fov);
		screenslice2.h = height;

		screenslice.x =0;
		screenslice.y =0;
		screenslice.w = width -int(width*(twopi-viewdir)/fov);
		screenslice.h = height;

		SDL_BlitSurface(backimg , &screenslice   , prescreen, &screenslice2 );
	}
}

SDL_Surface* renderFrame(SDL_Surface* canvas, double rposx, double rposy, double directionx, double directiony){

	double camx = fovconst * directiony;
	double camy = -fovconst * 1 * directionx;
	double camstepx = -camx * 2 / (width);
	double camstepy = -camy * 2 / (width);
	double lookrayx = directionx+camx;
	double lookrayy= directiony+camy;

	int xsign, ysign, cursquarex, cursquarey;
	double xdist, ydist;
	bool hitx;
	int c;
	double interceptx, intercepty;
	double d, dd;
	int scaledrawheight, drawheight, startdraw;
	double h,s;
	int hit;



	SDL_LockSurface (canvas) ;



	for (int curcol = 0; curcol < width; curcol++) {
		if (lookrayx >= 0){
			xsign=1;
		}
		else{
			xsign= -1;
		}
		if (lookrayy >= 0) {
			ysign=1;
		}
		else{
			ysign=-1;
		}
		cursquarex = int(rposx);
		cursquarey = int(rposy);
		c = 0;
		while (c == 0){
			if (lookrayx == 0.0){
				xdist = 2.0;
			}
			else{
				xdist=(cursquarex+(1+xsign)/2-rposx)/lookrayx;
			}
			if (lookrayy == 0.0){
				ydist = 2.0;
			}
			else{
				ydist=(cursquarey+(1+ysign)/2-rposy)/lookrayy;
			}

			if (xdist > ydist){
				cursquarey += ysign;
				hitx = false;
			}
			else{
				cursquarex += xsign;
				hitx = true;
			}
			c = worldMap[cursquarex][cursquarey];
		}
		if (hitx){
			// order matters here, n.b.
			interceptx= double (cursquarex +(1-xsign)/2) ;
			intercepty= rposy + (interceptx-rposx)*lookrayy/lookrayx;
		}
		else{
			intercepty= double (cursquarey +(1-ysign)/2);
			interceptx= rposx + (intercepty-rposy)*lookrayx/lookrayy;
		}

		dd = ((rposx-interceptx)*(rposx-interceptx) + (rposy-intercepty)*(rposy-intercepty))/ ((lookrayx)*(lookrayx)+(lookrayy)*(lookrayy)) ;
		d = sqrt (dd);

		// originally, scaledrawheight=int(vscale*height/d);
		// keep it even to make seams between pixels look better-- doesn't fix middle of line of screen by itself
		scaledrawheight=2*int(vscale*height/d/2);
		


		if (height < scaledrawheight) {
			drawheight = height;
		}
		else{
			drawheight= scaledrawheight;
		}

		startdraw = int((height-drawheight)/2);
		
		h=32.0*(scaledrawheight-drawheight)/scaledrawheight;

		s=63.999999/scaledrawheight; //rather than 64.0-- a fudge factor to keep the middle row free from noise due to rounding
		
		if (hitx){ 
			hit=int(64*(intercepty-int(intercepty)));
		}
		else{
			hit=int(64*(interceptx-int(interceptx)));

		} 
		
		// this is a mess due to recasting, but not sure how to change, since this is how SDL works.
		// moved locking/unlocking to parent loop, but not sure if this is a good idea. calls sqrt() while locked
		// todo: add facilities for different textures, 
//		SDL_LockSurface (canvas) ;
		int canvpitch = canvas->pitch;
		int c = (curcol)* canvas->format->BytesPerPixel;
		void *canvaspixels = canvas->pixels;
		Uint16 texpitch = hello->pitch;
		int lim = (drawheight +startdraw) * canvpitch +c + (int) canvaspixels;
		Uint8 *k = (Uint8 *)hello->pixels + (hit)* hello->format->BytesPerPixel;

		for (int j= (startdraw)*(canvpitch)+c + (int) canvaspixels; (j< lim); j+= canvpitch){
			*(Uint32 *)j = *(Uint32 *)((int(h))*(texpitch)+k);
			h += s;
		}
//		SDL_UnlockSurface (canvas);
		lookrayx += camstepx;
		lookrayy += camstepy;
	}
	SDL_UnlockSurface (canvas);
	return canvas;
}









