
% This is where we try doing some nmr according to the new approach of
% 03/01. 
% this is the Tweety example.

% NOTE: this approach looks clumsy and it takes a number of steps to 
% apply the default. But this procedure could be built into Alma if
% we want this to be a 'standard' way to do default reasoning.

% . defaults are represented as:
%%%      named(fif(bird(X), conclusion(defcon(bf(X), flies(X)))), bf(X)).
%%%      default(bf(X)).
% . Preferneces are stated as:
%    prefer(pnf(X), bf(X)).
% 4. Processign the defaults:
%    1. match the lhs
%    2. assert defcon(name, form)
%    3. Check that this default can be applied
%    4. If it can, delete the appropriate weaker ones
%    5. add the formula to the kb if need be


% penguins are birds
if(penguin(X), bird(X)).

% Birds fly
named(fif(bird(X), conclusion(defcon(bf(X), flies(X)))), bf(X)).
default(bf(X)).

% Penguins don't fly
named(fif(penguin(X), conclusion(defcon(pnf(X), not(flies(X))))),  pnf(X)).
default(pnf(X)).

prefer(pnf(X), bf(X)).

% if there is no stronger default, apply the one under consideration.
fif(and(defcon(N1, F1), 
	eval_bound(\+ pos_int_u(prefer(N2, N1)), [N1])), 
    conclusion(apply(N1, F1))).

% if there is a stonger default, but it has not applied, apply the one
%  under consideration.
fif(and(defcon(N1, F1),
	and(prefer(N2, N1),
	    eval_bound(\+ pos_int_u(defcon(N2, F2)), [N2]))),
    conclusion(apply(N1, F1))).

% problem with deleting the stuff before adding F 
fif(apply(N, F), conclusion(addit(F))).
% if we have decided to add the consequence of a default and there is a 
% less preferred default that is in the KB, distrust the descendants
% of that default.
fif(and(apply(N1, F1),
	and(prefer(N1, N2),
	    and(defcon(N2, F2),
	       eval_bound(form_to_name(defcon(N2, F2), NA), [N2, F2])))),
    conclusion(eval_bound(distrust_descendants(NA), [NA]))).

% very very stupid 
fif(addit(F), conclusion(F)).

% contrafixing
% Recall that once there is a contradiction, ALma automatically distrust
% the contradictands and their consequences. What we need to do here is 
% to decide which of the contradictands, if any, we ought to reinstate.

% there are these cases for contradictions:
% 1. between a default and a non-default, and here the non-default should 
%    be reinstated
% 2. between defaults for which there is no preference. In this case 
%    we don't reinstate either. In this case though, Alma should distrust
%    teh defaults down, not just the consequences of the defaults. If the
%    default reasoning is not builf into Alma, this can't be done there
%    and it will have to be specified here. Need to work on that.
% 3. between non-defaults. Here too we don't reinstate either of the
%    contradictands.


% the deriv_fringe is a list of lists of axioms or observations used to 
% derive a formula. Each list corresponds to one derivation of the formula. 
% So, when we get a contra, we find the fringes
fif(and(contra(C1, C2, T), eval_bound(name_to_deriv_fringe(C1, CD), [C1])),
    conclusion(deriv_fringe(C1, CD))).
fif(and(contra(C1, C2, T), eval_bound(name_to_deriv_fringe(C2, CD), [C2])),
    conclusion(deriv_fringe(C2, CD))).

% then we verify the fringes to see whether one of the contradictands 
% depends on a default while the other does not.
% this could be written as a prolog program called from Alma. That
% might be a better solution.
fif(deriv_fringe(C, F), conclusion(fringe_subset(C, F))).

fif(fringe_subset(C, []), conclusion(not(depends_on_default(C)))).

fif(and(fringe_subset(C, [X|Y]), no_defaults(X)),
    conclusion(fringe_subset(C, Y))).
fif(fringe_subset(C, [X|Y]), conclusion(one_dependency(C, X, X))).

fif(and(one_dependency(C, [X|Y], Z), default(X)),
    conclusion(depends_on_default(C))).
fif(and(one_dependency(C, [X|Y], Z), 
	eval_bound(\+ pos_int(default(X)), [X])), 
    conclusion(one_dependency(C, Y, Z))).
fif(one_dependency(C, [], X), conclusion(no_defaults(X))).


% If C1 depends on no default but C2 does, then we prefer C1
fif(and(contra(C1, C2, T), and(depends_on_default(C2), 
			       not(depends_on_default(C1)))),
    conclusion(reinstate(C1))).
fif(and(contra(C1, C2, T), and(depends_on_default(C1), 
			       not(depends_on_default(C2)))),
    conclusion(reinstate(C2))).





