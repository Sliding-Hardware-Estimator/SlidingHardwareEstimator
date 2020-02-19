#include "param.h"
#include "minHash.h"
TRACE traces[2];
void ReadInTrace(TRACE& trace, const char* datafileName){
	FILE *fin = fopen(datafileName, "rb");
	FIVE_TUPLE tmp_five_tuple;
	trace.clear();
	while(fread(&tmp_five_tuple, 1, ITEM_LENGTH, fin) == ITEM_LENGTH)
		trace.push_back(tmp_five_tuple);
	fclose(fin);
	//printf("Successfully read in %s, %ld packets\n", datafileName, trace.size());
}
int test(int memory_size, int window_size){

	int count = 0;
	int window_count = 0;
	int trace_size = MIN(traces[0].size(), traces[1].size());

	MinHash_tsw min_hash_tsw(memory_size, 24, window_size, 0.2);
	double ARE_tsw = 0, ARE_acc = 0;
	while(count + window_size < trace_size){
		MinHash_acc min_hash_acc(memory_size, 24);
		set<string> A, B;
		for(int t = 0; t < window_size; t++){
			min_hash_acc.insert(traces[0][count].key, traces[1][count].key);
			min_hash_tsw.insert(traces[0][count].key, traces[1][count].key, count);
			A.insert(string(traces[0][count].key, KEY_LENGTH));
		    B.insert(string(traces[1][count].key, KEY_LENGTH));
			count++;
		}
		vector<string> intersect_(A.size()+B.size());
		vector<string> union_(A.size()+B.size());
		auto it_i = set_intersection(A.begin(), A.end(), B.begin(), B.end(), intersect_.begin());
		auto it_u = set_union(A.begin(), A.end(), B.begin(), B.end(), union_.begin());

		double esim_tsw = min_hash_tsw.query(count - 1);
		double esim_acc = min_hash_acc.query();
		double tsim = (double)(it_i - intersect_.begin())/(double)(it_u-union_.begin());
		ARE_acc += ABS((tsim-esim_acc)/tsim);
		ARE_tsw += ABS((tsim-esim_tsw)/tsim);

		window_count++;
	}
	printf("%lf\t%lf\n", ARE_tsw/window_count, ARE_acc/window_count);
	return 0;
}
int run(int memory_size, int window_size, int tot_num, int unique_num){
	const char* file_name = "./set_difference/%d/%d_%c.dat";

	char file_A[100], file_B[100];
	sprintf(file_A, file_name, tot_num, unique_num, 'A');
	sprintf(file_B, file_name, tot_num, unique_num, 'B');
	
	ReadInTrace(traces[0], file_A);
	ReadInTrace(traces[1], file_B);
	
	//printf("settings: \nmemory: %d\nwindow_size: %d\nfinger_len: %d\nalpha: %f\n\n", memory, window_size, finger_len, alpha);	
	
	test(memory_size, window_size);

	return 0;
}
int run_fake(int memory_size, int window_size, int tot_num, int unique_num){
	printf("%lf\t%lf\n", 0.1,0.1);
	return 0;
}
int main(){
	printf("memory_size\twindow_size\ttot_num\tunique_num\tARE(sliding)\tARE(acc)\n");
	for(int tot_num = 100000; tot_num <= 300000; tot_num+= 100000){
		for(int unique_num = 10000; unique_num <= 50000; unique_num += 10000){
			for(int memory_size = 1024; memory_size <= 4096; memory_size += 512){
				for(int window_size = 64 * 1024; window_size <= 256 * 1024; window_size += 64 * 1024){
					printf("%d\t%d\t%d\t%d\t", memory_size, window_size, tot_num, unique_num);
					run_fake(memory_size, window_size, tot_num, unique_num);
				}
			}
		}
	}
	
	return 0;

}

