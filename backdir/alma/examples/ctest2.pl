/*

try to prefer things now

*/

named(p(a), peeofa).
if(q(X), not(p(X))).
q(a).
if(and(contra(X, Y, Z), eval_bound(X = peeofa, [X])), reinstate(peeofa)).
if(and(contra(Y, X, Z), eval_bound(X = peeofa, [X])), reinstate(peeofa)).

