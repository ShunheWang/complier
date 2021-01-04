/* simple preprocessing machine for c code
*   1. delete all comments
*   2. delete 'enter' and extra spaces ... within c code
*/
#include <stdio.h>
#include <stdlib.h>
#include "constantManager.h"
#include "preprocessing.h"


/** eStateNull --> start to record in every time
*   eStateOne --> '/' or '/"'
*   eStateTwo --> '/*'  or 'abc '
*   eStateThree --> '/*abc' or '/* abc'
*   eStateFour --> '/*abc*'
*/
enum CHECK_STATE
{
    eStateNull,eStateOne,eStateTwo,eStateThree,eStateFour
};


int getState(const char c)
{
    if(c == '/')
        return 1;
    else if(c == '*')
        return 2;
    else if(c == '\"')
    {
        return 4;
    }
    else if(c == ' ')
    {
        return 5;
    }
    else
        return 3;
}

void doPreprocessing()
{
    FILE *fp = NULL;
    FILE *wfp = NULL;
    if(fp = fopen(FILE_URL,FILE_R) == NULL){
        printf("%s","file doesnt exist");
        return false;
    }
    wfp = fopen(FILE_AFTER_PREPREPROCESS,"a");
        //printf("cannot open %s",&wfp);
        //exit(0);
    char intc;
    int counter = 0;
    while((intc = fgetc(fp))!=EOF)
    {
        char outc;
        if(deleteComments(intc,&outc) == true && deleteEnters(intc,&counter) == false  && deleteSpaces(intc) == false)
        {
            //printf("%c",outc);
            int inputResult;
            if((inputResult = fputc(outc,wfp)) < 0) {
                printf("%s","write file fail");
                return false;
            }
        }
    }
    closeFile(wfp);
}

int deleteEnters(const int c, int *counterp)
{
    if(*counterp == 1 && c == '\n')
    {
        //printf("%c, %d",c, *counterp);
        return true;
    }
    else if(c == '\n')   // Input is enter and counterp is 0 at the same time
    {
        //printf("%c, %d",c, *counterp);
        *counterp = 1;
    }
    else    // Input is not enter and counterp is 1 at the same time
    {
        //printf("%c, %d",c, *counterp);
        *counterp = 0;
    }
    return false;

}

int deleteSpaces(const int c)
{
    static int enter_record = 0;    // \n
    static int ds_record = 0; // "
    static int is_letter = 0;
    static int space_record = 0;
    //printf("%c",c);

    /** check "". when "" happens, contents will not change*/
    if(ds_record == 0 && c == '"')
    {
        ds_record = 1;
        return false;
    }
    else if(ds_record == 1)
    {
        if(c == '"')
        {
            ds_record = 0;
        }
        return false;
    }

    if(space_record == 0 && (c == '\n' || getState(c) == 5))    // delete next time
    {
        space_record = 1;
        //printf("%d",space_record);
        return false;
    }
    else if(space_record == 1 && getState(c) == 5)      // delete extra spaces
    {
        //printf("%d",2);
        return true;
    }
    else if(space_record == 1 && getState(c) == 3)
    {
        //printf("%d",3);
        space_record = 0;
        return false;
    }
    return false;
}


int deleteComments(int intc,char *outc)
{
    static enum CHECK_STATE state = eStateNull;
    int nextState = getState(intc);
    if(state == eStateNull && nextState == 1)   // '/'
    {
        state = eStateOne;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateOne && nextState == 2)    // '/*'
    {
        state = eStateTwo;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateTwo && nextState == 2)    // '/**'
    {
        state = eStateFour;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateTwo && nextState == 3)    // '/*abc'
    {
        state = eStateThree;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateThree && nextState == 3)    // '/*abc'
    {
        state = eStateThree;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateThree && nextState == 3)    // '/*abcd'
    {
        state = eStateThree;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateThree && nextState == 2)    // '/*abc*'
    {
        state = eStateFour;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateFour && nextState == 3)    // '/*abc*a'
    {
        state = eStateThree;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateFour && nextState == 1)    // '/*abc*/'
    {
        state = eStateNull;
        //printf(" %d, %d\n",state,nextState);
    }
    else if(state == eStateNull)    // '/*abc*/'
    {
        //printf(" %d, %d\n",state,nextState);
        *outc = intc;
        return true;
    }
    return false;

}


/**
* close file
*/
void closeFile(const FILE *fp)
{
    fclose(fp);
}


