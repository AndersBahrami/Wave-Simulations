#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include "include/SDL2/SDL.h"
#include "include/SDL2/SDL_image.h"
#include <map>
#include <optional>


// THE WAVE EQUATION PDE ::
//
// @_tt( u(x,t) ) = c^2 @_xx( u(x,t) )
// x : [0, 2pi]  
// t : [0, 10]
// u_0 = sin(x)
// u(0,t) = 0
// u(2pi,t) = 0
//

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

// const int NUM_X_STEPS = 100;
const int NUM_T_STEPS = 1000;
const double LEFT_X_BOUND = 0;
const double RIGHT_X_BOUND = 2 * M_PI;
const double DEFAULT_VALUE = 11111.111;

const double DURATION = 7;
const double DELTA_T = DURATION/NUM_T_STEPS;
const double DELTA_X = (RIGHT_X_BOUND - LEFT_X_BOUND)/SCREEN_WIDTH;

const double c = 1; // wave speed
const double C = DELTA_T/DELTA_X * c; // must be less than 1 for solution to remain stable

// using output_type = std::optional<double>;
using output_type = double;
// using io_type = std::pair< std::optional<int>, double >;

// initial conditions
double u_0(int x, int t=0){ // x \in [0, 2pi]
    double input = x * DELTA_X;
    return SCREEN_HEIGHT/8 * (sin(input * 10)+cos(input*3));
}

// computes numerical solution recursively
double u(int x, int t, output_type** output_map){

    // boundary conditions (Neumann)
    if (x <= 0 || x >= SCREEN_WIDTH - 1){
        return 0;
    }
    // initial conditions
    if (t <= 1){
        return u_0(x);
    }
    // if coordinate is already in the map, then we simply return that.
    // if (knowns[2][x] != DEFAULT_VALUE){
    //     return knowns[2][x];
    // }
    // computes the solution
    // double output = 2*u(x,t-1, computed_vals) - u(x, t-2, computed_vals)
    //     + C*C*( u(x+1, t-1, computed_vals) - 2*u(x,t-1, computed_vals)+u(x-1, t-1, computed_vals) );
    double output = 2*output_map[1][x] - output_map[0][x] + C*C*( output_map[1][x+1] - 2*output_map[1][x] + output_map[1][x-1] );
    output_map[2][x] = output;
    // updates
    // if (t == 2 && x >=95 && x <= 105){
    //     std::cout << "is 2 : " << output_map[0][100] << std::endl;
    //     std::cout << "output at this time : " << 2*output_map[1][x] << std::endl;
    // }
    // std::cout << output_map[1][100] << "\n";
    

    return output;
}


int main(){

    // setup window
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 0xFF );
    SDL_RenderClear( renderer ); // SDL_RenderSetScale(renderer, .5, .5); // SDL_RenderPresent( renderer );

    bool quit = false;
    int t = 0;
    SDL_Event e;
    double displacement;
    
    // initialize output map
    output_type** output_map = new output_type*[3];
    for (int i = 0; i < 3; i++){
        output_map[i] = new output_type[SCREEN_WIDTH];
    }
    for (int x = 0; x < SCREEN_WIDTH; x++){
        output_map[0][x] = u_0(x);
        output_map[1][x] = u_0(x);
        output_map[2][x] = DEFAULT_VALUE;
        // if (x >=95 && x <= 105){
        //     std::cout << "test 1 : " << output_map[1][x] << std::endl;
        //     std::cout << "test 2 : " << u_0(x) << std::endl;
        // }
    }

    // computes the solution for each time t
    while (!quit && t < NUM_T_STEPS){

        while( SDL_PollEvent( &e ) != 0 ){
			//User requests quit
			if( e.type == SDL_QUIT )
			{
				quit = true;
			}
        }

        for (int x = 0; x < SCREEN_WIDTH; x++){
            SDL_SetRenderDrawColor( renderer, 255*5*t/NUM_T_STEPS, 255, 0, 255 );
            displacement = -1 * u(x, t, output_map);
            // std::cout << "displacement : " << displacement << ", and updated map : " << output_map[0][100] << "\n";
            SDL_RenderDrawPointF( renderer, x, SCREEN_HEIGHT/2 + displacement );
        }
        SDL_RenderPresent( renderer );


        // clear screen
        for (int y = 0; y < SCREEN_HEIGHT; y++){
            for (int x = 0; x < SCREEN_WIDTH; x++){
                SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
                SDL_RenderDrawPointF( renderer, x, y );
            }
        }
        SDL_RenderClear( renderer );


        // prepare output_map for next use (after t++ occurs)
        if (t >= 2) {
            for (int x = 0; x < SCREEN_WIDTH; x++){
                output_map[0][x] = output_map[1][x];
                output_map[1][x] = output_map[2][x];
                output_map[2][x] = DEFAULT_VALUE;
            }
        }
        t++;
    }

    for (int i = 0; i < 3; i++){
        delete[] output_map[i]; 
    }
}




// double gradient(int x, int t);
// double gradient(int x, int t){
//     u(x, t - DELTA_T);
// }

// double del_xx(int x, int t);
// double del_xx(int x, int t){
//     double output = u(x+DELTA_X, t-DELTA_T) - 2*u(x, t-DELTA_T) + u(x-DELTA_X, t-DELTA_T);
//     output /= DELTA_X*DELTA_X;
//     return output;
// }

// double del_tt(int x, int t);
// double del_tt(int x, int t){
//     double output = u(x, t+DELTA_T) - 2*u(x, t) + u(x, t-DELTA_T);
//     output /= DELTA_T*DELTA_T;
//     return output;
// }