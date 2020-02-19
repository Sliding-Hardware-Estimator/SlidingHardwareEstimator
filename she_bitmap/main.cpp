
//SHE-bitmap
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
#include "../load_data.h"
#include "she_bitmap.h"
using namespace std;

she_bitmap she_bm;

BOBHash32 hash_id;
unordered_map<int, int> inmap;

int TWS = 5;
const int insnum = 1e6, window = (1 << 16);
double ans[MAXNUM + window + 10];
void test()
{
    printf("alpha:%.3lf\n", she_bitmap::alpha);
    for (int mem = (1 << 11); mem <= (1 << 16); mem <<= 1)
    {
        double sum = 0;

        int repmax = 1;
        int cnt = 0;
        for (int rep = 1; rep <= repmax; rep++)
        {
            inmap.clear();
            she_bm.init(window, mem);
            double ans = 0;
            for (int i = 1; i <= window * 2; i++)
            {
                she_bm.insert(flow[i]);
            }
            for (int i = window + 1; i <= window * 2; i++)
            {
                inmap[flow[i]]++;
                if (inmap[flow[i]] == 1)
                    ans++;
            }
            for (int i = window * 2 + 1; i <= window * (2 + TWS); i++)
            {
                she_bm.insert(flow[i]);
                inmap[flow[i]]++;
                if (inmap[flow[i]] == 1)
                    ans++;
                inmap[flow[i - window]]--;
                if (inmap[flow[i - window]] == 0)
                    ans--;
                if (i % (window / 2) == 0)
                {
                    double ss = 0;
                    ss = (double)she_bm.query() / ans;
                    printf("query time%d\t%.6lf\n", i, ss - 1.0);

                    sum += fabs(ss - 1.0);
                    cnt++;
                }
            }
        }
        printf("memory:%d Byte\t ave error:%.6lf\n", she_bm.memory / 8, sum / cnt);
    }
}
int main()
{

    load_data1_CAIDA();

    test();
    return 0;
}
