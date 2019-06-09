#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <stdio.h>
#include <zconf.h>
#include <sys/un.h>
#include <unistd.h>


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


    struct sockaddr_un client{};
    client.sun_family = AF_UNIX;
    memcpy(client.sun_path, SOCKET_NAME.c_str(), SOCKET_NAME.size());
    check_error(connect(file_descriptor, (sockaddr *) (&client), sizeof(client)), "connect");


    struct msghdr msg{};
    bzero(buffer, BUFFER_SIZE);
    struct iovec IO{};
    IO.iov_base = &data;
    IO.iov_len = sizeof(data);

    msg.msg_iov = &IO;
    msg.msg_iovlen = 1;
    msg.msg_control = buffer;
    msg.msg_controllen = sizeof(buffer);

    check_error(recvmsg(file_descriptor, &msg, 0), "recvmsg");

    struct cmsghdr* cmsg = CMSG_FIRSTHDR(&msg);


    string message;
    cout << ":q to quit" << endl;

    dup2(*CMSG_DATA(cmsg), 1);
    while (true) {
        cerr << "Enter message ";
        getline(cin, message);
        if (message == ":q") {
            return 0;
        }
        cout << message << endl;
    }

}