#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_WORDS 1000

typedef struct {
    int id;
    char token[20]; // 识别到的单词或符号
    char sign[20];  // 助记符
    char value[20];      // 内码值
} Token;

Token keywords[MAX_WORDS]; // 存储关键字和符号
Token identifiers[MAX_WORDS]; // 存储标识符
Token numbers[MAX_WORDS]; // 存储数字
int id_count = 0; // 标识符计数
int num_count = 0; // 数字计数
int keyword_count = 0; // 关键字计数

// 函数原型声明
void Scanner(const char* filename);
void Lexscan(FILE* fp);
void Isalpha(FILE* fp);
void Isnumber(FILE* fp);
void Isanotation(FILE* fp);
void Isother(FILE* fp);
void Output();


void Scanner(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        printf("Failed to open source file.\n");
        return;
    }
    while (!feof(fp)) {
        Lexscan(fp);
    }

    fclose(fp);
    Output(); // 在文件处理完毕后输出二元式
}

void Lexscan(FILE* fp) {
    char c;
    // 修改处：改为循环读取，直到遇到EOF
    while ((c = fgetc(fp)) != EOF) {
        if (isalpha(c) || c == '_') {
            ungetc(c, fp); // 把字符放回去，以便Isalpha处理
            Isalpha(fp);
        } else if (isdigit(c)) {
            ungetc(c, fp); // 把字符放回去，以便Isnumber处理
            Isnumber(fp);
        } else if (c == '/') {
            ungetc(c, fp); // 把字符放回去，以便Isanotation处理
            Isanotation(fp);
        } else {
            ungetc(c, fp); // 把字符放回去，以便Isother处理
            Isother(fp);
        }
    }
}


void Isalpha(FILE* fp) {
    char word[100]; // 假设单词不超过100个字符
    int idx = 0;
    int isKeyword = 0;
    int inid = 0;
    char c = fgetc(fp);
    while (isalpha(c) || isdigit(c) || c == '_') {
        word[idx++] = c;
        c = fgetc(fp);
    }
    ungetc(c, fp); // 把最后读取的非标识符字符放回去
    word[idx] = '\0';

    // 在关键字数组中查找
    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(word, keywords[i].token) == 0) {
            printf("(%s, %s)\n", keywords[i].token, keywords[i].value); // 控制台输出
            isKeyword = 1;
            FILE *fp = fopen("../output_tokens.txt", "a+");
            fprintf(fp, "(%s, %s)\n", keywords[i].token, keywords[i].value);//打印到文件
            break;
        }
    }
    //在标识符数组查找
    for (int i = 0; i < id_count; i++) {
        if (strcmp(word, identifiers[i].token) == 0) {
            inid = 1;
            break;
        }
    }

    // 检查是否为关键字或已知标识符，否则添加到标识符表
    // 例如，这里只是简单地添加到标识符数组，实际上应该先检查是否已存在
    if(!isKeyword){
        strcpy(identifiers[id_count].token, word);
        strcpy(identifiers[id_count].sign, "id");
        char num[20];
        sprintf(num,"%d",id_count);
        //如果没在标识符数组里则写入标识符数组
        if(!inid){
        strcpy(identifiers[id_count].value,num); // 或者其他唯一标识
        id_count++;}}
}


void Isnumber(FILE* fp) {
    char num[100]; // 假设数字不超过100个字符
    int idx = 0;
    int isKeyword = 0;
    char c = fgetc(fp);
    while (isdigit(c)) {
        num[idx++] = c;
        c = fgetc(fp);
    }
    ungetc(c, fp); // 把最后读取的非数字字符放回去
    num[idx] = '\0';

    // 在关键字数组中查找
    for (int i = 0; i < keyword_count; i++) {
        if (strcmp(num, keywords[i].token) == 0) {
            printf("(%s, %s)\n", keywords[i].token, keywords[i].value); // 控制台输出
            isKeyword = 1;
            FILE *fp = fopen("../output_tokens.txt", "a+");
            fprintf(fp, "(%s, %s)\n", keywords[i].token, keywords[i].value);//打印到文件
            break;
        }
    }

    // 添加到数字数组
    if(!isKeyword){
        strcpy(numbers[num_count].token, num);
        strcpy(numbers[num_count].sign, "num");
        char num[20];
        sprintf(num,"%d",num_count);
        strcpy(numbers[num_count].value,num); // 或者其他唯一标识
        num_count++;}
}



void Isanotation(FILE* fp) {
    char c = fgetc(fp); // 读取第一个 '/'
    char next = fgetc(fp); // 读取紧接着的字符

    if (next == '/') {
        // 单行注释，读到行尾
        while ((c = fgetc(fp)) != '\n' && c != EOF);
    } else if (next == '*') {
        // 多行注释，读到 "*/"
        do {
            while ((c = fgetc(fp)) != '*' && c != EOF);
            if (c == EOF) break;
        } while ((c = fgetc(fp)) != '/' && c != EOF);
    } else {
        // 不是注释，是除号
        ungetc(next, fp); // 把 next 放回去，因为它不是注释的一部分
        // 处理除号...
        printf("(%s, %s)\n", keywords[21].token, keywords[21].value); // 控制台输出
        FILE *fp = fopen("../output_tokens.txt", "a+");
        fprintf(fp, "(%s, %s)\n", keywords[21].token, keywords[21].value);//打印到文件
    }
}



void Isother(FILE* fp) {
    char c = fgetc(fp);
    int isSymbol = 0;
    // 根据读取的字符处理特殊符号
    // 这里以处理 ';' 为例

    // 在关键字数组中查找符号
    for (int i = 0; i < keyword_count; i++) {
        if (keywords[i].token[0] == c && strlen(keywords[i].token) == 1) { // 假设符号只有一个字符
            printf("(%s, %s)\n", keywords[i].token, keywords[i].value); // 控制台输出
            isSymbol = 1;
            FILE *fp = fopen("../output_tokens.txt", "a+");
            fprintf(fp, "(%s, %s)\n", keywords[i].token, keywords[i].value);//打印到文件
            return;
        }
    }
//    if(!isSymbol){
//        if (c == ';') {
//            // 假设我们将 ';' 的内码值设为一个特定的值，例如100
//            Token token;
//            strcpy(token.token, ";");
//            strcpy(token.sign, "semicolon");
////            token.value = 100; // 为了示例，假设 ';' 的内码值为100
//            char num[20];
//            sprintf(num,"%d",);
//            strcpy(token.value)
//            // 添加到keywords数组或输出
//        } else {
//            // 处理其他字符...
//        }
//    }
//    printf("compile error");
//    exit(0);
}

void Output() {
    // 输出函数实现
    printf("\nIdentifiers:\n");
    for (int i = 0; i < id_count; i++) {
        printf("(%s, %s)\n", identifiers[i].token, identifiers[i].value);
        FILE *fp = fopen("../output_tokens.txt", "a+");
        fprintf(fp, "(%s, %s)\n", identifiers[i].token, identifiers[i].value);//打印到文件
    }

    printf("\nNumbers:\n");
    for (int i = 0; i < num_count; i++) {
        printf("(%s, %s)\n", numbers[i].token, numbers[i].value);
        FILE *fp = fopen("../output_tokens.txt", "a+");
        fprintf(fp, "(%s, %s)\n", numbers[i].token, numbers[i].value);//打印到文件
    }
    //遍历关键字和符号
//    printf("\nKeywords and Symbols:\n");
//    // 假设我们简单地将keyword_count用作演示，实际上应当根据keywords数组中的数据进行输出
//    for (int i = 0; i < 45; i++) {
//        printf("(%s, %s)\n", keywords[i].token, keywords[i].value);
//    }
}



int main() {
    // 初始化keywords数组索引
    int count = 0;
    FILE *file = fopen("E:\\project\\@DevCode\\LexicalAnalyzer\\key_word.txt", "r+");
    if (!file) {
        printf("fail open file");
    }
    //填充关键字
    while (fscanf(file, "%d %s %s %s\n", &keywords[count].id,
                  keywords[count].token, keywords[count].sign, keywords[count].value)) {
        count++;
        if (count >= MAX_WORDS) break; // 防止数组越界
    }

    //    // 打印读取的关键字验证
//    for (int i = 0; i < count; i++) {
//        printf("ID: %d, Token: %s, Sign: %s, Value: %d\n", keywords[i].id, keywords[i].token, keywords[i].sign, keywords[i].value);
//    }

//    // 更新关键字计数
    keyword_count = count;

//    fclose(fp); // 关闭文件


//
//  调用Scanner函数
    Scanner("E:\\project\\@DevCode\\LexicalAnalyzer\\input.txt");

    return 0;
}




