#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <zconf.h>

using namespace std;


const int BUFFER_SIZE = 1024;
const string SOCKET_NAME = "/tmp/os_socket.socket";

char buffer[CMSG_SPACE(sizeof(int))];
char data[BUFFER_SIZE];

void check_error(int value, const char* message) {
    if (value == -1) {
        perror(message);
        exit(EXIT_FAILURE);
    }
}




int main() {
    int file_descriptor = socket(AF_UNIX, SOCK_STREAM, 0);
    check_error(file_descriptor, "socket");
    unlink(SOCKET_NAME.c_str());

    struct sockaddr_un server{};
    server.sun_family = AF_UNIX;
    memcpy(server.sun_path, SOCKET_NAME.c_str(), SOCKET_NAME.size());

    check_error((bind(file_descriptor, (sockaddr *) (&server), sizeof(server))), "bind");
    check_error(listen(file_descriptor, SOMAXCONN), "listen");

    int client_descriptor;
    while(true) {
        client_descriptor = accept(file_descriptor, nullptr, nullptr);
        check_error(client_descriptor, "accept");

        struct msghdr msg{};
        struct cmsghdr *cmsg;
        bzero(buffer, BUFFER_SIZE);
        struct iovec IO{};
        IO.iov_base = &data;
        IO.iov_len = sizeof(data);

        msg.msg_iov = &IO;
        msg.msg_iovlen = 1;
        msg.msg_control = buffer;
        msg.msg_controllen = sizeof (buffer);

        cmsg = CMSG_FIRSTHDR(&msg);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        cmsg->cmsg_len = CMSG_LEN(sizeof(int));


        check_error(sendmsg(client_descriptor, &msg, 0), "sendmsg");
    }
} 