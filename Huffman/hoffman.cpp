#include<iostream>
#include<string>
#include<algorithm>
#include<fstream>
#include<map>
#include<vector>
#include <stdlib.h>
#include<queue>
#include<bitset>
using namespace std;
string s,s1,s2;
class node{//建立霍夫曼树节点类
    node* m;
    node* ls;
    node* rs;
    int value;
    char c;
    bool tail;
public:
    node(){
        ls=rs=NULL;
        tail=false;
    }
    node(int v){
        ls=rs=NULL;
        tail=false;
        value=v;
    }
    node(int v,char ch){
        ls=rs=NULL;
        tail=true;
        value=v;
        c=ch;
    }
    friend bool operator<(const node a,const node b){
        return a.value>b.value;
    }
    bool if_tail(){
        return tail;
    }
    void connect(node*l,node*r){
        ls=l;
        rs=r;
    }
    char getch(){return c;}
    node* getls(){return ls;}
    void setself(node* self){m=self;}//优先队列重载那里不方便处理临时变量回找地址，干脆直接存自己的地址
    node* getself(){return m;}
    node* getrs(){return rs;}
    int getvalue(){return value;}
};
class Huffman_tree{//霍夫曼树类
    node root;//根节点
    map<char,string>m;
public:
    void build(vector<pair<int,char>>tmp){//传入统计结果
        priority_queue<node>q;
        for(int i=0;i<tmp.size();i++){
            node* leaf=new node(tmp[i].first,tmp[i].second);
            leaf->setself(leaf);
            q.push(*leaf);//初始化优先队列，每个节点都标记为叶子，也是每一个编码串的尾巴
        }
        while(q.size()>1){
            node tree1,tree2;
            tree1=q.top();
            q.pop();
            tree2=q.top();
            q.pop();
            node* nodetmp=new node(tree1.getvalue()+tree2.getvalue());//去除前两个最小的合并
            nodetmp->setself(nodetmp);//自己保存自己的地址
            nodetmp->connect(tree1.getself(),tree2.getself());//tree12是临时变量，原本叶子的地址要访问之前存的
            q.push(*nodetmp);//入队
        }
        root=q.top();
        string s="";
        dfs(root,s);//dfs一遍，处理好每一个字母对应的编码串，放入map中
    }
    void dfs(node u,string s){
        if(u.if_tail()){
            m[u.getch()]=s;
            return;
        }
        if(u.getls()){//左儿子走0
            dfs(*u.getls(),s+'0');
        }
        if(u.getrs()){//右二子走1
            dfs(*u.getrs(),s+'1');
        }
    }
    void compress(const string& input_file, const string& output_file) {//压缩
        ifstream file(input_file, ios::in);
        ofstream file_zip(output_file, ios::out | ios::binary);
        if (!file.is_open() || !file_zip.is_open()) {
            cout << "无法打开文件" << endl;
            return;
        }
        char ch;
        string bit_string;
        while(file.get(ch)) {
            bit_string+=m[ch];//根据编码表将字符转换为比特流
        }
        //将比特流转换为字节存储
        while (bit_string.size()%8!=0) {
            bit_string+='0';//补全比特流为8的倍数
        }
        for (int i = 0;i<bit_string.size();i+=8) {
            string byte = bit_string.substr(i, 8);
            char b=static_cast<char>(bitset<8>(byte).to_ulong());//将8位比特转换为一个字节,存到char里面
            file_zip.write(&b, 1);//写入
        }
        file.close();
        file_zip.close();
        cout <<"文件已压缩为 "<<output_file<<endl;
    }
    void unpack(const string& input_file, const string& output_file){//解压缩
        ifstream file(input_file, ios::in | ios::binary);
        ofstream file_unzip(output_file, ios::out);
        if (!file.is_open() || !file_unzip.is_open()) {
            cout << "无法打开文件" << endl;
            return;
        }
        // 从根节点开始遍历霍夫曼树
        node* current = &root;
        char byte;
        string bit_string;
        while (file.get(byte)) {
            // 将字节转换为二进制字符串
            bit_string += bitset<8>(byte).to_string();//写得时候是按一字节写得，读也用一字节读
        }
        int tmp=0;
        while(tmp<bit_string.size()){//遍历霍夫曼树
            if(current->if_tail()){
                char ch=current->getch();//如果到达叶子，说明找到了一个字母，输出
                file_unzip.write(&ch,1);
                current=&root;//重置为根节点继续走
            }
            if(bit_string[tmp]=='0'){//0走左
                current=current->getls();
            }
            else{//1走右
                current=current->getrs();
            }
            tmp++;
        }
        file.close();
        file_unzip.close();
        cout <<"文件已解压缩为 "<<output_file<<endl;
    }
};
Huffman_tree huffman;//建树
void compress(){
    ifstream file(s);
    // ifstream file("test.txt");
    if(!file.is_open()){
        cout<<"未读入文件"<<endl;
    }
    map<char,int>cnt;
    char ch;
    while(file.get(ch)){
        cnt[ch]++;
    }
    file.close();
    vector<pair<int,char>>tmp;
    for(auto i=cnt.begin();i!=cnt.end();i++){
        tmp.push_back({i->second,i->first});
    }//前面都是统计，用map，在放到vector里面传入霍夫曼树
    huffman.build(tmp);
    huffman.compress(s,s1);
}
void unpack(){
    huffman.unpack(s1,s2);
}
int main(){
    cout<<"请输入文件名"<<endl;
    cin>>s;
    s1=s+"_zip.txt";
    s2=s+"_unpack.txt";
    s=s+".txt";
    compress();
    unpack();
}