#include<vector>
#include<unistd.h>
#include<iostream>
#include<unordered_map>
#include<string>
#include<stdio.h>
#include<stdlib.h>
#include<memory.h>
#include<set>
#include<algorithm>
#include"BOBHash32.h"
#define END_FILE_NO 2
#define START_FILE_NO 0
#define KEY_LENGTH 13
#define ITEM_LENGTH 13
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define ABS(a) ((a)>0?(a):(-(a)))
using namespace std;
struct FIVE_TUPLE{
	char key[ITEM_LENGTH];
};
typedef vector<FIVE_TUPLE> TRACE;
typedef unordered_map<string, int> FREQ_RECORD;
