#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "constantManager.h"
#include "headers\\lexmeAnalysis.h"

/**
*   Define constant variables
*/

const int DOUBLE_BUFFER_SIZE = 11;
/**
*   define keyword struct
*/
typedef struct KEY_WORDS
{
    char *name; // string name, in other words,dynamic char array
    int inlineCode;
    int size;   // store name array size
}Element;

int state = 0;

/**
*   initialize all key words
*/
Element keywords[] = { {"main",1},{"int",2},{"if",3},{"else",4},{"while",5},{"for",6},{"read",7},{"write",8},{"bool",9},{"break",10},{"case",11},{"catch",12},{"char",13},{"class",14},{"const",15},{"continue",16},{"default",17},

                        {"delete",18},{"do",19},{"double",20},{"enum",21},{"false",22},{"true",23},{"float",24},{"friend",25},{"goto",26},{"inline",27},{"long",28},{"new",29},{"private",30},{"protected",31},{"public",32},

                        {"return",33},{"short",34},{"signed",35},{"sizeof",36},{"static",37},{"struct",38},{"switch",39},{"this",40},{"try",41},{"typedef",42},{"unsigned",43},{"using",44},{"virtual",45},{"void",46},{"include",47},

                        {"iostream",48},{"namespace",49},{"std",50},

                        {"(",121},{")",122},{",",123},{";",124},{"{",125},{"}",126},{"#",127},{"+",210},{"-",211},{"*",212},{"/",213},{"<",214},{"<=",215},{">",216},{">=",217},{"!=",218},{"==",219},{"=",220},{"<>",221}
                    };

int lexme_size = -1;
Element lexmes[50];

/**
*   define two pointer for finding correct lexme
*/
//char *lexmeBegin = NULL;
//char *forward = null;


void updateBuffer(char buffer[],int *offsetp){
    int size = DOUBLE_BUFFER_SIZE;
    char c;
    int i;
    FILE *fp = NULL;
    if((fp = fopen(FILE_AFTER_PREPREPROCESS,"r")) == NULL){
        printf("%s","open file fail");
        return false;
    }
    if(fseek(fp,*offsetp,SEEK_SET) < 0) {
        buffer[i] = EOF;    // add 'EOF'in the last
        return false;
    }  // cursor moves to previous position
    for(i=0; i<DOUBLE_BUFFER_SIZE;i++) {
        if((c = fgetc(fp)) != EOF) {
            buffer[i] = c;
        }else{
            buffer[i] = EOF;    // add 'EOF'in the last
            return false;
        }
    }
    //update offset
    *offsetp = *offsetp + i + 1;

}

char* relop(char *lexmeBegin,char *forward,int state){
    forward++;
    switch(state){
        case 1: //  +
            if(*forward == '+'){
                getToken(lexmeBegin,forward);
                return forward;
            }else if(*forward == '-'){
                error();
            }else if(*forward == '!'){
                error();
            }else if(*forward == '*'){
                error();
            }else if(*forward == '/'){
                error();
            }else if(*forward == '?'){
                error();
            }else if(*forward == '('){
                error();
            }else if(*forward == ')'){
                error();
            }else if(*forward == '{'){
                error();
            }else if(*forward == '}'){
                error();
            }else{
                forward--;
                getToken(lexmeBegin,forward);
                return forward;
            }
            break;
        case 2: //  -
            if(*forward == '-'){
                getToken(lexmeBegin,forward);
                return forward;
            }else if(*forward == '+'){
                error();
            }else if(*forward == '!'){
                error();
            }else if(*forward == '*'){
                error();
            }else if(*forward == '/'){
                error();
            }else if(*forward == '?'){
                error();
            }else if(*forward == '('){
                error();
            }else if(*forward == ')'){
                error();
            }else if(*forward == '{'){
                error();
            }else if(*forward == '}'){
                error();
            }else{
                forward--;
                getToken(lexmeBegin,forward);
                return forward;
            }
            break;
        case 3: //  *
            forward--;
            getToken(lexmeBegin,forward);
            return forward;
            break;
        case 4: //  /
            forward--;
            getToken(lexmeBegin,forward);
            return forward;
            break;




        }

}


void doLexmeAnalysis(){
    int offset = 0;
    int *offsetp = &offset; // offset of read file

    /**
    *   Double buffer area
    */
    char buffer1[DOUBLE_BUFFER_SIZE];
    char buffer2[DOUBLE_BUFFER_SIZE];
    buffer1[DOUBLE_BUFFER_SIZE - 1] = EOF;
    buffer2[DOUBLE_BUFFER_SIZE - 1] = EOF;
    updateBuffer(buffer1,offsetp);
    int *flag = buffer1;    // record current buffer
    char *lexmeBegin = buffer1;
    char *forward = buffer1;
    while(*forward != EOF){
        switch(*forward){
            case '+':
                forward = relop(lexmeBegin,forward,1);
                forward++;
                lexmeBegin = forward;
                break;

            case '-':
                forward = relop(lexmeBegin,forward,2);
                forward++;
                lexmeBegin = forward;
                break;

            case '*':
                forward = relop(lexmeBegin,forward,3);
                break;

            case '/':
                forward = relop(lexmeBegin,forward,4);
                break;

            case ' ':
            case '\n':
                lexmeBegin++;
                forward++;
                break;




        }
    }

}



Element create_element(int init_size){
    Element e;
    e.name = (char*) malloc(sizeof(char)*init_size);     // create dynamic name array within new word
    e.size = init_size;
    return e;
}

void free_element(Element *e){
    free(e->name);
    e->name = NULL;
    e->size = 0;
    e->inlineCode = 0;
}

void getToken(char *lexmeBegin,char *forward){
    // open file
    // write in
    int token_size = forward - lexmeBegin + 1;  // computing string size via two pointer address and one string that is cross two buffer, there may be a problem
    //printf("%d",token_size);
    char cur_str[token_size];
    int i = 0;
    while(lexmeBegin != (forward+1)){
        cur_str[i++] = *lexmeBegin++;
    }
    int code_result = getInlineCode(cur_str);
    Element e = create_element(sizeof(cur_str));    // get new lexme
    //copy new word into name array of new element
    strcpy(e.name,cur_str);
    e.inlineCode = code_result;
    //save to array
    lexmes[++lexme_size] = e;
    //if(lexme_size == 50){
        saveLexme();
    //}
}

void saveLexme(){
    FILE *fp = NULL;
    if((fp = fopen(FILE_LEXMES,"a")) == NULL){
        printf("%s","open file fail");
        return false;
    }

    // add additional signals for each word
    char c1 = ',';
    char *c1_p = &c1;
    int i;

    for(i=0; i<lexme_size+1; i++){
        //char *array_p = (char *)malloc(sizeof(lexmes[i].name+sizeof(lexmes[i].inlineCode)+3));

        // write each element into file
        int result;
        if(result = fputs("<",fp)<0){
            printf("write file fail, error code is (%d)",result);
            return false;
        }
        if(result = fputs(lexmes[i].name,fp)<0){
            printf("write file fail, error code is (%d)",result);
            return false;
        }
        if(result = fputs(",",fp)<0){
            printf("write file fail, error code is (%d)",result);
            return false;
        }
        if(result = fprintf(fp,"%d",lexmes[i].inlineCode)<0){   // 'fprintf' and 'fscanf' can write and read the file formatly
            printf("write file fail, error code is (%d)",result);
            return false;
        }
        if(result = fputs(">\n",fp)<0){
            printf("write file fail, error code is (%d)",result);
            return false;
        }
        //release current element in main memory
        free_element(&lexmes[i]);
    }

    //reset lexme_size
    lexme_size = -1;
    fclose(fp);
}

int getInlineCode(const char curStr[]){
    int i;
    for(i=0;i<sizeof(keywords)/sizeof(keywords[0]);i++){
        if(strcmp(keywords[i].name,curStr) == 0){
            return keywords[i].inlineCode;
        }
    }
    return 0;
}


void error(){
    printf("%s","codes error!");
    exit(1);
}

