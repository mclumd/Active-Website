
% lifschitz e2

% a is a block
block(a).

% b is a block
block(b).

or(onTable(a), onTable(b)).

% if a block is not known ot be on the table, it is not.
if(and(block(X), not(eval_bound(pos_int(onTable(X)), [X]))),
   not(known(onTable(X)))).




