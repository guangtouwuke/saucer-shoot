# README

This is a "Hello, World" networking program for Dragonfly.

It uses a classic "authoritative server" implementation.  The server's
game state is the final word.

Only the Server spawns game objects.

The Client gathers user input but doesn't act upon it. Instead, it
Sends all user input to the Server.

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

Stars - synchronized when the client first connects.  They are never
synchronized after that since they are strictly decorative.

Saucers - synchronized when the client first connects.  Also, when
created, destroyed or moved to start.

Heroes - synchronized when created, moved or destroyed.

-------------------------------------

Have fun!

-- Mark

