#include<iostream>
#include<windows.h>
#include<cstring>
#include<string>
#include<sstream>
#include<cmath>


using namespace std;

string* createDataBlock(string str,int m);
string* addCheckBits(string *str,int row,int m);
string appendCRC(string frame,string polgen);
string  receivedFrame(string frame,float p);
void checkCRC(string frame,string polgen);
string* discardCRC(string frame,string rcvframe,string polgen,int row);
string* removeCheckBits(string *str,int row);
void showOutput(string *output,int row);

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
    string frame=appendCRC(cws,polgen);


    string rcvframe=receivedFrame(frame,p);

    checkCRC(rcvframe,polgen);

    cout<<"Data block after removing CRC checksum bits :"<<endl;
    string* rcvblock=discardCRC(frame,rcvframe,polgen,row);
    cout<<endl;

    cout<<"Data block after removing check bits : "<<endl;
    string* wcbits=removeCheckBits(rcvblock,row);
    cout<<endl;

    showOutput(wcbits,row);
    cout<<endl;

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

string appendCRC(string frame,string polgen){
    string tempf=frame;
    int plen=polgen.length();
    for(int i=0;i<plen-1;i++)
        frame=frame+"0";
    int flen=frame.length();

    int *temp=new int[plen];
    int *divisor=new int[plen];
    int w,idx=0;
    for(int i=0;i<plen;i++){
        w=(int)polgen.at(i)-'0';
        divisor[i]=w;
    }

    idx=0;
    int pos=0;
    for(int i=0;i<flen;i++){
        w=(int)frame.at(i)-'0';
        if(idx==0){
            if(w==0)
                continue;
        }
        temp[idx]=w;
        idx++;
        //cout<<"idx  "<<idx<<endl;
        if(idx>=plen){
            for(int j=0;j<plen;j++){
                if(temp[j]==divisor[j])
                    temp[j]=0;
                else
                    temp[j]=1;
            }
            idx=0;
            int k;
            for(k=0;k<plen;k++){
                if(temp[k]==1)
                    break;
            }
            pos=k;
            while(k<plen){
                temp[idx]=temp[k];
                idx++;
                k++;
            }
        }
    }

    for(int j=plen-1;j>0;j--){
        temp[j]=temp[idx-1];
        temp[idx-1]=0;
        idx--;
        if(idx<=0)
            break;
    }
    stringstream s;
    for(int i=1;i<plen;i++){
        s<<temp[i];
    }
    cout<<tempf;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
    cout<<s.str()<<endl<<endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    frame=tempf+s.str();

    return frame;
}

string receivedFrame(string frame,float p){
    string rcvframe;
    int len=frame.length();
    float fp;
    int u,v;
    for(int i=0;i<len;i++){
        fp=1.0*rand()/RAND_MAX;
        if(fp<p){
            u=(int)frame.at(i)-'0';
            if(u==0)
                rcvframe=rcvframe+"1";
            else
                rcvframe=rcvframe+"0";
        }
        else{
            rcvframe=rcvframe+frame.at(i);
        }
    }

    cout<<"received frame"<<endl;
    for(int i=0;i<len;i++){
        u=(int)frame.at(i)-'0';
        v=(int)rcvframe.at(i)-'0';
        if(u == v){
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
            cout << v;
        }
        else{
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
            cout << v;
        }
    }
    cout<<endl<<endl;
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);



    return rcvframe;
}

void checkCRC(string frame,string polgen){
    int plen=polgen.length();
    int flen=frame.length();

    int *temp=new int[plen];
    int *divisor=new int[plen];
    int w,idx=0;
    for(int i=0;i<plen;i++){
        w=(int)polgen.at(i)-'0';
        divisor[i]=w;
    }

    idx=0;
    int pos=0;
    for(int i=0;i<flen;i++){
        w=(int)frame.at(i)-'0';
        if(idx==0){
            if(w==0)
                continue;
        }
        temp[idx]=w;
        idx++;
        //cout<<"idx  "<<idx<<endl;
        if(idx>=plen){
            for(int j=0;j<plen;j++){
                if(temp[j]==divisor[j])
                    temp[j]=0;
                else
                    temp[j]=1;
            }
            idx=0;
            int k;
            for(k=0;k<plen;k++){
                if(temp[k]==1)
                    break;
            }
            pos=k;
            while(k<plen){
                temp[idx]=temp[k];
                idx++;
                k++;
            }
        }
    }

    int f=0;
    for(int i=0;i<plen;i++){
        if(temp[i]==1)
            f=1;
    }
    if(f==1)
        cout << "Result of CRC checksum matching : error detected"<<endl;
    else
        cout << "Result of CRC checksum matching : no error detected"<<endl;

    cout<<endl;
}

string* discardCRC(string frame,string rcvframe,string polgen,int row){
    int plen=polgen.length();
    int flen=rcvframe.length();

    //string rstr=rcvframe.substr(0,);
    flen=flen-plen+1;
    string *str=new string[row];

    //int len=flen/row;
    int u,v,j;
    for(int i=0;i<row;i++){
        j=i;
        while(j<flen){
            u=(int)frame.at(j)-'0';
            v=(int)rcvframe.at(j)-'0';
            if(u == v){
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
                cout << v;
            }
            else{
                SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 12);
                cout << v;
            }
            str[i]=str[i]+rcvframe.at(j);
            j=j+row;
        }
        cout<<endl;
    }
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);

    return str;
}

string *removeCheckBits(string *str,int row){
    int len=str[0].length();
    string *retblock=new string[row];

    int k=1;
    for(int i=0;i<row;i++){
        k=1;
        for(int j=0;j<len;j++){
            if((k-1) != j){
                retblock[i]=retblock[i]+str[i].at(j);
            }
            else
                k=k*2;
        }
        cout<<retblock[i]<<endl;
    }

    return retblock;
}

void showOutput(string *output,int row){
    string str;
    int len=output[0].length();
    int val=0,v=0,p=0;
    char c;
    for(int i=0;i<row;i++){
        for(int j=0;j<len;j++){
            v=(int)output[i].at(j)-'0';
            val=val+v*pow(2,7-p);
            p++;
            if((j+1)%8 == 0){
                c=val;
                str=str+c;
                val=0;
                p=0;
            }
        }
    }
    cout<<"Output frame : "<<str<<endl;

}
