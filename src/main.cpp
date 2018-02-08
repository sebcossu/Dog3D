#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "sebutils.h" // uses namespace 'su'

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

SDL_Window* win = NULL;
SDL_Renderer* renderer = NULL;

// initialize SDL
void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	win = SDL_CreateWindow("Dog3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // using hardware acceleration and vsync
    //renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE); // using software acceleration and no vsync
}

// update screen
void draw()
{
    SDL_RenderPresent(renderer);
}

// clear screen
void cls(su::RGBA color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderClear(renderer);
}

// deallocate all and close SDL
void quitSDL()
{
	//Destroy window
	SDL_DestroyRenderer( renderer );
	SDL_DestroyWindow( win );
	win = NULL;
	renderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

// draws a vertical line of 1 pixel at coordinates (x,startY) of length endY and color rgba
void drawVerticalLine(SDL_Renderer* r, int x, int startY, int endY, su::RGBA rgba)
{
    SDL_SetRenderDrawColor(r, rgba.r, rgba.g, rgba.b, rgba.a);
    SDL_RenderDrawLine(r, x, startY, x, endY);
}


int main(int argc, char* argv[])
{
    std::string dataFolder = "data/";
    std::string defaultMap = "1.dog";
    std::string mapName;

    if( argc > 2 )
    {
        printf("Error: too many arguments.\n\nUsage: \n\tWOLF3D.exe <mapname.dog>\n\tWOLF3D.exe\n\n");
        return -1;
    }
    else if( argc == 2 )
    {
        mapName = /*dataFolder + */argv[1];
    }
    else
    {
        mapName = dataFolder + defaultMap;
    }

    std::cout << "Loading: " << mapName << std::endl << std::endl;

    // std::string mapName = "data/home.dog";
    su::Level level;
    bool loaded = su::loadLevel(mapName, &level); // load map from file

    if(!loaded)
    {
        std::cout << "Error: Cannot load map: " << mapName << std::endl;
        return -1; // cannot load the level from map file
    }

    for(uint8_t i = 0; i < level.height; i++) // print the map
    {
        for(uint8_t j = 0; j < level.width; j++)
        {
            std::cout << level.level[i].at(j);
        }
        std::cout << std::endl;
    }

    // print map infos
    std::cout << std::endl << "Map of size: " << level.height << "x" << level.width << std::endl;
    std::cout << "Player starting position: (" << level.player.pos.x << ", " << level.player.pos.y << ")" << std::endl << std::endl;

    su::CoordDouble pos = { level.player.pos.x, level.player.pos.y };
    su::CoordDouble dir = { -1, 0 };
    su::CoordDouble plane = { 0, 0.66 };

    double curTick = 0;
    double prevTick = 0;

    bool quit = false;

    SDL_Event e;
    const Uint8 *state;

    init();

    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0) // check if the user quits
        {
            if(e.type == SDL_QUIT) // deallocate everything
            {
                quit = true;
                quitSDL();
            }
        }

        for(int x = 0; x < SCREEN_WIDTH; x++) // for all x on the screen resolution
        {
            double cameraX = 2*x / double(SCREEN_WIDTH) - 1;
            su::CoordDouble rayPos = { pos.x, pos.y };
            su::CoordDouble rayDir = { dir.x + plane.x * cameraX, dir.y + plane.y * cameraX };
            su::CoordInt mapc = { int(rayPos.x), int(rayPos.y) };
            su::CoordDouble sideDist;
            su::CoordDouble deltaDist = { sqrt(1 + (rayDir.y * rayDir.y)/(rayDir.x * rayDir.x)), sqrt(1 + (rayDir.x * rayDir.x)/(rayDir.y * rayDir.y)) };
            double perpWallDist;
            su::CoordInt step;
            int hit = 0;
            int side;

            if(rayDir.x < 0)
            {
                step.x = -1;
                sideDist.x = (rayPos.x - mapc.x) * deltaDist.x;
            }
            else
            {
                step.x = 1;
                sideDist.x = (mapc.x + 1.0 - rayPos.x) * deltaDist.x;
            }

            if(rayDir.y < 0)
            {
                step.y = -1;
                sideDist.y = (rayPos.y - mapc.y) * deltaDist.y;
            }
            else
            {
                step.y = 1;
                sideDist.y = (mapc.y + 1.0 - rayPos.y) * deltaDist.y;
            }

            while(hit == 0) // calculate ray until it hits an object
            {
                if(sideDist.x < sideDist.y)
                {
                    sideDist.x += deltaDist.x;
                    mapc.x += step.x;
                    side = 0;
                }
                else
                {
                    sideDist.y += deltaDist.y;
                    mapc.y += step.y;
                    side = 1;
                }

                if(level.level[mapc.x][mapc.y]>'0') // the ray has hit a non-zero block
                {
                    hit = 1;
                }
            }

            // distance based on camera direction
			if (side == 0)
            {
                perpWallDist = fabs((mapc.x - rayPos.x + (1 - step.x) / 2) / rayDir.x);
            }
			else
            {
                perpWallDist = fabs((mapc.y - rayPos.y + (1 - step.y) / 2) / rayDir.y);
            }

			// the line to draw...
			int lineHeight = abs(int(SCREEN_HEIGHT / perpWallDist)); // height
			int drawStart = -lineHeight / 2 + SCREEN_HEIGHT / 2; // y1
			if (drawStart < 0) drawStart = 0;
			int drawEnd = lineHeight / 2 + SCREEN_HEIGHT / 2; // y2
            if (drawEnd >= SCREEN_HEIGHT) drawEnd = SCREEN_HEIGHT - 1;


            su::RGBA color;
            switch(level.level[mapc.x][mapc.y])
            {
                case '1': // RED
                    color.r = 255;
                    color.g = 0;
                    color.b = 0;
                    color.a = 255;
                    break;
                case '2': // GREEN
                    color.r = 0;
                    color.g = 255;
                    color.b = 0;
                    color.a = 255;
                    break;
                case '3': // BLUE
                    color.r = 0;
                    color.g = 0;
                    color.b = 255;
                    color.a = 255;
                    break;
                case '4': // BLACK
                    color.r = 140;
                    color.g = 70;
                    color.b = 20;
                    color.a = 255;
                    break;
                default:
                    std::cout << "Unknown object of type <<" << level.level[mapc.x][mapc.y] << ">> at x:" << mapc.x << ", y:" << mapc.y << std::endl;
                    return -8;
            }

            if(side == 1)
            {
                color = { color.r/2, color.g/2, color.b/2, color.a };
            }
            drawVerticalLine(renderer, x, drawStart, drawEnd, color);
        }

        prevTick = curTick;
        curTick = SDL_GetTicks();
        double frameRate = (curTick - prevTick)/1000.0; // calculate framerate
        //printf("FPS: %f\n", 1.0/frameRate);

        // update screen and clear screen
        su::RGBA clsclr = {100, 100, 100, 255};
        draw();
        cls(clsclr);

		double moveSpeed = frameRate * 5.0; // movement speed
		double rotationSpeed = frameRate * 3.0; // rotation speed
        double angle = 1.571; // angle for side movement (-left/+right)

		state = SDL_GetKeyboardState(NULL);

		if (state[SDL_SCANCODE_W]) // forward movement with 'W' key
		{
            if(level.level[int(pos.x + dir.x * moveSpeed)][int(pos.y)] == '0') pos.x += dir.x * moveSpeed;
            if(level.level[int(pos.x)][int(pos.y + dir.y * moveSpeed)] == '0') pos.y += dir.y * moveSpeed;
		}

        if (state[SDL_SCANCODE_S]) // backward movement with 'S' key
		{
            if(level.level[int(pos.x - dir.x * moveSpeed)][int(pos.y)] == '0') pos.x -= dir.x * moveSpeed;
            if(level.level[int(pos.x)][int(pos.y - dir.y * moveSpeed)] == '0') pos.y -= dir.y * moveSpeed;
		}

		if (state[SDL_SCANCODE_J]) // left movement with 'J' key
        {
            double newDirX = dir.x * cos(angle) - dir.y * sin(angle);
            double newDirY = dir.x * sin(angle) + dir.y * cos(angle);

            if(level.level[int(pos.x + newDirX * moveSpeed)][int(pos.y)] == '0') pos.x += newDirX * moveSpeed;
            if(level.level[int(pos.x)][int(pos.y + newDirY * moveSpeed)] == '0') pos.y += newDirY * moveSpeed;
        }

		if (state[SDL_SCANCODE_L]) // right movement with 'L' key
        {
            double newDirX = dir.x * cos(-angle) - dir.y * sin(-angle);
            double newDirY = dir.x * sin(-angle) + dir.y * cos(-angle);

            if(level.level[int(pos.x + newDirX * moveSpeed)][int(pos.y)] == '0') pos.x += newDirX * moveSpeed;
            if(level.level[int(pos.x)][int(pos.y + newDirY * moveSpeed)] == '0') pos.y += newDirY * moveSpeed;
        }

		if (state[SDL_SCANCODE_D]) // rotate to the right with 'D' key
		{
            double oldDirX = dir.x;
            dir.x = dir.x * cos(-rotationSpeed) - dir.y * sin(-rotationSpeed);
            dir.y = oldDirX * sin(-rotationSpeed) + dir.y * cos(-rotationSpeed);

            double oldPlaneX = plane.x;
            plane.x = plane.x * cos(-rotationSpeed) - plane.y * sin(-rotationSpeed);
            plane.y = oldPlaneX * sin(-rotationSpeed) + plane.y * cos(-rotationSpeed);
		}

		if (state[SDL_SCANCODE_A]) // rotate to the left with 'A' key
		{
            double oldDirX = dir.x;
            dir.x = dir.x * cos(rotationSpeed) - dir.y * sin(rotationSpeed);
            dir.y = oldDirX * sin(rotationSpeed) + dir.y * cos(rotationSpeed);

            double oldPlaneX = plane.x;
            plane.x = plane.x * cos(rotationSpeed) - plane.y * sin(rotationSpeed);
            plane.y = oldPlaneX * sin(rotationSpeed) + plane.y * cos(rotationSpeed);
		}
		if (state[SDL_SCANCODE_ESCAPE])
        {
            quit = true;
            quitSDL();
        }
    }

    return 0;
}
