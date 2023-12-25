#include <iostream>
#include <vector>
#include <unistd.h>
#include <climits>

using namespace std;
string program;//程序的源码字符串
char ch;
int index_p=0,sym=0; //p是program的索引，sym是返回的状态码，
const char *filename="../test1.txt";

FILE* pFile; //文件指针
int line_number=0;
vector<pair<string,int>> tokens;
//保留字
vector<string> keyword={"if","else","while","do","main","int","float","double","return","const","void","continue","break","char","unsigned","enum", "long","switch", "case", "unsigned", "auto","static"};
void GetToken(string& token);
void processDigit(string& token);
void processLetter(string& token);
void processSpecial(string& token);
void printLine();
/*
 * sym值对应表
 *  0 初始状态
 *  -2 识别错误，DFA未正确识别
 *  -1 程序指针已经到末尾
 *  1 遇到换行，请求主程序换行操作
 *  2 结果为一般标识符
 *  3-24 对应keyword列表的字符串
 *  25 对应字符常量
 *  26-30 识别到数字（详细码请看processDigit函数）
 *  31-50 识别到特殊符号（详细请看processSpecial函数）
 *
 * */

int main(){
    index_p=0;
    if((pFile=fopen(filename, "r")) == nullptr){
        cout<<"文件不存在！"<<endl;
        return 0;
    }
    do{
        ch= fgetc(pFile);
        program.push_back(ch);
    }while(ch!=EOF);
    string token;
    index_p=0;
    bool end_of_file=false;
    do
    {
        if(end_of_file) break;
        GetToken(token);
        switch(sym)
        {
            case-1:cout<<"文章已经到头"<<endl;end_of_file=true;
                break;
            case-2:{
                cout<<endl<<"warning:DFA未正确识别,出错token：("<<token.back()<<")。出错位置:行号"<<line_number+1<<endl;
                tokens.emplace_back(token,sym);
            };
                break;
            case 1:{
                printLine();
            }
                break;
            default:tokens.emplace_back(token,sym);break;
        }
    }while(index_p<program.size());
}

void GetToken(string& token){
    token.clear();
    sym=0;
    if(index_p>=program.size()){
        sym=-1;//文件已经到头，无法分析。
    }
    while(program[index_p]==' '||program[index_p]=='\t'||program[index_p]=='\n'){
        if(index_p+1>=program.size()){
            sym=-1;//文件已经到头，无法分析。
        }
        if(program[index_p]=='\n'){
            sym=1;
            ++line_number;
            ++index_p;
            return;
        }
        ++index_p;
    }
    char start=program[index_p];
    //处理注释
    if(start=='/'){
        if(program[index_p+1]=='/'){
            index_p= index_p+2;
            while(program[index_p]!='\n') ++index_p;
            ++index_p;
        }else if(program[index_p+1]=='*'){
            index_p = index_p+2;
            while(program[index_p]!='*'&&program[index_p+1]!='/') ++index_p;
            index_p+=2;
        }
        start=program[index_p];
    }

    if((start>='a'&&start<='z')||(start>='A'&&start<='Z')||start=='_'){
        processLetter(token);
    }
    else if(start>='0'&&start<='9'){
        processDigit(token);
    }else{
        processSpecial(token);
    }
}

void processDigit(string& token) {
    enum State { START, OCT_HEX_FLOAT_ZERO, HEX, OCTAL_END, FLOAT,FLOAT_END,HEX_END,DECIMAL_END_FLOAT_CHECK ,INT_SUFFIX,ERROR };
    /*
     * 终态有
     * OCT_HEX_FLOAT_ZERO 识别出数字0，也是十进制整数
     * OCTAL_END 识别出八进制整数
     * FLOAT_END 十进制浮点数
     * HEX_END 十六进制整数
     * DECIMAL_END_FLOAT_CHECK 十进制整数
     * INT_SUFFIX带后缀的十进制整数
     *
     * sym对应值
     * 26 十进制整数
     * 27 十进制带L后缀整数
     * 28 十进制浮点数
     * 29 十六进制整数
     * 30 八进制整数
     *
     * */
    State currentState = START;
    bool isEND=false;
    while (index_p < program.size() && !isEND) {
        char c = program[index_p];
        switch (currentState) {
            case START:
                if (c == '0') {
                    currentState = OCT_HEX_FLOAT_ZERO;
                } else if (isdigit(c)) {
                    currentState = DECIMAL_END_FLOAT_CHECK;
                } else {
                    currentState=ERROR;
                }
                break;

            case OCT_HEX_FLOAT_ZERO:
                if (c == 'x' || c == 'X') {
                    currentState = HEX;
                } else if (isdigit(c) && c != '8' && c != '9') {
                    currentState = OCTAL_END;
                } else if (c == '.') {
                    currentState = FLOAT;
                } else {
                    isEND=true;
                }
                break;

            case HEX:
                if ((!isxdigit(c))&&(!isalpha(c))) {
                    currentState = ERROR;
                }else{
                    currentState=HEX_END;
                }
                break;

            case OCTAL_END:
                if (c < '0' || c > '7') {
                    isEND=true;
                }
                break;

            case FLOAT:
                if (!isdigit(c)) {
                    currentState = ERROR;
                }else{
                    currentState = FLOAT_END;
                }
                break;

            case FLOAT_END:
                if (!isdigit(c)) {
                    isEND=true;
                }
                break;
            case HEX_END:
                if ((!isxdigit(c))&&(!isalpha(c))) {
                    isEND=true;
                }
                break;

            case DECIMAL_END_FLOAT_CHECK:
                if (!isdigit(c)) {
                    if (c == '.') {
                        currentState = FLOAT;
                    } else if (c == 'L') {//假设就一种后缀吧
                        currentState = INT_SUFFIX;
                    } else {
                        isEND=true;
                    }
                }
                break;
            case ERROR:
                break;
            case INT_SUFFIX:
                isEND=true;
                break;
        }

        if (currentState != ERROR&&!isEND) {
            token.push_back(c);
            ++index_p;
        }
    }
    // 根据状态设置sym值
    if (currentState == DECIMAL_END_FLOAT_CHECK ||currentState == OCT_HEX_FLOAT_ZERO ) {
        sym = 26;
    } else if (currentState == INT_SUFFIX) {
        sym = 27;
    } else if (currentState == FLOAT_END) {
        sym = 28;
    } else if(currentState == HEX_END){
         sym = 29;
    }else if(currentState == OCTAL_END){
        sym = 30;
    }else{
        sym = -2;//不是终态之一，报错；
    }

}


void processLetter(string& token){
    while((program[index_p]>='a'&&program[index_p]<='z')||(program[index_p]>='A'&&program[index_p]<='Z')||program[index_p]=='_'){
        token.push_back(program[index_p]);
        ++index_p;
    }
    bool isString=false;
    while(program[index_p]!=' '&&program[index_p]!='\t'&&program[index_p]=='\n'){
        token.push_back(program[index_p]);
        ++index_p;
        isString= true;
    }
    if(isString){
        sym=25;
        return;
    }
    sym=2;//目前暂定为一般标识符
    ch=token.back();
    for(int i=0;i<keyword.size();++i){
        if(keyword[i]==token){
            sym+=i+1;
            return;
        }

    }
}


void processSpecial(string& token){
    char special=program[index_p];
    switch(special)
    {
        case'+':sym=31;break;
        case'-':sym=32;break;
        case'*':sym=33;break;
        case'/':sym=34;break;
        case'=':{
            char next=program[index_p+1];
            if(next=='='){
                sym=44;
                ++index_p;
                break;
            };//识别到==
            sym=35;//识别到=
        };break;
        case'<':{
            char next=program[index_p+1];
            if(next=='='){
                sym=50;
                ++index_p;
                break;
            };//识别到<=
            sym=36;//识别到<
        };break;
        case'{':sym=37;break;
        case'}':sym=38;break;
        case';':sym=39;break;
        case'(':sym=40;break;
        case')':sym=41;break;
        case'\'':sym=42;break;
        case'\"':sym=43;break;
        case'!':{
            char next=program[index_p+1];
            if(next!='='){
                sym=-2;//未识别到！=
                break;
            };
            sym=45;
            ++index_p;
        };break;
        case'&':{
            char next=program[index_p+1];
            if(next!='&'){
                sym=-2;//未识别到&&
                break;
            };
            sym=46;
            ++index_p;
        };break;
        case'|':{
            char next=program[index_p+1];
            if(next!='|'){
                sym=-2;//未识别到||
                break;
            };
            sym=47;
            ++index_p;
        };break;
        case'>':{
            char next=program[index_p+1];
            if(next=='='){
                sym=49;
                ++index_p;
                break;
            };//识别到>=
            sym=48;//识别到>
        };break;
        default:sym=-2;break;
    }
    token.push_back(program[index_p]);
    ++index_p;
}

void printLine(){
    string currentLine;
    for(const auto& token:tokens){
        currentLine.append(token.first);
        currentLine.push_back(' ');
    }
    cout<<line_number<<": "<<currentLine<<endl;
    cout<<"analysis: "<<endl;
    for(const auto& token:tokens){
        cout<<"{ token: ("<<token.first<<") sym: ("<<token.second<<") } "<<endl;
    }
    cout<<"end"<<endl<<endl;
    tokens.clear();
}