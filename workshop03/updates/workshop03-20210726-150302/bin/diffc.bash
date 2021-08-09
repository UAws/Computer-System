#!/bin/bash
# runs diff with colour coded output

# assumes first file is actual test output and second file is expected test output
# every line of diff's output should be prefixed by +, -, or |
# + indicates an unexpected line of output
# - indicates a missing line of output
# | indicates an expected line of output

# the diff output is piped through diff-colours
# Meta characters (non ASCII) are given the prefix 'M-' followed by their ASCII equivalent
# ASCII control characters 0 to 31 are output as '^@' to '^_' respectively
# ASCII delete is output as '^?'
#
# if the output is a terminal ANSI escape sequences add colours to the output
# + lines are coloured red on a greyish background
# - lines are coloured blue on a greyish background
# | lines are drawn faint
# Meta characters, ASCII control characters and delete are highlighted in reverse video


# check we have at least two arguments
if [ "${#*}" -lt 2 ] ; then 
    echo "usage: diff [options] <pathname1> <pathname2>"
    exit -1
fi

# look for extra arg to pass to diff colours, -z and -R are not accepted by diff
# -z no colours, -R force colours
diffargs=()
coloursarg=
while [ "${#*}" -gt 2 ] ; do
    if [ "x${1}" == "x-z" ] ; then
        coloursarg="-n"
    elif [ "x${1}" == "x-R" ] ; then
        coloursarg="-y"
    else
        diffargs+=("${1}")
    fi
    shift 1
done

# search for the diff-colours executable - it should be where this script is
DIFFC="`dirname $0`"/diff-colours

# if not found silently replace it with cat -t
if [ ! -x "${DIFFC}" ] ; then
    DIFFC="cat -t"
else
    DIFFC="${DIFFC} ${coloursarg}"
fi

# now run the real diff with our extra arguments
if [ "x${coloursarg}" = "x-y" ] ; then
    /usr/bin/diff -ad --old-line-format=$'+%L' --new-line-format=$'-%L' --unchanged-line-format=$'|%L' $diffargs $@ | ${DIFFC} | less -R
else
    /usr/bin/diff -ad --old-line-format=$'+%L' --new-line-format=$'-%L' --unchanged-line-format=$'|%L' $diffargs $@ | ${DIFFC}
fi
