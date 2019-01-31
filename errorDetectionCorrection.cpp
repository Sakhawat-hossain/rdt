#include<iostream>
#include<windows.h>
#include<cstring>
#include<string>


using namespace std;

string* createDataBlock(string str,int m);
string* addCheckBits(string *str,int row,int m);
string appendCRC(string cws,string polgen);
string  receivedFrame(string frame,int row,float p);

int main(){

    //char *str;
    string str;
    int m;
    float p;
    string polgen;

    cout<<"Enter data string : ";
    getline(cin,str);
    cout<<"Enter number of data bytes in a block : ";
    cin>>m;
    cout<<"Enter probability <p> : ";
    cin>>p;
    cout<<"Enter generator polynomial : ";
    cin>>polgen;

    int strlen=str.length();
    while((strlen%m) != 0){
        str=str+"~";
        strlen++;
    }
    cout<<endl;
    cout<<"Data string after padding : "<<str<<endl;
    cout<<endl;

    string *strblock=createDataBlock(str,m);

    string *blockc=addCheckBits(strblock,str.length()/m,m);

    string cws;
    cout<<"Data bits after column-wise serialization : "<<endl;
    int row=str.length()/m;
    int len=blockc[0].length();
    for(int i=0;i<len;i++){
        for(int j=0;j<row;j++){
            cws=cws+blockc[j].at(i);
        }
    }
    cout<<cws<<endl<<endl;

    cout<<"Data bits after appending CRC checksum <sent frame> : "<<endl;
    cout<<cws;
    cws=cws+polgen;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
    cout<<polgen<<endl<<endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    receivedFrame(cws,row,p);

    return 0;
}
string* createDataBlock(string str,int m){
    int len=str.length();

    string *asscib=new string[len/m];
    int k=0,j;
    for(int i=0;i<len;i++){
        j=0;
        char c=str.at(i);
        while(j<8){
            if(128&c)
                asscib[k]=asscib[k]+"1";
            else
                asscib[k]=asscib[k]+"0";
            j++;
            c=c<<1;
        }
        if((i+1)%m == 0)
            k++;
    }

    cout<<"Data block <ascii code of m characters in each row>"<<endl;
    len=len/m;
    for(int i=0;i<len;i++){
        cout<<asscib[i]<<endl;
    }
    cout<<endl;

    return asscib;
}

string* addCheckBits(string *str,int row,int m){
    string *block=new string[row];

    cout<<"Data block after adding check bits : "<<endl;
    int k=2,idx=1;
    int len=8*m;

    for(int i=0;i<row;i++)
        block[i]=block[i]+"0";

    for(int i=0;i<len;i++){
        if((k-1)==idx){
            for(int j=0;j<row;j++){
                block[j]=block[j]+"0"+str[j].at(i);
            }
            idx += 2;
            k = k*2;
        }
        else{
            for(int j=0;j<row;j++){
                block[j]=block[j]+str[j].at(i);
            }
            idx += 1;
        }
    }

    int count=0;
    len=block[0].length();
    for(int j=0;j<row;j++){ //for each row
        k=1; //check bit position
        //idx=1; // interval
        while(k<len){  // for a row
            //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
            int n=k;
            count=0;
            while(n<=len){
                for(int i=0;i<k;i++){ // calculate check bit
                    if((n+i)>len) break;
                    string t;//cout<<" o- "<<(n+i)<<endl;
                    t.push_back(block[j].at(n+i-1));
                    if(t.compare("1")==0){
                        count++;
                    }
                }
                n=n+k*2;
            }
            if((count%2)==1) block[j].replace(k-1,1,"1");
            k=k*2;
        }
    }

    for(int i=0;i<row;i++){
        k=1;
        for(int j=0;j<len;j++){
            if(j==k-1){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
                cout<<block[i].at(j);
                k=k*2;
            }
            else{
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                cout<<block[i].at(j);
            }
        }
        cout<<endl;
    }
    cout<<endl;

    return block;
}

string appendCRC(string cws,string polgen){

}

string receivedFrame(string frame,int row,float p){
    string rcvframe;
    int len=frame.length();
    float fp;
    for(int i=0;i<len;i++){
        fp=rand()/RAND_MAX*1.0;
        if(fp>p)
            cout<<"fp = "<<fp<<"  max = "<<RAND_MAX<<endl;
    }
    return frame;
}




