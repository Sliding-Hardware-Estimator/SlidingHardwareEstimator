
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
#include "../load_data.h"
#include "./she_bloomfilter.h"
using namespace std;

she_bloomfilter she_BF;

unordered_map<int, bool> inmap, inmap2;

double solve_equation(double Q)
{
    double l = 0, r = 50, x;
    while (r - l > 1e-7)
    {
        x = (l + r) / 2;
        if (pow(Q, x) * (log(Q) * x - 1) + Q > 0)
            r = x;
        else
            l = x;
    }
    return l;
}
const int TWS = 5;
const int insnum = 10 * (1 << 16), window = (1 << 16);
double ans[insnum + window * TWS + 5];
void test()
{
    double ss = 0;
    for (int i = insnum - 5 * window + 1; i <= insnum; i++)
    {
        inmap[flow[i]] = 1;
    }
    ss = inmap.size();

    inmap.clear();
    for (int i = insnum - window + 1; i <= insnum; i++)
    {
        inmap[flow[i]] = 1;
    }

    for (int gsize = 64; gsize >= 64; gsize /= 2)
    {

        for (int mem = (1 << 18); mem <= (1 << 22); mem <<= 2)
        {
            double rmem = (double)mem / window;

            memset(ans, 0, sizeof(ans));
            for (int hashnum = 8; hashnum <= 8; hashnum++)
            {
                double Q = pow(1 - 1. / gsize, ss / 5 * hashnum * (gsize + 1) / (rmem * window));
                double R = solve_equation(Q);
                for (double alpha = R; alpha <= R; alpha *= 1.2)
                {
                    inmap.clear();
                    inmap2.clear();
                    for (int i = insnum - window + 1; i <= insnum; i++)
                    {
                        inmap[flow[i]] = 1;
                    }
                    she_BF.init(window, mem, hashnum, alpha, gsize);
                    for (int i = 1; i <= insnum; i++)
                        she_BF.insert(flow[i]),
                            inmap2[flow[i]] = 1;

                    for (int i = 1; i <= insnum; i++)
                        ans[i] = she_BF.query(flow[i], window) - inmap[flow[i]];

                    for (int i = insnum + 1; i <= insnum + window * 5; i++)
                    {
                        she_BF.insert(flow[i]),
                            inmap2[flow[i]] = 1;

                        if (i % (window / 2) == 0)
                        {
                            double sum = 0, cnt = 0;
                            for (int j = 1; j <= window * TWS; j++)
                            {
                                int f = (rand() << 15) | rand();
                                if (inmap2.find(f) != inmap2.end())
                                    continue;
                                sum += she_BF.query(f, window) - inmap[f];
                                cnt++;
                            }
                            printf("query time:%d\tfalse positive alphae:%.8lf\n", i, sum / cnt);
                        }
                    }
                    printf("window:%d\tgroupsize:%d\thashnum:%d\tmemory:%d Byte\t alpha:%.3lf\t|",
                           window, gsize, hashnum, int(she_BF.memory / 8), alpha);
                    printf("\n");
                }
            }
        }
    }
}
int main()
{

    load_data1_CAIDA();

    test();
    return 0;
}
