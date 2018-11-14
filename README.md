# README

This is a 2-player networking game for Dragonfly.

It uses a classic "authoritative server" implementation.  The server's
game state is the final word.

Only the Server spawns game objects.

The Client gathers user input but doesn't act upon it. Instead, it
Sends all user input to the Server.

## To Run

Build, making sure to link in Dragonfly version 4.5 (or later).

Start up Server:

> game -s (.\game.exe on Windows)

Start up Client:

> game -c (.\game.exe on Windows)

Note: if running from inside Visual Studio, command line arguments can
be set in the Solution properties.


## Features

+ Provides a textbox for the client to enter the server server name.
+ Connects client to server using the NetworkManager.
+ Provides Server and Client classes for respective functions.
+ Provides a Role singleton to provide for context of game code.
+ Does basic synchronization of game objects.
+ Sends input from player on client to server.

## Flow

### Connection

Server

+ main() creates Server
+ main() populates world
+ when Client connects, starts synchronizing Objects

Client

+ main() creates Client
+ Client creates ServerName (a TextEntry)
+ Client waits until ServerName inactive
+ Client gets Server name from ServerName 
+ Client connects to Server
+ Client receives serialized Objects

### Synchronization

Server: generates for: Stars, Saucers, Heroes and Bullets.  Resolves
all collisions for Saucers, Heroes and Bullets.  When Hero dies,
creates GameOver object.  When GameOver terminates, game ends.

Client: keeps Saucers, Heroes and Bullets as SPECTRAL.  Sends all
input (mouse and keyboard) to Server. 

Stars - synchronized when the client first connects.  They are never
synchronized after that since they are strictly decorative.

Saucers - synchronized when the client first connects.  Also, when
created, destroyed or moved to start.

Heroes - synchronized when created, moved or destroyed.

GameOver - not synchronized. Created as needed on each node.

Explosion - not synchronized. Created as needed on each node.

-------------------------------------

Have fun!

-- Mark

