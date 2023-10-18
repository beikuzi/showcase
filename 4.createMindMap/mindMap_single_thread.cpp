
#include <algorithm>
#include <cmath>
#include <codecvt>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fstream>
#include <functional> //bind
#include <iostream>
#include <locale> //解决中文乱码
#include <map>
#include <memory>
#include <queue>
#include <sstream>
#include <stack>

using namespace std;
string fontName = "STHeiti"; // 使用字体，不然windows下可能显示不了中文
void test(string s) {
	// cout<<"测试："<<s<<"\n";
}
string color[]{
	"white",   // 表格底色
	"#A9CCE3", // 背景色,灰蓝
	"#FF8C00", // 框架色，橙色
};
struct stringProcess {
	const double amp = 3;  // 根据文本长度,来限定长度上限时的放大系数
	const bool setLen = 0; // 为true时，为表格内所有文字设定换行字数
	const int maxLineLen = 50;
	int upLen(int len) { // 字符串换行长度计算
		if (setLen) return maxLineLen;
		return sqrt(len) * amp;
	}
	string ridSuf(const string &s) { // 去掉最后一个后缀
		string res = s;
		int len = res.length();
		for (int i = len - 1; i >= 0; i--) {
			if (res[i] == '.') {
				len = i;
				break;
			}
		}
		res = res.substr(0, len);
		return res;
	}
	string split(int len, const string &s) { // 每隔指定长度插入\n来进行拆分
		string res = s;
		int pos = 0, last = 0;
		int hanzi = 0; // 记录当前是汉字的第几部分
		// 对于代码块本身自带换行\n，因此遇到\n的时候重新统计
		while (pos < res.length()) {
			if (res[pos] == '\n') {
				last = pos;
				pos++;
				continue;
			}
			if (res[pos] < 0) { // 汉字部分
				hanzi = (hanzi + 1) % 3;
			}
			// 有两种情况，要么此时有新汉字有记录，要么没汉字没记录
			if (pos - last > len * 2 and hanzi == 0 and pos != res.length() - 1) {
				res.insert(pos + 1, "\n");
				pos += 1;
				last = pos;
			}
			pos++;
		}
		test("split结束");
		return res;
	}
	string cut(int len, const string &s) {
		// 在考虑汉字的情况下，尽量将字符串切割在len长度附近
		string res = s;
		int pos = 0;
		int hanzi = 0;
		int length = s.length();
		while (pos < length) {
			if (res[pos] < 0) { // 汉字部分
				hanzi = (hanzi + 1) % 3;
			}
			if (pos >= len - 1 and hanzi == 0 and pos != length - 1) {
				res = res.substr(0, pos + 1);
				break;
			}
			pos++;
		}
		test("cnt结束");
		return res;
	}
	string deal(const string &s) { // 当需要保留原格式时，转html语义
		string res = s;
		int pos = 0;
		while (pos < res.length()) {
			if (res[pos] == ' ') {
				res.replace(pos, 1, "&nbsp;");
				pos += 6;
			} else if (res[pos] == '"') {
				res.replace(pos, 1, "&quot;");
				pos += 6;
			} else if (res[pos] == '\t') {
				// res.replace(pos, 1, "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
				// pos += 6 * 8;
				res.replace(pos, 1, "&#9;");
				pos += 4;
			} else if (res[pos] == '<') {
				res.replace(pos, 1, "&lt;");
				pos += 4;
			} else if (res[pos] == '>') {
				res.replace(pos, 1, "&gt;");
				pos += 4;
			} else if (res[pos] == '{') {
				res.replace(pos, 1, "&#123;");
				pos += 6;
			} else if (res[pos] == '}') {
				res.replace(pos, 1, "&#125;");
				pos += 6;
			} else if (res[pos] == '\\') {
				res.replace(pos, 1, "\\\\");
				pos += 2;
			} else if (res[pos] == '\n') {
				res.replace(pos, 1, "\\l");
				pos += 2;
			} else {
				pos++;
			}
		}
		test("deal结束");
		return res;
	}
} str;

struct graph {
	struct edge {
		int next, v;
	} empty;
	vector<edge> e;
	vector<int> head;
	vector<int> out; // 出度
	int k = -1;
	void add(int u, int v) {
		e.push_back(empty);
		if (v >= head.size()) { // 前提是v一定比u大，因为所有点都要扫head和out
			head.resize(v + 1, -1);
			out.resize(v + 1, 0);
		}
		e[++k].next = head[u];
		e[k].v = v;
		head[u] = k;
		out[u]++;
	}
};

struct DOT {
	int edgeId = 0;
	string head(string col) {
		stringstream ss;
		ss << "digraph g{\n"
		   << "graph [\n"
		   << "\trankdir=\"LR\";\n" // 设置图的排列方向为从左到右
		   << "\tbgcolor=\"" << col << "\"\n"
		   << "\tsplines=false;\n"
		   << "];\n"
		   << "node [\n"
		   << "\tstyle = \"filled\"\n"
		   << "\tshape = \"record\"\n"
		   << "\tfillcolor = \"" << color[0] << "\"\n"
		   << "];\n"
		   << "edge [\n"
		   //<<"\tconstraint=\"false\"\n"
		   << "\tcolor=\"red\"\n"
		   << "];\n";
		return ss.str();
	}
	string end() {
		string s = "}\n";
		return s;
	}
	/*
	"node1" [
		label = "<0> 0xf7fc4380| <f1> | <f2> |-1"
	];
	*/
	string node(int x, string s) { // 节点编号和输出内容
		stringstream ss;
		ss << "\"node" << x << "\" [\n"
		   << "label = \"" << s << "\"\n"
		   << "];\n";
		return ss.str();
	}
	/*
	"node0":0 -> "node1":0 [
		id = 0
	];
	*/
	string edge(int u, int v) { // 在bfs里判断是否连边
		stringstream ss;
		ss << "\"node" << u << "\":f" << v << " -> "
		   << "\"node" << v << "\":f0 [\n"
		   << "id = " << edgeId++ << '\n'
		   << "];\n";
		return ss.str();
	}
	// string frame(int u,const stack<int>& s,const unique_ptr<graph>&
	// g){//建立框架,但是out为0的不包括进来
	//     stack<int>st=s;
	//     stringstream ss;
	//     ss<<"subgraph cluster_"<<u<<"{\n"
	//     <<"\tcolor=\""<<color[2]<<"\"\n"
	//     <<"\t";
	//     while(!st.empty()){
	//         int v=st.top();st.pop();
	//         ss<<"node"<<v<<' ';
	//     }
	//     ss<<"\n}\n";
	//     return ss.str();
	// }//框架也解决不了线遮挡文本的问题，不需要框架模式
} dot;

struct mindMap {
	string codeblock(ifstream &fin) { // 用于处理代码块的读入
		string res = "", input = "";
		while (getline(fin, input)) {
			if (input.find("```") != string::npos) break;
			res = res + input + "\n";
		}
		return res;
	}
	void bfs(const unique_ptr<graph> &g, map<int, string> &pId, ofstream &fout) { // 生成dot
		queue<int> q;
		q.push(0);
		queue<string> ed; // 边在结点后输出
		while (!q.empty()) {
			int u = q.front();
			q.pop();
			int maxlen = min((int)pId[u].length(), 20); // 当前节点和子节点的最大文本长度
			// 第一遍先从当前节点和子节点得到最大文本长度,以及建立有向边
			// 同时，由于链向式导致顺序相反，使用栈压入一次来回正顺序
			stack<int> s;
			test("bfs第1部分");
			for (int i = g->head[u]; ~i; i = g->e[i].next) {
				int v = g->e[i].v;
				maxlen = max(maxlen, int(pId[v].length()));

				if (g->out[v] != 0) {
					ed.push(dot.edge(u, v));
					q.push(v);
				}
				s.push(v);
			}
			int len = sqrt(maxlen) * str.amp;

			/*
			"node1" [
				label = "<0> 0xf7fc4380| <f1> | <f2> |-1"
			];
			*/
			// 第二遍建立结点
			test("bfs节点标题");
			stringstream ss;
			int titleLen = max(10, len);
			if (u != 0 and pId[u].length() > titleLen) { // 如果不是文件名字，直接省略到20个以内
				pId[u] = str.cut(titleLen, pId[u]) + "...";
			}
			ss << "<f0> " << str.deal(str.split(len, pId[u]));
			test("bfs标题处理完成");
			// fout<<dot.frame(u,s,g);
			while (!s.empty()) {
				int v = s.top();
				s.pop();
				ss << "| <f" << v << "> " << str.deal(str.split(len, pId[v]))
				   << "\\l";
			}
			test("bfs节点预处理完成");
			string res = ss.str();
			fout << dot.node(u, res); // 完成这个结点
			test("bfs节点生成结束");
		}
		while (!ed.empty()) {
			fout << ed.front();
			ed.pop();
		}
	}
	string giveId(const int &fileId, const string &s) { // 为文件分配前缀id，保证不让重名不同同格式文件，输出到同一个文件上
		stringstream ss;
		ss << "_" << fileId << "_" << s;
		return ss.str();
	}
	void txtToPng(const int &fileId, const string &fileName) {
		test("进入文件：" + fileName);
		ifstream fin;
		ofstream fout;
		string txtFileName = "txt/" + fileName;
		string idFileName = giveId(fileId, str.ridSuf(fileName));
		string dotFileName = "dot/" + idFileName + ".dot";
		string pngFileName = "png/" + idFileName + ".png";
		// cout<<fileName<<' '<<dotFileName<<' '<<pngFileName<<'\n';
		fin.close(); // 防止读写失败出现问题
		fout.close();
		fin.open(txtFileName);
		fout.open(dotFileName);

		if (!fin.is_open()) {
			cout << "读入失败：" << txtFileName << '\n';
			fin.close();
			return;
		}
		if (!fout.is_open()) {
			cout << "写入失败：" << dotFileName << '\n';
			fin.close();
			fout.close();
			return;
		}

		fout << dot.head(color[1]);

		unique_ptr<graph> g = make_unique<graph>();
		stack<int> s;
		map<int, string> pId; // 编号对应段落
		map<int, int> pCnt;	  // 对应段落的缩进数
		dot.edgeId = 0;

		// 以文件名字建立根节点
		s.push(0);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN64)
		pId[0] = "根节点";
#else
		pId[0] = txtFileName; // windows下读中文目录，有汉字乱码bug，除非txt下文件全部英文
#endif
		pCnt[0] = -1;

		int paranow = 0; // 当前段落编号
		string input;	 // 读入
		while (getline(fin, input)) {
			int len = input.length();
			int iCnt = 0; // 当前段落缩进数
			while (iCnt < len and input[iCnt] == '\t') {
				iCnt++;
			}
			if (iCnt == len) continue;				 // 无视空行
			if (input.find("```") != string::npos) { // 代码块处理
				input = codeblock(fin);
			} else { // 正常处理
				// 只保留缩进后面的部分
				input = input.substr(iCnt, len);
			}
			test("读入部分1");
			pId[++paranow] = input;
			pCnt[paranow] = iCnt;
			test("读入部分2");
			// 一直弹出栈顶，直到栈顶段落数小于当前
			while (pCnt[s.top()] >= iCnt) {
				s.pop();
			}
			// 连接栈顶与当前点
			g->add(s.top(), paranow);
			// 当前点放栈顶
			s.push(paranow);
		}
		test("读入完成");
		bfs(g, pId, fout);
		test("bfs完成");
		fout << dot.end();
		fin.close();
		fout.close();
		cout << txtFileName << ":已完成\n";

#if defined(WIN32) || defined(_WIN32) || defined(__WIN64)
		string dotCommand = "dot -Gfontnames=\"" + fontName + "\" -Nfontname=\"" + fontName + "\" -Tpng:cairo:cairo " + dotFileName + " -o " + pngFileName;
#else
		string dotCommand = "dot -Tpng " + dotFileName + " -o " + pngFileName;
#endif
		system(dotCommand.c_str());
	}
} mmp;

int main() {
	setlocale(LC_ALL, "");
	priority_queue<string> que; // 文本名队列
	string path = "txt/";
	DIR *dir = opendir(path.c_str());
	struct dirent *entry;
	if (dir != nullptr) {
		while ((entry = readdir(dir)) != nullptr) {
			string s = entry->d_name;
			// if (entry->d_type == DT_REG) {
			if (entry->d_name[0] != '.') {
				que.push(s);
			}
		}
		closedir(dir);
	}
	int fileId = 0;
	while (!que.empty()) {
		++fileId;
		string fileName = que.top();
		que.pop();
		mmp.txtToPng(fileId, fileName);
	}

	// cout<<"按任意键结束";
	// cin.get();
	return 0;
}
