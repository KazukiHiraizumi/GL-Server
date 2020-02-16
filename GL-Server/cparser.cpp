#include <mutex>
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <assert.h>
#include <queue>
#include <sstream>
#include <string>
#include <mutex>

std::queue<std::string> cparser_queue;
int cparser_busy=0;

void cparser(char* buffer) {
    cparser_busy = 1;
    std::stringstream ss;
    ss << buffer;
    std::string buf;
    while (std::getline(ss, buf)) {
        std::stringstream ss2;
        ss2 << buf;
        std::string buf2;
        while (std::getline(ss2, buf2, ';')) {
            std::stringstream ss3;
            ss3 << buf2;
            std::string buf3;
            while (std::getline(ss3, buf3, ' ')) {
                cparser_queue.push(buf3);
            }
        }
        if (!cparser_queue.empty()) cparser_queue.push(std::string("*"));
    }
    cparser_busy = 0;
}

/*
char* rcv = "W 1 2 3 4 5;B 1 2 3;C 1 2 \nB 1 0\n";

int main(int argc, char** argv) {
    set(rcv);
    while (!fifo.empty()) {
        std::cout << fifo.front() << "\n";
        fifo.pop();
    }
    return 0;
}
*/
