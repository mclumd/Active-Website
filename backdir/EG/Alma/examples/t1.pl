% test file

forall(X, if(p(X), q(X))).
forall(X, if(and(q(X), r(X)), s(X))).
p(a).
p(b).
forall(X, r(X)).
if(s(a), alma(format('~n~nGot s(a)~n~n', []))).


