#ifndef RE_NFA_H
#define RE_NFA_H


struct State;
typedef struct State state;

struct State
{
    int charnum;  //char 1 byte; int 4 byte;  byte duiqi;

    state* out;
    state* out1;
};









char* infix_to_postfix(char*);

state* RE_to_NFA(char*);

int RE_macth_string(char*,state*);








#endif