/*
File dbman/history.pl
By: kpurang
What: code here concerns the history
      essentially dumps it to a file

Todo: code to retrieve history
      send the history to an external process that can do that
      define the sorts of manipulation alma will need
*/

% historicize(+List)
% Adds the list of new nodes to the history, at the current step.
% dumps into file if need be.
historicize(List):- !,
    (verbose(true)-> (print('historicize '), print(List), nl); true),
    step_number(S), 
    dump_history(List).
% removed the following line coz this just eats up memory for no reason
%    assert(history(S, List)).
%

dump_history(_):-
    history_dump(false), !.
dump_history(List):- !,
    step_number(XX),
    history_stream(S),
    put(S, 10), write_term(S, step(XX), [max_depth(0)]), put(S, 10) ,
    dump_each_hist(S, List).

dump_each_hist(S, []):- !, flush_output(S).
dump_each_hist(S, [X|Y]):- !,
    get_formula(X, F),
    get_junk(X, [FIB|_]),
    get_nodename(X, XName),
    get_parents(X, Xparents),
    sprint_one([F, FIB], SX),
    name(SS, SX),
    name(SPC, "  "),
    write_term(S, add(SS), [max_depth(0)]),
    write(S, SPC),
    write_term(S, XName, [max_depth(0)]),
    write(S, SPC),
    write_term(S, Xparents, [max_depth(0)]),
    put(S, 10),
    dump_each_hist(S, Y).

his_deletes:-
    retract(deleted_forms(F)),
    assert(deleted_forms([])),
    deadel(F).

deadel(_):-
    history_dump(false), !.
deadel(F):-
    history_stream(S),
    wedel(S, F).

wedel(S, []):-  
    write(S, 'End of Step----------------------------------'), put(S, 10),
    flush_output(S), !.
wedel(S, [X|Y]):-
    (verbose(true)-> (print('dumping '), print(X), nl); true),
    get_formula(X, F),
    get_junk(X, [FIB|_]),
    get_nodename(X, XName),
    get_parents(X, Xparents),
%    write_term(S, delete(F), [max_depth(0)]),
    sprint_one([F, FIB], SX),
    name(SS, SX),
    name(SPC, "  "),
    write_term(S, delete(SS), [max_depth(0)]),
    write(S, SPC),
    write_term(S, XName, [max_depth(0)]),
    write(S, SPC),
    write_term(S, Xparents, [max_depth(0)]),
    put(S, 10),
    wedel(S, Y).

