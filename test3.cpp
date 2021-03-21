#include<iostream>
#include<bitset>
#include<string>
#include<vector>
#include<map>
#include<fstream>
using namespace std;


//important constants
const int MemorySize = 1048576;
const int numOfReg = 32;
const string reglist[32] = {
        "z  ",
        "at ",
        "v0 ","v1 ",
        "a0 ","a1 ","a2 ","a3 ",
        "t0 ","t1 ","t2 ","t3 ","t4 ","t5 ","t6 ","t7 ",
        "s0 ","s1 ","s2 ","s3 ","s4 ","s5 ","s6 ","s7 ",
        "t8 ","t9 ",
        "k0 ","k1 ",
        "gp ","sp ","fp ",
        "ra " 
};

typedef bitset<8>  byte8;
typedef bitset<32> byte32;


byte8 mem[MemorySize];
byte8 DRAM[1024][1024];
byte8 RowBuffer[1024];
int row;
int col;
int ROW_ACCESS_DELAY;
int COL_ACCESS_DELAY;
int indexs=0;
int rowindex=-1;
int oldrowindex=-2;
int oldrow1index=-2;
byte32 reg[numOfReg];
int cycles=0;
    int Memstart;
class Mem{

    public:

    map <string,int> registerMap; 
    vector<int> linepos;

    private:
    bool flag = true;

    public:

    Mem(string code){

        int addr = 0;

        //parses into instructions
        linepos.push_back(0);
        for(int i=0;i<code.length();i++){
            if(code[i] == '\t' || code[i] == ' ' || code[i] == ',' || code[i] == '\n'){
                int k = addr%4;
                if(k>0){
                    while(k<4){
                        mem[addr] = (byte8)32;
                        k++;
                        addr++;
                    }
                }
                if(code[i] == '\n'){
                    linepos.push_back(addr);
                }
            }
            else{
                mem[addr] = (byte8)code[i];
                addr++;
            }
        }
        int k = addr%4;
        if(k>0){
            while(k<4){
                mem[addr] = (byte8)32;
                k++;
                addr++;
            }
        } 
        Memstart = addr;

        //adds registers in Register array
        for(int i=0;i<32;i++){
            registerMap[reglist[i]] = i;
        }
        for (row = 0; row<1024 ; row++)
        {
            for(col = 0; col<1024; col++)
            {
                DRAM[row][col] = mem[indexs];
                indexs++;
            }
        }    
    }

    void execute(int ex){ 
        if(ex<Memstart){ 
            cout<<"\nExecuting cycle position : "<<cycles+1<<endl;
            if(ex%4==0){
                int token = findtoken(ex);
                int nextex = ex+16;
                switch (token){
                           //add
                    case 1:{
                                int val1=getreg(ex+4);
                                int val2=getreg(ex+8);
                                int val3=getreg(ex+12);
                                reg[val1] = (byte32)(reg[val2].to_ulong()+reg[val3].to_ulong());
                                cycles++;
                                break;
                           }
                           //sub
                    case 2:{
                                int val1=getreg(ex+4);
                                int val2=getreg(ex+8);
                                int val3=getreg(ex+12);
                                reg[val1] = (byte32)(reg[val2].to_ulong()-reg[val3].to_ulong());
                                cycles++;
                                break;
                           }
                           //mul
                    case 3:{
                                int val1=getreg(ex+4);
                                int val2=getreg(ex+8);
                                int val3=getreg(ex+12);
                                reg[val1] = (byte32)(reg[val2].to_ulong()*reg[val3].to_ulong());
                                cycles++;
                                break;
                           }
                           //beq
                    case 4:{
                                int val1=getreg(ex+4);
                                int val2=getreg(ex+8);
                                if(reg[val1]==reg[val2])
                                    nextex = getloc(ex+12);
                                    cycles++;                    
                                break;
                           }
                           //bne
                    case 5:{
                                int val1=getreg(ex+4);
                                int val2=getreg(ex+8);
                                if(reg[val1]!=reg[val2])
                                    nextex = getloc(ex+12);
                                cycles++;
                                break;
                           }
                           //slt
                    case 6:{
                                int val1=getreg(ex+4);
                                int val2=getreg(ex+8);
                                int val3=getreg(ex+12);
                                if(reg[val2].to_ulong()<reg[val3].to_ulong())
                                    reg[val1] = (byte32)1;
                                else
                                    reg[val1] = (byte32)0;
                                cycles++;
                                break;
                           }
                           //j
                    case 7:{
                               nextex = getloc(ex+4);
                               cycles++;
                               break;
                           }
                           //lw
                    case 8:{
                               cycles++;
                               int val1 = getreg(ex+4);
                               int val2 = getreg(ex+8);
                               int address = reg[val2].to_ulong();
                               oldrowindex=rowindex; 
                               rowindex = (int) (Memstart+address)/1024; 
                               int rowbufferindex=0;                             
                               if (oldrowindex != rowindex)
                               {
                                    for (int colm=0;colm<1024;colm++)
                                    {
                                        RowBuffer[rowbufferindex] = DRAM[rowindex][colm];
                                        rowbufferindex++;
                                    }
                                    cycles=cycles+ROW_ACCESS_DELAY;
                                    //col delay
                                    reg[val1]=(byte32)(RowBuffer[(Memstart+address) % 1024].to_string()+RowBuffer[(Memstart+address) % 1024+1].to_string()+RowBuffer[(Memstart+address) % 1024+2].to_string()+RowBuffer[(Memstart+address) % 1024+3].to_string());
                                    cycles = cycles+COL_ACCESS_DELAY;
                               }
                                else{
                                    //col delay
                                    reg[val1]=(byte32)(RowBuffer[(Memstart+address) % 1024].to_string()+RowBuffer[(Memstart+address) % 1024+1].to_string()+RowBuffer[(Memstart+address) % 1024+2].to_string()+RowBuffer[(Memstart+address) % 1024+3].to_string());
                                    cycles = cycles+COL_ACCESS_DELAY;

                                }                            
                               nextex = ex+12;
                               break;
                           }
                           //sw
                    case 9:{
                               cycles++;
                               int val1 = getreg(ex+4);
                               int val2 = getreg(ex+8);
                               int address = reg[val2].to_ulong();
                               string memval = reg[val1].to_string();
                               oldrow1index=rowindex; 
                               rowindex = (int) (Memstart+address)/1024;
                    
                               int rowbufferindex=0;
                               if (oldrow1index != rowindex){
                                   //activating new row
                                   for (int colm=0;colm<1024;colm++)
                                    {
                                        RowBuffer[rowbufferindex] = DRAM[rowindex][colm];
                                        rowbufferindex++;
                                    }
                                    cycles=cycles+ROW_ACCESS_DELAY;

                                    for(int i=0;i<4;i++){ 
                                        //mem[Memstart+val2+i] = (byte8)(memval.substr(i*8,8));
                                        //DRAM[(int) Memstart/1024][(Memstart % 1024)+val2+i] = (byte8)(memval.substr(i*8,8));
                                        RowBuffer[(Memstart+address)%1024+i] = byte8(memval.substr(i*8,8));
                                    }
                
                                    cycles = cycles+COL_ACCESS_DELAY;;
                               }
                                else{
                                    for(int i=0;i<4;i++){ 
                                        //mem[Memstart+val2+i] = (byte8)(memval.substr(i*8,8));
                                        //DRAM[(int) Memstart/1024][(Memstart % 1024)+val2+i] = (byte8)(memval.substr(i*8,8));
                                        RowBuffer[Memstart+address+i] = byte8(memval.substr(i*8,8));
                                    }
                                    cycles = cycles+COL_ACCESS_DELAY;
                                }
                                //writeback
                                int b=0;
                                for(int a=0;a<=1023;a++)
                                {
                                    DRAM[(int) (Memstart+address)/1024][a]=RowBuffer[b];
                                    b++;
                                }
                                cycles=cycles+ROW_ACCESS_DELAY;
                                nextex = ex+12;
                
                                break;
                           }
                           //addi
                    case 10:{
                                int val1 = getreg(ex+4);
                                int val2 = getreg(ex+8);
                                reg[val1] = (byte32)(reg[val2].to_ulong() + getval(ex+12));
                                cycles++;
                                break;
                            }
                }
                if(flag){
            
                    printregs();
                    execute(nextex);
                }
            }
        }
    }

    int findtoken(int pos){
        int ret;
        if(pos>=Memstart){
            cout<<"ERROR, token not found at pos "<<pos<<endl;
            flag = false;
        }
        else{
        string word;
        for(int i=pos;i<pos+4;i++){
            word = word + (char)(mem[i].to_ulong());
        }
        if(word == "add ") ret=1;
        else if(word == "sub ") ret=2;
        else if(word == "mul ") ret=3;
        else if(word == "beq ") ret=4;
        else if(word == "bne ") ret=5;
        else if(word == "slt ") ret=6;
        else if(word == "j   ") ret=7;
        else if(word == "lw  ") ret=8;
        else if(word == "sw  ") ret=9;
        else if(word == "addi") ret=10;
        else {
            cout<<"ERROR, token not found at pos "<<pos<<endl;
            flag = false;
        } //raise error 

        }

        return ret;
    }

    int getreg(int pos){
        int loc;
        if(pos>=Memstart){
            cout<<"ERROR, token not found at pos "<<pos<<endl;
            flag = false;
        }
        else{
        if((char)mem[pos].to_ulong() == '$'){
            string word;
            for(int i=pos+1;i<pos+4;i++){
                word = word + (char)(mem[i].to_ulong());
            }
            loc = registerMap.find(word)->second; 
        }

        }
        if(loc >= 32){
            cout<<"ERROR, register not found at pos "<<pos<<endl;
            flag = false;
        }
        return loc;
    }
    int getloc(int pos){
        int loc;
        if(pos>=Memstart){
            cout<<"ERROR, token not found at pos "<<pos<<endl;
            flag = false;
        }
        else{
        loc = getval(pos);
        if(loc>= linepos.size()){
            cout<<"ERROR, no line "<<loc<<" from pos "<<pos<<endl;
            flag = false;
        }
        }
        return linepos[loc];
    }

    void printregs(){
        cout<<"Registers:\n";
        for(int i=0;i<32;i++){
            cout<<hex<<reg[i].to_ulong()<<dec<<" ";
        }
        cout<<endl;
    }
    void printcode(){
        for(int i=Memstart;i<10000;i+=4){
            if ((char)mem[i].to_ulong() != 0){
                cout<<"Mem "<<i<<"-"<<i+3<<": "<<(char)mem[i].to_ulong()<<(char)mem[i+1].to_ulong()<<(char)mem[i+2].to_ulong()<<(char)mem[i+3].to_ulong()<<endl;
            }//cout<<"Mem "<<i<<" : "<<(char)mem[i].to_ulong()<<endl;            
        }
    }
    void printcode2(){
        int rowmax =(int)Memstart / 1024;
        for(int rows=0; rows<=rowmax; rows++){
            for(int cols=0; cols<1024;cols+=4){
                cout<<"DRAM"<<" "<<rows<<" "<<cols<<"-"<<cols+3<<" "<<(char)DRAM[rows][cols].to_ulong()<<(char)DRAM[rows][cols+1].to_ulong()<<(char)DRAM[rows][cols+2].to_ulong()<<(char)DRAM[rows][cols+3].to_ulong()<<endl;
            }
            cout<<"\n";
        }
    }
    void printcode3(){
        for (int buf=0;buf<=1023;buf++){
            cout<<(char)RowBuffer[buf].to_ulong();
        }     
    }
    int getval(int pos){
        string word;
        for(int i=pos;i<pos+4;i++){
            int ab = (mem[i].to_ulong());
            if((ab<48 && ab!=32 )|| ab>57 ){
               cout<<"ERROR, invalid integer"<<ab<<" at pos "<<pos<<endl; 
               flag = false;
               break;
            }
            word = word + (char)ab ;
        }
        //cout<<word;
        return stoi(word);
    }
};

int main(int argc, char* argv[]){
    //take input from file and make a new object with it
    //execute the object once
    string code,line;
    ifstream file;
    file.open(argv[1]);
    if(file.is_open()){
        while(getline(file,line)){
            code+="\n"+line;
        }
    }
    ROW_ACCESS_DELAY = atoi(argv[2]);
    COL_ACCESS_DELAY = atoi(argv[3]);
    //reg[0] = (byte32)10;
    Mem obj(code);
    cout<<"\nText File in Memory:\n";
    obj.printcode();
    obj.printcode2();
    obj.execute(0);
    //obj.printcode3();
    cout<<"\nNo. of Cycles executed : "<<cycles<<endl;
    //cout<< Memstart;
    return 0; 
}
