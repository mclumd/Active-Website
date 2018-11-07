%
%first test for contras
%

p(a).
if(p(X), q(X)).
if(q(X), not(p(X))).
if(not(p(X)), r(X)).

if(and(contra(C1, C2, T), eval_bound(name_to_parents(C1, P1), [C1])),
   bad_parents(P1)).

if(and(contra(C1, C2, T), eval_bound(name_to_parents(C2, P2), [C2])),
   bad_parents(P2)).

