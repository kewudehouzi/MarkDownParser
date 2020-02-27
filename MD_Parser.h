#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <utility>
#include <cctype>
#include <cstdio>
#include <vector>
#include <cstring>
#include <algorithm>
using namespace std;

#define MAXLEN 10000

enum
{
    nul = 0,
    paragraph = 1,
    href = 2,
    ul = 3,
    ol = 4,
    li = 5,
    em = 6,
    strong = 7,
    hr = 8,
    br = 9,
    image = 10,
    quote = 11,
    h1 = 12,
    h2 = 13,
    h3 = 14,
    h4 = 15,
    h5 = 16,
    h6 = 17,
    blockcode = 18,
    code = 19
};

const std::string frontTag[] = {
    "", "<p>", "", "<ul>", "<ol>", "<li>", "<em>", "<strong>",
    "<hr color=#CCCCCC size=1 />", "<br />",
    "", "<blockquote>",
    "<h1 ", "<h2 ", "<h3 ", "<h4 ", "<h5 ", "<h6 ", //右边的尖括号预留给添加其他的标签属性, 如 id
    "<pre><code>", "<code>"};

//HTML后置标签
const std::string backTag[] = {
    "", "</p>", "", "</ul>", "</ol>", "</li>", "</em>", "</strong>",
    "", "", "", "</blockquote>",
    "</h1>", "</h2>", "</h3>", "</h4>", "</h5>", "</h6>",
    "</code></pre>", "</code>"};

struct node{
    int type;
    vector<node*> v;
    string elem[3];
    node(int _type) : type(_type) {}
};


class MarkDownParser{
private:
    string content;
    node *root, *now;
    char s[MAXLEN];
string to_string(int n){
        string s;
        while(n){
            s+=n%10+'0';
            n/=10;
        }
        reverse(s.begin(), s.end());
        return s;
    }
    //判断是否添加分割线
/*    bool IsCutLine(char *src);
    //跳过行首空格
    pair<int, char*> SkipSpace(char *src);
    //判断类型
    pair<int, char*> JudgeType(char *src);
    //插入段落，需处理加粗斜体转义等情况。
    void insert(node *v, const string &src);
    //？？？
    inline node *findnode(int depth);
    //？？？
    inline void mkpara(node *v);
    //？？？

    void dfs(node *v);*/

public:
    MarkDownParser(const string &filename){
        /*
         * 构造函数初始化
         * 处理段落、代码块、标题、表格、有序/无序列表等
         *


        */
        std::ifstream in(filename);
        root = new node(nul);
        now = root;
        bool NewPara = false; //是否是新段落 为True时新起一段
        bool InBlock = false; //是否在代码块内
        int CntTitle = 0;     //标题深度
        int CntList = 0;      //列表深度

        while(!in.eof()){
            
            //读取一行
            in.getline(s, MAXLEN);

            //需再读一行
            if (!InBlock && IsCutLine(s)){
                now = root;
                now->v.push_back(new node(hr));
                NewPara = false;
                continue;
            }

            //跳过开头空格，同时开头空格作为列表分层的依据
            std::pair<int, char*> ps = SkipSpace(s);

            if(!InBlock && ps.second == nullptr){
                now = root;
                NewPara = true;
                continue;
            }

            //处于代码块中时仅需将内容输入进节点中
            if(InBlock){
                now->v.back()->elem[0] += string(s) + '\n';
                continue;
            }
            //分析该行类型
            std::pair<int, char*> JT = JudgeType(ps.second);

            //处理 ``` 的情况
            if(JT.first == blockcode){
                InBlock ? now->v.push_back(new node(nul)) : now->v.push_back(new node(blockcode));
                InBlock = !InBlock;
                continue;
            }

             //标题
            if (JT.first >= h1 && JT.first <= h6){


                now->v.push_back(new node(JT.first));
                now->v.back()->elem[0] = "tag" + to_string(++CntTitle);
                std::string fak = to_string(CntTitle) + ". " + string(JT.second);
                insert(now->v.back(), fak);
                continue;
         //先不搞目录       Cins(Croot, tj.first - h1 + 1, string(tj.second), cntTag);
            }

            //处理段落，
            if(JT.first == paragraph){
                now->v.push_back(new node(nul));
                insert(now->v.back(), string(JT.second));
                continue;
            }
           
            
        }
        in.close();
        dfs(root);
    }

    std::string getContents() { return content; }

    ~MarkDownParser()
    {
        destroy<node>(root);
    }
private:

    bool IsCutLine(char *src){
        int cnt = 0;
        char *ptr = src;
        while (*ptr)
        {
            //如果不是 空格、tab、-或者*，那么不需要换行
            if (*ptr != ' ' && *ptr != '\t' && *ptr != '-')
                return false;
            if (*ptr == '-')
                cnt++;
            ptr++;
        }
        //如果出现 --- 则需要增加一个分割线，这时需要换行
        return (cnt >= 3);

    }

    pair<int, char*> SkipSpace(char *src){
        //如果该行内容k为空，则直接返回
        if ((int)strlen(src) == 0)
            return make_pair(0, nullptr);
        //统计空格键和Tab键的个数
        int cntspace = 0, cnttab = 0;
        //从该行的第一个字符读取，统计空格键和Tab键
        //当遇到不是空格和Tab时，立即停止
        for (int i = 0; src[i] != '\0'; i++)
        {
            if (src[i] == ' ')
                cntspace++;
            else if (src[i] == '\t')
                cnttab++;
            //如果内容前有空格和Tab，那么将其统一按Tab的个数处理
            //其中，一个Tab = 四个空格
            else
                return make_pair(cnttab + cntspace / 4, src + i);
        }
        //全为空格
        return make_pair(cnttab + cntspace / 4, nullptr);
    }

    pair<int, char*> JudgeType(char *src){
        char *ptr = src;
        while(*ptr == '#'){
            ptr++;
        }
        //如果出现空格，则说明是'<h>'标签
        if (ptr - src > 0 && *ptr == ' ')
            return make_pair(ptr - src + h1 - 1, ptr + 1);
        //重置分析位置
        ptr = src;
        //如果出现```则说明是代码块
        if (strncmp(ptr, "```", 3) == 0)
            return make_pair(blockcode, ptr + 3);
        //如果出现* + -.并且他们的下一个字符为空格，则说明是列表
        if (strncmp(ptr, "-", 2) == 0)
            return make_pair(ul, ptr + 1);
        //如果出现 > 且下一字符为空格，则说明是引用
        if (*ptr == '>' && (ptr[1] == ' '))
            return make_pair(quote, ptr + 1);
        //如果出现的是数字，且下一个字符是 . 则说明是有序列表
        char *ptr1 = ptr;
        while (*ptr1 && (isdigit(*ptr1)))
            ptr1++;
        if (ptr1 != ptr && *ptr1 == '.' && ptr1[1] == ' ')
            return make_pair(ol, ptr1 + 1);
        //否则就是普通段落
        return make_pair(paragraph, ptr);
    }


    template <typename T> void destroy(T *v){
        for (int i = 0; i < (int)v->v.size(); i++)
        {
            destroy(v->v[i]);
        }
        delete v;
    }


    void insert(node *v, const string &src)
    {
        int n = (int)src.size();
        bool incode = false, inem = false, instrong = false, inautolink = false;
        v->v.push_back(new node(nul));

        for (int i = 0; i < n; i++)
        {
            char ch = src[i];
            if (ch == '\\')
            {
                ch = src[++i];
                v->v.back()->elem[0] += string(1, ch);
                continue;
            }

            //处理行内代码
            if (ch == '`' && !inautolink)
            {
                incode ? v->v.push_back(new node(nul)) : v->v.push_back(new node(code));
                incode = !incode;
                continue;
            }

            v->v.back()->elem[0] += string(1, ch);
            if (inautolink)
                v->v.back()->elem[1] += string(1, ch);
        }
        if (src.size() >= 2)
        {
            if (src.at(src.size() - 1) == ' ' && src.at(src.size() - 2) == ' ')
                v->v.push_back(new node(br));
        }
    }
    bool isHeading(node *root){
        return root->type >= h1 && root->type <= h6;
    }
    void dfs(node *root){
        if (root->type == paragraph && root->elem[0].empty() && root->v.empty())
            return;
        content += frontTag[root->type] + '\n';

        bool flag = true;

        if (isHeading(root))
        {
            content += "id=\"" + root->elem[0] + "\">" + '\n';
            flag = false;
        }

        if (flag)
        {
            content += root->elem[0] + '\n';
            flag = false;
        }
        //递归遍历所有
        for (int i = 0; i < (int)root->v.size(); i++){
      //      cout<<"num： "<<i + 1<<endl;
            dfs(root->v[i]);
        }
            

        //拼接为结束标签
        content += backTag[root->type] + '\n';
    }
};
