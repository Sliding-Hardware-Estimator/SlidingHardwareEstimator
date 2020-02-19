#include "param.h"
#include "she_cm.cpp"
TRACE traces[END_FILE_NO - START_FILE_NO + 1];
void ReadInTraces(){
	for(int datafileCnt = START_FILE_NO; datafileCnt <= END_FILE_NO; datafileCnt++){
		char datafileName[100];
		sprintf(datafileName, "./DataSet/CAIDA/formatted0%d.dat", datafileCnt);
		
		FILE *fin = fopen(datafileName, "rb");
		cout << datafileName << endl;

		FIVE_TUPLE tmp_five_tuple;
		traces[datafileCnt].clear();

		while(fread(&tmp_five_tuple, 1, ITEM_LENGTH, fin) == ITEM_LENGTH){
			traces[datafileCnt].push_back(tmp_five_tuple);
		}
		fclose(fin);
		printf("Successfully read in %s, %ld packets\n", datafileName, traces[datafileCnt].size());
	}
	printf("\n");
}

int test_trace(const TRACE& trace, const int trace_id, const int window_sz, const int memory){
    int time_cnt = 0;
    int window_num = 0;
    int width = memory / (8 * 16);
    SHE_CM SHE_cm_sketch(window_sz, BETA * window_sz, 8, width);
    while(time_cnt + window_sz <= trace.size() && window_num <= 20){
        FREQ_RECORD Real_Freq;
        for(int t = 0; t < window_sz; t++, time_cnt++){
            string key((const char*)(trace[time_cnt].key + 8), KEY_LENGTH);
            Real_Freq[key]++;
            SHE_cm_sketch.insert(time_cnt, key.c_str());
        }
        printf("%d, %d, %d, %d, %d, %f\n", 
            window_num,
            trace_id, 
            window_sz,
            Real_Freq.size(),
            memory,
            SHE_cm_sketch.calc_ARE(time_cnt, Real_Freq));
        window_num++;
    }
	return 0;
}
int main(){
    ReadInTraces();
    printf("window, trace id, window size, flow amount, memory, ARE\n");
    for(int i = START_FILE_NO; i <= END_FILE_NO; i++){
        for(int k = 1; k <= 256; k *= 4){
            test_trace(traces[i], i, 1024 * k, 1024 * 1024 * 1);
            test_trace(traces[i], i, 1024 * k, 1024 * 1024 * 2);
            test_trace(traces[i], i, 1024 * k, 1024 * 1024 * 4);
        }
    }
    return 0;
}
