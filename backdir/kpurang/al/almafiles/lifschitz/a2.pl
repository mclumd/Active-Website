
% Lifschitz A1

% Blocks a and b are heavy.

and(block(a), heavy(a)).
and(block(b), heavy(b)).

% Heavy blocks are normally located on the table

named(fif(and(block(X), heavy(X)),
	  conclusion(defcon(heavyOnTable(X), onTable(X)))),
      heavyOnTable(X)).
default(heavyOnTable(X)).

% a is not on the table
not(onTable(a)).

% b is red
red(b).

