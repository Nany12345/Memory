all: Main

Main: Main.o global.o quadtree.o solution.o solutionsset.o tlistset.o ttreeset.o 
	g++ -std=c++11 -std=gnu++11 -o Main Main.o global.o quadtree.o solution.o solutionsset.o tlistset.o ttreeset.o 

Main.o: Main.cpp quadtree.h ttreeset.h mfront.h mfront2.h problem.h 
	g++ -std=c++11 -std=gnu++11 -c Main.cpp

global.o: global.cpp global.h
	g++ -std=c++11 -std=gnu++11 -c global.cpp

quadtree.o: quadtree.cpp quadtree.h
	g++ -std=c++11 -std=gnu++11 -c quadtree.cpp
	
solution.o: solution.cpp solution.h
	g++ -std=c++11 -std=gnu++11 -c solution.cpp

solutionsset.o: solutionsset.cpp solutionsset.h
	g++ -std=c++11 -std=gnu++11 -c solutionsset.cpp

tlistset.o: tlistset.cpp tlistset.h
	g++ -std=c++11 -std=gnu++11 -c tlistset.cpp

ttreeset.o: ttreeset.cpp ttreeset.h problem.h
	g++ -std=c++11 -std=gnu++11 -c ttreeset.cpp	
	
clean:
		rm *o Main