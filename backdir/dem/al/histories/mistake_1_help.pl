% helper for mistake.pl


:- ensure_loaded(library(basics)).

% parsing derivations:

/*

  deriv(25,[[deriv(penguins_dont_fly,[]),
             deriv(22,[[deriv(swim_bird_penguin,[]),
                        deriv(swims_tweety,[]),
                        deriv(bird_tweety,[])],
                       [deriv(swim_bird_penguin,[]),
                        deriv(bird_tweety,[]),
                        deriv(swims_tweety,[])]])]]) 


[penguins_dont_fly,swim_bird_penguin,swims_tweety,bird_tweety,swim_bird_penguin,bird_tweety,swims_tweety] ;

hmm.. still need to sort this

*/

get_derivation_leaf(deriv(X, []), [X]):- !.
get_derivation_leaf([], []):- !.
get_derivation_leaf(deriv(_, [X|R]), SX):- !,
    get_derivation_leaf(X, XD),
    get_derivation_leaf(R, RD),
    ord_union(XD, RD, SX).
get_derivation_leaf([X|R], SX):- !,
    get_derivation_leaf(X, XD),
    get_derivation_leaf(R, RD),
    ord_union(XD, RD, SX).
    

% from tweet_help.pl


% prolog helper file to be used with tweet.pl

contains([], _):- fail, !.
contains([Y|Z], X):- !,
    member(X, Y); contains(Z, X).

