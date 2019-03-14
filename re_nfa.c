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



typedef struct Mlist mlist;
struct Mlist
{
    int msize;
    state** s;
};




typedef struct Slist slist;


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
int ismatch(mlist*);
static mlist* start_list(state*,mlist*);
void step(mlist*,int,mlist*);
void addstate(mlist*,state*);

static node* create_node(state**,node*);
static slist* create_list(state**);

static slist* append(slist*,slist*);
static void  patch(slist*,state*);



typedef struct Frag frag;

struct Frag
{
    state* start;
    slist* outlist;
};


static state* create_state(int,state*,state*);
static int is_accept_state(state*);
static int is_split_state(state*);
static void delete_state(state*);
static void delete_frag(frag*);
static frag* create_frag(state*,slist*);


static int is_alphabet_or_digit(char);
static int is_single_symbol(char);
static int is_dot_or(char);



/******* static method implementation  **/


int ismatch(mlist *l)
{
	int i;

	for(i=0; i<l->msize; i++)
		if(l->s[i]->charnum==257)
			return 1;
	return 0;
}

void step(mlist*cl,int c,mlist*nl)
{
    int i;
	state *s;

	listid++;
	nl->msize = 0;
	for(i=0; i<cl->msize; i++){
		s = cl->s[i];
		if(s->charnum == c)
			addstate(nl, s->out);
	}
}

void addstate(mlist* nl,state*s)
{
    if(s == NULL || s->lastlist == listid)
		return;
	s->lastlist = listid;
	if(s->charnum == 256){
		/* follow unlabeled arrows */
		addstate(nl, s->out);
		addstate(nl, s->out1);
		return;
	}
	nl->s[nl->msize++] = s;
}


static mlist* start_list(state*start,mlist*m)
{
    m->msize=0;
    listid++;
    addstate(m, start);
    return m;
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
        (*(p->var))=s;
        p=p->next;
    }

    // p=l->head;

    // printf("patch effect\n");
    // for(int i=0;i<l->size;++i)
    // {
    //     printf(":%c\n",(*(p->var))->charnum);
    //     p=p->next;
    // }
    // printf("\n\n\n");

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


    // printf("frag infor:\n");
    // printf("Start:%c\n",nf->start->charnum);
    // printf("Outlist:\n");
    // node* p=nf->outlist->head;
    // for(int i=0;i<nf->outlist->size;++i)
    // {   
    //     if(*(p->var)!=NULL)
    //         printf("%c\n",(*(p->var))->charnum);
    //     p=p->next;
    // }

    // printf("\n\n\n\n");
    return nf;
}

static void delete_frag(frag*f)
{
    free(f->outlist);
    free(f);
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

static void delete_state(state*s)
{
    if(s==NULL)
        return ;
    if(s->charnum==257)
    {
        free(s);
        return;
    }

    delete_state(s->out);
    delete_state(s->out1);
    free(s);
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

    /***List*
startlist(State *s, List *l)
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

    //printf("init---------->\n");
    for(int i=0;i<leng;++i)
    {
        frag* f;
        frag* f2;
        frag* f1;
        state* s;
        
        switch(post[i])
        {
            case '*':
                f=stack[--s_size];
                s=create_state(256,f->start,NULL);
                patch(f->outlist,s);
                frag* nf=create_frag(s,create_list(&(s->out1)));
                stack[s_size++]=nf;
                delete_frag(f);
                break;

            case '+':
                f=stack[--s_size];
	            s=create_state(256,f->start,NULL);
	            patch(f->outlist, s);
	            stack[s_size++]=create_frag(f->start, create_list(&s->out1));
                delete_frag(f);
                break;
            
            case '?':
                f=stack[--s_size];
	            s = create_state(256,f->start,NULL);
                stack[s_size++]=create_frag(s,append(f->outlist,create_list(&(s->out1))));
                delete_frag(f);
                break;
            
            case '|':
                f2=stack[--s_size];
	            f1=stack[--s_size];
	            s = create_state(256,f1->start,f2->start);
                stack[s_size++]=create_frag(s,append(f1->outlist,f2->outlist));
	            delete_frag(f2);
                delete_frag(f1);
                break;
            
            case '.':
                f2=stack[--s_size];
	            f1=stack[--s_size];
	            patch(f1->outlist, f2->start);
	            stack[s_size++]=create_frag(f1->start, f2->outlist);
                delete_frag(f2);
                delete_frag(f1);
	
                break;

            default:
                s= create_state(post[i],NULL,NULL);
                f= create_frag(s,create_list(&(s->out)));
                
                stack[s_size++]=f;
                break;

        }

        
    }

    frag* f=stack[--s_size];
    state* match=create_state(257,NULL,NULL);

	patch(f->outlist, match);
    state* result=f->start;

    delete_frag(f);
	return result;

}




int RE_macth_string(char*match_string,state*start)
{

    listid=0;
    int leng=strlen(match_string);
    mlist l1;
    l1.msize=0;
    l1.s=malloc(sizeof(state*)*leng);
    mlist l2;
    l2.msize=0;
    l2.s=malloc(sizeof(state*)*leng);

    mlist*cl;
    mlist*nl;
    mlist*t;

    

    cl=start_list(start,&l1);
    nl=&l2;


    for(;*match_string !='\0';++match_string)
    {
        step(cl, *match_string, nl);
        t = cl; cl = nl; nl = t;
    }



    int result=ismatch(cl);
    delete_state(start);


}


int main()
{
    
    char* infix="ab";
    char* post=infix_to_postfix(infix);

    state* start=RE_to_NFA(post);

    printf("%d\n",RE_macth_string("ac",start));





    //  char* infix_test1="a(bb)*|c";
    //  post=infix_to_postfix(infix_test1);
    //  state* start=RE_to_NFA(post);
    //  printf("%d\n",RE_macth_string("abbbbb",start));


    // char* infix_test2="a?(bb)*|c+";
    
    // post=infix_to_postfix(infix_test2);
    // state* start=RE_to_NFA(post);
    // printf("%d\n",RE_macth_string("cccc",start));

    return 0;

}






