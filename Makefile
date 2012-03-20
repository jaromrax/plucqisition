all: twoplugins


twoplugins: plug_analyze.so plug_queue.so

plug_queue.so:  plug_queue.o
	gcc -shared -o plug_queue.so plug_queue.o

plug_queue.o: plug_queue.cpp
	g++ -fPIC -c plug_queue.cpp `root-config --libs --cflags --glibs`



plug_analyze.so:	  plug_analyze.o
	gcc -shared -o plug_analyze.so plug_analyze.o

plug_analyze.o: plug_analyze.cpp 
	g++ -fPIC -c plug_analyze.cpp `root-config --libs --cflags --glibs`


clean:
	  rm plug_analyze.so  plug_queue.so plug_analyze.o plug_queue.o

##	g++ -fPIC -c plug_queue.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_queue.so plug_queue.o
##	g++ -fPIC -c plug_analyze.cpp `root-config --libs --cflags --glibs`  &&  gcc -shared -o plug_analyze.so plug_analyze.o
