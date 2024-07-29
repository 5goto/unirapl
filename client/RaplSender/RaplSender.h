#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


class RaplSender {
    private:
        int clientSocket;
        struct sockaddr_in serverAddress;

        RaplSender();

    public:
        ~RaplSender();

        static RaplSender* get_instance() {
    			if (instance == nullptr) {
    				instance = new RaplSender();
    			}
                return instance;
            }

        static RaplSender* instance;

        std::string request(std::string);
};
