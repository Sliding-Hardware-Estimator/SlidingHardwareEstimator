
class she_bitmap
{
public:
    constexpr static const double alpha = 0.2;
    static const int
        MAX_CELL_NUM = 32 * 1e5 + 5,
        MAX_HASH_NUM = 50;
    int
        window, 
        memory,
        hash_num,
        max_cell_num, 
        group_size;
    long long relax_window_size;
    BOBHash32 hash[MAX_HASH_NUM];
    BOBHash32 hash_time_offset;
    bitset<MAX_CELL_NUM> bm;
    bool time_stamp[MAX_CELL_NUM];
    int current_time;
    void init(int _window, int _memory,
              int _hash_num = 1, int _group_size = 64)
    {
        for (int i = 0; i < hash_num; i++)
        {
            hash[i].initialize(rand() % MAX_PRIME32);
        }
        hash_time_offset.initialize(rand() % MAX_PRIME32);
        window = _window;
        memory = _memory;
        hash_num = _hash_num;
        group_size = _group_size;
        max_cell_num = _memory * group_size / (group_size + 1);
        relax_window_size = window * (1 + alpha);
    }
    bool get_timestamp(int tx, int pos)
    {
        int gid = pos / group_size;
        //int t_add=hash_time_offset.run((char*)&gid, sizeof(int))&((1<<29)-1);
        int t_add = relax_window_size * gid / (max_cell_num / group_size);
        return ((tx + t_add) / relax_window_size) & 1;
    }
    void check_timestamp(int pos)
    {
        int gid = pos / group_size;

        if (get_timestamp(current_time, pos) == time_stamp[gid])
            return;
        time_stamp[gid] = get_timestamp(current_time, pos);

        int st = pos / group_size * group_size;
        for (int i = st; i < st + group_size && i < max_cell_num; i++)
            bm[i] = 0;
    }
    void insert(int x)
    {
        current_time++;
        for (int i = 0; i < hash_num; i++)
        {
            int pos = hash[i].run((char *)&x, sizeof(int)) % max_cell_num;
            check_timestamp(pos);
            bm[pos] = 1;
        }
    }
    int query()
    {
        int ns = 0;
        double u = 0, x;
        for (int i = 0; i <= (max_cell_num - 1) / group_size; i++)
        {
            check_timestamp(i * group_size);
            int t_add = relax_window_size * i / (max_cell_num / group_size);

            x = (current_time + t_add) % relax_window_size;
            if (x < (1 - alpha) * window)
                continue;

            for (int j = i * group_size;
                 j < i * group_size + group_size && j < max_cell_num; j++)
            {
                if (bm[j] == 0)
                    u++;
                ns++;
            }
        }
        return -max_cell_num * log(u / ns);
    }
};
