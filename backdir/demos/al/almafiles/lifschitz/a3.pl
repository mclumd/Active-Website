
% Lifschitz A1

% Blocks a and b are heavy.

and(block(a), heavy(a)).
and(block(b), heavy(b)).

% Heavy blocks are normally located on the table

named(fif(and(block(X), heavy(X)),
	  conclusion(defcon(heavyOnTable(X), onTable(X)))),
      heavyOnTable(X)).
default(heavyOnTable(X)).

% Heavy blocks are normally red

named(fif(and(block(X), heavy(X)),
	  conclusion(defcon(heavyRed(X), red(X)))),
      heavyRed(X)).
default(heavyRed(X)).

% a is not on the table
not(onTable(a)).

% b is not red
not(red(b)).


