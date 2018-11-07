/*
File: res/resolve.pl
By: kpurang
What: does the resolution

Todo: there has to be a more efficient way
      add strucutre to the clauses to make more efficient      

*/

:- ensure_loaded(library(sets)).
:- ensure_loaded(library(ordsets)).


resolve(C01, C02, Tar, Res):- !,
    list_to_ord_set(C01, C1),
    list_to_ord_set(C02, C2),
    copy_term([C1, C2, Tar], [CC1, CC2, Tar2]),
    resolve(CC1, CC1, CC2, CC2, Tar2, [], Res, Flag).
% we are done
% if flag is true, there is a resolution, so union the clauses
resolve(C1, [], C2, _, T, X, TRC, XX) :- 
    \+ var(XX), !,
    union(C1, C2, R),
    union([[R, T]], X, TRC).
resolve(_, [], _, _, _, X, X, _) :- !.
% for each literal in the second clause, try to resolve with each literal
% in the first clause
resolve(C1, [X|Y], C2, [], T, I, F, Flag):- !, 
    resolve(C1, Y, C2, C2, T, I, F, Flag).
resolve(C1, [X|Y], C2, [XX|YY], T, I, F, Flag):- 
    copy_term([C1, C2, X, Y, XX, YY, T], [CC1, CC2, CX, CY, CXX, CYY, T2]),
    negate_lit(CX, NX),
    NX = CXX, !, 
    del_element(NX, CC2, C3),
    del_element(CX, CC1, C11),
% here we want to continue looking for possible resolvents.
% start resolving again, but this time with the clauses without the unified
% literals and with the substitutions
    resolve(C3, C3, C11, C11, T2, I, IF, true),
% the following lets us find whether the same thing resolves with other
% literals later.
% KP: jan 13 2000. REMOVED THAT
%    resolve(C1, [X|Y], C2, YY, T, IF, F, F2).
    T = T2, F = IF.
resolve(C1, CC1, C2, [X|Y], I, F, T, Flag):- !,
    resolve(C1, CC1, C2, Y, I, F, T, Flag).

% what on earth is this for??

find_res(_, []):- !, fail.
find_res(X, [X|_]):- !.
find_res(X, [_|Y]):- !,
    find_res(X, Y).

