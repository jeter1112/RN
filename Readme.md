# Regular Expression to NFA

a program which converts RE to NFA.

some insights:
    1. regular expression <===> DFA
    2. small RE  -----> big RE
       small DFA -----> big DFA
    3. so regular -----> DFA
       is means how to extract small RE to small DFA. then build big DFA.
     <=====> How to grow small DFA to big DFA.

The first step is to map small RE to small DFA;
Then abstract small DFA to elements of big DFA.  (do this step recursively).
----> How to build big DFA?
using operations, | * ? +.  ----> abstarct DFAs ----> big DFA.

specific step:
    1. expression is important is every language, here, we can convert infix expression to postfix expression;
    2. read postfix expression, when read a symbol, constuct a small DFA, whild read a RE operation, convert to an abstarct DFA;
    3. after the DFA constucts, write a function to match a given string, for each char in the string, go to next states, until the last states contain accept ones.
     
    