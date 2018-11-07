/*
File: dbman/index.pl
By: kpurang
What: code to do with indexing and getting nodes.

Todo: index first arg too
      make this into an external c function that will be more efficient
      tna the prolog indexing

*/

:- ensure_loaded(library(arg)).
:- ensure_loaded(library(sets)).

%XXX says that the above has been modified for 1starg indexing

% index_nodes(+List of nodes)
% given a list of nodes, find the predicates they contain and their 
% polarities and insert in the func_node table

%
% NEED TO USE THE STANDARD NODE ACCESS HERE
%
index_nodes([]):- !.
index_nodes([node(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)|Y]):- !,
    get_formula(node(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9), F),
    insert_node_table(A0, F),
    index_nodes(Y).

% index_new_node(+a node)
% given a node, find the literals it contains and their 
% polarities and insert in the func_new_node table
index_new_node(new_node(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9)):- !,
    get_formula(new_node(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9), F),
    insert_new_node_table(A0, F).

% given a node name and its formulas, insert the name in the appropriate
% indices

insert_node_table(_, []):- !.
insert_node_table(N, [F1|Fn]):- !,
    insert_one(N, F1),
    insert_node_table(N, Fn).

% just the above for one literal

insert_one(_, []):- !.
insert_one(Name, not(N)):- !,
    hash_it(N, H),
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Inserting not: '), print(DBGS, not(N))
%          , print(DBGS, H), nl(DBGS)); true),    
    (var(H) -> 
	 (functor(N, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(F, FVAR),
	  (retract(node_index(F, PL, MI)) ->
	       (add_element(Name, MI, MII),
		assert(node_index(F, PL, MII)));
	  (assert(node_index(F, [], [Name])),
	   (retract(pred2hash(Fu, P2H)) ->
		(add_element(F, P2H, PP2H),
		 assert(pred2hash(Fu, PP2H)));
	   (assert(pred2hash(Fu, [F])))))));
    ((retract(node_index(H, PH, MH)) ->
	  (add_element(Name, MH, MMH),
	   assert(node_index(H, PH, MMH)),
	   functor(N, F, _),
	   (retract(pred2hash(F, F2H)) ->
		(add_element(H, F2H, FFH),
		 assert(pred2hash(F, FFH)));
	   (assert(pred2hash(F, [H]))))
	  );
     (assert(node_index(H, [], [Name])),
      functor(N, F, _),
      (retract(pred2hash(F, F2H)) ->
	   (add_element(H, F2H, FFH),
	    assert(pred2hash(F, FFH)));
      (assert(pred2hash(F, [H])))))))).
insert_one(Name, N):- !,
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Inserting: '), print(DBGS, N)
%          , print(DBGS, H), nl(DBGS)); true),    
    hash_it(N, H),
    (var(H) -> 
	 (functor(N, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(F, FVAR),
	  (retract(node_index(F, PL, MI)) ->
	       (add_element(Name, PL, PLL),
		assert(node_index(F, PLL, MI)));
	  (assert(node_index(F, [Name], [])),
	   (retract(pred2hash(Fu, P2H)) ->
		(add_element(F, P2H, PP2H),
		 assert(pred2hash(Fu, PP2H)));
	   (assert(pred2hash(Fu, [F])))))));
    ((retract(node_index(H, PH, MH)) ->
	  (add_element(Name, PH, PPH),
	   assert(node_index(H, PPH, MH)),
	   functor(N, F, _),
%
% must be a bug here. added this then it does not even start 
%
	   (retract(pred2hash(F, Fh)) -> 
		(add_element(H, Fh, Fhh),
		 assert(pred2hash(F, Fhh)));
	   assert(pred2hash(F, [H])))
	  );
     (assert(node_index(H, [Name], [])),
      functor(N, F, _),
      (retract(pred2hash(F, F2H)) ->
	   (add_element(H, F2H, FFH),
	    assert(pred2hash(F, FFH)));
      (assert(pred2hash(F, [H])))))))).
    

% given a node name and its formulas, insert the name in the appropriate
% indices

insert_new_node_table(_, []):- !.
insert_new_node_table(N, [F1|Fn]):- !,
    insert_new_one(N, F1),
    insert_new_node_table(N, Fn).

% just the above for one literal

insert_new_one(_, []):- !.
insert_new_one(Name, not(N)):- !,
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Inserting new not: '), print(DBGS, not(N))
%          , print(DBGS, H), nl(DBGS)); true),    
    hash_it(N, H),
    (var(H) -> 
	 (functor(N, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(F, FVAR),
	  (retract(new_node_index(F, PL, MI)) ->
	       (add_element(Name, MI, MII),
		assert(new_node_index(F, PL, MII)));
	  (assert(new_node_index(F, [], [Name])),
	   (retract(newpred2hash(Fu, P2H)) ->
		(add_element(F, P2H, PP2H),
		 assert(newpred2hash(Fu, PP2H)));
	   (assert(newpred2hash(Fu, [F])))))));
    ((retract(new_node_index(H, PH, MH)) ->
	  (add_element(Name, MH, MMH),
	   assert(new_node_index(H, PH, MMH)),
	  functor(N, F, _),
	  (retract(newpred2hash(F, F2H)) ->
	       (add_element(H, F2H, FFH),
		assert(newpred2hash(F, FFH)));
	  (assert(newpred2hash(F, [H]))))
	  );
     (assert(new_node_index(H, [], [Name])),
      functor(N, F, _),
      (retract(newpred2hash(F, F2H)) ->
	   (add_element(H, F2H, FFH),
	    assert(newpred2hash(F, FFH)));
      (assert(newpred2hash(F, [H])))))))).
insert_new_one(Name, N):- !,
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Inserting new: '), print(DBGS, N)
%          , print(DBGS, H), nl(DBGS)); true),    
    hash_it(N, H),
    (var(H) -> 
	 (functor(N, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(F, FVAR),
	  (retract(new_node_index(F, PL, MI)) ->
	       (add_element(Name, PL, PLL),
		assert(new_node_index(F, PLL, MI)));
	  (assert(new_node_index(F, [Name], [])),
	   (retract(newpred2hash(Fu, P2H)) ->
		(add_element(F, P2H, PP2H),
		 assert(newpred2hash(Fu, PP2H)));
	   (assert(newpred2hash(Fu, [F])))))));
    ((retract(new_node_index(H, PH, MH)) ->
	  (add_element(Name, PH, PPH),
	   assert(new_node_index(H, PPH, MH)),
	   functor(N, F, _),
	   (retract(newpred2hash(F, Fh)) -> 
		(add_element(H, Fh, Fhh),
		 assert(newpred2hash(F, Fhh)));
	   assert(newpred2hash(F, [H])))
	  );
     (assert(new_node_index(H, [Name], [])),
      functor(N, F, _),
      (retract(newpred2hash(F, F2H)) ->
	   (add_element(H, F2H, FFH),
	    assert(newpred2hash(F, FFH)));
      (assert(newpred2hash(F, [H])))))))).
    

% if first arg is constant, OK
% if is var, return var
% if dont exist, return functorname
% so the things with var go where?? stick it in functorname$$
hash_it(C, H):- !,
    functor(C, F, N),
    (N >= 1 ->
	 (arg(1, C, A),
	  (var(A) ->
	       true;
	  (functor(A, FF, _),
	   hash_term([F, FF], H))));
    (H = F)).


%XXX


% TODO THIS ONE LATER
% looks like this is used to delete formulas given only the clause. we need 
% to find the nodes that contains all the literals we are interestef in, in
% the appropriate polarity
% intersect_nodes(+[Predicate, Pol] list, -Intersection)
% given a predicate list, find all the sets of nodes that contain all 
% these predicates and intersect all the sets, keeping track of polarities
% intersection is a set of names of nodes.
/*
can't work with the pred/pol thing.
need to get the actual clause.

DO NOT USE THIS 

intersect_nodes(P, I):- 
    get_all_sets(P, [], SP), !,
    intersection(SP, I).
intersect_nodes(_, []).

intersect_new_nodes(P, I):- 
    get_all_new_sets(P, [], SP), !,
    intersection(SP, I).
intersect_new_nodes(_, []).

*/

intersectnodes(Form, Intersect):- 
    getallsets(Form, [], SP), !,
    intersection(SP, Intersect).
intersectnodes(_, []).

intersectnewnodes(Form, Intersect):- 
    getallnewsets(Form, [], SP), !,
    intersection(SP, Intersect).
intersectnewnodes(_, []).

% given a formula, make a list of the nodes that could be that formula
% for each clause.

getallsets([], X, X):- !.
getallsets([not(F)|FR], In, Out):- !,
    hash_it(F, H),
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  node_index(Ff, _, S1), 
	  getallsets(FR, [S1|In], Out));
    (node_index(H, _, S1),
     getallsets(FR, [S1|In], Out))).
getallsets([F|FR], In, Out):- !,
    hash_it(F, H),
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  node_index(Ff, S1, _), 
	  getallsets(FR, [S1|In], Out));
    (node_index(H, S1, _),
     getallsets(FR, [S1|In], Out))).


getallnewsets([], X, X):- !.
getallnewsets([not(F)|FR], In, Out):- !,
    hash_it(F, H),
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  new_node_index(Ff, _, S1), 
	  getallnewsets(FR, [S1|In], Out));
    (new_node_index(H, _, S1),
     getallnewsets(FR, [S1|In], Out))).
getallnewsets([F|FR], In, Out):- !,
    hash_it(F, H),
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  new_node_index(Ff, S1, _), 
	  getallnewsets(FR, [S1|In], Out));
    (new_node_index(H, S1, _),
     getallnewsets(FR, [S1|In], Out))).


    


/**********

DO NOT USE

% get_all_sets(+List of [Preds, Pol], +initial list of sets, -final los)
% given a list of predicates, make a list of the sets of nodes in
% which they occur with the appropriate polarity.
%  If any of the sets is empty, fail. 
%
% NOTE. This is looking for things that are ALTEADY in teh db. Those other
% NEW things will be missed altogether.
%
get_all_sets([], I, I):- true.			  % want a cut here???
get_all_sets([[X, plus]|Y], I, F):-
    func_node(X, [], _), !,
    get_all_sets(Y, I, F).
get_all_sets([[X, plus]|Y], I, F):-
    func_node(X, XP, _), !, 
    get_all_sets(Y, [XP|I], F).
get_all_sets([[X, minus]|Y], I, F):-
    func_node(X, _, []), !, 
    get_all_sets(Y, I, F).
get_all_sets([[X, minus]|Y], I, F):-
    func_node(X, _, XM), !, 
    get_all_sets(Y, [XM|I], F).
get_all_sets(_, _, _):- fail.			  % redundant

get_all_new_sets([], I, I) :- true.		  % want a cut here?
get_all_new_sets([[X, plus]|Y], I, F):-
    func_new_node(X, [], _), !,
    get_all_new_sets(Y, I, F).
get_all_new_sets([[X, plus]|Y], I, F):-
    func_new_node(X, XP, _), !, 
    get_all_new_sets(Y, [XP|I], F).
get_all_new_sets([[X, minus]|Y], I, F):-
    func_new_node(X, _, []), !, 
    get_all_new_sets(Y, I, F).
get_all_new_sets([[X, minus]|Y], I, F):-
    func_new_node(X, _, XM), !, 
    get_all_new_sets(Y, [XM|I], F).
get_all_new_sets(_, _, _):- fail.			  % redundant

**********/

%%
%% WHAT IS THIS FOR
%% AND WHAT IS IT DOING HERE???

% variant_nodes(+Node, +Node list)
% if there is a node in Node list that is similar to Node, succeed.
% A and B are similar if E A subssubsumes B and B subsumes A
% nov 24: i now return the list of similar guys.
variant_nodes(_, [], []):- !.
variant_nodes(N, [X|Y], [X|Z]):-
    get_form(N, NF),
    get_form(X, XF),
    variant_lits(NF, XF), !,
    variant_nodes(N, Y, Z). 
variant_nodes(N, [X|Y], Z):-
    variant_nodes(N, Y, Z).

% nov 24. new variant lits. sort the formulas then find if they are
% variants. the previous version was finding too many variants
% i should not be doing this list to ord set business so many times.
% make sure all forms are always ord_sets.

variant_lits(X, Y):- !,
    list_to_ord_set(X, Xo),
    list_to_ord_set(Y, Yo),
    variant(Xo, Yo).
/*
variant_lits([], []):- !.
variant_lits([X|Y], [Q|W]):-
    variant(X, Q), !,
    variant_lits(Y, W).
variant_lits([X|Y], [Q|W]):-
    find_variant(X, W, WW), !,
    variant_lits(Y, [Q|WW]).
*/

find_variant(_, [], _):- !, fail.
find_variant(X, [Z|Y], Y):-
    variant(X, Z), !.
find_variant(X, [Z|Y], [Z|QQ]):-
    find_variant(X, Y, QQ), !.

% get_variant(+Form, +List of nodes, -Variant)
% return the node whose formula is form, else fail

get_variant(_, [], _):- fail.
get_variant(NF, [X|Y], X):-
    get_form(X, XF),
    variant_lits(NF, XF), !.
get_variant(N, [X|Y], V):-
    get_variant(N, Y, V).

% get_unifiers(+Form, +List of nodes, Unifiers, -FInalUnifiers)
% return the nodes that unify with Form

get_unifiers(_, [], X, X):- !.
get_unifiers(NF, [X|Y], IZ, FZ):-
    get_form(X, XF),
    XF = NF, !,
    get_unifiers(NF, Y, [X|IZ], FZ).
get_unifiers(NF, [X|Y], IZ, FZ):-
    get_form(X, XF), !,
    get_unifiers(NF, Y, IZ, FZ).

% get_subsumers(+Form, +List of nodes, Subs, -FInalSuba)
% return the nodes that Form subsumes

get_subsumers(_, [], X, X):- !.
get_subsumers(NF, [X|Y], IZ, FZ):-
    get_form(X, XF),
%    XF = NF, !,
    subsumes_chk(NF, XF), !,
    get_subsumers(NF, Y, [X|IZ], FZ).
get_subsumers(NF, [X|Y], IZ, FZ):-
    get_subsumers(NF, Y, IZ, FZ).





%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% ok things look good down below.

%
% THIS TO BE CHANGED TO UNINDEX(NODE, FORMULA)
%

% IS OK this interface
% unindex(+Node, +Name)
% remove indices for Name in lists of form predicates.
unindex(Node, Name):- !,
    get_formula(Node, Form),
    unindexone(Form, Name).
% Form is a list of literals

unindexone([], _):- !.
unindexone([not(F)|FR], Name):- !,
    hash_it(F, H),
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Unindexing: '), print(DBGS, not(F))
%          , print(DBGS, H), nl(DBGS)); true),    
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  retract(node_index(Ff, Sp, S1)), 
	  del_element(Name, S1, S2),
	  ((Sp = [], S2 = []) -> 
		(retract(pred2hash(Fu, P2h1)),
		 del_element(Ff, P2h1, P2h2),
		 assert(node_index(Ff, Sp, S2)),
		 assert(pred2hash(Fu, P2h2))
		);
	  assert(node_index(Ff, Sp, S2))));
    (retract(node_index(H, Sp, S1)),
     del_element(Name, S1, S2),
     ((Sp = [], S2 = []) -> 
	   (functor(F, Fun, _),
	    retract(pred2hash(Fun, P2h1)),
	    del_element(H, P2h1, P2h2),
	    assert(node_index(H, Sp, S2)),
	    assert(pred2hash(Fun, P2h2))
		);
     assert(node_index(H, Sp, S2))))),
    unindexone(FR, Name).
unindexone([F|FR], Name):- !,
    hash_it(F, H),
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Unindexing: '), print(DBGS, F)
%          , print(DBGS, H), nl(DBGS)); true),    
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  retract(node_index(Ff, S1, Sm)), 
	  del_element(Name, S1, S2),
	  ((Sm = [], S2 = []) -> 
		(retract(pred2hash(Fu, P2h1)),
		 del_element(Ff, P2h1, P2h2),
		 assert(node_index(Ff, S2, Sm)),
		 assert(pred2hash(Fu, P2h2))
		);
	  assert(node_index(Ff, S2, Sm))));
    (retract(node_index(H, S1, Sm)),
     del_element(Name, S1, S2),
     ((Sm = [], S2 = []) -> 
	   (functor(F, Fun, _),
	    retract(pred2hash(Fun, P2h1)),
	    del_element(H, P2h1, P2h2),
	    assert(node_index(H, S2, Sm)),
	    assert(pred2hash(Fun, P2h2))
		);
     assert(node_index(H, S2, Sm))))),
    unindexone(FR, Name).

/*****
SHOULD NOT BE USED!!!

unindex_each(_, []):- !.
unindex_each(Name, [[Pred, plus]|X]):- !,
    retract(func_node(Pred, PL, MN)),
    del_element(Name, PL, PLL),
    assert(func_node(Pred, PLL, MN)),
    unindex_each(Name, X).
unindex_each(Name, [[Pred, minus]|X]):- !,
    retract(func_node(Pred, PL, MN)),
    del_element(Name, MN, MNN),
    assert(func_node(Pred, PL, MNN)),
    unindex_each(Name, X).
*****/



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

unindex_new_done:- 
    retract(done_new(X)), 
    get_formula(X, Forms),
    get_name(X, Name),
    unindexnewone(Forms, Name),
    retract(X),
    fail.
unindex_new_done:- !.


unindexnewone([], _):- !.
unindexnewone([not(F)|FR], Name):- !,
    hash_it(F, H),
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Unindexing: '), print(DBGS, not(F))
%          , print(DBGS, H), nl(DBGS)); true),    
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  retract(new_node_index(Ff, Sp, S1)), 
	  del_element(Name, S1, S2),
	  ((Sp = [] , S2 = []) -> 
		(retract(newpred2hash(Fu, P2h1)),
		 del_element(Ff, P2h1, P2h2),
		 assert(newpred2hash(Fu, P2h2)),
		 assert(new_node_index(Ff, Sp, S2))
%% we need to have this above coz there can be many occurrences of the same
%% pred in one formula. later we can test for teh existence of the new_node_
%% index before doing the retraction.
		);
	  assert(new_node_index(Ff, Sp, S2))));
    (retract(new_node_index(H, Sp, S1)),
     del_element(Name, S1, S2),
     ((Sp = [] , S2 = []) -> 
	   (functor(F, Fun, _),
	    retract(newpred2hash(Fun, P2h1)),
	    del_element(H, P2h1, P2h2),
	    assert(new_node_index(H, Sp, S2)),
	    assert(newpred2hash(Fun, P2h2))
	   ); 
     assert(new_node_index(H, Sp, S2))))),
    unindexnewone(FR, Name).
unindexnewone([F|FR], Name):- !,
    hash_it(F, H),
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Unindexing: '), print(DBGS, F)
%          , print(DBGS, H), nl(DBGS)); true),    
    (var(H) ->
	 (functor(F, Fu, _),
	  name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  retract(new_node_index(Ff, S1, Sm)), 
	  del_element(Name, S1, S2),
	  ((Sm = [] , S2 = []) -> 
		(retract(newpred2hash(Fu, P2h1)),
		 del_element(Ff, P2h1, P2h2),
		 assert(new_node_index(Ff, S2, Sm)),
		 assert(newpred2hash(Fu, P2h2))
		);
	  assert(new_node_index(Ff, S2, Sm))));
    (retract(new_node_index(H, S1, Sm)),
     del_element(Name, S1, S2),
     ((Sm = [] , S2 = []) -> 
	   (functor(F, Fun, _),
	    retract(newpred2hash(Fun, P2h1)),
	    del_element(H, P2h1, P2h2),
	    assert(new_node_index(H, S2, Sm)),
	    assert(newpred2hash(Fun, P2h2))
		); 
     assert(new_node_index(H, S2, Sm))))),
    unindexnewone(FR, Name).



/*****
NOT TO BE USED
unindex_new_each(_, []):- !.
unindex_new_each(Name, [[Pred, plus]|X]):- !,
    retract(func_new_node(Pred, PL, MN)),
    del_element(Name, PL, PLL),
    assert(func_new_node(Pred, PLL, MN)),
    unindex_new_each(Name, X).
unindex_new_each(Name, [[Pred, minus]|X]):- !,
    retract(func_new_node(Pred, PL, MN)),
    del_element(Name, MN, MNN),
    assert(func_new_node(Pred, PL, MNN)),
    unindex_new_each(Name, X).
****/

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%% % 

% here we try to
% isolate all accesses to the table mapping predicates to nodes.

% given a node and a polarity, return the clauses in which this clause
% appears with that polarity

% this will be changed to do the new style indexing.
/*****
NOT TO BE USED

pred_to_node(P, Pol, Set):-
    Pol = plus -> 
    func_node(P, Set, _);
    func_node(P, _, Set).

pred_to_new_node(P, Pol, Set):-
    Pol = plus -> 
    func_new_node(P, Set, _);
    func_new_node(P, _, Set).

********/

% REPLACE THESE BY MATCH_FORM(FORM, POL, SET)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% match_form(F, Set)
% F is a literal
% Pol is plus or minus
% Set is the set of nodes that contain things that match the literal and are 
% of the opposite polarity
% SHOULD IT BE SAME POL? MIGHT GET CONFUSING MEBBE 2 VERSINOS
/*
get the functor
find the hash value
if it is a variable, union all the guys in the pred2hash for this functor
if it is a constant, get the guys in the corresponding thing and union
  the ones inthe variable ones.

*/
%% SAME POLARITY HERE!!!!!!! so can substitute in predtonode BUT at pred2node,
%% need to give the OPPOSITE polarity.

% should really take care of cases where there is no match. do not want
% thisto just die. on the other habd, don't want backtracking. :( Maybe best
% to have a "clean_node_index" etc that does the right thing
%

match_form(F, Pol, S):- !,
    hash_it(F, H),
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Matching: '), print(DBGS, F)
%          , print(DBGS, H), nl(DBGS)); true),    
    (var(H)->					  % arg1 var
	 (functor(F, Fu, _),
	  cpred2hash(Fu, Flist),
	  unionallofem(Flist, Pol, [], S));
    (Pol = plus ->				  % positive
	 (cnode_index(H, S1, _),
	  functor(F, Fu, _),
	  (Fu = H -> S = S1;			  % no arg
	  (name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  cnode_index(Ff, S2, _),
	  union(S1, S2, S))));
    (cnode_index(H, _, S1),
     functor(F, Fu, _),
     (Fu = H -> S = S1;
     (name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
     cnode_index(Ff, _, S2),
     union(S1, S2, S)))))).


match_new_form(F, Pol, S):- !,
    hash_it(F, H),
%    ( debug_level(3) -> 
%          (debug_stream(DBGS), print(DBGS,'Matching new: '), print(DBGS, F)
%          , print(DBGS, H), nl(DBGS)); true),    
    (var(H)->					  % arg1 var
	 (functor(F, Fu, _),
	  cnewpred2hash(Fu, Flist),
	  unionallofemnew(Flist, Pol, [], S));
    (Pol = plus ->				  % positive
	 (cnew_node_index(H, S1, _),
	  functor(F, Fu, _),
	  (Fu = H -> S = S1;			  % no arg
	  (name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
	  cnew_node_index(Ff, S2, _),
	  union(S1, S2, S))));
    (cnew_node_index(H, _, S1),
     functor(F, Fu, _),
     (Fu = H -> S = S1;
     (name(Fu, FUNC), append(FUNC, [36, 36], FVAR), name(Ff, FVAR),
     cnew_node_index(Ff, _, S2),
     union(S1, S2, S)))))).

cnode_index(F, P, M):-
    node_index(F, P, M), !.
cnode_index(_, [], []):- !.
cnew_node_index(F, P, M):-
    new_node_index(F, P, M), !.
cnew_node_index(_, [], []):- !.
cpred2hash(X, Y):-
    pred2hash(X, Y), !.
cpred2hash(_, []):- !.
cnewpred2hash(X, Y):-
    newpred2hash(X, Y), !.
cnewpred2hash(_, []):- !.


unionallofem([], _, X, X):- !.
unionallofem([X|XR], plus, In, Out):- !,
    node_index(X, P, _),
    union(P, In, Int),
    unionallofem(XR, plus, Int, Out).
unionallofem([X|XR], minus, In, Out):- !,
    node_index(X, _, P),
    union(P, In, Int),
    unionallofem(XR, minus, Int, Out).

unionallofemnew([], _, X, X):- !.
unionallofemnew([X|XR], plus, In, Out):- !,
    new_node_index(X, P, _),
    union(P, In, Int),
    unionallofem(XR, plus, Int, Out).
unionallofemnew([X|XR], minus, In, Out):- !,
    new_node_index(X, _, P),
    union(P, In, Int),
    unionallofem(XR, minus, Int, Out).



/*
Test cases:
assert:
*/
insert_test:- 
    insert_one(1, foo(a, 1)),
    insert_one(11, foo(a, 11)),
    insert_one(2, foo(b, 2)),
    insert_one(22, foo(b, 22)),
    insert_one(3, foo(c, 3)),
    insert_one(33, foo(c, 33)),
    insert_one(4, foo(X, 4)),
    insert_one(5, foo(Y, 5)),
    insert_one(6, foo(d(e), 6)),
    insert_one(7, foo(f(g), 7)).

% looks like tings are OK


dump_kb(F):-
    open(F, 'write', S),
    tell(S),
    listing(node),
    listing(node_index),
    listing(pred2hash),
    listing(new_node),
    listing(new_node_index),
    listing(newpred2hash),
    close(S).


% node_index(H, L1, L2)
% H is a hash value
% L1 is a list of nodes where the term correspomding to H occurs +vely
% L2 is a list of nodes where the term correspomding to H occurs -vely
% one of the hashvalues will be the predname itself fro the case that 
% the arg is a var
% :- dynamic node_index/3.
% :- dynamic new_node_index/3.

% pred2hash(P, H)
% P is a predicate name
% H is a list of hash values as above that P occurs in
% :- dynamic pred2hash/2.

