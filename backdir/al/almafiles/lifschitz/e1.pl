% lifschitz e1

% a is on the table
and(block(a), onTable(a)).

% b is a block
block(b).

% if a block is not known ot be on the table, it is not.
if(and(block(X), not(eval_bound(pos_int(onTable(X)), [X]))),
   not(known(onTable(X)))).




