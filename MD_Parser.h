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
    ul = 3, //unordered list
    ol = 4, //ordered list
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
    vector<node*> son;
    node *fa;
    string elem[3];
    node(int _type, node *f) : type(_type), fa(f) {}
};


class MarkDownParser{
private:
    string content;
    node *root, *now;
    char s[MAXLEN];

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
        std::ifstream FileIn(filename);
        root = new node(nul, nullptr);
        now = root;
        bool NewPara = false; //是否是新段落 为True时新起一段
        bool InBlock = false; //是否在代码块内
        int CntTitle = 0;     //标题深度
        int CntList = 0;      //列表深度
        while(!FileIn.eof()){

            //从文件中读取一行
            FileIn.getline(s, MAXLEN);
            //处理不在代码块内且需要换行的情况
            if (!InBlock && IsCutLine(s)){
                now = root;
                now->son.push_back(new node(hr, now));
                NewPara = false;
                continue;
            }

            //计算一行中的空格数和Tab数
            std::pair<int, char *> Space = SkipSpace(s);

            //如果没有位于代码块中，且没有统计到空格和Tab,则直接读取下一行
            if(Space.second == nullptr){
                if(InBlock){
                    now->son.back()->elem[0] += string(s) + '\n';
                }
                else{
                    now = root;
                    NewPara = true;
                }
                continue;
            }

            //分析该行文本的类型
            std::pair<int, char *> TJ = JudgeType(Space.second);

            //如果是代码块类型
            if(TJ.first == blockcode){
                //如果位于代码块中，则push一个空类型的节点
                InBlock ? now->son.push_back(new node(nul, now)) : now->son.push_back(new node(blockcode, now));
                InBlock = !InBlock;
                continue;
            }

            //如果在代码块中，直接将内容拼接到当前节点中
            if(InBlock){
                now->son.back()->elem[0] += string(s) + '\n';
                continue;
            }

            //如果是段落则直接添加到末尾。
            if(TJ.first == paragraph){
                
                if(NewPara){
                    now = root;
                    now->son.push_back(new node(paragraph, now));
                }
                else{
                    now = FindNode(root);
                }
                insert(now->son.back(), string(TJ.second));
                NewPara = false;
                continue;
            }

            //如果是标题则添加到root的末尾
            if(TJ.first >= h1 && TJ.first <= h6){
                root->son.push_back(new node(TJ.first, root));
                root->son.back()->elem[0] = "tag" + to_string(++CntTitle);
                insert(root->son.back(), string(TJ.second));
                continue;
            }
            
            //如果是无序列表
            if(TJ.first == ul){
                //列表的阶为行首空格/4
                int lvl = Space.first/4;
                now = root;
                pair<bool, node*> tmp = FindList(lvl, now);
                if(tmp.first){
                    //无序表
                    now = tmp.second;
                    if(now->son.back()->type != ul){
                        now->son.push_back(new node(ul, now));
                    }
                    now = now->son.back();
                    now->son.push_back(new node(li, now));
                    now = now->son.back();
                    now->son.push_back(new node(paragraph, now));
                    insert(now->son.back(), string(TJ.second));
                }else if(NewPara){
                    //认为是代码块
                    now = FindNode(root);
                    now->son.push_back(new node(paragraph, now));
                    insert(now->son.back(), '`' + string(TJ.second));
                }else{
                    now = FindNode(root);
                    insert(now->son.back(), string(TJ.second));
                }
            }

            //如果是有序列表
            if(TJ.first == ol){
                //列表的阶为行首空格/4
                int lvl = Space.first/4;
                now = root;
                pair<bool, node*> tmp = FindList(lvl, now);
                if(tmp.first){
                    //有序表
                    now = tmp.second;
                    if(now->son.back()->type != ol){
                        now->son.push_back(new node(ol, now));
                    }
                    now = now->son.back();
                    now->son.push_back(new node(li, now));
                    now = now->son.back();
                    now->son.push_back(new node(paragraph, now));
                    insert(now->son.back(), string(TJ.second));
                }else if(NewPara){
                    //认为是代码块
                    now = FindNode(root);
                    now->son.push_back(new node(paragraph, now));
                    insert(now->son.back(), '`' + string(TJ.second));
                }else{
                    now = FindNode(root);
                    insert(now->son.back(), string(TJ.second));
                }
            }

        }
        FileIn.close();
        dfs(root);
    }

    std::string getContents() { return content; }

    ~MarkDownParser()
    {
        destroy<node>(root);
    }
private:

    string to_string(int n){
        string s;
        while(n){
            s+=n%10+'0';
            n/=10;
        }
        reverse(s.begin(), s.end());
        return s;
    }

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
                return make_pair(cnttab*4 + cntspace, src + i);
        }
        //全为空格
        return make_pair(cnttab*4 + cntspace, nullptr);
    }

    //寻找n阶列表
    node* FindNode(node *root){
        node *p = root;
        while(!p->son.empty() && p->son.back()->type != nul)
            p = p->son.back();
        p = p->fa;
        return p;
    }

    pair<bool, node*> FindList(int lvl, node *root){
        for(int i = 0; i < lvl; i++){
            if(root->son.back()->type == ul || root->son.back()->type == ol)
                root = root->son.back();
            else
                return make_pair(false, root);
        }
        return make_pair(true, root);
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
        for (int i = 0; i < (int)v->son.size(); i++)
        {
            destroy(v->son[i]);
        }
        delete v;
    }


    void insert(node *v, const string &src)
    {
        int n = (int)src.size();
        bool incode = false, inem = false, instrong = false, inautolink = false;
        v->son.push_back(new node(nul, v));

        for (int i = 0; i < n; i++)
        {
            char ch = src[i];
            if (ch == '\\')
            {
                ch = src[++i];
                v->son.back()->elem[0] += string(1, ch);
                continue;
            }

            //处理行内代码
            if (ch == '`' && !inautolink)
            {
                incode ? v->son.push_back(new node(nul, v)) : v->son.push_back(new node(code, v));
                incode = !incode;
                continue;
            }

            v->son.back()->elem[0] += string(1, ch);
            if (inautolink)
                v->son.back()->elem[1] += string(1, ch);
        }
        //处理行末有二空格时加入换行符
        if (src.size() >= 2)
        {
            if (src.at(src.size() - 1) == ' ' && src.at(src.size() - 2) == ' ')
                v->son.push_back(new node(br, v));
        }
    }
    bool isHeading(node *root){
        return root->type >= h1 && root->type <= h6;
    }
    void dfs(node *root){
        if (root->type == paragraph && root->elem[0].empty() && root->son.empty())
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
        for (int i = 0; i < (int)root->son.size(); i++){
      //      cout<<"num： "<<i + 1<<endl;
            dfs(root->son[i]);
        }
            

        //拼接为结束标签
        content += backTag[root->type] + '\n';
    }
};
