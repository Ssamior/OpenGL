
# DOC :
#
# $@  	Le nom de la cible
# $< 	Le nom de la première dépendance
# $^ 	La liste des dépendances
# $? 	La liste des dépendances plus récentes que la cible
# $* 	Le nom du fichier sans suffixe



# compiler
CC=g++

# compilation flags/dir/marcro
CFLAGS=-Wall -O2
INC_DIR=-I /usr/include -I /usr/include/bullet 
MACROS=-DUNICODE -D_UNICODE -Dlinux -DLINUX

# link flags/dir/lib
LFLAGS=-Wall
LIB_DIR=-L /usr/lib/x86_64-linux-gnu/
#  -L /lib/lib/Bullet
LIBS=-lglfw -lassimp -lIL -lILU -lGLEW -lGL -lBulletDynamics -lBulletCollision -lLinearMath
# -lGLU -lX11 -lDevIL



#CPPLST=$(wildcard ./src/*.cpp)
#OBJLST1=$(CPPLST:.cpp=.o)
#OBJLST2=$(OBJLST1:/src/=/build/linux/)

SRC_DIR=./src/
#BLD_DIR=./build/linux/
EXE_DIR=./bin/

MYSRC  := $(wildcard ./src/*.cpp ./src/3d/*.cpp)
MYOBJ   = $(MYSRC:.cpp=.o)


################# build .o ##################
.SUFFIXES: .cpp

.cpp.o:
	$(CC) $(CFLAGS) $(MACROS) $(INC_DIR)  -c $<   -o $@
################# build .o ##################


################# build exe ##################
MYTARGET=gameLinux.exe

$(MYTARGET): $(MYOBJ)
	echo LINK...
	#$(CC) $(LFLAGS) $(LIB_DIR) $(LIBS) $(MYOBJ) -o $(EXE_DIR)$(MYTARGET)
	$(CC) $(MYOBJ) $(LFLAGS) $(LIB_DIR) $(LIBS) -o $(EXE_DIR)$(MYTARGET)


default all: $(MYTARGET)
################# build exe ##################


################### clean ####################
clean:
	rm -rf $(MYOBJ) $(EXE_DIR)$(MYTARGET)
################### clean ####################


################ downloadlib #################
downloadlib:
	sudo apt-get install -y libglfw3 libglfw3-dev 
	sudo apt-get install -y libassimp4 libassimp-dev assimp-utils 
	sudo apt-get install -y libglm-dev 
	sudo apt-get install -y libbullet-dev libbullet2.87 
	sudo apt-get install -y libdevil-dev libdevil1c2 
	sudo apt-get install -y libglew-dev libglew2.0
################ downloadlib #################


################### debug ####################
debug:
	echo libs:     $(LIBS)
	echo libdir:   $(LIB_DIR)
	echo 
	echo libs:     $(MACROS)
	echo incdir:   $(INC_DIR)
	echo 
	echo mysrc:    $(MYSRC)
	echo myobj:    $(MYOBJ)
	echo 
	echo mytarget: $(MYTARGET)
	echo blddir:   $(BLD_DIR)
	echo compilo:  $(CC)
	echo 
	echo lflags:   $(LFLAGS)
	echo cflags:   $(CFLAGS)
################### debug ####################

