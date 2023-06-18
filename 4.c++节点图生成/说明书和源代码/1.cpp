#include <iostream>
#include <fstream>
#include<cstdio>
#include<cstring>
#include<cmath>
#include<queue>
#include<dirent.h>
#include<cstdlib>
#include<sstream>
#include<map>
#include<set>
using namespace std;
typedef pair<int,int>pii;
const string fName1="txt",fName2="dot",fName3="png";//文件夹名字
const int maxn=1e6+7;

string fontName="SimHei";
bool bold=1;//开启加粗，因为在windows下展示可能会比较奇怪

string color[10]={
    "pink",//背景颜色
    "blue",//框颜色
    "#FF8C00",//真实点颜色
    "#66CCff",//虚假点颜色
    "#45b97c",//注释点颜色
    "red",//图相连实线
    "ForestGreen",//注释相连虚线
};
ifstream fin;
ofstream fout;

string deal(string s){//检查是否存在后缀，去掉最后一个后缀
    int len=s.length();
    for(int i=len-1;i>=0;i--){
        if(s[i]=='.'){
            len=i;
            break;
        }
    }
    s=s.substr(0,len);
    return s;
}
string deal2(string s){//检查这个字符串是否有特殊符号，将他替代
    string result=s;
    int pos=0;
     // 替换 '<'
    while ((pos = result.find('<', pos)) != std::string::npos) {
        result.replace(pos, 1, "&lt;");
        pos += 4; // 由于 "&lt;" 的长度为4，所以更新 pos 的位置
    }

    // 替换 '>'
    pos = 0;
    while ((pos = result.find('>', pos)) != std::string::npos) {
        result.replace(pos, 1, "&gt;");
        pos += 4; // 由于 "&gt;" 的长度为4，所以更新 pos 的位置
    }

    // 替换 '"'
    pos = 0;
    while ((pos = result.find('"', pos)) != std::string::npos) {
        result.replace(pos, 1, "&quot;");
        pos += 6; // 由于 "&quot;" 的长度为6，所以更新 pos 的位置
    }

    // 替换 '\''
    // pos = 0;
    // while ((pos = result.find('\'', pos)) != std::string::npos) {
    //     result.replace(pos, 1, "&apos;");
    //     pos += 6; // 由于 "&apos;" 的长度为6，所以更新 pos 的位置
    // }
    
    if(bold)result="<<b>"+result+"</b>>";
    return result;
}

struct edge{
    int v,next;
}empty;
vector<edge>e;
int k=-1,head[maxn];
void add(int u,int v){
    e.push_back(empty);
    e[++k].next=head[u];
    head[u]=k;
    e[k].v=v;
}
int st[maxn];//state,2这个点是真实点，3这个点虚拟，4这个点注释
int in[maxn],out[maxn];//这个点的出入度

int txtCnt=0;
map<string,int>mp;//将原内容转化为编号
map<int,string>mp2;//编号转回原内容
int turn(string s){
    if(mp[s])return mp[s];
    else{
        mp[s]=++txtCnt;
        mp2[txtCnt]=s;
        return txtCnt;
    }
}
int make(string s){//给一个新的虚拟点
    ++txtCnt;//虚假点
    mp2[txtCnt]=s;
    return txtCnt;
}

struct dot{
    string head(){
        string s="digraph Example{\ngraph [bgcolor=\""+color[0]+"\"";
        //s+=",size=\"1600,900\"";
        s+="]";
        return s;
    }
    string end(){
        string s="}";
        return s;
    }
    stringstream frame(int x){//建立以x点为中心的框架
        stringstream ss;
        ss <<"subgraph cluster_"<<x<<"{\n"
            <<"\tcolor=\""<<color[1]<<"\"\n"
            //<<"\tlabel=\""<<s<<"\"\n"
            <<"\t"<<x;
        return ss;
    }
    string pt(int x){//普通点
        stringstream ss;
        ss<<x<<" ["
            <<"label="<<deal2(mp2[x])<<","
            <<"shape=\""<<"Mrecord"<<"\","
            <<"style=\""<<"filled"<<"\","
            <<"fillcolor=\""<<color[2]<<"\""
            <<"]";
        return ss.str();
    }
    string vpt(int x){//虚拟点
        stringstream ss;
        ss<<x<<" ["
            <<"label="<<deal2(mp2[x])<<","
        	<<"shape=\""<<"box"<<"\","
            <<"style=\""<<"filled"<<"\","
            <<"fillcolor=\""<<color[3]<<"\""
            <<"]";
        return ss.str();
    }
    string comt(int x){//comment，注释点
        stringstream ss;
        ss<<x<<" ["
            <<"label="<<deal2(mp2[x])<<","
            <<"shape=\""<<"box"<<"\","
            <<"style=\""<<"filled"<<"\","
            <<"fillcolor=\""<<color[4]<<"\""
            <<"]";
        return ss.str();
    }
    string ptl(){//普通连边
        stringstream ss;
        ss<<"}["
            <<"style=\""<<"solid"<<"\","
            <<"color=\""<<color[5]<<"\""
            <<"]";
        return ss.str();
    }
    string cmtl(){//注释连边
        stringstream ss;
        ss<<"}["
            <<"style=\""<<"dashed"<<"\","
            <<"color=\""<<color[6]<<"\","
            <<"dir=\""<<"none"<<"\""
            <<"]";
        return ss.str();
    }
    stringstream sRank(){//设置相同等级保证水平
        stringstream ss;
        ss<<"{rank=same;";
        return ss;
    }
    string pic(int x){
        stringstream ss;
        ss<<x<<" ["
            <<"label=\""<<""<<"\","
            <<"image=\""<<mp2[x]<<"\","
            <<"shape=\""<<"box"<<"\","
            <<"]";
        return ss.str();
    }
}dt;

bool vis[maxn];
void bfs(int x){
    queue<int>q;
    queue<int>q1,q2;//非注释节点和注释节点
    q.push(x);
    while(!q.empty()){
        int u=q.front();q.pop();
        if(vis[u])continue;vis[u]=1;
        for(int i=head[u];~i;i=e[i].next){
            int v=e[i].v;
            if(in[v]==0 and out[v]==0)continue;
            q.push(v);
            if(st[v]==4)q2.push(v);
            else q1.push(v);
        }
        while(!q1.empty()){
            stringstream ss;
            ss<<u<<" -> {";
            while(!q1.empty()){
                int v=q1.front();q1.pop();
                ss<<v;
                if(!q1.empty())ss<<",";
            }
            ss<<dt.ptl();
            fout<<ss.str()<<'\n';

        }
        while(!q2.empty()){
            stringstream ss;
            ss<<u<<" -> {";
            while(!q2.empty()){
                int v=q2.front();q2.pop();
                ss<<v;
                if(!q2.empty())ss<<",";
            }
            ss<<dt.cmtl();
            fout<<ss.str()<<'\n';
        }
    }

}
void init(){
    k=-1;
    memset(head,-1,sizeof(head));
    memset(vis,0,sizeof(vis));
    memset(in,0,sizeof(in));
    memset(out,0,sizeof(out));
    memset(st,0,sizeof(st));
    txtCnt=0;
    mp.clear();
    mp2.clear();
    e.clear();
}


void solve(string name){
    init();
    string path1,path2,path3;//文本、dot、png的路径
    path1=fName1+"/"+name;
    path2=fName2+"/"+deal(name)+".dot";
    path3=fName3+"/"+deal(name)+".png";
    fin.open(path1);
    fout.open(path2);
    cout<<path1<<'\n'<<path2<<'\n'<<path3<<'\n';
    //return;

    fout<<dt.head()<<'\n';
    
    string s;
    while(getline(fin,s)){
        stringstream ss(s);
        if(s=="")continue;
        string rl;//这个边的依赖关系relay
        ss>>rl;
        int num=rl[0]-48;
        if(num<1 or num>8)continue;//猜测这一行是分割行
        ss>>s;
        int u=turn(s);st[u]=2;
        if(rl=="1" or rl=="2"){//直接实边与实边相连
            while(ss>>s){
                int v=turn(s);
                if(rl=="1"){//剩下的点都是第一个点的子节点
                    add(u,v);
                    out[u]++,in[v]++;
                }
                else if(rl=="2"){//剩下的点都是第一个点的父节点
                    add(v,u);
                    in[u]++,out[v]++;
                }
                st[v]=2;
            }
        }
        else if(rl=="3"){//这个边是注释
            while(ss>>s){
                int v=turn(s);
                add(u,v);
                in[v]++,out[u]++;
                st[v]=4;//是注释状态
            }
        }
        //框架模式
        else if(rl=="4"){//剩下的点是子节点，通过虚拟店
            stringstream ss2=dt.frame(u);
            while(ss>>s){
                int v=turn(s),w=make(s);//v是真实点,w是虚拟点
                st[v]=2,st[w]=3;
                add(u,w);
                add(w,v);
                out[u]++,in[w]++;//如果真实的v没有入读其他出度，只显示真实v
                ss2<<' '<<w;
            }
            ss2<<"\n}";
            fout<<ss2.str()<<'\n';
        }
        else if(rl=="5"){//剩下父节点，通过虚拟
            int w=make(s);
            stringstream ss2=dt.frame(w);
            while(ss>>s){
                int v=turn(s);//v是真实点,w是虚拟点
                st[v]=2,st[w]=3;
                add(v,w);
                add(w,u);
                out[v]++,in[w]++;//如果真实的v没有入读和其他出度，那么只会显示虚拟v
                ss2<<' '<<v;
            }
            ss2<<"\n}";
            fout<<ss2.str()<<'\n';
        }
        //流程图模式+框架
        else if(rl=="6" or rl=="7"){
            stringstream ss2=dt.frame(u);
            stringstream ss3=dt.sRank();//水平or竖直
            while(ss>>s){
                int v=make(s),w=turn(s);
                st[v]=3;
                add(u,v);
                add(v,w);
                out[u]++,in[v]++;
                ss2<<' '<<v;
                if(rl=="7"){
                    ss3<<v<<";";
                }
                u=v;
            }
            
            if(rl=="7")ss2<<ss3.str()<<"}\n";//框架中的点rank相同
            ss2<<"\n}";
            fout<<ss2.str()<<'\n';
        }
        else if(rl=="8"){//图片状态
            while(ss>>s){
                int v=turn(s);
                add(u,v);
                in[v]++,out[u]++;
                st[v]=5;//图片状态
            }
        }
    }

    for(int i=1;i<=txtCnt;i++){
        if(in[i]!=0 or out[i]!=0){//这个点有效
            string s;
            if(st[i]==2)s=dt.pt(i);//真实点
            else if(st[i]==3)s=dt.vpt(i);//虚拟店
            else if(st[i]==4)s=dt.comt(i);//注释点
            else if(st[i]==5)s=dt.pic(i);//图片
            fout<<s<<'\n';
            if(!vis[i])bfs(i);
        }
    }

    fout<<dt.end();
    fin.close();fout.close();
    string f="dot -Gfontnames=\""+fontName+"\" -Nfontname=\""+fontName+"\" -Tpng:cairo:cairo "+path2+" -o "+path3;
    //LaTeX渲染
    
    system(f.c_str());
}
int main(){
    DIR* dir;
    struct dirent* entry;
    dir=opendir(fName1.c_str());
    queue<string>q;
    if(dir!=nullptr){
        while((entry= readdir(dir))!= nullptr){
            if(entry->d_type== DT_REG){//只要文件，过滤掉.和..那些文件夹
                q.push(entry->d_name);
            }
        }
        closedir(dir);
    }
    while(!q.empty()){
        //cout<<q.front()<<'\n';
        solve(q.front());
        q.pop();
    }
    return 0;
}
