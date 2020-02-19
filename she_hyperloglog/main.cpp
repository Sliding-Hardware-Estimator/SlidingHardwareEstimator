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

#include "./she_hyperloglog.h"
#include "../load_data.h"
using namespace std;

SHE_hyperloglog she_hll;


extern int flow[MAXNUM+10];

BOBHash32 hash_id;
unordered_map<int, int> inmap;
const int window = (1 << 21);
double ans[window * 12 + 10];
int mem[1000] = {256, 1024, 8192};
void test()
{
    printf("alpha:%.3lf window:%d\n", SHE_hyperloglog::alpha, window);
    for (int k = 0; k < 3; k++)
    {
        double sum = 0;

        int id = mem[k] * 8 / 6;
        int repmax = 1;
        int cnt = 0;
        for (int rep = 1; rep <= repmax; rep++)
        {
            inmap.clear();
            she_hll.init(window, id);
            double ans = 0;
            for (int i = 1; i <= window * 2; i++)
            {
                she_hll.insert(flow[i]);
            }
            for (int i = window + 1; i <= window * 2; i++)
            {
                inmap[flow[i]]++;
                if (inmap[flow[i]] == 1)
                    ans++;
            }
            for (int i = window * 2 + 1; i <= window * 7; i++)
            {
                she_hll.insert(flow[i]);
                inmap[flow[i]]++;
                if (inmap[flow[i]] == 1)
                    ans++;
                inmap[flow[i - window]]--;
                if (inmap[flow[i - window]] == 0)
                    ans--;
                if (i % (window / 2) == 0)
                {
                    double ss = 0;
                    ss = (double)she_hll.query() / ans;
                    printf("query time:%d\t relative error %.6lf\n", i, ss - 1.0);
                    sum += fabs(ss - 1.0);
                    cnt++;
                }
            }
        }
        printf("memory:%d Byte\t ave error:%.6lf\n", she_hll.memory / 8, sum / cnt);
    }
}
int main()
{

    load_data1_CAIDA();

    test();
    return 0;
}