//
// Created by suncx on 2020/8/19.
//

#ifndef REACTOR_BUFFER_H
#define REACTOR_BUFFER_H

#include <string>

namespace reactor::net {
    using std::string;
    using byte = unsigned char;

    class Buffer final {
    private:
        byte *buf;
        size_t read_idx;
        size_t write_idx;
        size_t capacity;
        static const char CRLF[];
        static const int RESERVED_SIZE; // 预留空间大小
        static const int INITIAL_SIZE;  // 默认初始大小（不包括预留空间）

        /**
         * 确保缓冲区可写入 n 字节数据
         * @param n
         */
        void ensure_writable(size_t n);

    public:
        explicit Buffer(size_t init_size = INITIAL_SIZE);

        ~Buffer();

        /**
         * 返回可读字节的长度
         * @return 可读字节的长度
         */
        [[nodiscard]]
        size_t readable_bytes() const;

        /**
         * 返回当前可写字节的长度，当扩容后可写字节的长度也相应增大
         * @return 当前可写字节的长度
         */
        [[nodiscard]]
        size_t writable_bytes() const;

        /**
         * 返回当前预留区的大小
         * @return 预留区大小
         */
        [[nodiscard]]
        size_t reserved_bytes() const;

        /**
         * 返回缓冲区的起始地址
         * @return 缓冲区起始地址
         */
        [[nodiscard]]
        const byte *begin() const;

        /**
         * 返回缓冲区的起始地址
         * @return 缓冲区起始地址
         */
        byte *begin();

        /**
         * 返回缓冲区的可写地址
         * @return 缓冲区可写地址
         */
        [[nodiscard]]
        const byte *begin_write() const;

        /**
         * 返回缓冲区的可写地址
         * @return 缓冲区可写地址
         */
        byte *begin_write();

        /**
         * 返回缓冲区可偷窥数据的起始地址
         * @return 偷窥缓冲区数据的起始地址
         */
        [[nodiscard]]
        const byte *peek() const;

        /**
         * 返回缓冲区中回车换行符的地址
         * @return 缓冲区中回车换行符的地址
         */
        [[nodiscard]]
        const byte *find_crlf() const;

        /**
         * 返回缓冲区中以 start 位置开始的回车换行符的地址
         * @param start 查找的起始地址
         * @return 缓冲区中从 start 位置开始的回车换行符的地址
         */
        const byte *find_crlf(const byte *start) const;

        /**
         * 返回缓冲区中换行符的地址
         * @return 缓冲区中换行符的地址
         */
        [[nodiscard]]
        const byte *find_eol() const;

        /**
         * 返回缓冲区中以 start 位置开始的换行符的地址
         * @param start 查找的起始地址
         * @return 缓冲区中以 start 位置开始的换行符的地址
         */
        const byte *find_eol(const byte *start) const;

        /**
         * 从缓冲区中读取 n 个字节的数据
         * @param n 要读取字节的长度
         */
        void retrieve(size_t n);

        /**
         * 读取缓冲区中所有可读数据
         */
        void retrieve_all();

        /**
         * 从缓冲区读取一直到 end 地址的数据
         * @param end 读取数据的 end 地址
         */
        void retrieve_until(const byte *end);

        void retrieve_64();

        void retrieve_32();

        void retrieve_16();

        void retrieve_8();

        /**
         * 读取 n 字节数据，并将数据以字符串返回
         * @param n 读取数据的字节数
         * @return 大小为 n 的字符串
         */
        string retrieve_string(size_t n);

        /**
         * 读取所有可读数据，并将数据以字符串返回
         * @return 字符串
         */
        string retrieve_all_string();

        /**
         * 向缓冲区中追加以 data 地址开始，大小为 n 字节的数据
         * @param data 待追加数据的起始地址
         * @param n 追加数据的字节长度
         */
        void append(const byte *data, size_t n);

        void append(const void *data, size_t n);

        void append(const string &msg);

        ssize_t read_from_fd(int fd, int *err_no);
    };
}

#endif //REACTOR_BUFFER_H