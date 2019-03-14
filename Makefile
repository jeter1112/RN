re_nfa:	re_nfa.o
	gcc -o re_nfa re_nfa.o
re_nfa.o: re_nfa.c re_nfa.h
	gcc -c re_nfa.c
clean:
	rm -rf *.o re_nfa