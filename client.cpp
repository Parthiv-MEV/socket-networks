#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in temp2;
    temp2.sin_family = AF_INET;
    temp2.sin_port = htons(6969);
    inet_pton(AF_INET, "127.0.0.1", &temp2.sin_addr);
    connect(s, (struct sockaddr *)&temp2, sizeof(temp2));
    char *msg = "Helllloooooooo";
    send(s, msg, strlen(msg), 0);

    char buf[100] = {0};
    recv(s, buf, sizeof(buf), 0);

    printf("Server is saying: %s\n", buf);

    close(s);
}