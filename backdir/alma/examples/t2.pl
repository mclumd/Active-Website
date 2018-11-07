% a backward search test.

forall(X, bif(p(X), q(X))).
p(a).
p(b).
bs(q(X)).
forall(X, if(q(X), alma(format('q(~a)~n', X)))).



