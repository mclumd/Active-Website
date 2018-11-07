% nmr_2.pl
% This is the Tweety example in method 2 and with a helper file
% the helper file is nmr_2_help.pl
% kpurang Apr 01
%
% Note the way defaults are specified:
% named(fif(<PREMISE>, conclusion(defcon(<NAME>, <CONCLUSION>))),
%       <NAME>).
% <NAME> is to be parameterized by the variables of interest in the default
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% penguins are birds
if(penguin(X), bird(X)).

% Birds fly
named(fif(bird(X), conclusion(defcon(bf(X), flies(X)))), 
      bf(X)).
default(bf(X)).

% Penguins don't fly
named(fif(penguin(X), conclusion(defcon(pnf(X), not(flies(X))))),  
      pnf(X)).
default(pnf(X)).

prefer(pnf(X), bf(X)).









