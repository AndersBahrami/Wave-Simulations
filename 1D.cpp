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
const double DEFAULT_VALUE = 0;

const int NUM_T_STEPS = 100000;
const double LEFT_X_BOUND = 0;
const double RIGHT_X_BOUND = 2*M_PI;

const double DURATION = 10;
const double DELTA_T = DURATION/NUM_T_STEPS;
const double DELTA_X = (RIGHT_X_BOUND - LEFT_X_BOUND)/(SCREEN_WIDTH);

const double c = 10; // wave speed
const double C = DELTA_T/DELTA_X * c; // Courant number: must be less than 1 for solution to remain stable

// initial conditions
double u_0(int x, int t=0){
    double input = (x-SCREEN_WIDTH) * DELTA_X;
    return SCREEN_HEIGHT/8 * (sin(input)+ 2*std::pow(2.71828,-(100*input*input)));
}

// computes numerical solution recursively
double u(int x, int t, double** output_map){
    if (x <= 0){ output_map[2][x] = 0; return 0; } // boundary conditions
    if (x >= SCREEN_WIDTH){ 
        double output = 2*output_map[1][x] - output_map[0][x] + 2*C*C*( - output_map[1][x] + output_map[1][x-1] );
        output_map[2][x] = output;
        return output;
    }
    if (t <= 0){ output_map[2][x] = u_0(x); return u_0(x); }  // initial conditions
    
    double output = 2*output_map[1][x] - output_map[0][x] + C*C*( output_map[1][x+1] - 2*output_map[1][x] + output_map[1][x-1] );
    output_map[2][x] = output;
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
    double displacement = 0;
    
    // initialize output map (stores new and previous 2 iteration values for computing the derivatives)
    double** output_map = new double*[3];
    for (int i = 0; i < 3; i++){
        output_map[i] = new double[SCREEN_WIDTH+1];
    }
    for (int x = 0; x < SCREEN_WIDTH+1; x++){
        output_map[0][x] = u_0(x);
        output_map[1][x] = u_0(x);
        output_map[2][x] = DEFAULT_VALUE; // where the newly computed solution will be stored
    }

    // computes the solution for each time t
    while (!quit && t < NUM_T_STEPS){
        // checks if user quits (closes window)
        while( SDL_PollEvent( &e ) != 0 ){ if( e.type == SDL_QUIT ){ quit = true; } }
        
        // computes and outputs solution to screen
        for (int x = 0; x < SCREEN_WIDTH+1; x++){
            displacement = -u(x, t, output_map);
            SDL_SetRenderDrawColor( renderer, std::min( 2*std::abs(displacement), 255.0 ), 125, 150, 255 );
            SDL_RenderDrawPointF( renderer, x, SCREEN_HEIGHT/2 + displacement-1 );
            SDL_RenderDrawPointF( renderer, x, SCREEN_HEIGHT/2 + displacement );
            SDL_RenderDrawPointF( renderer, x, SCREEN_HEIGHT/2 + displacement+1 );
        }
        SDL_RenderPresent( renderer );
        SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
        SDL_RenderClear( renderer );

        // prepare output_map for next use by shifting each row back
        for (int x = 0; x < SCREEN_WIDTH+1; x++){
            output_map[0][x] = output_map[1][x];
            output_map[1][x] = output_map[2][x];
            output_map[2][x] = DEFAULT_VALUE; // resets row where future t+1 solution will be stored
        }
        t+=1;
    }

    for (int i = 0; i < 3; i++){
        delete[] output_map[i]; 
    }
}