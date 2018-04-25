all: orbit

orbit: orbit.cpp
	g++ orbit.cpp libggfonts.a -Wall -lX11 -lGL -lGLU -lXext -lm -lrt -o orbit #-D UNIT_TEST -D ALEX_UNIT_TEST

clean:
	rm -f orbit

