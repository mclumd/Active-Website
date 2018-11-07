% nmr_2_hrlp.pl
% helper file for applying defaults (method 2)
% kpurang Apr 01


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% to apply defaults:
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% check if a default should be applied.
% verify_applicability(N, F) succeeds if:
%   there is no default preferred to N OR
%   there is such a default but it has not been applied.
%   AND
%   the negation of the formula is not in the KB and does not depend
%   on defaults.

verify_applicability(N, F):-
    gather_all_u(prefer(_, N), []), !,
    not_preempted(F).
verify_applicability(N, F):-
    gather_all_u(prefer(_, N), L), !,
    none_applied(N, L),
    not_preempted(F).

none_applied(_, []):- !.
none_applied(N, [X|Y]):-
    copy_term(N, NN),
    name_to_formula(X, [prefer(XN, NN)]),
    gather_all_u(applied_default(XN, _), []), !,
    none_applied(N, Y).

not_preempted(not(F)):- !,
    gather_all_u(F, L),
    verify_L(L).
not_preempted(F):- !,
    gather_all_u(not(F), L),
    verify_L(L).

verify_L([]):- !.
verify_L([X|Y]):-
    name_to_deriv_fringe(X, F),
    deriv_is_defaulty(F), !,
    verify_L(Y).

% distrust_less_preferred(N)
% N is the name of a formula. 
% find if there is any formula N2 st prefer(N, N2) and there is
% applied_default(N2, _). If so, distrust_descendants(N2)


distrust_less_preferred(N):- !,
    gather_all_u(prefer(N, X), L), 
    do_each_lp(N, L).
do_each_lp(_, []):- !.
do_each_lp(N, [X|Y]):- !,
    name_to_formula(X, [prefer(N, XD)]),
    gather_all_u(applied_default(XD, _), L),
    distrust_these(L),
    do_each_lp(N, Y).
distrust_these([]):- !.
distrust_these([X|Y]):-
    distrust_descendants(X),
    distrust_these(Y).


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% to deal with contradictions
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% given a deriv fringe, verify whether any of the derivations has no defaults.
%
% default_derivs(L)
% L is a list of lists of formulas
% if all of the lists contain at least one default, return true.
% else false

deriv_is_defaulty([]):- !.
deriv_is_defaulty([X|Y]):-
    no_default(X), !, 
    fail.
deriv_is_defaulty([X|Y]):-
    deriv_is_defaulty(Y).

no_default([]):- !.
no_default([X|Y]):-
    gather_all_u(default(X), []), !,
    no_default(Y).
no_default(_):- !, fail.



