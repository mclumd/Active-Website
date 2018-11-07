
:- ensure_loaded(library(lineio)).


process_file(F):-
	see(F),
	repeat,
	process1, !.

process1:-
	eat_empty(X).


eat_empty(X):-					% eats blank lines
	repeat,
	get_line(X, _),
	\+ blank_line(X), !.

blank_line([]):- true.				% checks if has blank line
blank_line([X|Y]):-
	\+ X = 32,
	fail.
blank_line([32|Y]):-
	blank_line(Y).


get_head(X):-					% gets first line of next bib
	true.


strip_blanks([32|Y], Z):- !,			% strip leading blanks in line
	strip_blanks(Y, Z).
strip_blanks(X, X).

