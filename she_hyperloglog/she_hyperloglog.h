//SHE-hyperloglog
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
// #include "../load_data.h"
using namespace std;

class SHE_hyperloglog
{
public:
    constexpr static const double alpha = 0.2;
    static const int MAX_CELL_NUM = 1e6;
    int
        window,
        max_cell_num,
        relax_window_size,
        memory;
    BOBHash32 hash_time_offset;
    BOBHash32 hashs;
    bool time_stamp[MAX_CELL_NUM];
    int num[MAX_CELL_NUM];
    int current_time;
    SHE_hyperloglog()
    {
        hash_time_offset.initialize(rand() % MAX_PRIME32);
        hashs.initialize(rand() % MAX_PRIME32);
    }
    void init(int _window, int _max_cell_num)
    {
        window = _window;
        max_cell_num = _max_cell_num;
        relax_window_size = window * (1 + alpha);
        memory = max_cell_num * 6;
        hash_time_offset.initialize(rand() % MAX_PRIME32);
        hashs.initialize(rand() % MAX_PRIME32);
        relax_window_size = window * (1 + alpha);
    }
    bool get_timestamp(int tx, int pos)
    {
        //int t_add = hash_time_offset.run((char *)&pos, sizeof(int)) & ((1 << 29) - 1);
        int t_add = relax_window_size * pos / (max_cell_num );
        return ((tx + t_add) / relax_window_size) & 1;
    }
    void insert(int x)
    {
        current_time++;

        int pos = hash_time_offset.run((char *)&x, sizeof(int)) % max_cell_num;
        if ((get_timestamp(current_time, pos) != time_stamp[pos]))
            time_stamp[pos] = get_timestamp(current_time, pos),
            num[pos] = 0;
        uint32_t hs = hashs.run((char *)&x, sizeof(int));
        int k = 1;
        while (hs & 1)
            k++, hs >>= 1;
        num[pos] = max(num[pos], k);
    }
    int query()
    {
        long double x, sum = 0, m = 0;
        for (int i = 0; i < max_cell_num; i++)
        {
            if ((get_timestamp(current_time, i) != time_stamp[i]))
                time_stamp[i] = get_timestamp(current_time, i),
                num[i] = 0;

            int t_add = hash_time_offset.run((char *)&i, sizeof(int)) & ((1 << 29) - 1);
            x = (current_time + t_add) % relax_window_size + 1;
            if (x < (1 - alpha) * window)
                continue;
            sum += 1 / (pow(2, num[i]));
            m++;
        }
        double a = (double)0.7213 / (1 + 1.079 / m);
        if (m <= 64)
            a = 0.709;
        if (m <= 32)
            a = 0.697;
        if (m <= 16)
            a = 0.673;
        return a * m * max_cell_num / sum;
    }
} ;
