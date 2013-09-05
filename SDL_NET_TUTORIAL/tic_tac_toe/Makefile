# Automatically generated Makefile for project: tic_tac_toe
# Created on  by gaten for sdltutorials.com
# Comments, suggestions and bugs email: gaten DOT net AT gmail DOT com

# Compiler
CC=g++

# Objects
OBJECTS=CApp.o CApp_OnCleanup.o CApp_OnEvent.o CApp_OnInit.o CApp_OnLoop.o CApp_OnRender.o CEvent.o CSurface.o

# Compiler flags
CPPFLAGS=-Wall -g `sdl-config --cflags`

# Linker flags
LDFLAGS=`sdl-config --libs` -lSDL -lSDL_image

main: CApp.cpp
	$(CC) $(CPPFLAGS) -c CApp.cpp CApp_OnCleanup.cpp CApp_OnEvent.cpp CApp_OnInit.cpp CApp_OnLoop.cpp CApp_OnRender.cpp CEvent.cpp CSurface.cpp
	$(CC) $(OBJECTS) $(CPPFLAGS) $(LDFLAGS) -o tic_tac_toe

# cleanup
clean:
	rm -rf $(OBJECTS)

# EOF
