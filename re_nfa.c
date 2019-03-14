#include"re_nfa.h"
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include<assert.h>
#include<stdio.h>
/*********
 * static method declaireration
 * 
 * */

static int is_alphabet_or_digit(char);
static int is_single_symbol(char);

static int is_dot_or(char);



/******* static method implementation  **/


static int is_alphabet_or_digit(char target)
{

  return (target<58 && target>47) || (target<91 && target>64)||(target<123 &&target>96);

}


static int is_single_symbol(char target)
{
    return (target=='*'||target =='?'||target =='+');
}

static int is_dot_or(char target)
{
    return (target=='.'||target =='|');
}






/*** API implementation ****/


char* infix_to_postfix(char* infix)
{
    int len=strlen(infix);
    assert(len>0);


    printf("infix:%s\n",infix);

    char infix_strong [len+len/2+1];

    char* p=infix;
    char* q=infix_strong;
    *q=*p;
    for (++p,++q;*p!='\0';++p,++q)
    {
        if(is_alphabet_or_digit(*p)||*p=='(')
        {
            if(is_alphabet_or_digit(*(p-1))||is_single_symbol(*(p-1))||(*(p-1)==')'))
                {
                    *q='.';
                    *(++q)=*p;
                }
            else
            {
             *q=*p;
            }
        }
        else
        {
             *q=*p;
        }
        
    }
    *q='\0';
    printf("strong:%s\n",infix_strong);


    int stronglen=strlen(infix_strong);

    char* post=malloc(stronglen+1);

    char stack[stronglen];
    int s_size=0;

    q=infix_strong;
    char* tip=post;

    /***
     * priority: ) * ? + . | (
     * 
     */

    for(;*q!='\0';++q)
    {   
       
        if(is_alphabet_or_digit(*q)||is_single_symbol(*q))  //notice single operator
        {
            *(tip++)=*q;
           
        }
        else if(is_dot_or(*q))
        {
             
            if(s_size==0 ||stack[s_size-1]=='(')
            {
                stack[s_size++]=*q;
            }
            else
            {
                *(tip++)=stack[--s_size];
                stack[s_size++]=*q;
            }
            
            
        }
        else if(*q==')')
        {   
            
            while(stack[s_size-1]!='(')
            {
                *(tip++)=stack[--s_size];
                
                assert(s_size!=0);
            }
            s_size--;

        }
        else if(*q=='(')
        {
            stack[s_size++]='(';
            
        }
    }
        

    while(s_size!=0)
    {
        *(tip++)=stack[--s_size];
    }


    *(tip)='\0';

    printf("post:%s\n",post);
    return post;

}


int main()
{
    char* infix="ab";
    infix_to_postfix(infix);

    printf("\n\n\n");
    char* infix_test1="a(bb)*|c";
    infix_to_postfix(infix_test1);

    char* infix_test2="a?(bb)*|c+";
    infix_to_postfix(infix_test2);
    return 0;

}






