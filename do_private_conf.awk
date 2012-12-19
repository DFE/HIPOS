#! /usr/bin/awk -f

# remember all non-comment lines from the first file (= private.conf)
# array entries are accessed via the first field, i.e. the option to be set
# this means that all kinds of assignment operators will work
# ! it also means that whitespaces surrounding assignments are _not_ optional !
# (which is in accordance to oe's style guide)
NR==FNR && $1 !~ /^#/ {a[$1]=$0};

# in the second file (= local.conf)
NR!=FNR {
    # if we encounter an option that is set in private.conf
    if ($1 in a) {
        # make it a comment and add the line from private.conf afterwards
        print "# " $0 "\n" a[$1]
        # forget that option
        delete a[$1]
    } else {
        # echo
        print $0
    }
}

END {
    # add all values read from private.conf that are still left
    # at the end of the file
    printf "\n# additional values set by %s\n", ARGV[1]
    for (i in a) print a[i]
}