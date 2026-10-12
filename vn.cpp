#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

bool receive(int fd, int* data, size_t n) {
    while (n > 0) {
        int k = recv(fd, data, n, 0);
        if (k <= 0) return false;
        data += k;
        n -= k;
    }
    return true;
}

bool send(int fd, const int* data, size_t n) {
    while (n > 0) {
        int k = send(fd, data, n, 0);
        if (k <= 0) return false;
        data += k;
        n -= k;
    }
    return true;
}

int get16(const int* p) {
    return (int(p[0]) << 8) | p[1];
}

int main(int argc, char** argv) {
    if (argc != 4) {
        cout << "Usage: ./vn <oracle-ip> <my-ip> <my-udp-port>\n";
        return 1;
    }

    const char* oracle_ip = argv[1];
    const char* my_ip = argv[2];
    int udp_port = std::stoi(argv[3]);

    sockaddr_in self{};
    self.sin_family = AF_INET;
    self.sin_port = htons(udp_port);
    if (inet_pton(AF_INET, my_ip, &self.sin_addr) != 1) return 1;

    int tcp = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in oracle{};
    oracle.sin_family = AF_INET;
    oracle.sin_port = htons(5000);
    inet_pton(AF_INET, oracle_ip, &oracle.sin_addr);
    connect(tcp, (sockaddr*)&oracle, sizeof(oracle));

    int msg[9] = {0, 7, 1};
    std::memcpy(msg + 3, &self.sin_addr, 4);
    std::memcpy(msg + 7, &self.sin_port, 2);
    if (!send(tcp, msg, sizeof(msg))) return 1;
    std::cout << "Connected, waiting for oracle to talk to me..." << std::endl;

    while (true) {
        int header[2];
        if (!receive(tcp, header, 2)) break;
        int length = get16(header);

        std::vector<int> body(length);
        if (!receive(tcp, body.data(), length)) break;

        int count = get16(body.data() + 1);

        std::cout << "LINK_STATE: ";
        for (int i = 0; i < count; ++i) {
            size_t p = 3 + 9 * i;
            char node = char(body[p]);
            int cost = get16(body.data() + p + 7);
            if (i) std::cout << ", ";
            std::cout << node << "=" << cost;
        }
        std::cout << std::endl;
    }

    std::cout << "Oracle gone goo goo ga ga\n";
    close(tcp);
}