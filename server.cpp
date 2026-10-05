#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
    int s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in temp1;
    temp1.sin_family = AF_INET;
    temp1.sin_port = htons(6969);
    temp1.sin_addr.s_addr = INADDR_ANY;

    bind(s, (struct sockaddr *)&temp1, sizeof(temp1));
    listen(s, 3);
    int client = accept(s, NULL, NULL);
    char buf[100] = {0};
    recv(client, buf, sizeof(buf), 0);
    printf("Client is saying: %s\n", buf);
    char *msg = "Hiiiiiiiii";
    send(client, msg, strlen(msg), 0);

    close(client);
    close(s);
}