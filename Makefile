

oneD:
	g++ 1D.cpp -o 1D --std=c++17 -I include -L lib -l SDL2-2.0.0 -l SDL2_image-2.0.0

twoD:
	g++ 2D.cpp -o 2D --std=c++17 -I include -L lib -l SDL2-2.0.0 -l SDL2_image-2.0.0