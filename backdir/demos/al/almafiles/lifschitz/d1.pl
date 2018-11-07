% lifschitz d1

% after an action is performed things normally stay the same
named(fif(and(do(A, S), holds(F, S)), 
	  conclusion(defcon(inert, holds(F, done(A, S))))),
      inert).

% any time the robot grasps a block it will be in its hand
if(do(grasp(B), S), 
   holds(inhand(B), done(grasp(B), S))).

% if a block is inhand, after moving it to the table, it will be on the table
if(and(holds(inhand(B), S), do(move(B), S)), 
   holds(ontable(B), done(move(B), S))).

if(and(holds(inhand(B), S), do(move(B), S)), 
   not(holds(inhand(B), done(move(B), S)))).

% initially block a is not in hand
not(holds(inhand(a), s0)).

% initially block a is not on table
not(holds(ontable(a), s0)).

% robot grasps, waits, and moves
do(grasp(a), s0).
do(wait, done(grasp(a), s0)).
do(move(a), done(wait, done(grasp(a), s0))).


