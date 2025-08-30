#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define MTL 256

static const char* keywords[] = {
    "auto","break","case","char","const","continue","default","do",
    "double","else","enum","extern","float","for","goto","if","inline","int",
    "long","register","restrict","return","short","signed","sizeof","static",
    "struct","switch","typedef","union","unsigned","void","volatile","while",
    "_Alignas","_Alignof","_Atomic","_Bool","_Complex","_Generic","_Imaginary",
    "_Noreturn","_Static_assert","_Thread_local"
};
static const int keywordCount = sizeof(keywords)/sizeof(keywords[0]);

static const char* ops[] = {
    /* 3-char */
    "<<=", ">>=", "...",
    /* 2-char */
    "->","++","--","==","!=", "<=", ">=", "&&","||",
    "+=","-=","*=","/=","%=","&=","^=","|=",
    "<<",">>",
    /* 1-char operators/punctuators often treated as operators here */
    "=","+","-","*","/","%","<",">","!","&","|","^","~","?",":","."
};
static const int opCount = sizeof(ops)/sizeof(ops[0]);

static bool isSeparator(char c) {
    return strchr(",;(){}[]", c) != NULL;
}

static bool isKeyword(const char* s) {
    for (int i = 0; i < keywordCount; ++i)
        if (strcmp(s, keywords[i]) == 0) return true;
    return false;
}

/* Try to match the *longest* operator/punctuator starting at s */
static int matchOperator(const char* s, const char** matched) {
    int bestLen = 0;
    const char* best = NULL;
    for (int i = 0; i < opCount; ++i) {
        int n = (int)strlen(ops[i]);
        if (n > bestLen && strncmp(s, ops[i], (size_t)n) == 0) {
            bestLen = n;
            best = ops[i];
            if (bestLen == 3) break; 
        }
    }
    if (matched) *matched = best;
    return bestLen;
}

static void printHeader(void) {
    printf("%-20s | %s\n", "Lexeme", "Token Type");
    printf("---------------------+---------------------\n");
}
static void printTok(const char* lexeme, const char* type) {
    printf("%-20s | %s\n", lexeme, type);
}

static void lexer(const char* code) {
    size_t i = 0, n = strlen(code);
    char buf[MTL];

    while (i < n) {
        char c = code[i];

        if (isspace((unsigned char)c)) { ++i; continue; }

        if (c == '/' && i + 1 < n) {
            if (code[i+1] == '/') {           
                i += 2;
                while (i < n && code[i] != '\n') ++i;
                continue;
            } else if (code[i+1] == '*') {    
                i += 2;
                while (i + 1 < n && !(code[i] == '*' && code[i+1] == '/')) ++i;
                if (i + 1 < n) i += 2;  
                continue;
            }
        }

        /* String literal */
        if (c == '"') {
            size_t j = 0; buf[j++] = code[i++]; /* opening " */
            while (i < n) {
                buf[j++] = code[i];
                if (code[i] == '\\' && i + 1 < n) { buf[j++] = code[++i]; ++i; continue; }
                if (code[i] == '"') { ++i; break; }
                ++i;
                if (j >= MTL-2) break;
            }
            buf[j] = '\0';
            printTok(buf, "StringLiteral");
            continue;
        }

        /* Char literal */
        if (c == '\'') {
            size_t j = 0; buf[j++] = code[i++]; /* opening ' */
            while (i < n) {
                buf[j++] = code[i];
                if (code[i] == '\\' && i + 1 < n) { buf[j++] = code[++i]; ++i; continue; }
                if (code[i] == '\'') { ++i; break; }
                ++i;
                if (j >= MTL-2) break;
            }
            buf[j] = '\0';
            printTok(buf, "CharLiteral");
            continue;
        }

        /* Identifier or keyword */
        if (isalpha((unsigned char)c) || c == '_') {
            size_t j = 0;
            while (i < n && (isalnum((unsigned char)code[i]) || code[i] == '_')) {
                if (j < MTL-1) buf[j++] = code[i];
                ++i;
            }
            buf[j] = '\0';
            printTok(buf, isKeyword(buf) ? "Keyword" : "Identifier");
            continue;
        }

        /* Number: (simple) integer literal */
        if (isdigit((unsigned char)c)) {
            size_t j = 0;
            while (i < n && isdigit((unsigned char)code[i])) {
                if (j < MTL-1) buf[j++] = code[i];
                ++i;
            }
            buf[j] = '\0';
            printTok(buf, "IntegerConstant");
            continue;
        }

        /* Operators & punctuators (maximal-munch) */
        {
            const char* matched = NULL;
            int len = matchOperator(&code[i], &matched);
            if (len > 0) {
                /* classify a few as separators if you want a separate class */
                if (len == 1 && isSeparator(code[i])) {
                    char s[2] = { code[i], 0 };
                    printTok(s, "Separator");
                } else {
                    char s[4] = {0,0,0,0};
                    for (int k = 0; k < len && k < 3; ++k) s[k] = code[i+k];
                    printTok(s, "Operator");
                }
                i += (size_t)len;
                continue;
            }
        }

        /* Single-character separators not in ops[] */
        if (isSeparator(c)) {
            char s[2] = { c, 0 };
            printTok(s, "Separator");
            ++i;
            continue;
        }

        /* Unknown / unhandled */
        {
            char s[2] = { c, 0 };
            printTok(s, "Unknown");
            ++i;
        }
    }
}

int main(void) {
    const char* code = "int main() { int a *= 5 + 3; return a; }";
    printHeader();
    lexer(code);
    return 0;
}