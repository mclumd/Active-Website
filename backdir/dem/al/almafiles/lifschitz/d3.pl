% lifschitz d3

% after an action is performed things normally stay the same
named(fif(and(do(A, S), holds(F, S)), 
	  conclusion(defcon(inert, holds(F, done(A, S))))),
      inert).
default(inert).

% a block is on the table iff it si not on the floor
%fif(holds(ontable(B), S), conclusion(not(holds(onfloor(B), S)))).

if(holds(ontable(B), S), not(holds(onfloor(B), S))).
if(not(holds(ontable(B), S)), holds(onfloor(B), S)).

% any time the robot grasps a block it will NORMALLY be in its hand
named(fif(do(grasp(B), S), 
	  conclusion(defcon(graspInHand, holds(inhand(B), 
					       done(grasp(B), S))))),
      graspInHand).
default(graspInHand).

prefer(graspInHand, inert).

% when the robot moves a block onto the table, it will normally be ontable
named(fif(do(move(B), S), 
	  conclusion(defcon(moveTable, holds(ontable(B), done(move(B), S))))),
      moveTable).
default(moveTable).

% we prefer the moveTable default to inertia
prefer(moveTable, inert).

% moving a block that is not inthe hand is an exception
fif(and(not(holds(inhand(B))), do(move(B), S)),
    conclusion(not(holds(ontable(B), done(move(B), S))))).

% initially block a is not in hand
not(holds(inhand(a), s0)).

% initially block a is on the floor 
holds(onfloor(a), s0).

% robot grasps, waits, and moves
do(grasp(a), s0).
do(wait, done(grasp(a), s0)).
do(move(a), done(wait, done(grasp(a), s0))).

% a block cannot be in the hand and on the table at the same time
not(and(holds(inhand(B), S), holds(ontable(B), S))).

