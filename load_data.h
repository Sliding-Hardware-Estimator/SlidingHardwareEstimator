
const int MAXNUM=3*1e7+10;
int flow[MAXNUM+10];

void load_data0(){
    for(int i=1;i<=MAXNUM;i++) {
        flow[i]=i;
    }
}
void load_data1_CAIDA(){
    BOBHash32 hash_id;
    hash_id.initialize(rand()%MAX_PRIME32);
    ifstream input( "../../DataSet/CAIDA/formatted00.dat", ios::in | ios::binary );
    char buf[2000]={0};
    for(int i=1;i<=MAXNUM;i++) {
        if(!input.read(buf, 16)){
            printf("ERROR\n");
            break;
        }
        flow[i]=hash_id.run(buf+8, 16);
    }
}
void load_data2_campus(){
    BOBHash32 hash_id;
    hash_id.initialize(rand()%MAX_PRIME32);
    
    ifstream input( "./DataSet/campus/campus.dat", ios::in | ios::binary );
    char buf[2000];
    for(int i=1;i<=MAXNUM;i++) {
        if(!input.read(buf, 13)){
            printf("ERROR\n");
            break;
        }
        flow[i]=hash_id.run(buf, 4);
    }
}
void load_data3_new_zipf(){
    BOBHash32 hash_id;
    hash_id.initialize(rand()%MAX_PRIME32);
    
    ifstream input( "./DataSet/new_zipf/030.dat", ios::in | ios::binary );
    char buf[2000];
    for(int i=1;i<=MAXNUM;i++) {
        if(!input.read(buf, 4)){
            printf("ERROR\n");
            break;
        }
        flow[i]=hash_id.run(buf, 4);
    }
}
void load_data4_webpage(){
    BOBHash32 hash_id;
    hash_id.initialize(rand()%MAX_PRIME32);
    
    ifstream input( "./DataSet/webpage/webdocs_form00.dat", ios::in | ios::binary );
    char buf[2000];
    for(int i=1;i<=MAXNUM;i++) {
        if(!input.read(buf, 13)){
            printf("ERROR\n");
            break;
        }
        flow[i]=hash_id.run(buf, 4);
    }
}