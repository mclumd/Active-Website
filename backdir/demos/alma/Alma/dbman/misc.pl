/*
File: dbman/misc.pl
By: kpurang
What: miscellaneous things

Todo: some of these must go away

*/


% distrusted(+Name)
% deindexes Name and adds a distrusted(Name) in the kb.
% called with eval_bound

distrusted(Name, T):- !,
    find_node(Name, FOrm),
    step_number(T),
    unindex(FOrm, Name).
distrusted(_, _):- !.

% fix_parents(+Parents, +ChildName)
/*
fix_parents([], _):- !.
fix_parents([X|Y], CName):-
    retract(node(X, A, S, D, F, G, Z, XX, C, V)), !,
    assert(node(X, A, S, D, F, G, Z, XX, [CName|C], V)),
    fix_parents(Y, CName).
fix_parents([X|Y], CName):-
    fix_parents(Y, CName).
*/

fix_parents([], _):- !.
fix_parents([[X|Y]|Z], Cname):- !,
    fix_parents([X|Y], Cname),
    fix_parents(Z, Cname).
fix_parents([X|Y], Cname):-
    find_node(X, Xn), !,
    retract(Xn),
    get_kids(Xn, Xk),
    set_kids(Xn, [Cname|Xk], Xnn),
    assert(Xnn),
    fix_parents(Y, Cname).
fix_parents([_|Y], C):- !,
    fix_parents(Y, C).
fix_parents(_, _):- !.

assert_list([]):- !.
assert_list([X|Y]):- !,
    assert(X),
    assert_list(Y).



% get_predicates(+Node, -[Predicate, Pol] list)
% given a node, return the list of [predicate, pol] in the node formula
get_predicates(N, L):- !,
    get_form(N, NF),
    gp(NF, [], L).
gp([], X, X).
gp([not(X)|Y], I, F):- !,
    functor(X, XF, _),!,
    gp(Y, [[XF, minus]|I], F).
gp([X|Y], I, F):- !,
    functor(X, XF, _),
    gp(Y, [[XF, plus]|I], F).


%
% given a list of formulas and the parent of these, assert the formulas in
% the kb.
%

af_lwp([], _):- !.
af_lwp([X|W], Par):- !,
    convert_form(X, XX, Y),
    (var(Y) -> Y = if; true),
    merge_forms(XX, MForms),
    make_new_nodes(MForms, NN1),
    set_parents_list(NN1, Par, NN2),
    assert_list(NN2),
    af_lwp(W, Par).
af_lwp([_|Y], P):- !,
    print(user_error, 'Error in af_lwp'),
    af_lwp(Y, P).


%
% assert literals with parents.
%
ass_litswp([], _, _, _):- !.
ass_litswp([X|Y], Par, Junk, Type):- 
    make_new_node(X, NX),
    set_priority(NX, 1, NX2),
    set_parents(NX2, Par, NX3),
    get_name(NX, NName),
    fix_parents(Par, NName),
    set_type(NX3, Type, NX4), set_junk(NX4, Junk, NX5),
    assert(NX5),
    ass_litswp(Y, Par, Junk, Type).


negate(not(X), X):- !.
negate(X, not(X)) :- !.