% Nixon diamond in the method II

% republicans are not pacifists
named(fif(republican(X), conclusion(defcon(repnotpac(X), not(pacifist(X))))),
      repnotpac(X)).
default(repnotpac(X)).

% quakers are pacifists
named(fif(quaker(X), conclusion(defcon(quakerpac(X), pacifist(X)))),
      quakerpac(X)).
default(quakerpac(X)).

and(republican(nixon), quaker(nixon)).



