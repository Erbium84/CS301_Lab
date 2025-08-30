#include <stdio.h>
#include <string.h>
#include <ctype.h>

int countTokens(const char *code) {
    int count = 0;
    int i = 0;

    // List of two-character operators for easy checking.
    const char* two_char_ops[] = {
        "->", "++", "--", "<<", ">>", "<=", ">=", "==", "!=",
        "&&", "||", "+=", "-=", "*=", "/=", "%=", "&=", "^=", "|="
    };
    int num_two_char_ops = sizeof(two_char_ops) / sizeof(two_char_ops[0]);

    while (code[i] != '\0') {
        // 1. Skip all whitespace characters
        if (isspace(code[i])) {
            i++;
            continue;
        }

        // 2. Handle comments (they are not tokens)
        // Single-line comment
        if (code[i] == '/' && code[i + 1] == '/') {
            while (code[i] != '\n' && code[i] != '\0') {
                i++;
            }
            continue;
        }
        // Multi-line comment
        if (code[i] == '/' && code[i + 1] == '*') {
            i += 2; // Move past '/*'
            while (code[i] != '\0' && (code[i] != '*' || code[i + 1] != '/')) {
                i++;
            }
            if (code[i] != '\0') {
                i += 2; // Move past '*/'
            }
            continue;
        }

        // 3. Handle identifiers and keywords (e.g., 'main', 'int', 'my_var')
        if (isalpha(code[i]) || code[i] == '_') {
            count++;
            i++;
            while (isalnum(code[i]) || code[i] == '_') {
                i++;
            }
            continue;
        }

        // 4. Handle numbers (e.g., 123, 4.56) - treated as a single token
        if (isdigit(code[i])) {
            count++;
            i++;
            while (isdigit(code[i]) || code[i] == '.') { // Simple check for numbers
                i++;
            }
            continue;
        }
        
        // 5. Handle string literals (e.g., "hello") - one token
        if (code[i] == '"') {
            count++;
            i++;
            while (code[i] != '\0' && code[i] != '"') {
                if (code[i] == '\\') i++; // Skip escaped characters like \"
                i++;
            }
            if(code[i] == '"') i++;
            continue;
        }

        // 6. Handle char literals (e.g., 'a') - one token
        if (code[i] == '\'') {
            count++;
            i++;
            while (code[i] != '\0' && code[i] != '\'') {
                if (code[i] == '\\') i++; // Skip escaped characters like \'
                i++;
            }
            if(code[i] == '\'') i++;
            continue;
        }

        // 7. Handle multi-character operators (e.g., '++', '>=', '->')
        int found_multi_char = 0;
        for (int j = 0; j < num_two_char_ops; j++) {
            if (strncmp(&code[i], two_char_ops[j], 2) == 0) {
                count++;
                i += 2;
                found_multi_char = 1;
                break;
            }
        }
        if (found_multi_char) {
            continue;
        }

        // 8. If none of the above, it's a single-character token (e.g., ';', '+', '{')
        // We check for non-whitespace to be safe.
        if (!isspace(code[i])) {
            count++;
            i++;
        }
    }

    return count;
}


int main() {
    // Example C code to be tokenized
    char c_code[1000];
    printf("Enter $ to terminate program\n");

    char x;
    int i=0;
    scanf("%c", &x);
    c_code[i]=x;
    i++;
    while(x!='$'){
        scanf("%c", &x);
        c_code[i]=x;
        i++;
    }
    c_code[i-1]='\0';

    int token_count = countTokens(c_code);

    
    printf("Total number of tokens found: %d\n", token_count);

    return 0;
}