#include <iostream>
#include <curl/curl.h>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <ctime>
#include <sstream>
#include <queue>
using namespace std;
using nlohmann::json;

ifstream fin;
ofstream fout;

const int maxn=2e6+8;
char response[maxn];

string apikey = "114514";
string url = "https://api.openai.com/v1/chat/completions";
string model = "gpt-3.5-turbo";
int tokens = 3000;
string setting = "你是猫娘";
string prompt = "回复我，测试喵~";
string init_prompt;//还没有转义的原始关键词输入

string in1="设定.ini";
string in2="提问.txt";
string out1="回答.txt";
string out2="回答.md";
string out3="历史记录.txt";

int start_tim;
int end_tim;

string turn(string s) {
    string res="\"";
    int len=s.length();
    for(int i=0;i<len;i++){
        if(s[i]=='\"')res+="\\\"";
        else if(s[i]=='\\' and i+1<len and s[i+1]!='n')res+="\\\\";
        else if(s[i]=='\t')res+="\\t";
        else res+=s[i];
    }
    return res + '\"';
}


size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    char *str = (char*) stream;  
    // 获得传入的字符串指针,通过curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);设置传入指针为response
    strncat(str, (char*)ptr, size * nmemb);  // 将输出内容追加到字符串末尾
    return size * nmemb;
}
void curlset() {
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if(res != CURLE_OK){
        fprintf(stderr, "curl_global_init()初始化失败:%s\n", curl_easy_strerror(res)); // 添加这行代码
        cout<<"curl_global_init初始化失败"; 
    }

    CURL *curl = curl_easy_init();
    if (curl) {
        // 设置请求的 URL 地址
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // 设置 POST 请求
        curl_easy_setopt(curl, CURLOPT_POST, 1L);

        // 设置 API 请求所需的 HTTP 头信息
        curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + apikey).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        //设置 data 信息
        string data;
        data += "{";
        data += turn("model") + ": " + turn(model) + ", ";
        data += turn("max_tokens") + ": " + to_string(tokens) + ", ";
        data += turn("messages") + ": [" +
                R"({"role": "system", "content": )"+turn(setting)+"}, "+
                R"({"role": "user", "content": )"+turn(prompt)+"}]";
        data += "}";

        cout << data << '\n';
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

        // 设置Header和Body的接收地址，CURLOPT_WRITEFUNCTION的第4个参数
        //curl_easy_setopt(curl, CURLOPT_HEADERDATA, stdout);
        

        // 执行 API 请求
        curl_easy_perform(curl);
        return;
    }
    cout<<"curl_easy_init初始化失败"<<'\n';
}
void input(){
    fin.open("设定.ini");
    string s;
    while(getline(fin,s)){
        if(s=="apikey:(https://platform.openai.com/account/api-keys)"){
            fin>>apikey;
        }
        if(s=="gpt类型url:(https://platform.openai.com/docs/models/model-endpoint-compatibility)"){
            fin>>url;
        }
        if(s=="gpt模型:"){
            fin>>model;
        }
        if(s=="单次最大输出tokens:($0.002=1000tokens,输入+输出tokens不超过4096)"){
            fin>>tokens;
        }
        if(s==R"(gpt角色设定:("role":"system","content":))"){
            setting="";
            while(getline(fin,s)){
                setting+=s+"\\n";
            }
        }
    }
    fin.close();


    fin.open("提问.txt");
    prompt="";
    while(getline(fin,s)){
        init_prompt+=s+"\n";
        prompt+=s+"\\n";
    }

    fin.close();
}
void output() {
    end_tim=time(nullptr);
    string s=response;
    cout<<s<<"\n\n"<<"正在输出到文本……";
    // 解析json字符串
    json j = json::parse(s);
    int prompt_tokens = j["usage"]["prompt_tokens"];
    int completion_tokens = j["usage"]["completion_tokens"];
    string model=j["model"];
    string message = j["choices"][0]["message"]["content"];
    fout.open("回答.txt");
    fout<<message;
    fout.close();
    fout.open("回答.md");
    fout<<message;
    fout.close();

    queue<string>q;
    fin.open("历史记录.txt");
    stringstream ss;
    ss<<fin.rdbuf();
    fin.close();
    fout.open("历史记录.txt");
    fout<<"提问时间: "<<start_tim<<'\n';
    fout<<"接收时间: "<<end_tim<<'\n';
    fout<<"模型: "<<model<<"\n\n";
    fout<<"输入tokens: "<<prompt_tokens<<'\n';
    fout<<"输出tokens: "<<completion_tokens<<'\n';
    fout<<"提问: \n"<<init_prompt<<'\n';
    fout<<"回答: \n"<<message<<'\n';
    fout<<"------------------------------------------------------------\n";
    fout<<ss.str();
}

int main() {    
    start_tim=time(nullptr);
    input();
    curlset();
    output();
    return 0;
}
