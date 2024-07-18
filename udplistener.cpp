// got some starter code from gpt

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <sstream>
#include <vector>

#define PORT 2390
#define BUFFER_SIZE 1024

#include <rrd.h>

const char *rrdfile = "garagetemp.rrd";

void updateMyRRD(std::string);

int main()
{
    int sockfd;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);

    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        std::cerr << "Socket creation failed" << std::endl;
        return -1;
    }

    // Filling server information
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        close(sockfd);
        return -1;
    }

    std::cout << "Listening for UDP packets on port " << PORT << "..." << std::endl;

    while (true)
    {
        int n = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0)
        {
            std::cerr << "Receive failed" << std::endl;
            close(sockfd);
            return -1;
        }

        buffer[n] = '\0';
        // std::cout << "Received packet: " << buffer << std::endl;

        std::string packetString = buffer;

        updateMyRRD(packetString);
    }

    close(sockfd);
    return 0;
}

std::vector<std::string> split(const std::string &str, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }

    return tokens;
}

void updateMyRRD(std::string mystring)
{
    std::cout << mystring << std::endl;
    std::vector<std::string> mylist = split(mystring, ' ');

    // for (std::string token : mylist)
    // {
    //     std::cout << token << std::endl;
    // }

    try
    {
        double temperature = std::stod(mylist[0]);
        double humidity = std::stod(mylist[1]);
        double heatindex = std::stod(mylist[2]);

        // time_t now = time(nullptr);

        char update_command[128];
        snprintf(update_command, sizeof(update_command),
            "N:%.2f:%.2f:%.2f", temperature, humidity, heatindex);

        const char* argv[] = {
            "update", // not used
            rrdfile,
            update_command
        };

        int argc = sizeof(argv) / sizeof(argv[0]);

        int rrd_status = rrd_update(argc, (char**)argv);

    }
    catch (const std::invalid_argument &e)
    {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
        return;
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Out of range: " << e.what() << std::endl;
        return;
    }
}