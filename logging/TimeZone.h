//
// Created by suncx on 2020/8/16.
//

#ifndef REACTOR_TIMEZONE_H
#define REACTOR_TIMEZONE_H

#include <ctime>
#include <memory>
#include <vector>
#include <string>
#include "NonCopyable.h"
#include "Exception.h"
#include "LogStream.h"

using std::shared_ptr;
using std::vector;
using std::string;
using std::logic_error;

const int seconds_per_day = 24 * 60 * 60;

struct Transition {
    time_t gm_time;
    time_t local_time;
    int local_time_idx;

    Transition(time_t gm, time_t local, int idx) : gm_time(gm), local_time(local),
                                                   local_time_idx(idx) {}
};

struct Comp {
    bool compare_gmt;

    explicit Comp(bool compare) : compare_gmt(compare) {}

    bool operator()(const Transition &lhs, const Transition &rhs) const {
        if (compare_gmt) return lhs.gm_time < rhs.gm_time;
        else return lhs.local_time < rhs.local_time;
    }

    bool equals(const Transition &lhs, const Transition &rhs) const {
        if (compare_gmt)return lhs.gm_time == rhs.gm_time;
        else return lhs.local_time == rhs.local_time;
    }
};

struct LocalTime {
    time_t gm_offset;
    bool is_dst;
    int arr_idx;

    LocalTime(time_t offset, bool dst, int idx) : gm_offset(offset), is_dst(dst), arr_idx(idx) {}
};

class File : public NonCopyable {
private:
    FILE *p_file;
public:
    explicit File(const char *filename) : p_file(fopen(filename, "r")) {}

    virtual ~File() {
        if (valid()) {
            auto re = fclose(p_file);
            if (unlikely(re != 0)) ERROR_EXIT("file close error.");
        }
    }

    bool valid() const { return p_file != nullptr; }

    string read(size_t n) const {
        char buf[n];
        auto n_read = fread(buf, 1, n, p_file);
        if (n_read != n) throw logic_error("no enough data!");
        return string(buf, n);
    }

    int32_t read_int32() {
        int32_t x = 0;
        auto n_read = fread(&x, 1, sizeof(x), p_file);
        if (n_read != sizeof(x)) throw logic_error("bad int data!");
        // 大端字节序换成本机字节序
        return be32toh(x);
    }

    byte read_byte() {
        byte x = 0;
        auto n_read = fread(&x, 1, sizeof(x), p_file);
        if (n_read != sizeof(x)) throw logic_error("bad int data!");
        // 大端字节序换成本机字节序
        return x;
    }

};

struct Data {
    vector<Transition> transitions;
    vector<LocalTime> local_times;
    vector<string> names;
    string abbreviation;    // 缩写
};

class TimeZone final : public NonCopyable {
private:

    friend bool read_timezone_file(const char *tz_file, Data *data);

    shared_ptr<Data> data;
public:

    explicit TimeZone(const char *zone_file);

    TimeZone(int east_of_utc, const char *tz_name);

    TimeZone() = default;

    bool valid() const;

    tm to_localtime(time_t seconds_since_epoch) const;

    time_t from_localtime(const tm &t) const;

    static tm to_utc_time(time_t seconds_since_epoch, bool yday = false);

    static time_t from_utc_time(const tm &t);

    static time_t from_utc_time(int year, int month, int day, int hour, int minute, int second);
};

/**
 * reference: https://man7.org/linux/man-pages/man5/tzfile.5.html
 * @param tz_file
 * @param data
 * @return
 */
bool read_timezone_file(const char *tz_file, Data *data) {
    File file(tz_file);
    if (!file.valid()) return false;

    // The magic four-byte ASCII sequence
    string header = file.read(4);
    if (header != "TZif") throw logic_error("bad head.");
    string version = file.read(1);  // 2 or 3
    file.read(15);  // 15 个字节保留以供未来使用

    // 6 个 4 字节整数值
    int32_t tzh_ttisutcnt = file.read_int32();
    int32_t tzh_ttisstdcnt = file.read_int32();
    int32_t tzh_leapcnt = file.read_int32();
    int32_t tzh_timecnt = file.read_int32();//29
    int32_t tzh_typecnt = file.read_int32();//3
    int32_t tzh_charcnt = file.read_int32();//12

    vector<int32_t> trans;
    trans.reserve(tzh_timecnt);
    for (int i = 0; i < tzh_timecnt; ++i) trans.emplace_back(file.read_int32());

    vector<int> local_times;
    local_times.reserve(tzh_timecnt);
    for (int i = 0; i < tzh_timecnt; ++i)
        local_times.emplace_back(file.read_byte());

    for (int i = 0; i < tzh_typecnt; ++i) {
        int32_t gmtoff = file.read_int32();
        byte isdst = file.read_byte();
        byte ab = file.read_byte();
        data->local_times.emplace_back(gmtoff, isdst, ab);
    }

    for (int i = 0; i < tzh_timecnt; ++i) {
        auto local_idx = local_times[i];
        time_t local_time = trans[i] + data->local_times[local_idx].gm_offset;
        data->transitions.emplace_back(trans[i], local_time, local_idx);
    }
    data->abbreviation = file.read(tzh_charcnt);
    return true;
}

#endif //REACTOR_TIMEZONE_H
