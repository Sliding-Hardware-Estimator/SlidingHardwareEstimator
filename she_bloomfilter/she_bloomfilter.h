
//SHE-BF
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <bitset>
#include <time.h>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "../BOBHash32.h"
using namespace std;

class she_bloomfilter
{
public:
    static const int
        MAX_CELL_NUM = 1e7,
        MAX_HASH_NUM = 50;
    int
        window, //window size
        memory,
        hash_num, //number of hash function
        max_cell_num,
        group_size;
    long long relax_window_size;
    double alpha;
    BOBHash32 hash[MAX_HASH_NUM];
    BOBHash32 hash_time_offset;
    bool time_stamp[MAX_CELL_NUM];
    bitset<MAX_CELL_NUM> bf;
    int current_time;
    void init(int _window, int _memory,
              int _hash_num, double _alpha,
              int _group_size)
    {
        window = _window,
        memory = _memory,
        hash_num = _hash_num,
        max_cell_num = memory;
        alpha = _alpha;
        group_size = _group_size;
        current_time = 0;
        for (int i = 0; i < hash_num; i++)
        {
            hash[i].initialize(rand() % MAX_PRIME32);
        }
        hash_time_offset.initialize(rand() % MAX_PRIME32);
        for (int i = 0; i < max_cell_num; i++)
            bf[i] = 0;
        relax_window_size = window * (1 + alpha);
    }
    bool get_timestamp(int tx, int pos)
    {
        int gid = pos / group_size;
        //int t_add = hash_time_offset.run((char *)&gid, sizeof(int)) & ((1 << 29) - 1);
        int t_add = relax_window_size * gid / (max_cell_num / group_size);
        return int((tx + t_add) / (alpha * window)) & 1;
    }
    void check_timestamp(int pos)
    {
        int gid = pos / group_size;

        if (get_timestamp(current_time, pos) == time_stamp[gid])
            return;
        time_stamp[gid] = get_timestamp(current_time, pos);

        int st = pos / group_size * group_size;
        for (int i = st; i < st + group_size && i < max_cell_num; i++)
            bf[i] = 0;
    }
    void insert(int x)
    {
        current_time++;
        for (int i = 0; i < hash_num; i++)
        {
            int pos = hash[i].run((char *)&x, sizeof(int)) % max_cell_num;
            check_timestamp(pos);
            bf[pos] = 1;
        }
    }
    bool query(int x, int window)
    {
        for (int i = 0; i < hash_num; i++)
        {
            int pos = hash[i].run((char *)&x, sizeof(int)) % max_cell_num;
            check_timestamp(pos);
            if (bf[pos] == 0 && get_timestamp(current_time - window + 1, pos) == time_stamp[pos / group_size])
                return 0;
        }
        return 1;
    }
};
