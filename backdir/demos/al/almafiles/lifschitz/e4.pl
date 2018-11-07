
% lifschitz e4

% a is a heavy block
and(block(a), heavy(a)).

% b is a block
block(b).

% blocks that are not known to be heavy are on the table.
if(and(block(X), not(eval_bound(pos_int(heavy(X)), [X]))),
   onTable(X)).



