
/*

{} 内只有键值对
[] 内只有 {}，只处理[]内的第一个{}
递归处理每一层，最外层为第一层，value对应的[]中的{}、value对应的{}属于第二层，以此类推
创建每个model时，先使用数组记录每个属性的类型和名称[[name,type],...]，再生成model

 
*/

#include <stdio.h>
#include <stdlib.h>
#include "property_list.h"
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#define nm_brackets 1000

void handleBigBrackets(char *className);
void handleMidBrackets(char *className);
void handleProperty(PropertyList *plist);
void handleQuoteMarks(void);
void handleColon(void);
char nextChar(void);
void createModel(PropertyList *plist, char *className);
int isBracketMatch(int left, int right);
void writeSentence(FILE *fh, char *format, ...);

FILE *inputFile = NULL;

char brackets[nm_brackets];
int bracketCount = 0;

char ch;

int main(int argc, char *argv[]) {
	
	if (argc != 2) {
		printf("param error\n");
		return 1;
	}

	char *filepath = argv[1];
	inputFile = fopen(filepath, "r");
	if (inputFile == NULL) {
		printf("open failed\n");
		return 1;
	}

	while((ch = nextChar()) != EOF) {
		switch (ch) {
		case '{': 
			handleBigBrackets("");
			break;
		case '[': 
			handleMidBrackets("");
			break;
		default: 
			break;
		}
	}

	fclose(inputFile);
	printf("process finished\n");

	return 0;
}

void handleBigBrackets(char *className) {
	/*
	处理到下一个匹配的括号为止
	大括号代表一个模型的开始，需要创建数组，用于保存属性描述
	*/
    int leftBracketIndex = bracketCount - 1;
	PropertyList *plist = malloc(sizeof(PropertyList));
	
	while (1) {
        ch = nextChar();
        if (ch == '"') {
            handleProperty(plist);
        }
        if (ch == '}') {
            if (isBracketMatch(leftBracketIndex, bracketCount - 1)) {
                
                // 创建模型
                createModel(plist, className);
                break;
                
            }else {
                continue;
            }
        }else if (ch == ',') {
            continue;
        }else {
            printf("handleBigBrackets: syntax error\n");
            exit(1);
        }
	}

    propertyListDestroy(plist);
}

void handleMidBrackets(char *className) {
    
    int leftBracketIndex = bracketCount - 1;
    
    while (1) {
        ch = nextChar();
        if (ch == ']') {
            ch = nextChar();
            break;
        }else if (ch == '{') {
            handleBigBrackets(className);
            while ((ch = nextChar()) != EOF) {
                if (ch == ']') {
                    if (isBracketMatch(leftBracketIndex, bracketCount - 1)) {
                        return;
                    }
                }
            }
            
        }else {
            while ((ch = nextChar()) != EOF) {
                if (ch == ']') {
                    if (isBracketMatch(leftBracketIndex, bracketCount - 1)) {
                        return;
                    }
                }
            }
        }
    }
    
}

void handleProperty(PropertyList *plist) {
	
	char pname[100];
	int ptype = 0;

	int i = 0;
	while ((ch = nextChar()) != '"') {
		pname[i] = ch;
		i++;
	}
    pname[i] = '\0';

	nextChar();  // :

	// value  0-字符串，1-数字，2-id，3-数组
	ch = nextChar();
	if ((ch >= 48 && ch <= 57) || ch == 'n') {
		ptype = 1;
	}else if (ch == '{') {
		ptype = 2;
        handleBigBrackets(pname);
	}else if (ch == '[') {
		ptype = 3;
        handleMidBrackets(pname);
	}else {
		ptype = 0;
	}

	propertyListAppend(plist, pname, i, ptype);
    
    // 跳过逗号及其前面的所有内容
    while (ch != ',' && ch != '}') {
        ch = nextChar();
    }
}

char nextChar(void) {
	char ch = fgetc(inputFile);
	while (isspace(ch)) {
		ch = fgetc(inputFile);
	}
    if (ch == '{' || ch == '}' || ch == '[' || ch == ']') {
        brackets[bracketCount++] = ch;
    }
	return ch;
}

void createModel(PropertyList *plist, char *className) {
    
    char filename[100];
    sprintf(filename, "/Users/suidongyang/Desktop/%sModel.h", className);
    FILE *fh = fopen(filename, "w");
    
    // 0-字符串，1-数字，2-id，3-数组
    char *modifiers[4] = {"copy", "copy", "strong", "strong"};
    char *types[4] = {"NSString *", "NSString *", "id ", "NSArray *"};
    
    writeSentence(fh, "@interface %sModel: PDBaseDataModel\n\n", className);
    
    printf("---------- begin ----------\n");
    Property *p = plist->head;
    while (p != NULL) {
        printf("%s, %d\n", p->name, p->type);
        writeSentence(fh, "@property (nonatomic, %s) %s%s;\n", modifiers[p->type], types[p->type], p->name);
        p = p->next;
    }
    printf("----------- end -----------\n");
    
    writeSentence(fh, "\n@end\n");
    
    fclose(fh);
}

void writeSentence(FILE *fh, char *format, ...) {
    
    va_list args;
    va_start(args, format);

    char buffer[200];
    vsprintf(buffer, format, args); // sprintf 只能接受可变参数列表，vsprintf接受va_list
    
    fwrite(buffer, strlen(buffer), 1, fh);

    va_end(args);
}

int isBracketMatch(int left, int right) {
    int flag = 0;
    for (int i = left; i <= right; i++) {
        if (brackets[i] == '{' || brackets[i] == '[') {
            flag++;
        }else {
            flag--;
        }
    }
    return flag == 0;
}










