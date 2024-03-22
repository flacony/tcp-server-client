#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>

class ClientTCP {
private:
  std::string _name;
  size_t _frequency;
  struct sockaddr_in _addr;
  int _sock;

  std::string _getTimeStr() {
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);

    auto now = std::chrono::system_clock::now();
    char dateTime[21];
    std::strftime(dateTime, sizeof(dateTime), "[%Y-%m-%d %H:%M:%S", &tm);

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
                            now.time_since_epoch())
                        % 1000;

    char msStr[7];
    snprintf(msStr, sizeof(msStr), ".%03d] ",
             static_cast<int>(milliseconds.count()));

    std::string result = dateTime;
    result += msStr;
    return result;
  }

  void _sendMessage() {
    std::string message = _getTimeStr();
    message += _name;
    if (send(_sock, message.c_str(), message.size(), 0) <= 0) {
      perror("Sending message failure");
      exit(EXIT_FAILURE);
    }
  }

public:
  ClientTCP(const char* name, int port, int frequency)
      : _name(name), _frequency(frequency) {
    _addr.sin_addr.s_addr = htonl(INADDR_ANY);
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(port);
  }

  void connectToServer() {
    while (true) {
      if ((_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket failure");
        exit(EXIT_FAILURE);
      }

      if (connect(_sock, (struct sockaddr*)&_addr, sizeof(_addr)) < 0) {
        perror("Connection failure");
        exit(EXIT_FAILURE);
      }

      _sendMessage();
      close(_sock);
      std::this_thread::sleep_for(std::chrono::seconds(_frequency));
    }
  }

  ~ClientTCP() { close(_sock); }
};

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0]
              << " <client_name> <server_port> <connection_frequency>"
              << std::endl;
    return EXIT_FAILURE;
  }

  char* name = argv[1];
  int port = std::stoi(argv[2]);
  int frequency = std::stoi(argv[3]);

  if (port < 1 || frequency < 1) {
    std::cerr << "Error: Invalid arguments. Port number and frequency must be "
                 "positive integers."
              << std::endl;
    return EXIT_FAILURE;
  }

  ClientTCP client(name, port, frequency);
  client.connectToServer();

  return 0;
}
