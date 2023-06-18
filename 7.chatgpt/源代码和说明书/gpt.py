import requests
import chardet
import time

encoding=None
with open('提问.txt', 'rb') as file:
    raw_data = file.read()
    result = chardet.detect(raw_data)
    encoding = result['encoding']

url = "https://api.openai.com/v1/chat/completions";
apikey="114514";
model = "gpt-3.5-turbo"
tokens = 3000;
setting = "你是程序员";
prompt = "如果你收到这条语句，回复我：gpt收到响应，但是提问文本错误喵";

start_tim=time.time();
def input():
    with open('设定.ini', 'rb') as file:
        raw_data = file.read()
        result = chardet.detect(raw_data)
        encoding = result['encoding']

    with open('设定.ini', 'r', encoding=encoding) as f:
        global url;
        global apikey;
        global tokens;
        global setting;
        global model;
        
        setting="";
        line = f.readline()
        while(line):
            if(line=="apikey:(https://platform.openai.com/account/api-keys)\n"):
                apikey=f.readline().strip();
            if(line=="gpt类型url:(https://platform.openai.com/docs/models/model-endpoint-compatibility)\n"):
                url=f.readline().strip();
            if(line=="gpt模型:\n"):
                model=f.readline().strip();
            if(line=="单次最大输出tokens:($0.002=1000tokens,输入+输出tokens不超过4096)\n"):
                tokens=f.readline();
                tokens=int(tokens);
            if(line=='gpt角色设定:("role":"system","content":)\n'):
                setting = f.read();
            line = f.readline()
    
    
    with open('提问.txt', 'r', encoding=encoding) as f:
        global prompt;
        prompt=f.read();

def output(response):
    global start_tim;
    print(response);
    with open('历史记录.txt', 'rb') as file:
        raw_data = file.read()
        result = chardet.detect(raw_data)
        encoding = result['encoding']
    with open("历史记录.txt", 'r', encoding=encoding) as f:
        context=f.read();
    with open("回答.txt","w") as f:
        print(response['choices'][0]['message']['content'],file=f);
    with open("回答.md","w") as f:
        print(response['choices'][0]['message']['content'],file=f);
    with open("历史记录.txt","w") as f:
        print("提问时间: ",start_tim,file=f);
        print("接收时间: ",time.time(),file=f);
        print("模型: ",response['model'],file=f);
        print("输入tokens: ",response['usage']['prompt_tokens'],file=f);
        print("输出tokens: ",response['usage']['completion_tokens'],file=f);
        print("\n提问: ",file=f);
        print(prompt,file=f);
        print("\n回答: ",file=f);
        print(response['choices'][0]['message']['content'],file=f);
        print("\n------------------------------------------------------------\n",file=f);
        print(context,file=f);

def call_gpt_api():
    #print(apikey);
    #print(setting);
    #print(prompt);
    headers = {
        "Content-Type": "application/json",
        "Authorization": "Bearer "+apikey,
    }
    data = {
        "model": model,
        "max_tokens": tokens, 
        "messages":[
            {"role": "system", "content": setting},
            {"role": "user", "content": prompt}
        ],
    }
    print(data)
    try:
        response = requests.post(url, headers=headers, json=data)
        response.raise_for_status()  # 检查响应状态码，如果不是2xx，则抛出异常
        return response
    except requests.exceptions.RequestException as e:
        print("请求出错:", e)
        return None
    

input();
response = call_gpt_api();
print("已经得到回应")
output(response.json());
print("已经输出")