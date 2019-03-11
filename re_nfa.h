#ifndef RE_NFA_H
#define RE_NFA_H


struct State;
typedef struct State state;



char* infix_to_postfix(char*);

state* RE_to_NFA(char*);

bool RE_macth_string(char*,State*);








#endif