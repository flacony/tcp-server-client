# TCP client and server in C++

This project includes a simple TCP client and server written in C++ using sockets in Linux.

The server listens to the specified port and writes messages received from clients to a file `log.txt` in the directory of the executable file (if there is no such file, it will be created, otherwise the data will be supplemented). Each connection is processed in a separate thread.

Clients connect to the server with the specified period and send text messages with the time and their name in the format `[yyyy-mm-dd hh:mm:ss.ms] "client_name"`.

## Requirements

To compile this project, you will need a gcc or g++ compiler installed.

## Compile and run

### Server

To compile the server, run the following command:

```bash
g++ server.cpp -o server
```

To start the server, use the command:

```bash
./server <port>
```

where `<port>` is the port number that the server will listen to. Make sure that the port is free and not being used by another application.

### Client

To compile the client, run the following command:

```bash
g++ client.cpp -o client
```

To start the client, use the command:

```bash
./client <client> <port> <frequency>
```

where `<client>` is the name of the client, `<port>` is the IP address of the server, and `<frequency>` is the frequency of connection to the server. Make sure that your command line arguments are correct and match the server settings.

## Shutdown

Both the server and the client complete their work by pressing the Ctrl+C key combination.


