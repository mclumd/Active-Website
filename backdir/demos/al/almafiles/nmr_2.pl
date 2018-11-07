% nmr_2.pl
% Specifications for default application and contradiction handling
%  for nmr2. This should be included in all the nmr2 examples.
% the helper file is nmr_2_help.pl
% kpurang Apr 01
%
% Note the way defaults are specified:
% named(fif(<PREMISE>, conclusion(defcon(<NAME>, <CONCLUSION>))),
%       <NAME>).
% <NAME> is to be parameterized by the variables of interest in the default




%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% APPLYING DEFAULTS

% If F1 is a possible default conclusion of default N1, then we verify
% whether NI can really apply.
% If N1 can apply, we assert that it is applied, and if there are less
% preferred defaults, we distrust them and we add the conlcusion to the KB.

fif(and(defcon(N1, F1), 
	eval_bound(verify_applicability(N1, F1), [N1, F1])),
    conclusion(applied_default(N1, F1))).
fif(and(applied_default(N1, F1), 
	eval_bound(distrust_less_preferred(N1), [N1])),
    conclusion(F1)).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% HANDLING CONTRADICTIONS

% when we have a contradiction, we find the 'axioms' the derivation of
% each contradictand depends on

fif(and(contra(C1, C2, T), eval_bound(name_to_deriv_fringe(C1, CD), [C1])),
    conclusion(deriv_fringe(C1, CD))).
fif(and(contra(C1, C2, T), eval_bound(name_to_deriv_fringe(C2, CD), [C2])),
    conclusion(deriv_fringe(C2, CD))).

% given the 'axioms', we verify wheter the formula depends on some default.

fif(and(deriv_fringe(X, Y),
	eval_bound(deriv_is_defaulty(Y), [Y])),
    conclusion(depends_on_default(X))).

fif(and(deriv_fringe(X, Y),
	eval_bound(\+ deriv_is_defaulty(Y), [Y])),
    conclusion(not(depends_on_default(X)))).

% If C1 depends on no default but C2 does, then we prefer C1

fif(and(contra(C1, C2, T), and(depends_on_default(C2), 
			       not(depends_on_default(C1)))),
    conclusion(reinstate(C1))).
fif(and(contra(C1, C2, T), and(depends_on_default(C1), 
			       not(depends_on_default(C2)))),
    conclusion(reinstate(C2))).








