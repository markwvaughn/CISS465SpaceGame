all:	*.cpp
	g++ *.cpp `sdl-config --cflags --libs` -o test

e:	edit
edit:	*.cpp
	xemacs *.cpp &

r:	run
run:	test
	./test

c:	clean
clean:	test
	rm test *~

w:	wipe
wipe:
	rm test *~ makefile
