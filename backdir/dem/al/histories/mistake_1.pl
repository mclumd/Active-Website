
% file mistake2.pl
% requires mistake_help.pl 

% Feb 01
% kpurang


named(fif(bird(X), conclusion(flies(X))), birds_fly).
named(fif(penguin(X), conclusion(not(flies(X)))), penguins_dont_fly).

if(and(bird(X), breeds_in(X, antarctic)), 
   (or(penguin(X), or(petrel(X), skua(X))))).
if(petrel(X), not(dives_to(X, 150))).
if(skua(X), not(dives_to(X, 150))).

bird(tweety).
breeds_in(tweety, antarctic).
dives_to(tweety, 150).



% The default that penguins don't fly is preferred to that about birds
% flying.

prefer(penguins_dont_fly, birds_fly).


% This asserts in the database the leaves of the derivation of the
% formula we now believe (not quite).

fif(and(find_mistake(P, Time),
	and(distrusted(N1, T1),
	    and(eval_bound(name_to_formula(N1, [P]), [N1, P]),
		and(contra(N1, N2, TT),
		    and(eval_bound(name_to_derivation(N2, Deriv), [N2]),
			eval_bound(get_derivation_leaf(Deriv, Leaf), 
				   [Deriv])))))),
    conclusion(check_deriv(Leaf, P, Time))).

fif(and(find_mistake(P, Time),
	and(distrusted(N1, T1),
	    and(eval_bound(name_to_formula(N1, P), [N1, P]),
		and(contra(N2, N1, TT),
		    and(eval_bound(name_to_derivation(N2, Deriv), [N2]),
			eval_bound(get_derivation_leaf(Deriv, Leaf), 
				   [Deriv])))))),
    conclusion(check_deriv(Leaf, P, Time))).

% Here we check the leaves of the derivations for truth at the time we
% are interested in.

fif(check_deriv([], P, Time), conclusion(mistake(P, Time))).

fif(and(check_deriv([X|R], P, Time),
	and(eval_bound(pos_int(P, Time), [P, Time]),
	    and(eval_bound(name_to_formula(X, XF), [X]),
		eval_bound(pos_int(XF, Time), [XF])))),
    conclusion(check_deriv(R, P, Time))).


% The contradiction resolution axioms prefer a formula that is derived from
% a preferred default.

% if there is a contradiction, assert the parents in the database

if(and(contra(X, Y, Z), eval_bound(name_to_parents(X, PX), [X])),
   parentsof(X, PX)).
if(and(contra(Y, X, Z), eval_bound(name_to_parents(X, PX), [X])),
   parentsof(X, PX)).

% the contradictand which has a preferred parent, is reinstated

fif(and(parentsof(C1, P1),
       and(parentsof(C2, P2),
	   and(eval_bound(contains(P1, PP1), [P1, PP1]),
	       and(eval_bound(contains(P2, PP2), [P2, PP2]),
		   prefer(PP1, PP2))))),
   conclusion(resolved_contra(C1, C2))).

fif(and(parentsof(C1, P1),
       and(parentsof(C2, P2),
	   and(eval_bound(contains(P1, PP1), [P1, PP1]),
	       and(eval_bound(contains(P2, PP2), [P2, PP2]),
		   prefer(PP2, PP1))))),
   conclusion(resolved_contra(C2, C1))).

fif(resolved_contra(X, Y), conclusion(reinstate(X))).

fif(and(resolved_contra(X, Y), 
	eval_bound(name_to_formula(Y, YF), [Y])),
    conclusion(find_mistake(YF, Z))).
