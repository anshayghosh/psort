all: psort mkwords showwords

psort: psort.o helper.o
	gcc -Wall -g -o psort psort.o helper.o

psort.o: psort.c helper.h
	gcc -Wall -g -c psort.c

showwords: showwords.o helper.o
	gcc -Wall -g -o showwords showwords.o helper.o

showwords.o: showwords.c helper.h
	gcc -Wall -g -c showwords.c

mkwords: mkwords.o helper.o
	gcc -Wall -g -o mkwords mkwords.o helper.o -lm

mkwords.o: mkwords.c helper.h
	gcc -Wall -g -c mkwords.c

helper.o: helper.c helper.h
	gcc -Wall -g -c helper.c

clean:
	rm psort.o
	rm showwords.o
	rm mkwords.o
	rm helper.o
	rm psort
	rm showwords
	rm mkwords