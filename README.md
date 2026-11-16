# ft_irc

## Description

ft_irc is a C++ IRC server project inspired by classic chat services such as IRC networks. The goal of this project is to implement a functional server that can handle multiple clients simultaneously, manage user registration, create and manage channels, and process standard IRC commands in a real-time network environment.

This project was developed as part of the 42 curriculum and focuses on low-level socket programming, non-blocking I/O, protocol parsing, and robust C++ design. It demonstrates the ability to build a scalable server-side application from scratch while respecting the constraints of C++98 and Unix networking APIs.

## Features

### Core Server Features
- **Multi-client networking**: Handles several simultaneous client connections using sockets and polling
- **User registration flow**: Supports `PASS`, `NICK`, and `USER` commands with proper validation
- **Channel management**: Allows users to join channels, send messages, leave, invite others, and manage channel state
- **Message delivery**: Supports private messaging between users and broadcasting inside a channel
- **Protocol parsing**: Reads and interprets IRC commands from client input in a lightweight custom parser
- **Graceful shutdown**: Handles signal interruption and cleans up resources properly

### Implemented IRC Commands
- **Connection/registration**: `PASS`, `NICK`, `USER`, `CAP`, `PING`, `QUIT`
- **Channel operations**: `JOIN`, `PART`, `TOPIC`, `MODE`, `INVITE`, `KICK`
- **Messaging**: `PRIVMSG`
- **Information**: `WHO`

### Channel and User Management
- **Channel creation**: Automatically creates a channel when a user joins a new one
- **Operator handling**: Tracks channel operators and permission-based actions
- **Invite-only / password-protected channels**: Supports channel restrictions for access control
- **Topic management**: Allows setting and viewing a channel topic
- **Client tracking**: Kept in maps keyed by socket file descriptor and nickname for efficient lookup

## Technologies Used

- **Language**: C++98
- **Networking API**: POSIX sockets, `poll()`
- **Build System**: Makefile
- **Platform**: Linux
- **Protocol**: IRC

## Prerequisites

- Linux operating system
- GCC or equivalent C++ compiler
- Make
- Basic knowledge of IRC protocol behavior

## Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd ft_irc
   ```

2. Build the project:
   ```bash
   make
   ```

3. Run the server:
   ```bash
   ./ircserv <port> <password>
   ```

4. Connect to the server:
   We used HexChat for our test, but you can use the IRC client of your choice!

## Key Algorithms & Concepts

- **Socket programming**: Creation and management of TCP server sockets
- **Non-blocking event polling**: Use of `poll()` to monitor multiple client connections
- **Protocol parsing**: Extracting commands and arguments from raw client input
- **State management**: Tracking whether a client is authenticated, registered, and connected to channels
- **Data structures**: Maps and vectors for client/channel registry and message dispatch
- **Resource cleanup**: Proper memory and file descriptor management to avoid leaks
- **Concurrency awareness**: Managing multiple clients in a single server loop

## Learning Outcomes

This project demonstrates:
- Advanced C++ programming with object-oriented design
- Network programming fundamentals and client/server architecture
- Protocol implementation and parsing logic
- Efficient memory and resource management
- Practical use of maps, vectors, and custom data structures
- Event-driven server design with multiplexed I/O
- Understanding of IRC conventions and real-world chat system behavior
