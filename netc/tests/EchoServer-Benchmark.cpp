//
// Created by 孙诚雄 on 2021/7/13.
//

#include <sys/socket.h>
#include <netinet/in.h>
#include <cerrno>
#include <vector>
#include <arpa/inet.h>
#include <memory>
#include "Thread.h"
#include "ConsoleStream.h"
#include <cassert>

#ifdef __linux__

#include <cstring>
#include <unistd.h>

#endif

using reactor::core::Thread;
using reactor::core::CurrentThread;
using std::vector;
using std::unique_ptr;

void user_request() {

    int fd = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (fd < 0) RC_FATAL << "socket create error: " << ::strerror(errno);

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = ::inet_addr("192.168.2.2");
    server_addr.sin_port = htons(8080);

    int ret = ::connect(fd, reinterpret_cast<const sockaddr *>(&server_addr), sizeof(server_addr));
    if (ret < 0) {
        RC_ERROR << "connect error: " << ::strerror(errno);
        ::close(fd);
        return;
    }

    const char *msg = "hello world";
    size_t len = strlen(msg);
    ssize_t n = ::write(fd, msg, len);
    assert(n == len);

    // char buf[32];
    // n = ::read(fd, buf, sizeof(buf));
    // assert(n == len);
    // assert(strncmp(msg, buf, len) == 0);
    //
    // CurrentThread::sleep(6 * 1000);

    ::abort();

    // ret = ::close(fd);
    // if (ret < 0)
    //     RC_FATAL << "close fd(" << fd << ") error: " << ::strerror(errno);
}

int main(int argc, const char *argv[]) {

    const size_t CNT = atoi(argv[1]);

    vector<unique_ptr<Thread>> threads;
    threads.reserve(CNT);

    for (int i = 0; i < CNT; ++i) {
        threads.emplace_back(new Thread(user_request));
        threads.back()->start();
    }

    for (int i = 0; i < CNT; ++i) {
        threads[i]->join();
    }

    return 0;
}