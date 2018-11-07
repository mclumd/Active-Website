%
% cycle.pl
% example to illustrate cycling
% assert cycle(T, DT) where T is some time in the future that we want the
% cycling to start and DT is the period of the cycles.
%

% verify that the time is right for turning on

fif(and(cycle(S, DT), 
       and(now(S),
	   eval_bound(T is S + DT, [S, DT]))), 
    conclusion(recycle(T, DT))).

% add ON, delete OFF, and set up for turning off at the right time

fif(recycle(T, DT), conclusion(eval_bound(af('ON'), [T, DT]))).
fif(recycle(T, DT), conclusion(eval_bound(df('OFF'), [T, DT]))).
fif(recycle(T, DT), conclusion(uncycle(T, DT))).

% verify that the time is right for turning off

fif(and(uncycle(S, DT), 
       and(now(S),
	   eval_bound(T is S + DT, [S, DT]))), 
   conclusion(unrecycle(T, DT))).

% add OFF, delete ON, and set up for turning on at the right time

fif(unrecycle(T, DT), conclusion(eval_bound(af('OFF'), [T, DT]))).
fif(unrecycle(T, DT), conclusion(eval_bound(df('ON'), [T, DT]))).
fif(unrecycle(T, DT), conclusion(cycle(T, DT))).



