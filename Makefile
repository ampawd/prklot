PROGNAME	= parkingLotApp

CXX			= g++

SRC			= main.cpp \
		parking_car.cpp \
		parking_lot.cpp \
		shader.cpp \
		shader_manager.cpp


OBJS		= main.o \
		parking_car.o \
		parking_lot.o \
		shader.o \
		shader_manager.o \


BUILDIR 	= build

# use this for non standard (other than from /usr/include) include directories
# -I/some/path -I/some/path2
# INCDIR		= 

# use this for non standard (other than from /usr/lib) library directories 
# -L/some/path -L/some/path2
# LIBDIR		= 

CXXFLAGS	= -Wall -c -std=c++11

LDFLAGS		= -Wall

LIBS		= -lGL -lGLEW -lglfw


$(PROGNAME): $(OBJS)
	$(CXX) $(BUILDIR)/*.o $(LDFLAGS) $(LIBS) -o $(BUILDIR)/$(PROGNAME) 

# $(OBJS): 
# 	$(CXX) $(CXXFLAGS) $(INCDIR) $(SRC) -o $(patsubst %, $(BUILDIR)/%, $(OBJS))


main.o: 
	$(CXX) $(CXXFLAGS) $(INCDIR) main.cpp -o $(BUILDIR)/main.o

parking_car.o: 
	$(CXX) $(CXXFLAGS) $(INCDIR) parking_car.cpp -o $(BUILDIR)/parking_car.o

parking_lot.o: 
	$(CXX) $(CXXFLAGS) $(INCDIR) parking_lot.cpp -o $(BUILDIR)/parking_lot.o

shader.o: 
	$(CXX) $(CXXFLAGS) $(INCDIR) shader.cpp -o $(BUILDIR)/shader.o

shader_manager.o: 
	$(CXX) $(CXXFLAGS) $(INCDIR) shader_manager.cpp -o $(BUILDIR)/shader_manager.o


.PHONY: clean

clean:
	rm  $(BUILDIR)/*.o $(BUILDIR)/$(PROGNAME)

