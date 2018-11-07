% Lifschitz C2

% different names normally denote different objects
named(fif(and(names(X1, Y1), 
	      and(names(X2, Y2),
		  eval_bound(\+ X1 = X2, [X1, X2, Y1, Y2]))),
	  conclusion(defcon(dndo(X1, X2), different(Y1, Y2)))), 
      dndo(X1, X2)).
default(dndo(X1, X2)).

% different people normally have differnet fathers
named(fif(and(different(P1, P2), 
	      and(father(F1, P1), father(F2, P2))),
	  conclusion(defcon(dpdf(P1, P2), different(F1, F2)))),
      dpdf(P1, P2)).
default(dpdf(P1, P2)).

% joseph and benjamin have the same father
and(names(joseph, j), 
    and(names(benjamin, b), 
	and(father(fjb, j),
	    father(fjb, b)))).

% gaius and tiberius have the same father
and(names(gaius, g), 
    and(names(tiberius, t), 
	and(father(fgt, g),
	    father(fgt, t)))).

not(different(X, X)).

