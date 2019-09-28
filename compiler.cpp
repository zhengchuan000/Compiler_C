#include <iostream>
#include <fstream>
#include <string>
#include <map>
using namespace std;
//                                          词法分析器
/*
                                            KEYWORD
                                        ("auto",    2,  1)
                                        ("break",   2,  2)
                                        ("case",    2,  3)
                                        ("char",    2,  4)
                                        ("const",   2,  5)
                                        ("continue",2,  6)
                                        ("default", 2,  7)
                                        ("do",      2,  8)
                                        ("double",  2,  9)
                                        ("else",    2,  10)
                                        ("enum",    2,  11)
                                        ("extern",  2,  12)
                                        ("float",   2,  13)
                                        ("for",     2,  14)
                                        ("goto",    2,  15)
                                        ("if",      2,  16)
                                        ("int",     2,  17)
                                        ("long",    2,  18)
                                        ("register",2,  19)
                                        ("return",  2,  20)
                                        ("short",   2,  21)
                                        ("signed",  2,  22)
                                        ("sizeof",  2,  23)
                                        ("static",  2,  24)
                                        ("struct",  2,  25)
                                        ("switch",  2,  26)
                                        ("typedef", 2,  27)
                                        ("union",   2,  28)
                                        ("unsigned",2,  29)
                                        ("void",    2,  30)
                                        ("volatile",2,  31)
                                        ("while",   2,  32)

                                            DISTINCT & CALC
                                        ('+',       4,  33)
                                        ('-',       4,  34)
                                        ('*',       4,  35)
                                        ('/',       4,  36)
                                        ('<',       4,  37)
                                        ("<=",      4,  38)
                                        ('>',       4,  39)
                                        (">=",      4,  40)
                                        ('=',       4,  41)
                                        ("==",      4,  42)
                                        ("!=",      4,  43)
                                        (';',       3,  44)
                                        ('(',       3,  45)
                                        (')',       3,  46)
                                        ('^',       3,  47)
                                        (',',       3,  48)
                                        ('\',       3,  49)
                                        ('#',       3,  50)
                                        ('&',       3,  51)
                                        ('&&',      4,  52)
                                        ('|',       3,  53)
                                        ("||",      4,  54)
                                        ('%',       3,  55)
                                        ('~',       3,  56)
                                        ("<<",      4,  57)
                                        (">>",      4,  58)
                                        ("[",       3,  59)
                                        ("]",       3,  60)
                                        ("{",       3,  61)
                                        ("}",       3,  62)
                                        (".",       3,  63)
                                        (":",       3,  64)
                                        ("!",       3,  65)
                                        ("+=",      4,  66)
                                        ("++",      4,  67)
                                        ("-=",      4,  68)
                                        ("--",      3,  69)
                                        ("*=",      3,  70)
                                        ("/=",      4,  71)

*/

static char operatorDelimiter[33][10] = {
    "+",    "-",    "*",    "/",    "<",
    "<=",   ">",    ">=",   "=",    "==",
    "!=",   ";",    "(",    ")",    "^",
    ",",    "\'",   "#",    "&",    "&&",
    "|",    "||",   "%",    "~",    "<<",
    ">>",   "[",    "]",    "{",    "}",
    ".",    ":",    "!"
};

#define ID        0//infinit set, 
#define CONSTANT  1
#define KEYWORD   2
#define DISTINCT  3
#define CALC      4

struct node{
    char*    token;
    int      token_name;
    long int token_value;
};

static char reservedWordArr[32][20] = {
    "auto", "break", "case", "char", "const", "continue",
    "default", "do", "double", "else", "enum", "extern",
    "float", "for", "goto", "if", "int", "long",
    "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void",
    "volatile", "while"
};

map<string,int> reservedWordMap;
reservedWordMap["UNKNOEN"]     = 0;
reservedWordMap["auto"]        = 1;
reservedWordMap["break"]       = 2;
reservedWordMap["case"]        = 3;
reservedWordMap["char"]        = 4;
reservedWordMap["const"]       = 5;
reservedWordMap["continue"]    = 6;
reservedWordMap["default"]     = 7;
reservedWordMap["do"]          = 8;
reservedWordMap["double"]      = 9;
reservedWordMap["else"]        = 10;
reservedWordMap["enum"]        = 11;
reservedWordMap["extern"]      = 12;
reservedWordMap["float"]       = 13;
reservedWordMap["for"]         = 14;
reservedWordMap["goto"]        = 15;
reservedWordMap["if"]          = 16;
reservedWordMap["int"]         = 17;
reservedWordMap["long"]        = 18;
reservedWordMap["register"]    = 19;
reservedWordMap["return"]      = 20;
reservedWordMap["short"]       = 21;
reservedWordMap["signed"]      = 22;
reservedWordMap["sizeof"]      = 23;
reservedWordMap["static"]      = 24;
reservedWordMap["struct"]      = 25;
reservedWordMap["switch"]      = 26;
reservedWordMap["typedef"]     = 27;
reservedWordMap["union"]       = 28;
reservedWordMap["unsigned"]    = 29;
reservedWordMap["void"]        = 30;
reservedWordMap["volatile"]    = 31;
reservedWordMap["while"]       = 32;

static char idTable[32768][64];//存放所以的用户自定义变量
char*       wordSet[65536];//存放所有的划分出来的单词，不保证全部合法
int         wordSetPosition = 0;//用于遍历 wordSet
char        word[20] = {'\0'};//存放单个单词的每个字符
int         wordPosition = 0;//用于遍历word
int         keywordTag = -1;//用来表示word集合中的字符串是否是关键字，是为1，否则为0
list<node>  wordChain;





void createNode(string token, int token_name, long int token_value){
    strcut temp;
    strcpy(temp.token, token);
    temp.token_name = token_name;
    temp.token_value = token_value;
    wordChain.append(temp);
}

int getWordChain(){
    char buff[512];
    ifstream in("../test_case/text.txt");
    if(!in.is_open()){
       cout<<"open src file error!"<<endl;
    }
    while(!in.eof()){
        in.getline(buff,512);//从源文件中读取一行
        string strOfBuff = buff;
        int lenOfBuff = strOfBuff.length();//减少循环次数，节约空间开销
        while(int i<lenOfBuff){//默认初始化i为0
            while(buff[i] == ' '){//跳过每行开头的空格
                i++;
            }
            if(isalpha(buff[i])){//开头为字母,可能是比标识符或关键字
                word = {'\0'};//清空word集合
                wordPosition = 0;//重新初始化wordPosition
                word[wordPosition++] = buff[i++];
                while(isalpha(buff[i]) || isdiigt(buff[i]) || buff[i]=='_'){//后接字母数字下划线
                    word[wordPosition++] = buff[i++];
                }
                for(int q=0;q<32;q++){
                    if(strcmp(reservedWordArr[q], word) == 0){//匹配成功，确定该word为关键字
                        keywordTag = 1;
                        createNode(word, KEYWORD, reservedWordMap[word])
                        break;
                    }
                }
                if(keywordTag != 1){//word集合中的单词不是关键字，是用户自定义标识符
                    keywordTag = 0;
                    createNode(word, ID, -1);
                    strcpy(idTable[idTablePosition++], word);//将标识符注册到idTable中
                }
            }
            else if(isdigit(buff[i])){//数字常量， 数字开头
                word = {'\0'};
                wordPosition = 0;
                word[wordPosition++] = buff[i++];
                while(isdigit(buff[i])){//后接数字
                    word[wordPosition++] = buff[i++];
                }
                createNode(word, CONSTANT, -1);//常量没有必要有token_value
            }
            else{//不是标识符、不是关键字、不是数字常量
                if(buff[i] == '='){
                    if(buff[i+1] == '='){// 是"=="
                        createNode("==", DISTINCT, 42);
                    }
                    else{
                        createNode("=", DISTINCT, 41);
                    }
                }
                if(buff[i] == '+'){
                    if(buff[i+1] == '='){//是"+="
                        createNode("+=", DISTINCT, 66);
                    }
                    if(buff[i+1] == '+'){
                        createNode("++", CALC, 67);
                    }
                    else{
                        createNode("+", CALC, 33);
                    }
                }
                if(buff[i] == '-'){
                    if(buff[i] == '='){
                        createNode("-=", DISTINCT, 68);
                    }
                    else if(buff[i] == '-'){
                        createNode("--", CALC, 69);
                    }
                    else{
                        createNode("-", CALC, 34);
                    }
                }
                if(buff[i] == '*'){
                    if(buff[i] == '='){
                        createNode("*=", CALC, 70);
                    }
                    else{
                        createNode("*", CALC, 35);
                    }
                }
                if(buff[i] == '/'){//注释另外考虑
                    if(buff[i+1] == '='){
                        createNode("/=", CALC, 71);
                    }
                    else{
                        createNode("/", CALC, 36);
                    }
                }
                if(buff[i] == '<'){
                    if(buff[i+1] == '<'){
                        createNode("<<", CALC, 57);
                    }
                    else if(buff[i+1] == "="){
                        createNode("<=", CALC, 38);
                    }
                    else{
                        createNode("<", CALC, 37);
                    }
                }
                if(buff[i] == '>'){
                    if(buff[i+1] == '>'){
                        createNode(">>", CALC, 58);
                    }
                    if(buff[i+1] == '='){
                        createNode(">=", CALC, 40);
                    }
                    else{
                        createNode(">", CALC, 39);
                    }
                }
                if(buff[i] == '&'){
                    if(buff[i] == '&'){
                        createNode("&&", CALC, 52);
                    }
                    else{
                        createNode("&", CALC, 51);
                    }
                }
                if(buff[i] == '|'){
                    if(buff[i+1] == '||'){
                        createNode("||", CALC, 54);
                    }
                    else{
                        createNode("|", CALC, 53);
                    }
                }
                if(buff[i] == '!' && buff[i+1] == '='){
                    createNode("!=", CALC, 43);
                }
                if(buff[i] == ':'){
                    createNode(":", DISTINCT, 64);
                }
                if(buff[i] == ';'){
                    createNode(";", DISTINCT, 44);
                }
                if(buff[i] == '['){
                    createNode("[", DISTINCT, 59);
                }
                if(buff[i] == ']'){
                    createNode("]", DISTINCT, 60);
                }
                if(buff[i] == '('){
                    createNode("(", DISTINCT, 45);
                }
                if(buff[i] == ')'){
                    createNode(")", DISTINCT, 46);
                }
                if(buff[i] == '{'){
                    createNode("{", DISTINCT, 61);
                }
                if(buff[i] == '}'){
                    createNode("}", DISTINCT, 62);
                }
                if(buff[i] == '.'){
                    createNode(".", DISTINCT, 63);
                }
                if(buff[i] == ','){
                    createNode(",", DISTINCT, 48);
                }
                if(buff[i] == '~'){
                    createNode("~", DISTINCT, 56);
                }
                if(buff[i] == "%"){
                    createNode("%", DISTINCT, 55);
                }
            }
        }
    }
    return 0;
}




















//                                              语法分析器
/*
    采用递归下降分析方法
    具体步骤：
    1.先求所有非终结符的First集和Follow集
    2.对每一个非终结符构造对应的处理函数void procVn() Vn指代一个非终结符
    3.在main函数中调用procPROGRAM()
    文法来自龙书附录，然后更改为无左递归，无二义性，无回溯文法
/*
PROGRAM     ->  BLOCK
BLOCK       ->  {DECLS STMTS}
DECLS       ->  DECL DECLS| epsilon
DECL        ->  TYPE id;
TYPE        ->  BASIC TYPE_SCRIPT    定义数组，指针数组，变量 只支持一维,数组定义是java style，而不是C style
TYPE_SCRIPT ->  [num] TYPE_SCRIPT  | * [num] TYPE_SCRIPT | epsilon
BASIC       ->  int | float | char | double
STMTS       ->  STMT STMTS | epsilon                                               
STMT        ->  LOC = BOOL;                                              
               |if(BOOL) BLOCK ELSE_IF
               |while(BOOL) BLOCK
               |do STMT while (BOOL) BLOCK
               |for(id; REL; UNARY) BLOCK
               |break;
               |BLOCK
ELSE_IF     ->  else if (BOOL) BLOCK ELSE_IF | else BLOCK | epsilon                                                            
BOOL        ->  JOIN||BOOL  | JOIN                                                 有问题
JOIN        ->  EQUALITY JOIN_SCRIPT
JOIN_SCRIPT ->  &&JOIN | epsilon                                      
EQUALITY    ->  EQUALITY == REL  | EQUALITY != REL | REL                           有问题
REL         ->  EXPR < EXPR | EXPR <= EXPR | EXPR >= EXPR | EXPR > EXPR | EXPR
EXPR        ->  EXPR + TERM | EXPR - TERM  | TERM
TERM        ->  TERM*UNARY  | TERM / UNARY | TERM % UNARY | UNARY
UNARY       ->  !UNARY      | --UNARY      | ++UNARY      |  FACTOR
FACTOR      ->  (BOOL)      | LOC | num | real | true | false    
*/

//各个非终结符的first集和follow，用C++的集合来做
set<string> firstPROGRAM;
set<string> followPROGRAM;

set<string> firstBLOCK;
set<string> followBLOCK;

set<string> firstDECLS;
set<string> followDECLS;

set<string> firstDECL;
set<string> followDECL;

set<string> firstTYPE;
set<string> followTYPE;

set<string> firstTYPE_script;
set<string> followTYPE_script;

set<string> firstBASIC;
set<string> followBASIC;

set<string> firstSTMTS;
set<string> followSTMTS;

set<string> firstSTMTS_script;
set<string> followSTMTS_script;

set<string> firstSTMT;
set<string> followSTMT;

set<string> firstELSE_IF;
set<string> followELSE_IF;

set<string> firstLOC;
set<string> followLOC;

set<string> firstBOOL;
set<string> followBOOL;

set<string> firstJOIN;
set<string> followJOIN;

set<string> firstEQUALITY;
set<string> followEQUALITY;

set<string> firstREL;
set<string> followREL;

set<string> firstEXPR;
set<string> followEXPR;

set<string> firstTERM;
set<string> followTERM;

set<string> firstUNARY;
set<string> followUNARY;

set<string> firstFACTOR;
set<string> followFACTOR;





void error(){
    cout<<"errors occur!"<<endl;
}

void procPROGRAM(){
    //PROGRAM     ->  BLOCK
    procBLOCK();
}

void procBLOCK(){
    //BLOCK       ->  {DECLS STMTS}
    if(*(wordChain.begin()).token == "{"){
        wordChain.pop_front();
    }
    else{
        error();
        return;
    }
    procDECLES();
    procSTMTS();
}

void procDECLS(){
    //DECLS       ->  DECLS DECL| epsilon
    if( firstDECL.find(*(wordChain.begin()).token) ){
        procDECL();
        procDECLS();
    }
    else{
        return;//也就是理论课上所说的“空字消失”
    }
}

void procDECL(){
    //DECL        ->  TYPE id;
    procTYPE();
    if(searchIDTable(*(wordChain.begin())) == 1){
        wordChain.pop_front();
    }
}

void procTYPE(){
    //TYPE        ->  BASIC TYPE_SCRIPT    定义数组，指针数组，变量
    procBASIC();
    procTYPE_SCRIPT();
}

void procTYPE_SCRIPT(){
    //TYPE_SCRIPT ->  [num] TYPE_SCRIPT  | * [num] TYPE_SCRIPT | epsilon
    if(*(wordChain.begin()).token == "["){
        wordChain.pop_front();
        if(*(wordChain.begin()).token_name == 1){
            wordChain.pop_front();
        }
        if(*(wordChain.begin()).token == "]"){
            wordChain.pop_front();
        }
        procTYPE_SCRIPT();
    }
    else if(*(wordChain.begin()).token == "*"){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "["){
            wordChain.pop_front();
        }
        if(*(wordChain.begin()).token_name == 1){
            wordChain.pop_front();
        }
        if(*(wordChain.begin()).token == "]"){
            wordChain.pop_front();
        }
        procTYPE_SCRIPT();
    }
    else{
        return;//空字消失
    }

}

void procBASIC(){
    //BASIC       ->  int | float | char | double
    if(*(wordChain.begin()).token == "int"){
        wordChain.pop_front();
    }
    if(*(wordChain.begin()).token == "float"){
        wordChain.pop_front();
    }
    if(*(wordChain.begin()).token == "char"){
        wordChain.pop_front();
    }
    if(*(wordChain.begin()).token == "double"){
        wordChain.pop_front();
    }
    else{
        error();
    }
}

void procSTMTS(){
    //STMTS       ->  STMT STMTS | epsilon
    if(firstSTMT.find(*(wordChain.begin()).token)){
        procSTMT();
        procSTMTS();
    }
    else{
        return;
    }
}

void procSTMT(){
    /*STMT ->   LOC = BOOL;                                              
               |if(BOOL) BLOCK ELSE_IF
               |while(BOOL) BLOCK
               |do STMT while (BOOL) BLOCK
               |for(id; REL; UNARY) BLOCK
               |break;
               |BLOCK*/
    if( firstLOC.find(*(wordChain.begin()).token) ){
        procLOC();
        if(*(wordChain.begin()).token == "="){
            wordChain.pop_front();
        }
        procBOOL();
    }
    if(*(wordChain.begin()).token == "if"){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "("){
            wordChain.pop_front()
        }
        else{
            error();
        }
        procBOOL();
        if(*(wordChain.begin()).token == ")"){
            wordChain.pop_front()
        }
        else{
            error();
        }
        procBLOCK();
        procELSE_IF();
    }
    if(*(wordChain.begin()).token == "while"){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "("){
            wordChian.pop_front();
            procBOOL();
        }else{
            error();
        }
        if(*(wordChain.pop_front()).token == ")"){
            wordChain.pop_front();
        }else{
            error();
        }
        procBLOCK();
    }
    if(*(wordChain.begin()).token == "for"){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "("){
            wordChain.pop_front();
        }else{
            error();
        }
        if(*(wordChain.begin()).token_name == 0){
            wordChain.pop_front();
        }else{
            error();
        }
        if(*(wordChain.begin()).token == ";"){
            wordChain.pop_front();
        }else{
            error();
        }
        if(firstREL.find(*(wordChain.begin()).token)){
            wordChain.pop_front();
        }else{
            error();
            return;
        }
        if(*(wordChain.begin()).token == ";"){
            wordChain.pop_front();
        }else{
            error();
            return;
        }
        if(firstUNARY.find(*(wordChain.begin()).token)){
            wordChain.pop_front();
        }else{
            error();
            return;
        }
        procBLOCK();
    }
    if(*(wordChain.begin()).token == "break"){
        wordChain.pop_front();
    }
    if(*(wordChain.begin()).token == ";"){
        wordChain.pop_front();
    }
    if(firstBLOCK.find(*(wordChain.begin()).token)){
        procBLOCK();
    }
}

void procELSE_IF(){
    //ELSE_IF     ->  else if (BOOL) STMT ELSE_IF | else STMT | epsilon
    if(*(wordChain.begin()).token == "else"){
        if(*(wordChain.begin()).token == "if"){
            if(*(wordChain.begin()).token == "("){
                procBOOL();
            }else{
                error();
            }
            procSTMT();
        }else{
            error();
        }
        procELSE_IF();
    }
}

void procBOOL(){
    //BOOL        ->  JOIN||BOOL  | JOIN
    if(firstBOOL.find(*(wordChain.begin)).token){
        procJOIN();
        if(*(wordChain.begin()).token == "||"){
            wordChain.pop_front();
        }
        procBOOL();
    }
    else if(firstJOIN.find(*(wordChain.begin()).token)){
        procJOIN();
    }
}

void procJOIN(){
    //JOIN        ->  EQUALITY JOIN_SCRIPT
   procEQUALITY();
   procJOIN_SCRIPT();
}

void procJOIN_SCRIPT(){
    //JOIN_SCRIPT ->  &&JOIN | epsilon 
    if(*(wordChain.begin()).token == "&&"){
        wordChain.pop_front();
        procJOIN();
    }
    else{
        return;//空字消失
    }
}

void procEQUALITY(){
    //EQUALITY    ->  REL == EQUALITY  | REL != EQUALITY | REL
    //在这里突然发现可以不用消除回溯，先处理REL，然后根据REL后面的符号判断采用哪个产生式
    procREL();
    if(*(wordChain.begin()).token == "="){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "="){
            wordChain.pop_front();
            procEQUALITY();
        }
    }
    if(*(wordChain.begin()).token == "!"){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "="){
            wordChain.pop_front();
            procEQUALITY();
        }
    }
    else{// EQUALITY -> REL, 而前面已经有procREL()了，所以不需要处理了
        return;
    }
}

void procREL(){
    //REL         ->  EXPR < EXPR | EXPR <= EXPR | EXPR >= EXPR | EXPR > EXPR | EXPR
    procEXPR();
    if(*(wordChain.begin()).token == "<"){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "="){
            wordChain.pop_front();
        }
        procEXPR();
    }else if(*(wordChain.begin()).token == ">"){
        wordChain.pop_front();
        if(*(wordChain.begin()).token == "="){
            wordChain.pop_front();
        }
        procEXPR();
    }else{
        return;
    }
}

void procEXPR(){
    //EXPR        ->  TERM + EXPR | TERM - EXPR  | TERM
    procTERM();
    if(*(wordChain.begin()).token == "+" ||
       *(wordChain.begin()).token == "-"){
            wordChain.pop_front();
            procEXPR();
    }else{
        return;
    }
}

void procTERM(){
    //TERM        ->  UNARY * TERM  | UNARY / TERM | UNARY % TERM | UNARY
    procUNARY();
    if(*(wordChain.begin()).token == "*" || 
       *(wordChain.begin()).token == "/" ||
       *(wordChain.begin()).token == "%"){
           wordChain.pop_front();
           procTERM();
    }else{
        return;
    }
}

void procUNARY(){
    //UNARY       ->  !UNARY      | --UNARY      | ++UNARY      |  FACTOR
    if(*(wordChain.begin()).token == "!"  ||
       *(wordChain.begin()).token == "--" ||
       *(wordChain.begin()).token == "++"){
           procUNARY();
    }else{
        procFACTOR();
    }
}

void procFACTOR(){
    //FACTOR      ->  (BOOL)      | LOC | num | real | true | false   
    if(*(wordChain.begin()).token == "("){
        wordChain.pop_front();
        procBOOL();
        if(*(wordChain.begin()).token == ")"){
            wordChain.pop_front();
        }
    }
    if(firstLOC.find(*(wordChain.begin()).token)){
        procLOC();
    }
    if(*(wordChain.begin()).token_name == 0      ||
       *(wordChain.begin()).token_name == "True" ||
       *(wordChain.begin()).token_name == "False"||){
            wordChain.pop_front();
    }else{
        error();
        return;
    }
}




















//主函数
int main(){
    //词法分析 开始
    getWordChain();
    //词法分析 结束
    //语法分析 开始
    procPROGRAM();
    //语法分析 结束
    return 0;
}
