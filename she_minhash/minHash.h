#define INF 0x7fffffff
#include "BOBHash32.h"
class MinHash_tsw{
	public:
		int finger_len;
		int w;
		int window_size;
		double alpha;
		
		int relaxed_window_size;

		unsigned int* min_fingers[2];
		int* time_biases;
		bool* time_stamps;
		BOBHash32* hash;

		bool calc_stamp(int index, int time){
			return ((time+time_biases[index]) / relaxed_window_size) % 2;
		}

		MinHash_tsw(int memory_size, int finger_len_, int window_size_, double alpha_):
			finger_len(finger_len_), w(memory_size * 8 / (2 * finger_len_ + 1)), window_size(window_size_), alpha(alpha_){	
		//	printf("minHash_tsw %d\n", w);

			min_fingers[0] = new unsigned int[w];
			min_fingers[1] = new unsigned int[w];
			time_biases = new int[w];
			time_stamps = new bool[w];
			hash = new BOBHash32[w];
			relaxed_window_size = (1+alpha) * window_size;

			BOBHash32 tmp_hash(w + 1);
			for(int i = 0; i < w; i++){
				min_fingers[0][i] = INF;
				min_fingers[1][i] = INF;
				hash[i].initialize(i);
				time_biases[i] = tmp_hash.run((char*)&i, sizeof(int)) % relaxed_window_size;
				time_stamps[i] = calc_stamp(i, 0);
			}


		}

		~MinHash_tsw(){
			delete[] min_fingers[0];
			delete[] min_fingers[1];
			delete[] hash;
			delete[] time_stamps;
			delete[] time_biases;
		}
		void insert(const char *key_A, const char *key_B, int time){
			for(int i = 0; i < w; i++){
				unsigned int finger_A = hash[i].run(key_A, KEY_LENGTH) % (1<<finger_len);
				unsigned int finger_B = hash[i].run(key_B, KEY_LENGTH) % (1<<finger_len);

				bool stamp = calc_stamp(i, time);
				if(stamp != time_stamps[i]){
					min_fingers[0][i] = finger_A;
					min_fingers[1][i] = finger_B;
					time_stamps[i] = stamp;
				}
				else{
					min_fingers[0][i] = MIN(finger_A, min_fingers[0][i]);
					min_fingers[1][i] = MIN(finger_B, min_fingers[1][i]);
				}
			}
			return;
		}
		double query(int time){
			int count_in_window = 0;
			int count_valid = 0;
			for(int i = 0; i < w; i++){
				int interval = (time + time_biases[i]) % relaxed_window_size;
				if(interval < (int)((1+alpha) * window_size) && interval > (int)((1-alpha) * window_size)){
					count_in_window++;
					if(min_fingers[0][i] == min_fingers[1][i])
						count_valid++;
				}
			}
			return (double)count_valid/(double)count_in_window;
		}
};
class MinHash_acc{
	public:
		int finger_len;
		int w;
		unsigned int* min_fingers[2];
		BOBHash32* hash;

		MinHash_acc(int memory_size, int finger_len_):
			finger_len(finger_len_), w(memory_size * 8 / (2 * finger_len_)){	
		//	printf("minHash_acc %d\n", w);
			min_fingers[0] = new unsigned int[w];
			min_fingers[1] = new unsigned int[w];
			hash = new BOBHash32[w];
			for(int i = 0; i < w; i++){
				min_fingers[0][i] = INF;
				min_fingers[1][i] = INF;
				hash[i].initialize(i);
			}
		}

		~MinHash_acc(){
			delete[] min_fingers[0];
			delete[] min_fingers[1];
			delete[] hash;
		}
		int distinct = 0;
		void insert(const char *key_A, const char *key_B){
			for(int i = 0; i < w; i++){
				unsigned int finger_A = hash[i].run(key_A, KEY_LENGTH) % (1<<finger_len);
				unsigned int finger_B = hash[i].run(key_B, KEY_LENGTH) % (1<<finger_len);
				min_fingers[0][i] = MIN(finger_A, min_fingers[0][i]);
				min_fingers[1][i] = MIN(finger_B, min_fingers[1][i]);

			}
			return;
		}
		double query(){
			int count_valid = 0;
			for(int i = 0; i < w; i++){
				if(min_fingers[0][i] == min_fingers[1][i])
					count_valid++;
			}
			return (double)count_valid/(double)w;
		}
};
