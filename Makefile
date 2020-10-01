#
# Makefile for "Hello, Network" using Dragonfly
#
# Copyright Mark Claypool and WPI, 2018
#
# 'make depend' to generate new dependency list
# 'make clean' to remove all constructed files
# 'make' to build executable
#

CC= g++ 

### Uncomment only 1 of the below! ###

# 1) Uncomment below for Linux (64-bit)
LINKLIB= -ldragonfly -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -lrt
LINKDIR= -L../../dragonfly/ # path to dragonfly library
INCDIR= -I../../dragonfly/ # path to dragonfly includes
LIB= ../../dragonfly/libdragonfly.a
LINKDIR:= $(LINKDIR) -L$(HOME)/src/SFML/lib 
INCDIR:= $(INCDIR) -I$(HOME)/src/SFML/include

# 2) Uncomment below for Mac (64-bit)
#LINKLIB= -ldragonfly-mac64 -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio 
#LINKDIR= -L/usr/local/Cellar/sfml/2.3_1/lib/ -L../dragonfly/lib/ 
#INCDIR= -I/usr/local/Cellar/sfml/2.3_1/include/ -I../dragonfly/include/

######

#CFLAGS := -g
#DEBUG := $(DEBUG)

GAMESRC= \
        Bullet.cpp \
        Client.cpp \
        Explosion.cpp \
        GameOver.cpp \
        Hero.cpp \
        Points.cpp \
        Reticle.cpp \
        Role.cpp \
        Saucer.cpp \
        Server.cpp \
        ServerName.cpp \
	Star.cpp \
	util.cpp \

GAME= game.cpp
EXECUTABLE= game
OBJECTS= $(GAMESRC:.cpp=.o)

all: $(EXECUTABLE) Makefile $(LIB)

$(EXECUTABLE): $(ENGINE) $(OBJECTS) $(GAME) $(GAMESRC) $(LIB)
	$(CC) $(GAME) $(CFLAGS) $(OBJECTS) $(DEBUG) -o $@ $(INCDIR) $(LINKDIR) $(LINKLIB)

.cpp.o: 
	$(CC) -c $(INCDIR) $(CFLAGS) $(DEBUG) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE) core Makefile.bak *.log *~

depend: 
	makedepend *.cpp 2> /dev/null

# DO NOT DELETE

Bullet.o: Bullet.h Role.h Client.h ServerName.h Server.h Saucer.h
Client.o: Bullet.h Client.h ServerName.h Hero.h Reticle.h Points.h Saucer.h
Client.o: Server.h Star.h
Explosion.o: Explosion.h
GameOver.o: GameOver.h
Hero.o: Bullet.h Explosion.h GameOver.h Hero.h Reticle.h Role.h Client.h
Hero.o: ServerName.h Server.h util.h
Points.o: Points.h Role.h Client.h ServerName.h Server.h
Reticle.o: Reticle.h util.h
Role.o: Role.h Client.h ServerName.h Server.h
Saucer.o: /usr/include/stdlib.h Explosion.h Points.h Role.h Client.h
Saucer.o: ServerName.h Server.h Saucer.h
Server.o: Hero.h Reticle.h Points.h Role.h Client.h ServerName.h Server.h
Server.o: Saucer.h
ServerName.o: ServerName.h
Star.o: /usr/include/stdlib.h Star.h
game.o: /usr/include/unistd.h /usr/include/features.h
game.o: /usr/include/stdc-predef.h /usr/include/assert.h Client.h
game.o: ServerName.h Role.h Server.h Saucer.h Star.h
util.o: util.h
