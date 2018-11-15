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

Bullet.o: Bullet.h Role.h Server.h Saucer.h
Client.o: Client.h ServerName.h Saucer.h Server.h Star.h
Explosion.o: Explosion.h
Hero.o: Bullet.h Explosion.h Hero.h Reticle.h Role.h Server.h util.h
Reticle.o: Reticle.h util.h
Role.o: Role.h Server.h
Saucer.o: /usr/include/stdlib.h /usr/include/features.h
Saucer.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
Saucer.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Saucer.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
Saucer.o: /usr/include/endian.h /usr/include/bits/endian.h
Saucer.o: /usr/include/bits/byteswap.h /usr/include/bits/types.h
Saucer.o: /usr/include/bits/typesizes.h /usr/include/bits/byteswap-16.h
Saucer.o: /usr/include/sys/types.h /usr/include/time.h
Saucer.o: /usr/include/sys/select.h /usr/include/bits/select.h
Saucer.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Saucer.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Saucer.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Explosion.h
Saucer.o: Role.h Server.h Saucer.h
Server.o: Role.h Server.h
ServerName.o: ServerName.h
Star.o: /usr/include/stdlib.h /usr/include/features.h
Star.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
Star.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
Star.o: /usr/include/bits/waitflags.h /usr/include/bits/waitstatus.h
Star.o: /usr/include/endian.h /usr/include/bits/endian.h
Star.o: /usr/include/bits/byteswap.h /usr/include/bits/types.h
Star.o: /usr/include/bits/typesizes.h /usr/include/bits/byteswap-16.h
Star.o: /usr/include/sys/types.h /usr/include/time.h
Star.o: /usr/include/sys/select.h /usr/include/bits/select.h
Star.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
Star.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
Star.o: /usr/include/alloca.h /usr/include/bits/stdlib-float.h Star.h
game.o: /usr/include/unistd.h /usr/include/features.h
game.o: /usr/include/stdc-predef.h /usr/include/sys/cdefs.h
game.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
game.o: /usr/include/bits/posix_opt.h /usr/include/bits/environments.h
game.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
game.o: /usr/include/bits/confname.h /usr/include/getopt.h
game.o: /usr/include/assert.h Client.h ServerName.h Reticle.h Role.h Server.h
game.o: Saucer.h Star.h
util.o: util.h
