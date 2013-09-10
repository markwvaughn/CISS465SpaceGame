all:	*.cpp
	g++ *.cpp `sdl-config --cflags --libs` -o test.exe

e:	edit
edit:	*.cpp
	xemacs *.cpp &

r:	run
run:	test.exe
	./test.exe

c:	clean
clean:	test.exe
	rm test.exe *~

w:	wipe
wipe:
	rm test.exe *~ makefile
