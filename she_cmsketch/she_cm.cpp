#define counter_t uint16_t
#define CNT_MAX 1 << 15
#define ALPHA 1
#define BETA 2
class SHE_CM{
    private:
        int window_sz;
        int flush_window_sz;
        int d;
        int width;
        int group_num;
        counter_t** counters;
        bool** stamps;
        BOBHash32** hash_func;
    public:
        SHE_CM(int window_sz_, int flush_window_sz_, int d_ = 8, int width_ = 1024 * 32):
            window_sz(window_sz_), flush_window_sz(flush_window_sz_), d(d_), width(width_)
        {
            group_num = width / 64;
            counters = new counter_t*[d];
            stamps = new bool*[group_num];
            hash_func = new BOBHash32*[d];
            
            for(int i = 0; i < d; i++){
                counters[i] = new counter_t[width];
                for(int k = 0; k < width; k++){
                    counters[i][k] = 0;
                }
                stamps[i] = new bool[group_num];
                for(int k = 0; k < group_num; k++){
                    stamps[i][k] = 0;
                }
                hash_func[i] = new BOBHash32(i * 12 + 221);
            }

        }
        void insert(int time_cnt, const char* key, counter_t f = 1){
            for(int i = 0; i < d; i++){
                int index = hash_func[i]->run(key, KEY_LENGTH) % width;
                int group_index = index / 64;
                int h = (flush_window_sz * (group_index + i * group_num)) / (group_num * d);

                bool stamp = ((time_cnt + h) / flush_window_sz) % 2;
                
                if(stamp != stamps[i][group_index]){
                    stamps[i][group_index] = stamp;
                    int k = group_index * 64;
                    for(int t = 0; t < 64; t++, k++){
                        counters[i][k] = 0;
                    }
                }
                counters[i][index] += f;
            }
        }
        counter_t query(int time_cnt, const char* key){
            counter_t ret = CNT_MAX;
            for(int i = 0; i < d; i++){
                int index = hash_func[i]->run(key, KEY_LENGTH) % width;
                int group_index = index / 64;
                int h = (flush_window_sz * (group_index + i * group_num)) / (group_num * d);
                
                int age = (time_cnt + h) % flush_window_sz;
                
                if(age > ALPHA * window_sz){
                    ret = MIN(ret, counters[i][index]);
                }
            }
            return (ret==CNT_MAX)?0:ret;
        }
        double calc_ARE(int time_cnt, const FREQ_RECORD& Real_Freq){
            double RE_sum = 0;
            for(FREQ_RECORD::const_iterator it = Real_Freq.begin(); it != Real_Freq.end(); it++){
                int AE = ABS(it->second - query(time_cnt, it->first.c_str()));
                double RE = (double)AE/(double)(it->second);
                RE_sum += RE;
            }
            return RE_sum / (double)Real_Freq.size();
        }
};
