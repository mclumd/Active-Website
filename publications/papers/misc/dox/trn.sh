
# get rid of tags
grep -i "^[a-z]" /cdrom/trn_all/pt1_trn/*.trn

# extract context
grep "^[^0-9]" trn
