#!/usr/imports/bin/perl5

# while (<STDIN>) {
#     s/www|cs/foo/gi;
#     print;
#}

# while(<STDIN>){
#     s?\"http://www.informatik.uni-tuebingen.de/\"|"http://www.informatik.uni-tuebingen.de/dislog.html\"?\"http://www.cs.umd.edu/projects/active/laisem/dummy.html\"?gi;
#     print;
# }

while(<STDIN>){
s?\"http://www.cs.umd.edu/people/carolina.html\"|\"http://www.informatik.uni-tuebingen.de/\"|\"http://www.cs.umd.edu/people/godfrey/page.html\"|\"http://www.socsci.uci.edu/cogsci/HIPLab/AIC\"|\"http://www.cs.umd.edu/projects/active/laisem/fall93/laisem_fall93.html\"|\"http://www.cs.umd.edu/projects/active/laisem/fall94/laisem_fall94.html\"|\"/seminars/laisem/laisem.html\"|\"http://karna.cs.umd.edu:3264/people/jarek.html\"|\"http://www7.informatik.tu-muenchen.de/~weissg/ws-ecai96\"|\"http://karna.cs.umd.edu:3264/people/minker.html\"|\"http://www.cis.uni-muenchen.de/\"|\"http://www.cs.umd.edu/people/shekhar.html\"|\"http://www.informatik.uni-tuebingen.de/dislog.html\"|\"http://www.cs.umd.edu/projects/active/laisem/fall95/morreau2_abs.html\"|\"http://www.cs.umd.edu/projects/active/laisem/spring94/laisem_spring94.html\"|\"http://www.cs.umd.edu/projects/active/laisem/fall95/morreau1_abs.html\"?\"http://www.cs.umd.edu/projects/active/laisem/dummy.html\"?gi;
print;
}

