#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_image.h"

// THE WAVE EQUATION PDE ::
//
// @_tt( u(x,t) ) = c^2 @_xx( u(x,t) )
// x : [0, 2pi]  
// t : [0, T]
// u_0 = f(x)
// u(0,t) = 0
// u(2pi,t) = 0
//

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;
const double DEFAULT_VALUE = 111.111;

const int NUM_T_STEPS = 200000;
const double LEFT_X_BOUND = 0;
const double RIGHT_X_BOUND = 2*M_PI;
const double LEFT_Y_BOUND = 0;
const double RIGHT_Y_BOUND = 2*M_PI;

const double DURATION = 8;
const double DELTA_T = DURATION/NUM_T_STEPS;
const double DELTA_X = (RIGHT_X_BOUND - LEFT_X_BOUND)/(SCREEN_WIDTH);
const double DELTA_Y = (RIGHT_Y_BOUND - LEFT_Y_BOUND)/(SCREEN_HEIGHT);

const double c = 1; // wave speed
const double C = DELTA_T/(DELTA_X*DELTA_Y) * c; // Courant number: must be less than 1 for solution to remain stable

// initial conditions
double u_0(int x_idx, int y_idx, int t=0){
    double x = (x_idx-SCREEN_WIDTH/2);
    double y = (y_idx-SCREEN_HEIGHT/2);
    // if (std::abs(x*x*DELTA_X*DELTA_X + y*y*DELTA_Y*DELTA_Y) <= 0.2){
    //     return 100;
    // }
    // return 0;
    return 200 * ( 2*std::pow(2.71828,-(10*(x*x*DELTA_X*DELTA_X + y*y*DELTA_Y*DELTA_Y)))
    + 3*std::pow(2.71828,-10*((x-100)*(x-100)*DELTA_X*DELTA_X + (y-100)*(y-100)*DELTA_Y*DELTA_Y))
    + 7*std::pow(2.71828,-10*((x+100)*(x+100)*DELTA_X*DELTA_X + (y+100)*(y+100)*DELTA_Y*DELTA_Y))
    + std::pow(2.71828,-10*((x-100)*(x-100)*DELTA_X*DELTA_X + (y+100)*(y+100)*DELTA_Y*DELTA_Y))
    + std::pow(2.71828,-10*((x+100)*(x+100)*DELTA_X*DELTA_X + (y-100)*(y-100)*DELTA_Y*DELTA_Y))     );
}

// computes numerical solution recursively
double u(int x, int y, int t, double*** output_map){
    if (x <= 0){ output_map[2][x][y] = 0; return 0; } // boundary conditions
    if (x >= SCREEN_WIDTH){ output_map[2][x][y] = 0; return 0; }
    if (y <= 0){ output_map[2][x][y] = 0; return 0; } // boundary conditions
    if (y >= SCREEN_HEIGHT){ output_map[2][x][y] = 0; return 0; }
    if (t <= 0){ output_map[2][x][y] = u_0(x, y); return u_0(x, y); }  // initial conditions
    
    double output = 2*output_map[1][x][y] - output_map[0][x][y] 
    + C*C*( output_map[1][x+1][y] +  output_map[1][x-1][y] + output_map[1][x][y+1] +  output_map[1][x][y-1] - 4*output_map[1][x][y] );
    output_map[2][x][y] = output;
    return output;
}


int main(){
    // setup window
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
    SDL_RenderClear( renderer );

    bool quit = false;
    SDL_Event e;
    int t = 0;
    int displacement = 0;
    
    // initialize output map (stores new and previous 2 iteration values for computing the derivatives)
    double*** output_map = new double**[3];
    for (int i = 0; i < 3; i++){
        output_map[i] = new double*[SCREEN_WIDTH+1];
        for (int x = 0; x < SCREEN_WIDTH+1; x++){
            output_map[i][x] = new double[SCREEN_HEIGHT+1];
        }
    }
    for (int x = 0; x < SCREEN_WIDTH+1; x++){
        for (int y = 0; y < SCREEN_HEIGHT+1; y++){
            output_map[0][x][y] = u_0(x,y);
            output_map[1][x][y] = u_0(x,y);
            output_map[2][x][y] = DEFAULT_VALUE; // where the newly computed solution will be stored
        }
    }

    // computes the solution for each time t
    while (!quit && t < NUM_T_STEPS){
        // checks if user quits (closes window)
        while( SDL_PollEvent( &e ) != 0 ){ if( e.type == SDL_QUIT ){ quit = true; } }
        
        // computes and outputs solution to screen
        for (int x = 0; x < SCREEN_WIDTH+1; x++){
            for (int y = 0; y < SCREEN_HEIGHT+1; y++){
                displacement = -u(x, y, t, output_map);
                // displays every 10th computed frame
                if (t % 10 == 0){
                    // colors output pixel properly
                    if (std::abs(displacement) <= 100){
                        SDL_SetRenderDrawColor( renderer, 
                        0,                                                      std::min(0.25*std::abs(displacement),255.0),        2*std::abs(displacement)
                        , 255 );
                    }
                    else if (std::abs(displacement) <= 200){
                        SDL_SetRenderDrawColor( renderer, 
                        std::min(2*(std::abs(displacement)-100),255),           std::min(25 + (std::abs(displacement)-100),255),    200-2*(std::abs(displacement)-100)
                        , 255 );
                    }
                    else{
                        SDL_SetRenderDrawColor( renderer, 
                        std::min(200+(std::abs(displacement)-200),255),         125-0.25*(std::abs(displacement)-200),               0
                        , 255 );
                    }
                    // actually draws the point to buffer
                    SDL_RenderDrawPointF( renderer, x, y );
                }
            }
        }
        if (t % 10 == 0){
            SDL_RenderPresent( renderer );
        }
        if (t % 10 == 9){
            SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
            SDL_RenderClear( renderer );
        }

        // prepare output_map for next use by shifting each row back
        for (int x = 0; x < SCREEN_WIDTH+1; x++){
            for (int y = 0; y < SCREEN_HEIGHT+1; y++){
                output_map[0][x][y] = output_map[1][x][y];
                output_map[1][x][y] = output_map[2][x][y];
                output_map[2][x][y] = DEFAULT_VALUE; // resets row where future t+1 solution will be stored
            }
        }
        t+=1;
    }

    for (int i = 0; i < 3; i++){
        for (int x = 0; x < SCREEN_WIDTH+1; x++){
            delete[] output_map[i][x]; 
        }
    }
}