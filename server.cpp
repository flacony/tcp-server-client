#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

std::mutex logMutex;

class ServerTCP {
private:
  int _listener;
  struct sockaddr_in _addr;
  int _port;
  std::ofstream _logFile;

  void _handleClient(int client) {
    char buff[512] = {0};

    while (true) {
      if (recv(client, buff, sizeof(buff), 0) <= 0) {
        break;
      }

      logMutex.lock();
      _logFile << buff << std::endl;
      logMutex.unlock();
    }

    close(client);
  }

public:
  ServerTCP(int port) : _port(port) {}

  void start() {
    if ((_listener = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      perror("Socket failure");
      exit(EXIT_FAILURE);
    }

    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);

    if (bind(_listener, (struct sockaddr*)&_addr, sizeof(_addr)) < 0) {
      perror("Binding failure");
      exit(EXIT_FAILURE);
    }

    if (listen(_listener, 42) < 0) {
      perror("Listening failure");
      exit(EXIT_FAILURE);
    }

    _logFile.open("log.txt", std::ios::app);
    if (!_logFile) {
      std::cerr << "Error: Unable to open log.txt file for appending"
                << std::endl;
      exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << _port << std::endl;

    while (true) {
      int client;
      if ((client = accept(_listener, NULL, NULL)) < 0) {
        perror("Connection failure");
        continue;
      }
      std::thread clientThread(&ServerTCP::_handleClient, this, client);
      clientThread.detach();
    }
  }

  ~ServerTCP() { _logFile.close(); }
};

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <port>";
    return EXIT_FAILURE;
  }

  int port = std::stoi(argv[1]);

  if (port < 1) {
    std::cerr
        << "Error: Invalid port number. Port number must be a positive integer."
        << std::endl;
    return EXIT_FAILURE;
  }

  ServerTCP server(port);
  server.start();

  return 0;
}
