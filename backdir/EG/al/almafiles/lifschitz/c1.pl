% Lifschitz C1

% differnet names normally denote different objects

named(fif(and(names(X1, Y1), 
	      and(names(X2, Y2),
		  eval_bound(\+ X1 = X2, [X1, X2, Y1, Y2]))),
	  conclusion(defcon(dndo(X1, X2), different(Y1, Y2)))), 
      dndo(X1, X2)).
default(dndo(X1, X2)).

% ray and reiter denote the same thing
fif(and(names(ray, X), names(reiter, Y)), 
    conclusion(same(X, Y))).

% drew and mcdermott denote the same thing
fif(and(names(drew, X), names(mcdermott, Y)), 
    conclusion(same(X, Y))).

% same is not different
if(same(X, Y), not(different(X, Y))).

names(ray, p1).
names(drew, p2).
