% The nixon diamond
% we use the nmr approach I
% 

% republicans are not pacifists
fif(republican(X), conclusion(not(pacifist(X)))).

% quakers are pacifists
fif(quaker(X), conclusion(pacifist(X))).

% nixon is a republican
republican(nixon).

% nixon is a quaker
quaker(nixon).




% same ContraFixing as for Tweety
% if there is a contradiction, find the parents
% very simple contra fixing procedure.

if(and(contra(X, Y, Z), eval_bound(name_to_parents(X, PX), [X])),
   parentsof(X, PX)).
if(and(contra(Y, X, Z), eval_bound(name_to_parents(X, PX), [X])),
   parentsof(X, PX)).

% find which contradictand has a preferred parent, reinstate that one

fif(and(parentsof(C1, P1),
       and(parentsof(C2, P2),
	   and(eval_bound(contains(P1, PP1), [P1, PP1]),
	       and(eval_bound(contains(P2, PP2), [P2, PP2]),
		   prefer(PP1, PP2))))),
   conclusion(reinstate(C1))).

fif(and(parentsof(C1, P1),
       and(parentsof(C2, P2),
	   and(eval_bound(contains(P1, PP1), [P1, PP1]),
	       and(eval_bound(contains(P2, PP2), [P2, PP2]),
		   prefer(PP2, PP1))))),
   conclusion(reinstate(C2))).

