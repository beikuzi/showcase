#include<iostream>
#include<cstdio>
#include<algorithm>
#include<stack>
#include<queue>
#include<map>
#include<set>
#include<vector>
#include<string>
#include<cstring>
#include<ctime>
#include<cstdlib>
#include<iomanip>
#include<fstream>
#include<sstream>
using namespace std;
ofstream fout;
void tst(string s){
    //cout<<time(nullptr)<<'\n';
    //fout<<"处理： "<<s<<'\n';
    cout<<"处理： "<<s<<'\n';
}
struct edge{
    int u,v,next;
    int Tim,Cost,Walk,Cnt;
};
vector<edge>e,//用来跑图的边
ebus,//第一题的边
eTim,//第二题，地铁以时间为第一优先度进行图重构之后，汽车+地铁所有边
eWalk;//第三题，步行+乘坐第1的重构

const int maxn=2e6,maxm=5e6;
vector<int>head,head1,head2,head3;//跑图头边，第1，2，3题的头个边
//int head[maxn],head1[maxn],head2[maxn];
int k,k1,k2,k3;
void add(int u,int v,int Tim,int Cost,int Walk,int Cnt){
    edge etemp;
    etemp.next=head[u];
    head[u]=++k;
    etemp.u=u,etemp.v=v,
    etemp.Tim=Tim,etemp.Cost=Cost,etemp.Walk=Walk,etemp.Cnt=Cnt;
    e.push_back(etemp);
}
void add(int u,edge etemp){
    etemp.u=u;
    etemp.next=head[u];
    head[u]=++k;
    e.push_back(etemp);
}


typedef pair<int,int> pii;
map<pii,int>TP;//transportMap，交通映射
//1.将地铁路线T，地铁站点D映射成点对p
//2.或者将-1，公交路线L映射成点对p
map<int,pii>RTP;//reverseTransportMap，逆还原
int turn(int x,int y){//生成一个新的映射交通点
    static int Tcnt=0;//交通点数量
    if(TP[{x,y}])return TP[{x,y}];
    RTP[++Tcnt]={x,y};
    return TP[{x,y}]=Tcnt;
}

map<pii,int>PP;//planeMap，平面点映射
map<int,pii>RPP;//逆还原
int Pcnt=0;//总共有多少个虚拟点
int turn2(int S,int tp){//公交站和交通点，生成一个新的映射平面点
    if(PP[{S,tp}])return PP[{S,tp}];
    RPP[++Pcnt]={S,tp};
    return PP[{S,tp}]=Pcnt;
}


struct text{//文本
    ifstream fin;
    string file1="1.1 公汽线路信息.txt";
    string file2="1.2 地铁线路信息.txt";
    string file3="2.1 地铁T1线换乘公汽信息.txt";
    string file4="2.2 地铁T2线换乘公汽信息.txt";
    static const int maxL=1000,maxM=100;
   
    string Lmsg[maxL][3];//文本1读入
    string Mmsg[maxM][3];//文本2读入
    vector<string>Dmsg[maxM];//文本3和文本4读入，不同的地铁站点D存在的站点S
    int Lcnt=0;//读入有多少条公交车路线
    int Mcnt=0;//有多少条地铁路线
    int Scnt=0;//经过读入处理后，得出来有多少个汽车站点S
    int Dcnt=0;//有多少个地铁站点D
    void read(){
        fin.open(file1);
        string s;
        while(getline(fin,s)){
            if(s[0]!='L')break;
            Lcnt++;
            for(int i=0;i<3;i++){
                getline(fin,s);
                Lmsg[Lcnt][i]=s;
            }
        }
        fin.close();fin.open(file2);
        while(getline(fin,s)){
            if(s[0]!='T')break;
            Mcnt++;
            for(int i=0;i<3;i++){
                getline(fin,s);
                Mmsg[Mcnt][i]=s;
            }
        }
        fin.close();fin.open(file3);
        int now=1;//第一条地铁路线
        while(getline(fin,s)){
            if(s[0]!='D')break;
            Dmsg[now].push_back(s);

        }
        fin.close();fin.open(file4);
        now=2;//第二条地铁路线
        while(getline(fin,s)){
            if(s[0]!='D')break;
            Dmsg[now].push_back(s);
        }
        fin.close();
    }
}txt;

int sget(string s="缺省"){//从这个字符串中获取数字，直到读入字符串结束
    static string sread;
    static int now=0;
    static int cnt=0;
    if(s!="缺省"){//初始化
        sread=s;
        now=0;
        return -1;
    }
    int res=0;
    bool flag=0;
    while(sread[now]<'0' or sread[now]>'9'){//将所有非数字去掉
        if(sread[now]=='\0')break;
        now++;
    }
    while(sread[now]>='0' and sread[now]<='9'){
        if(sread[now]=='\0')break;
        flag=1;
        res=(res<<3)+(res<<1)+sread[now]-48;//res*10+sread[now]
        now++;
    }
    if(sread[now]=='\0'){//通过让sread清空，来表示已经读取完所有的数据
        sread="";
        now=0;
    }
    if(flag)return res;
    return -1;
}



const int maxs=5000;//数据中共有4000个S站点
//SL使用于源点汇点，SL和SM组合使用于地铁和公交换乘,MS用于站点通过地铁地上通达换站
set<int>SL[maxs];//与经过S站点的公交L有哪些

int addcnt=0;
void Bus(int L,int cas,int dir,string msg){//第L条公交车，计价模式cas和方向dir
    sget(msg);
    vector<int>vt;
    int res=0,len=0;
    while((res=sget())!=-1){
        vt.push_back(res);
        len++;
        SL[res].insert(L);
        txt.Scnt=max(txt.Scnt,res);
    }
    
    for(int i=0;i<len;i++){
        for(int j=i+1;j<len;j++){
            edge etemp;//转乘cnt=0
            etemp.Cnt=0;
            int dis=(dir==3)?min(j-i,len-(j-i)-1):j-i;//距离站数
            //环形min(j-i,len-(j-i)-1)，-1是因为文本额外将头尾给出，其他j-i，自身相连无所谓
            int u=turn2(vt[i],turn(-1,L));
            int v=turn2(vt[j],turn(-1,L));
            etemp.v=v;
            etemp.Tim=dis*3*2;//时间都算2倍来去掉小数
            etemp.Walk=etemp.Tim+0;//无步行时间，总时间就是Tim
            if(cas==1)etemp.Cost=1;//同一1元
            else{
                int ticket;
                if(dis<21)ticket=1;
                else if(dis<41)ticket=2;
                else ticket=3;
                etemp.Cost=ticket;
            }
            add(u,etemp);
            addcnt++;
            if(dir==3 or dir==4){//对于双向，两个点之间的计算值一样，直接反相边
                etemp.v=u;
                add(v,etemp);
                addcnt++;
            }
        }
    } 
}

void dealBus(){//处理公交车移动,当选择第三题时，要一并算入步行时间和转乘
    int Lcnt=txt.Lcnt;//公交路线数
    int cas=0;//计价模式
    string temp;
    for(int i=1;i<=Lcnt;i++){
        temp=txt.Lmsg[i][0].substr(0,6);
        if(temp=="单一"){
            cas=1;
        }
        else if(temp=="分段"){
            cas=2;
        }
        else{
            cout<<"数据格式错误: "<<Lcnt<<' '<<txt.Lmsg[Lcnt][0]<<'\n';
            continue;
        }
        temp=txt.Lmsg[i][1].substr(0,6);
        if(temp=="上行"){
            Bus(i,cas,1,txt.Lmsg[i][1]);
            Bus(i,cas,2,txt.Lmsg[i][2]);
        }
        else if(temp=="环形"){
            Bus(i,cas,3,txt.Lmsg[i][1]);
        }
        else{
            Bus(i,cas,4,txt.Lmsg[i][1]);
        }
    }
}

void dealBus2(){//处理公交车换乘,当选择第三题时，要一并算入步行时间和转乘
    edge etemp;
    int maxS=txt.Scnt;
    etemp.Cost=0;//没有换乘费用
    etemp.Tim=5*2;//汽车换乘等待5s，所有时间*2
    etemp.Cnt=1;//换乘1次
    etemp.Walk=7*2;//步行+换乘7分钟
    set<int>::iterator it1,it2;
    for(int i=1;i<=maxS;i++){//同一位置的公交车两两互通换乘
        for(it1=SL[i].begin();it1!=SL[i].end();++it1){
            for(it2=SL[i].begin();it2!=SL[i].end();++it2){
                if((*it1)==(*it2))continue;
                //cout<<(*it1)<<' '<<(*it2)<<'\n';
                int u=turn2(i,turn(-1,(*it1)));
                int v=turn2(i,turn(-1,(*it2)));
                etemp.v=v;
                add(u,etemp);
                addcnt++;
            }
        }
    }
}

const int maxd=maxs;
set<int>DT[maxd];//记录地铁站点D有哪些地铁路线经过
set<int>DS[maxd];//记录地铁站点D有哪些汽车站点S
set<int>SM[maxs];//经过S站点的地铁M(T,D)有哪些
set<int>sNode;//记录了所有的(S,(T,D))点
//set<int>MS[maxs];//与地铁口M相连的S有哪些

void Metro(int T,int cas,int dir,string msg){//第T条地铁，计价模式cas和方向dir
    sget(msg);
    vector<int>vt;
    int res=0,len=0;
    while((res=sget())!=-1){
        vt.push_back(res);
        len++;
        DT[res].insert(T);//记录这个站点
        txt.Dcnt=max(txt.Dcnt,res);
    }
    edge etemp;
    etemp.Tim=5;//2.5*2
    etemp.Cost=0;
    etemp.Walk=etemp.Tim+0;
    etemp.Cnt=0;
    if(dir==1 or dir==2 or dir==3 or dir==4){//向右侧相连
        for(int i=0;i<len-1;i++){
            int u=turn2(-1,turn(T,vt[i]));
            int v=turn2(-1,turn(T,vt[i+1]));
            etemp.v=v;
            add(u,etemp);
            addcnt++;
        }
    }
    if(dir==3 or dir==4){//向左侧相连
        for(int i=len-1;i>0;i--){
            int u=turn2(-1,turn(T,vt[i]));
            int v=turn2(-1,turn(T,vt[i-1]));
            etemp.v=v;
            add(u,etemp);
            addcnt++;
        }
    }
}

void dealMetro(){//实现地铁行驶边建立和地铁换乘边建立
    int Mcnt=txt.Mcnt;//地铁路线数
    int cas=0;//计价模式
    string temp;
    //(-1,T,D)之间相连
    for(int i=1;i<=Mcnt;i++){
        temp=txt.Mmsg[i][0].substr(0,6);
        if(temp=="票价"){//统一3元
            cas=1;
        }
        else{
            cout<<"数据格式错误: "<<i<<' '<<txt.Mmsg[Mcnt][0]<<'\n';
            continue;
        }
        temp=txt.Mmsg[i][1].substr(0,6);
        if(temp=="上行"){
            Metro(i,cas,1,txt.Mmsg[i][1]);
            Metro(i,cas,2,txt.Mmsg[i][2]);
        }
        else if(temp=="环形"){
            Metro(i,cas,3,txt.Mmsg[i][1]);
        }
        else{
            Metro(i,cas,4,txt.Mmsg[i][1]);
        }
    }

    //(-1,T,D)与(S,T,D)相连
    for(int i=1;i<=Mcnt;i++){//读取并记录所有与D相连的S
        vector<string>dmsg=txt.Dmsg[i];//第i条地铁信息
        int len=dmsg.size();
        for(int j=0;j<len;j++){
            sget(dmsg[j]);
            int D=sget();
            int S=-1;
            while((S=sget())!=-1){
                DS[D].insert(S);
            }
        }
    }
    set<int>::iterator it1,it2,it3;
    int Dcnt=txt.Dcnt;
    for(int i=1;i<=Dcnt;i++){//内部已经重构
        for(it1=DT[i].begin();it1!=DT[i].end();it1++){
            for(it2=DS[i].begin();it2!=DS[i].end();it2++){
                int D=i,T=*it1,S=*it2;
                SM[S].insert(turn(T,D));
                txt.Scnt=max(txt.Scnt,S);
                int u=turn2(S,turn(T,D));
                sNode.insert(u);
                int v=turn2(-1,turn(T,D));
                add(u,v,0,0,0,0);
                add(v,u,0,0,0,0);
                addcnt+=2;
            }
        }
    }
}

void dealMetro2(){//地铁换乘
    set<int>::iterator it1,it2;
    edge etemp;
    etemp.Tim=4*2;//换乘4分钟
    etemp.Walk=6*2;//加上步行换乘6分钟
    etemp.Cnt=1;
    etemp.Cost=0;
    int Dcnt=txt.Dcnt;
    for(int i=1;i<=Dcnt;i++){
        for(it1=DT[i].begin();it1!=DT[i].end();it1++){
            for(it2=DT[i].begin();it2!=DT[i].end();it2++){
                if((*it1)==(*it2))continue;
                int u=turn2(-1,turn(*it1,i));
                int v=turn2(-1,turn(*it2,i));
                etemp.v=v;add(u,etemp);
                etemp.v=u;add(v,etemp);//换乘双向
                addcnt+=2;
            }
        }
    }
}

void dealBus_Metro(){//处理公交和地铁的换乘,以及通过相同地铁口来到达不同公交点
    int Scnt=txt.Scnt;
    set<int>::iterator it1,it2;
    edge etemp1,etemp2;
    etemp1.Cost=etemp2.Cost=0;
    etemp1.Cnt=etemp2.Cnt=1;
    etemp1.Tim=7*2;//从地铁到公交
    etemp1.Walk=(7+4)*2;
    etemp2.Tim=6*2;
    etemp2.Walk=(6+4)*2;

    for(int i=1;i<=Scnt;i++){
        for(it1=SL[i].begin();it1!=SL[i].end();it1++){
            for(it2=SM[i].begin();it2!=SM[i].end();it2++){
                int u=turn2(i,turn(-1,(*it1)));
                int v=turn2(i,(*it2));
                etemp1.v=u;//到公交
                etemp2.v=v;//到地铁
                add(v,etemp1);
                add(u,etemp2);
                addcnt+=2;
            }
        }
    }
}


struct node{//映射的平面虚拟点最终跑出来的距离
    int id;
    int Tim,Cost,Walk,Cnt;
}nd[maxn];

struct sortTim{//时间优先
    bool operator()(const node &n1,const node &n2){
        if(n1.Tim!=n2.Tim)return n1.Tim>n2.Tim;
        if(n1.Cost!=n2.Cost)return n1.Cost>n2.Cost;
        if(n1.Cnt!=n2.Cnt)return n1.Cnt>n2.Cnt;
        return false;
    }
};
struct sortCost{//不考虑步行的价格优先
    bool operator()(const node &n1,const node &n2){
        if(n1.Cost!=n2.Cost)return n1.Cost>n2.Cost;
        if(n1.Tim!=n2.Tim)return n1.Tim>n2.Tim;
        if(n1.Cnt!=n2.Cnt)return n1.Cnt>n2.Cnt;
        return false;
    }
};
struct sortCost2{//考虑步行的价格优先
    bool operator()(const node &n1,const node &n2){
        if(n1.Cost!=n2.Cost)return n1.Cost>n2.Cost;
        if(n1.Walk!=n2.Walk)return n1.Walk>n2.Walk;
        if(n1.Tim!=n2.Tim)return n1.Tim>n2.Tim;
        if(n1.Cnt!=n2.Cnt)return n1.Cnt>n2.Cnt;
        return false;
    }
};
struct sortWalk{//步行优先
    bool operator()(const node &n1,const node &n2){
        if(n1.Walk!=n2.Walk)return n1.Walk>n2.Walk;
        if(n1.Tim!=n2.Tim)return n1.Tim>n2.Tim;
        if(n1.Cost!=n2.Cost)return n1.Cost>n2.Cost;
        if(n1.Cnt!=n2.Cnt)return n1.Cnt>n2.Cnt;
        return false;
    }
};

int preu[maxn],prew[maxn];
int SP,EP;//输入起点终点
int ST,ED;//重映射后跑图的起点终点
int vis[maxn];
void dijk_init(){
    memset(vis,0,sizeof(vis));
    memset(nd,0x3f,sizeof(nd));
    memset(preu,-1,sizeof(preu));
    memset(prew,-1,sizeof(prew));
    nd[ST]={0,0,0,0,0};
    nd[ST].id=ST;
}

bool dijkTim(){
    dijk_init();
    priority_queue<node,vector<node>,sortTim>q;
    q.push(nd[ST]);
    while(!q.empty()){
        node u=q.top();q.pop();
        // cout<<u.id<<'\n';
        if(vis[u.id])continue;vis[u.id]=1;
        if(u.id==ED)return 1;
        for(int i=head[u.id];~i;i=e[i].next){
            int vv=e[i].v;
            node v=nd[vv];
            edge w=e[i];
            if(v.Tim>u.Tim+w.Tim){
                v.id=vv;
                v.Tim=u.Tim+w.Tim;
                v.Cost=u.Cost+w.Cost;
                v.Walk=u.Walk+w.Walk;
                v.Cnt=u.Cnt+w.Cnt;
                nd[vv]=v;
                q.push(v);
                preu[vv]=u.id,prew[vv]=i;//记录前向
            }
        }
    }
    return 0;
}
bool dijkCost(){
    dijk_init();
    priority_queue<node,vector<node>,sortCost>q;
    q.push(nd[ST]);
    while(!q.empty()){
        node u=q.top();q.pop();
        // cout<<u.id<<'\n';
        if(vis[u.id])continue;vis[u.id]=1;
        if(u.id==ED)return 1;

        for(int i=head[u.id];~i;i=e[i].next){
            int vv=e[i].v;
            node v=nd[vv];
            edge w=e[i];
            //cout<<u.id<<' '<<u.Tim<<' '<<v.id<<' '<<v.Tim<<' '<<Tim<<'\n';
            if(v.Cost>u.Cost+w.Cost){
                v.id=vv;
                v.Tim=u.Tim+w.Tim;
                v.Cost=u.Cost+w.Cost;
                v.Walk=u.Walk+w.Walk;
                v.Cnt=u.Cnt+w.Cnt;
                nd[vv]=v;
                q.push(v);
                preu[vv]=u.id,prew[vv]=i;//记录前向
            }
        }
    }
    return 0;
}
bool dijkWalk(){
    dijk_init();
    priority_queue<node,vector<node>,sortWalk>q;
    q.push(nd[ST]);
    while(!q.empty()){
        node u=q.top();q.pop();
        if(vis[u.id])continue;vis[u.id]=1;
        if(u.id==ED)return 1;

        for(int i=head[u.id];~i;i=e[i].next){
            int vv=e[i].v;
            edge w=e[i];
            node v=nd[vv];
            //cout<<u.id<<' '<<u.Tim<<' '<<v.id<<' '<<v.Tim<<' '<<Tim<<'\n';
            if(v.Walk>u.Walk+w.Walk){
                v.id=vv;
                v.Tim=u.Tim+w.Tim;
                v.Cost=u.Cost+w.Cost;
                v.Walk=u.Walk+w.Walk;
                v.Cnt=u.Cnt+w.Cnt;
                nd[vv]=v;
                q.push(v);
                preu[vv]=u.id,prew[vv]=i;//记录前向
            }
        }
    }
    return 0;
}

bool dijkCost2(){
    dijk_init();
    priority_queue<node,vector<node>,sortCost2>q;
    q.push(nd[ST]);
    while(!q.empty()){
        node u=q.top();q.pop();
        // cout<<u.id<<'\n';
        if(vis[u.id])continue;vis[u.id]=1;
        if(u.id==ED)return 1;

        for(int i=head[u.id];~i;i=e[i].next){
            int vv=e[i].v;
            node v=nd[vv];
            edge w=e[i];
            //cout<<u.id<<' '<<u.Tim<<' '<<v.id<<' '<<v.Tim<<' '<<Tim<<'\n';
            if(v.Cost>u.Cost+w.Cost){
                v.id=vv;
                v.Tim=u.Tim+w.Tim;
                v.Cost=u.Cost+w.Cost;
                v.Walk=u.Walk+w.Walk;
                v.Cnt=u.Cnt+w.Cnt;
                nd[vv]=v;
                q.push(v);
                preu[vv]=u.id,prew[vv]=i;//记录前向
            }
        }
    }
    return 0;
}

void dijk2(){//第二题的地铁小模型跑图
    set<int>::iterator it;
    for(it=sNode.begin();it!=sNode.end();it++){
        ST=*it;
        dijk_init();
        priority_queue<node,vector<node>,sortTim>q;
        q.push(nd[ST]);
        while(!q.empty()){
            node u=q.top();q.pop();
            if(vis[u.id])continue;vis[u.id]=1;
            if(RPP[u.id].first!=-1 and u.id!=ST){
                edge etemp;
                etemp.u=ST;
                etemp.v=u.id;
                etemp.Cnt=nd[u.id].Cnt;
                etemp.Tim=nd[u.id].Tim;
                etemp.Walk=nd[u.id].Walk;
                etemp.next=head2[ST];
                etemp.Cost=3;//固定3元
                //如果这两个站是连同一个地铁口d，他们的费用设为多少。这里设无过闸费=0
                if(RTP[RPP[ST].second].second==RTP[RPP[u.id].second].second){
                    etemp.Cost=0;
                }
                head2[ST]=++k2;
                eTim.push_back(etemp);
                addcnt++;
            }
            for(int i=head[u.id];~i;i=e[i].next){
                int vv=e[i].v;
                node v=nd[vv];
                edge w=e[i];
                if(v.Tim>u.Tim+w.Tim){
                    v.id=vv;
                    v.Tim=u.Tim+w.Tim;
                    v.Cost=u.Cost+w.Cost;
                    v.Walk=u.Walk+w.Walk;
                    v.Cnt=u.Cnt+w.Cnt;
                    nd[vv]=v;
                    q.push(v);
                }
            }
        }
    }
}
void dijk3(){//第三题的地铁小模型跑图
    set<int>::iterator it;
    for(it=sNode.begin();it!=sNode.end();it++){
        ST=*it;
        dijk_init();
        priority_queue<node,vector<node>,sortWalk>q;
        q.push(nd[ST]);
        
        while(!q.empty()){
            node u=q.top();q.pop();
            if(vis[u.id])continue;vis[u.id]=1;
            if(RPP[u.id].first!=-1 and u.id!=ST){
                edge etemp;
                etemp.u=ST;
                etemp.v=u.id;
                etemp.Cnt=nd[u.id].Cnt;
                etemp.Tim=nd[u.id].Tim;
                etemp.Walk=nd[u.id].Walk;
                etemp.next=head3[ST];
                etemp.Cost=3;//固定3元
                //如果这两个站是连同一个地铁口d，他们的费用设为多少。这里设无过闸费=0
                if(RTP[RPP[ST].second].second==RTP[RPP[u.id].second].second){
                    etemp.Cost=0;
                }
                head3[ST]=++k3;
                eWalk.push_back(etemp);
                addcnt++;
            }
            for(int i=head[u.id];~i;i=e[i].next){
                int vv=e[i].v;
                node v=nd[vv];
                edge w=e[i];
                if(v.Walk>u.Walk+w.Walk){
                    v.id=vv;
                    v.Tim=u.Tim+w.Tim;
                    v.Cost=u.Cost+w.Cost;
                    v.Walk=u.Walk+w.Walk;
                    v.Cnt=u.Cnt+w.Cnt;
                    nd[vv]=v;
                    q.push(v);
                }
            }
        }
    }
}

stack<int>pre1;//前向点，即上一个状态
stack<int>pre2;//前向边，即乘车或者换乘
void path(int v){//路径还原
    pre1.push(v);
    if(v==-1 or v==ST)return;
    pre2.push(prew[v]);
    path(preu[v]);
}
string cplt(int num,int bit) {//to complete，补全
    stringstream ss;
    ss << setw(bit) << setfill('0') << num;
    return ss.str();
}
void pathout(int x){
    path(ED);
    while(!pre1.empty()){
        int u=pre1.top();pre1.pop();
        pii temp1=RPP[u];
        pii temp2=RTP[temp1.second];
        if(temp2.second==-1){
            if(u==ST)cout<<"起点:S"<<cplt(temp1.first,4)<<'\n';
            else cout<<"到达终点站:S"<<cplt(temp1.first,4)<<'\n';
        }
        else if(temp2.first==-1){
            cout<<"站点:S"<<cplt(temp1.first,4)
                <<"     公交:L"<<cplt(temp2.second,3)<<'\n';
        }
        else{
            cout<<"站点:S"<<cplt(temp1.first,4)
                <<"     地铁线路:T"<<cplt(temp2.first,2)
                <<"     地铁口:D"<<cplt(temp2.second,2)<<'\n';
        }
        if(!pre2.empty()){
            int w=pre2.top();pre2.pop();
            pii temp1=RPP[e[w].u];
            pii temp2=RTP[temp1.second];
            
            int u=e[w].u,v=e[w].v;

            cout<<"     [当前: ";
            if(RTP[RPP[u].second].first==-1 and RTP[RPP[v].second].first!=-1){
                cout<<"从公交转乘地铁";
            }
            else if(RTP[RPP[u].second].first!=-1 and RTP[RPP[v].second].first==-1){
                cout<<"从地铁转乘公交";
            }
            else if(RTP[RPP[u].second].first==-1 and RTP[RPP[v].second].first==-1){
                if(RTP[RPP[u].second].second==RTP[RPP[v].second].second){
                    cout<<"在公交L"<<cplt(RTP[RPP[u].second].second,3)<<"上行驶";
                }
                else {
                    if(RTP[RPP[v].second].second==-1)cout<<"下车";
                    else{
                        if(RTP[RPP[u].second].second==-1)cout<<"上汽车";
                        else cout<<"从公交 L"<<cplt(RTP[RPP[u].second].second,3)
                            <<" 换乘到 L"<<cplt(RTP[RPP[v].second].second,3);
                    }
                }
            }else{
                if(RTP[RPP[u].second].second!=RTP[RPP[v].second].second){
                    cout<<"从地铁 T"<<cplt(RTP[RPP[u].second].first,2)
                    <<" 地铁口 D"<<cplt(RTP[RPP[u].second].second,3)
                    <<" 换乘"<<e[w].Cnt<<"次 "
                    <<"到地铁 T"<<cplt(RTP[RPP[v].second].first,2)
                    <<" 地铁口 D"<<cplt(RTP[RPP[v].second].second,3);
                }
                else {
                    cout<<"走地铁通道移动";
                }
            }
            if(x==3)cout<<"     时间(带步行):"<<double(e[w].Walk)/2<<"min";
            cout<<"     时间:"<<double(e[w].Tim)/2.0<<"min  花费:"<<e[w].Cost<<"元 换乘:"<<e[w].Cnt<<"次]\n";
            // if(x==3)
            // cout<<"     总时间(带步行):"<<double(nd[v].Walk)/2.0<<"min";
            // cout<<"     时间:"<<double(nd[v].Tim)/2.0<<"min"
            //     <<"     花费:"<<nd[v].Cost<<"元"
            //     <<"     换乘:"<<nd[v].Cnt<<"次"<<"]\n";
        }
    }
}

vector<edge>eMetro;//地铁图重构的边
vector<int>headMetro;//地图重构的头边
int kMetro;
void init(){
    stringstream ss;

    cout<<"初始化开始"<<'\n';
    k=-1;
    //memset(head,-1,sizeof(head));
    head.resize(maxn,-1);

    tst("读入");
    txt.read();

    tst("公交行驶边换算中");
    dealBus();
    ss<<"建立了"<<addcnt<<"条公交行驶边";
    tst(ss.str());ss.str("");
    addcnt=0;
    
    tst("公交换乘边换算中");
    dealBus2();
    ss<<"建立了"<<addcnt<<"条公交换乘边";
    tst(ss.str());ss.str("");
    addcnt=0;

    ss<<"建立了"<<Pcnt<<"个公交虚拟点";
    tst(ss.str());ss.str("");
//对于不同地铁重构图方式进行分支
/*
    将记有汽车的e传给ebus，eTim，eWalk
    head传给head1，head2，head3
    k也要拷贝

    清空e和head来记录地铁小模型eMetro和headMetro
    2.跑地铁小模型时，使用eMetro和headMetro和kMetro

    跑出来边的记在eTim和head2和k2
    然后将eTim和head2传给e和head
    然后再往上面记载地铁与公交的换乘边
    3同理
*/
    ebus=eTim=eWalk=e;
    head1=head2=head3=head;
    k1=k2=k3=k;

    e.clear();
    head.clear();
    head.resize(maxn,-1);
    k=-1;

    tst("构建地铁小模型");
    tst("正在构建地铁行驶边");
    dealMetro();
    tst("正在构建地铁换乘边");
    dealMetro2();
    eMetro=e;
    headMetro=head;
    kMetro=k;
    
    ss<<"总共有"<<addcnt<<"个地铁小模型构建边";
    tst(ss.str());ss.str("");
    addcnt=0;

    tst("第二问：以乘车时间为优先，重构地铁模型");
    // e=eMetro;
    // head=headMetro;
    // k=kMetro;

    //跑Tim记录在eTim中和head2中
    dijk2();
    ss<<"建立了"<<addcnt<<"个重构边";
    tst(ss.str());ss.str("");
    addcnt=0;

    e=eTim;
    head=head2;
    k=k2;
    dealBus_Metro();
    ss<<"建立了"<<addcnt<<"个地铁-公交换乘边";
    tst(ss.str());ss.str("");
    addcnt=0;

    eTim=e;
    head2=head;
    k2=k;
    
    tst("第三问：以乘车时间+步行时间优先，重构地铁模型");
    e=eMetro;
    head=headMetro;
    k=kMetro;
    //跑Tim记录在eWalk中
    dijk3();
    ss<<"建立了"<<addcnt<<"个重构边";
    tst(ss.str());ss.str("");
    addcnt=0;

    e=eWalk;
    head=head3;
    k=k3;
    dealBus_Metro();
    ss<<"建立了"<<addcnt<<"个地铁-公交换乘边";
    tst(ss.str());ss.str("");
    addcnt=0;

    eWalk=e;
    head3=head;
    k3=k;
    
    eMetro.clear();
    headMetro.clear();

    ss<<"总共有"<<Pcnt<<"个虚拟点";
    tst(ss.str());ss.str("");

    cout<<"初始化完成";
}

void solve(int x){
    if(x==1){
        e=ebus;//第一问，只处理公交站
        head=head1;
        k=k1;
    }
    else if(x==2){//第二问，地铁优先考虑乘车
        e=eTim;
        head=head2;
        k=k2;
    }
    else if(x==3){//第三问，地铁优先考虑步行+乘车，由于题目所给数据较小，实际上二三问的是一样的
        e=eWalk;
        head=head3;
        k=k3;
    }
    set<int>::iterator it;

    ST=turn2(SP,turn(-1,-1));
    ED=turn2(EP,turn(-1,-1));
    
    //对于所有与SL相连的公交，与一个虚拟超级源点ST相连，表示这些车辆开始可上
    //tst("建立超级源汇点\n");
    for(it=SL[SP].begin();it!=SL[SP].end();it++){
        int u=turn2(SP,turn(-1,*it));
        add(ST,u,0,0,0,0);
    }
    for(it=SM[SP].begin();it!=SM[SP].end();it++){//可能会存在只有地铁没有公交，我们先下地铁花费6分钟
        int u=turn2(SP,*it);
        edge etemp;
        etemp.Cost=etemp.Tim=0;
        etemp.Cnt=1;
        etemp.Tim=6*2;
        etemp.Walk=10*2;
        etemp.v=u;
        add(ST,etemp);
    }
    //超级汇点，最终可以在这些车上下来
    for(it=SL[EP].begin();it!=SL[EP].end();it++){
        int v=turn2(EP,turn(-1,*it));
        add(v,ED,0,0,0,0);
    }
    for(it=SM[EP].begin();it!=SM[EP].end();it++){
        int v=turn2(EP,*it);
        edge etemp;
        etemp.Cost=etemp.Tim=0;
        etemp.Cnt=1;
        etemp.Tim=7*2;
        etemp.Walk=11*2;
        etemp.v=ED;
        add(v,etemp);
    }

    bool res=0;
    if(x==1 or x==2)res=dijkTim();
    else res=dijkWalk();
    if(!res){
        cout<<"起点和终点之间不存在可行路!"<<'\n';
        return;
    }
    if(x==1 or x==2)
        cout<<"问题"<<x
            <<"乘车时间最优: "<<double(nd[ED].Tim)/2.0<<"min  "
            <<"花费: "<<nd[ED].Cost<<"元  "
            <<"转乘: "<<nd[ED].Cnt<<"次\n";
    else
        cout<<"问题"<<x
            <<"总时间最优: "<<double(nd[ED].Walk)/2.0<<"min  "
            <<"乘车时间: "<<double(nd[ED].Tim)/2.0<<"min  "
            <<"花费: "<<nd[ED].Cost<<"元  "
            <<"转乘: "<<nd[ED].Cnt<<"次\n";
    pathout(x);
    cout<<"\n";

    if(x==1 or x==2)dijkCost();
    else dijkCost2();
    if(x==1 or x==2)
        cout<<"问题"<<x
        <<"花费最优: "<<nd[ED].Cost<<"元  "
        <<"乘车时间: "<<double(nd[ED].Tim)/2.0<<"min  "
        <<"转乘: "<<nd[ED].Cnt<<"次\n";
    else
        cout<<"问题"<<x
        <<"花费最优: "<<nd[ED].Cost<<"元  "
        <<"总时间: "<<double(nd[ED].Walk)/2.0<<"min  "
        <<"乘车时间: "<<double(nd[ED].Tim)/2.0<<"min  "
        <<"转乘: "<<nd[ED].Cnt<<"次\n";
    pathout(x);
    cout<<"\n";

    return;
}
void run(){
    stringstream ss;
    while(1){
        cout<<"\n请输入起点和终点后回车(两个数字，不带S，两数用空格、回车、TAB任意方式隔开)\n";
        //SP=1557,EP=481;
        cin>>SP>>EP;

        if(SP<=0 or EP<=0 or SP>txt.Scnt or EP>txt.Scnt){
            ss<<"不满足读入数据的站点S范围：1到"<<txt.Scnt<<",将结束";
            tst(ss.str());ss.str("");
            break;
        }
        solve(1);
        solve(2);
        solve(3);
    }
}
int main(){  
    // fout.open("test.txt");
    init();
    run();
    return 0;
}
