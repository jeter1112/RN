#include"re_nfa.h"
#include<stdlib.h>
#include<malloc.h>
#include<string.h>
#include<assert.h>
#include<stdio.h>
/*********
 * static struct.method declaireration
 * 
 * */


static struct Mlist;
typedef struct Mlist mlist;
struct Mlist
{
    int msize;
    state** s;
};



static struct Slist;
typedef struct Slist slist;

static struct Lnode;
typedef struct Lnode node;

struct Lnode
{
    state** var;
    node* next;
};
struct Slist
{
    int size;
    node* head;
};


static start_list(state*,mlist*);


static node* create_node(state**,node*);
static slist* create_list(state**);

static slist* append(slist*,slist*);
static void  patch(slist*,state*);


static struct Frag;
typedef struct Frag frag;

struct Frag
{
    state* start;
    slist* outlist;
};


static state* create_state(int,state*,state*);
static int is_accept_state(state*);
static int is_split_state(state*);


static frag* create_frag(state*,slist*);


static int is_alphabet_or_digit(char);
static int is_single_symbol(char);
static int is_dot_or(char);



/******* static method implementation  **/



static start_list(state*start,mlist*m)
{
    m->msize=1;
    m->s[0]=start;
}


static node* create_node(state**s,node*next)
{
    node* n=malloc(sizeof(node));
    assert(n!=NULL);
    n->var=s;
    n->next=next;
    return n;
}

static slist* create_list(state** s)
{
    slist* ns=malloc(sizeof(slist));
    assert(ns!=NULL);
    ns->head=create_node(s,NULL);
    ns->size=1;
}

static slist* append(slist*s1,slist*s2)
{
    
    if(s1->size==0)
        return s2;
    if(s2->size==0)
        return s1;

    node*p=s1->head;

    for(int i=1;i<s1->size;++i)
        p=p->next;
    p->next=s2->head;

    s1->size=s1->size+s2->size;

    return s1;

}


static void patch(slist* l,state* s)
{
    node* p=l->head;

    for(int i=0;i<l->size;++i)
    {
        (*(p->var))->out=s;
        p=p->next;
    }
}





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





static frag* create_frag(state*start,slist*l)
{
    frag* nf=malloc(sizeof(frag));
    nf->start=start;
    nf->outlist=l;
    return nf;
}


static state* create_state(int input,state*out,state*out1)
{
    state* ns=malloc(sizeof(state));
    ns->charnum=input;
    ns->out1=out1;
    ns->out=out;
}

static int is_accept_state(state*s)
{
    return s->charnum==257;
}

static int is_split_state(state* s)
{
    return s->charnum==256;
}







/*** API implementation ****/





char* infix_to_postfix(char* infix)
{
    int len=strlen(infix);
    assert(len>0);


    //printf("infix:%s\n",infix);

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
    //printf("strong:%s\n",infix_strong);


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

    //printf("post:%s\n",post);
    return post;

}


state* RE_to_NFA(char* post)
{
    int leng=strlen(post);
    frag* stack[leng];
    int s_size=0;


    for(int i=0;i<leng;++i)
    {

        switch(post[i])
        {
            case '*':
                frag* f=stack[--s_size];
                state* s=create_state(256,f->start,NULL);
                patch(f->outlist,s);
                frag* nf=create_frag(s,create_list(&(s->out1)));
                stack[s_size++]=nf;
                free(f);
                break;

            case '+':
                frag* f=stack[--s_size];
	            state* s=create_state(256,f->start,NULL);
	            patch(f->outlist, s);
	            stack[s_size++]=create_frag(f->start, list1(&s->out1));
                free(f);
                break;
            
            case '?':
                frag* f=stack[--s_size];
	            s = create_state(256,f->start,NULL);
                stack[s_size++]=create_frag(s,append(f->outlist,create_list(&(s->out1))));
                free(f);
                break;
            
            case '|':
                frag* f2=stack[--s_size];
	            frag* f1=stack[--s_size];
	            s = create_state(256,f1->start,f2->start);
                stack[s_size++]=create_frag(s,append(f1->outlist,f2->outlist));
	            free(f2);
                free(f1);
                break;
            
            case '.':
                frag* f2=stack[--s_size];
	            frag* f1=stack[--s_size];
	            patch(f1->outlist, f2->start);
	            stack[s_size++]=create_frag(f1->start, f2->outlist);
                free(f2);
                free(f1);
	
                break;

            default:
                state* s= create_state(post[i],NULL,NULL);
                frag* f= create_frag(s,create_list(&(s->out)));
                stack[s_size++]=f;
                break;

        }

        
    }

    frag* f=stack[--s_size];
    state* match=create_state(257,NULL,NULL);

	patch(f->outlist, match);
    state* result=f->start;

    free(f);
	return result;

}




int RE_macth_string(char*post,state*start)
{
    mlist l1;
    mlist l2;

    mlist*cl;
    mlist*nl;
    mlist*t;

    


}


int main()
{
    return 0;

}






