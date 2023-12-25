/*
 * 文法G
 * program -> block

block -> { stmts }

stmts -> stmt stmts | ε

stmt -> id = expr;
     | if ( bool ) stmt stmt_1
     | while (bool) stmt
     | do stmt while (bool_0)
     | break;
     | block

stmt_1 -> else stmt | ε

bool_0 -> expr bool_1

bool_1 -> < expr
          | <= expr
          | > expr
          | >= expr
          | ε

expr -> term expr_1

expr_1 -> + term expr_1
          | - term expr_1
          | ε

term -> factor term_1

term_1 -> * factor term_1
          | / factor term_1
          | ε

factor -> ( expr )
          | id
          | num
 *
 *
 * */
#include <iostream>
#include <string>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

string program_text;
int index_p = 0;

void program();
void block();
void stmts();
void stmt();
void stmt_1();
void bool_0();
void bool_1();
void expr();
void expr_1();
void term();
void term_1();
void factor();

// Utility functions
void consumeWhitespace() {
    while (program_text[index_p]==' '||program_text[index_p]=='\n'||program_text[index_p]=='\t') {
        index_p++;
    }
}

bool isIdChar(char c) {
    return isalnum(c) || c == '_';
}

string getId() {
    consumeWhitespace();
    string id;
    while (isIdChar(program_text[index_p])) {
        id.push_back(program_text[index_p++]);
    }
    return id;
}

string getNum() {
    consumeWhitespace();
    string num;
    while (isdigit(program_text[index_p])) {
        num.push_back(program_text[index_p++]);
    }
    return num;
}

void expect(const string& expected) {
    consumeWhitespace();
    for (char c : expected) {
        if (program_text[index_p] != c) {
            cerr << "Syntax error: expected '" << c << "', found '" << program_text[index_p] << "' at position " << index_p << endl;
            exit(EXIT_FAILURE);
        }
        index_p++;
    }
}


// Parser functions implementation
// 解析函数实现
void program() {
    cout << "program => block" << endl;
    block();
}

void block() {
    cout << "block => { stmts }" << endl;
    expect("{");
    stmts();
    expect("}");
}

void stmts() {
    cout << "stmts => stmt stmts | ε" << endl;
    consumeWhitespace();
    if (program_text[index_p] != '}') {
        stmt();
        stmts();
    } else {
        cout << "stmts => ε" << endl;
    }
}

void stmt() {
    consumeWhitespace();
    if (isIdChar(program_text[index_p])) { // Check if it's an identifier
        string id = getId();
        consumeWhitespace();
        if (program_text[index_p] == '=') {
            index_p++;
            cout << "stmt => id = expr;" << endl;
            expr();
            expect(";");
        } else {
            cerr << "Unexpected token after identifier: '" << program_text[index_p] << "'" << endl;
            exit(EXIT_FAILURE);
        }
    } else if (program_text.substr(index_p, 2) == "if") { // if or if-else statement
        index_p += 2;
        cout << "stmt => if (bool) stmt stmt_1" << endl;
        expect("(");
        bool_0();
        expect(")");
        stmt();
        stmt_1();
    } else if (program_text.substr(index_p, 5) == "while") { // while statement
        index_p += 5;
        cout << "stmt => while (bool) stmt" << endl;
        expect("(");
        bool_0();
        expect(")");
        stmt();
    } else if (program_text.substr(index_p, 2) == "do") { // do-while statement
        index_p += 2;
        cout << "stmt => do stmt while (bool_0);" << endl;
        stmt();
        expect("while");
        expect("(");
        bool_0();
        expect(")");
        expect(";");
    } else if (program_text[index_p] == '{') { // block
        cout << "stmt => block" << endl;
        block();
    } else if (program_text.substr(index_p, 5) == "break") { // break statement
        index_p += 5;
        cout << "stmt => break;" << endl;
        expect(";");
    } else {
        cerr << "Unexpected token: '" << program_text[index_p] << "'" << endl;
        exit(EXIT_FAILURE);
    }
}

void stmt_1() {
    consumeWhitespace();
    if (program_text.substr(index_p, 4) == "else") {
        index_p += 4;
        cout << "stmt_1 => else stmt" << endl;
        stmt();
    } else {
        cout << "stmt_1 => ε" << endl;
    }
}


void bool_0() {
    cout << "bool_0 => expr bool_1" << endl;
    expr();
    bool_1();
}

void bool_1() {
    cout << "bool_1 => < expr | <= expr | > expr | >= expr | ε" << endl;
    consumeWhitespace();
    if (program_text[index_p] == '<' || program_text[index_p] == '>') {
        index_p++;
        if (program_text[index_p] == '=') {
            index_p++;
        }
        expr();
    } else if (program_text[index_p] == '=') {
        index_p++;
        expect("=");
        expr();
    } else {
        cout << "bool_1 => ε" << endl;
    }
}

void expr() {
    cout << "expr => term expr_1" << endl;
    term();
    expr_1();
}

void expr_1() {
    cout << "expr_1 => + term expr_1 | - term expr_1 | ε" << endl;
    consumeWhitespace();
    if (program_text[index_p] == '+' || program_text[index_p] == '-') {
        char op = program_text[index_p++];
        cout << "expr_1 => " << op << " term expr_1" << endl;
        term();
        expr_1();
    } else {
        cout << "expr_1 => ε" << endl;
    }
}

void term() {
    cout << "term => factor term_1" << endl;
    factor();
    term_1();
}

void term_1() {
    cout << "term_1 => * factor term_1 | / factor term_1 | ε" << endl;
    consumeWhitespace();
    if (program_text[index_p] == '*' || program_text[index_p] == '/') {
        char op = program_text[index_p++];
        cout << "term_1 => " << op << " factor term_1" << endl;
        factor();
        term_1();
    } else {
        cout << "term_1 => ε" << endl;
    }
}

void factor() {
    cout << "factor => ( expr ) | id | num" << endl;
    consumeWhitespace();
    if (program_text[index_p] == '(') {
        expect("(");
        expr();
        expect(")");
    } else if (isIdChar(program_text[index_p])) {
        cout << "factor => id" << endl;
        getId();
    } else if (isdigit(program_text[index_p])) {
        cout << "factor => num" << endl;
        getNum();
    } else {
        cerr << "Unexpected token in factor: '" << program_text[index_p] << "'" << endl;
        exit(EXIT_FAILURE);
    }
}
int main() {
    ifstream file("../test2.txt");
    stringstream buffer;
    if (file.is_open()) {
        buffer << file.rdbuf();
        file.close();
        string file_content = buffer.str();
        program_text=file_content;
    } else {
        std::cerr << "无法打开文件" << std::endl;
    }
    program();
    return 0;
}
