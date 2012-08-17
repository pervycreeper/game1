

#include "SDL.h"
#include <stdio.h>
#include <math.h>

const int mapWidth = 24;
const int mapHeight= 24;

long long framecount=0;
double vscale=1;




double speedconst =.002;
double rotspeedconst =.0005;

const int width= 1024;
const int height = 768;


double fovconst=.66;

double fov = atan(fovconst)*2.0;

const double tau = asin(1.0) *4.0; //yes, I'm trying to be cute

int pixfov = int (double(width) * tau / fov);

SDL_Rect screenslice;
SDL_Rect screenslice2;


int frametime[3] = {17,17,16}; //assume 60 fps to start, may cap it there later


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
	bool quit;
};

inputstate playerstate;



//prototypes
SDL_Surface* renderFrame(SDL_Surface* prescreen, double rposx, double rposy, double directionx, double directiony);
SDL_Surface* hello = NULL; 
void drawbackground(double viewdir, SDL_Surface* backimg);
struct inputstate getstate(struct inputstate curstate);
bool updateworld ( struct inputstate curstate, double expectedtime);

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

	hello = SDL_LoadBMP( "greystone.bmp" );


	pano = SDL_LoadBMP( "pano.bmp" ); //background
	prescreen = SDL_LoadBMP( "t.bmp" ); //now used to set up a canvas of the right size, fix later


//	int frames = 0;

	struct inputstate curstate = {false, false, false, false, false, false, false, false};
	struct inputstate oldstate;
  

	bool mainloopdone = false;
	long long a = SDL_GetTicks();

	while (!mainloopdone)
	{
		oldstate = curstate;
		curstate = getstate(oldstate);
		double expectedtime = (frametime[0] + frametime[1] +frametime [2]) / 3.0 ;
		mainloopdone = updateworld (curstate, expectedtime); // if player wants to quit, updateworld will return true
		drawbackground(playerangle, pano);
		prescreen = renderFrame(prescreen, playerx , playery, cos(playerangle) , sin(playerangle));
		frametime [(framecount % 3)] = SDL_GetTicks()-a;
		a = SDL_GetTicks(); 
		printf("%d, %d, %d, %f, %d FPS\n", frametime[0], framecount, expectedtime, int(1000.0/expectedtime) ); //time frame

		//finally, draw screen
		SDL_BlitSurface(prescreen , NULL, screen, NULL );
		SDL_Flip( screen );
		framecount++;
	}
	SDL_FreeSurface( prescreen );
	SDL_Quit(); 
	return 0; 
}

struct inputstate getstate(struct inputstate curstate ){

	/*there is a design flaw here, where since multiple keys can have the 	
	same effect, lifting one while the other is depressed will result in 
	nullifying the effect of the other one. Will not fix for time being.
	*/

	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch(event.type){
		case SDL_KEYDOWN:
			{
				if (event.key.keysym.sym  ==  SDLK_ESCAPE){
					curstate.quit = true;}
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
					if (curstate.up == true){
						curstate.up= false;}
				}
				if ((event.key.keysym.sym  == SDLK_s) | (event.key.keysym.sym  == SDLK_DOWN)){

					if (curstate.down== true){
						curstate.down= false;}
				}
				if (event.key.keysym.sym  == SDLK_a){
					if (curstate.leftstrafe== true){


						curstate.leftstrafe= false;}}

				if (event.key.keysym.sym  == SDLK_d){
					if (curstate.rightstrafe==true){

						curstate.rightstrafe= false;}}
				if (event.key.keysym.sym  == SDLK_LEFT){

					if (curstate.left== true){
						curstate.left= false;}

				}					
				if (event.key.keysym.sym  == SDLK_RIGHT){
					if (curstate.right==true){
						curstate.right= false;}
				}
				break;
			}


		case SDL_QUIT:
			curstate.quit= true;
			break;
		default:
			break;
		}											
	}
	return curstate;
}

bool updateworld ( struct inputstate curstate, double expectedtime){
	double runconst;
	if (curstate.run== true){
		runconst = 1.0;}
	else{
		runconst = 3.0;}

	double speed = speedconst * runconst * expectedtime ;
	double rotspeed = rotspeedconst * runconst *expectedtime;
	if (curstate.quit== true){
		return true;}

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
			playerangle += tau;
		}
	}					
	if (curstate.right== true){
		playerangle+=rotspeed;
		if (playerangle>=tau){
			playerangle -= tau;
		}
	}
	return false;
}

void drawbackground(double viewdir, SDL_Surface* backimg){

	if ( viewdir + fov < tau){
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
		screenslice.w = int(width*(tau-viewdir)/fov);
		screenslice.h = height;

		SDL_BlitSurface(backimg , &screenslice   , prescreen, NULL );

		screenslice2.x =int(width*(tau-viewdir)/fov);
		screenslice2.y =0;
		screenslice2.w = width -int(width*(tau-viewdir)/fov);
		screenslice2.h = height;

		screenslice.x =0;
		screenslice.y =0;
		screenslice.w = width -int(width*(tau-viewdir)/fov);
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








