all: diff_test test

diff_test : diff_test.cpp ../diff.h
	g++ -ansi -Wall -I.. -o diff_test diff_test.cpp

test:
	./diff_test text1.txt text2.txt
