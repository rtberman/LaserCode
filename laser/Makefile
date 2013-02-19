#
# first define target file : laser
#              dependencies are the object files that build the program

laser: RaspberryPi.o laser.o
	g++ -o laser -l rt RaspberryPi.o laser.o -l bcm2835

# now define how each object file is a target and list dependencies and how
#  to build that object file if any dependencies change

RaspberryPi.o: RaspberryPi.h RaspberryPi.cpp
	g++ -g -c RaspberryPi.cpp

laser.o: RaspberryPi.h RaspberryPi.cpp laser.cpp
	g++ -g -c laser.cpp

clean:
	rm laser RaspberryPi.o laser.o

# that's all


