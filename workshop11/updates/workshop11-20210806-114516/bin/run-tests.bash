#!/bin/bash

# runs selected commands over matching test files
# assumptions / requirements:
# 1. usage: bin/run-tests [working] testprefix*
#
# 2. for each testsprefix all tests with a pathname that starts with ./tests/testprefix are run
#    . if no testprefix is provided than all tests in ./tests directory structure are run
#
# 3. the program to be run is taken from the top level directory in ./tests,
#    eg ./tests/parser/abc/xyz contains tests run by the parser program
#    . if the optional first argument 'working' is provided, the working versions of the
#      programs will be used and all the tests should pass!
#
# 4. individual tests have a description file with a name *-test containing 0, 1 or 2 lines
#    . the file can be empty
#    . line 1, a description, if missing use the * part of the filename as a description
#    . line 2, command line arguments, if missing no arguments are passed to test
#
# 5. test inputs are named:
#    . *-test-0-input
#      . if missing no input is used by the test
#
# 6. expected test outputs and exit status are named:
#    . *-test-expected-1-output-expected
#      . if missing the output is not checked
#    . *-test-expected-2-error-expected
#      . if missing the error output is not checked
#    . *-test-expected-exit-status
#      . if missing the exit status is not checked
#
# 7. the tests used for marking enforce buffered output, disable trace writes and disable log writes,
#    the results of running tests this way are named:
#    . *-test-tested-1-output
#    . *-test-tested-2-error
#    . *-test-tested-exit-status
#
# 8. the tests are also run with no buffered IO, with trace writes enabled and log writes enabled,
#    the results of running tests this way are named:
#    . *-test-trace-log-1-output
#    . *-test-trace-log-2-error
#    . *-test-trace-log-exit-status
#

# set locale to C
export LC_ALL=C

# nullglob is set so nothing runs if there are no test files
shopt -s nullglob

# cd to the root dir, the parent of bin, includes, lib, tests, etc.
THISPWD=`pwd`
cd `dirname $0`
cd ..

# check directories ...
check_dir()
{
    if [ ! -d "${1}" ] ; then
        echo "Cannot find the ${1} directory"
        exit -1
    fi
}
check_dir bin includes lib tests

# Tell students where we are and how to run the tests
PARENTDIR=`pwd`
if [ "x{$PARENTDIR}" != "x{$THISPWD}" ] ; then
    echo "Change to the directory ${PARENTDIR} if you want to cut-paste the test commands:"
fi

# check we have a command1 first
check_command()
{
    if [ ! -x "${1}" ] ; then
        echo "Cannot find executable ${1}"
        exit -1
    fi
}

### PRETTY STUFF ###

# if using an xterm - use headings and colour coded test results
if [ "${TERM:0:5}" == "xterm" -o "${TERM:0:2}" == "vt" ] ; then
    colorboldblack="\x1b[1;7;30m"
    colorboldmagenta="\x1b[1;7;35m"
    colorboldblue="\x1b[1;7;34m"
    colorboldgreen="\x1b[1;7;32m"
    colorboldcyan="\x1b[1;7;36m"
    colorboldred="\x1b[1;7;31m"
    undocolor="\x1b[0m"
    diff_colours_flag=-y
else
    colorboldblack=
    colorboldgreen=
    colorboldcyan=
    colorboldmagenta=
    colorboldblue=
    colorboldred=
    undocolor=
    diff_colours_flag=
fi

# construct the diff command to display
if [ -x bin/diffc ] ; then
    diffcmd=bin/diffc
else
    if [ -f bin/diffc.bash ] ; then
        diffcmd="bash bin/diffc.bash"
    else
        # if no diffc command, just use tell students to use diff
        diffcmd=diff
    fi
fi

# make a string all lowercase

lowercase=
to_lower_case()
{
    local str="${1}"
    if [ ${BASH_VERSION:0:1} -gt 3 ]
    then
        lowercase="${str,,}"
    else
        lowercase=$(tr "[:upper:]" "[:lower:]" <<< ${str})
    fi
}


# make a string of whitespace matching the longest parameter
padding=
setpadding()
{
    padding=
    while [ "${#@}" -gt 0 ] ; do
        while [ "${#padding}" -lt "${#1}" ] ; do
            padding="${padding} "
        done
        shift 1
    done
}


### COLLATE THE REQUESTED TESTS ###


exit_description=
describe_exit_status()
{
    case $1 in
    0)
        exit_description="success"
        ;;
    1)
        exit_description="an error"
        ;;
    2)
        exit_description="misuse of shell builtin"
        ;;
    64)
        exit_description="command line usage error"
        ;;
    65)
        exit_description="data format error"
        ;;
    66)
        exit_description="cannot open input"
        ;;
    67)
        exit_description="addressee unknown"
        ;;
    68)
        exit_description="host name unknown"
        ;;
    69)
        exit_description="service unavailable"
        ;;
    70)
        exit_description="internal software error"
        ;;
    71)
        exit_description="system error (e.g., can't fork)"
        ;;
    72)
        exit_description="critical OS file missing"
        ;;
    73)
        exit_description="can't create (user) output file"
        ;;
    74)
        exit_description="input/output error"
        ;;
    75)
        exit_description="temp failure; user is invited to retry"
        ;;
    76)
        exit_description="remote error in protocol"
        ;;
    77)
        exit_description="permission denied"
        ;;
    78)
        exit_description="configuration error"
        ;;
    126)
        exit_description="command cannot be executed"
        ;;
    127)
        exit_description="command not found"
        ;;
    128)
        exit_description="exit status not in range 0 to 255"
        ;;
    129)
        exit_description="SIGHUP"
        ;;
    130)
        exit_description="SIGINT"
        ;;
    131)
        exit_description="SIGQUIT"
        ;;
    132)
        exit_description="SIGILL"
        ;;
    133)
        exit_description="SIGTRAP"
        ;;
    134)
        exit_description="SIGABRT"
        ;;
    134)
        exit_description="SIGIOT"
        ;;
    135)
        exit_description="SIGBUS"
        ;;
    136)
        exit_description="SIGFPE"
        ;;
    137)
        exit_description="SIGKILL"
        ;;
    138)
        exit_description="SIGUSR1"
        ;;
    139)
        exit_description="SIGSEGV"
        ;;
    140)
        exit_description="SIGUSR2"
        ;;
    141)
        exit_description="SIGPIPE"
        ;;
    142)
        exit_description="SIGALRM"
        ;;
    143)
        exit_description="SIGTERM"
        ;;
    144)
        exit_description="SIGSTKFLT"
        ;;
    145)
        exit_description="SIGCHLD"
        ;;
    146)
        exit_description="SIGCONT"
        ;;
    147)
        exit_description="SIGSTOP"
        ;;
    148)
        exit_description="SIGTSTP"
        ;;
    149)
        exit_description="SIGTTIN"
        ;;
    150)
        exit_description="SIGTTOU"
        ;;
    151)
        exit_description="SIGURG"
        ;;
    152)
        exit_description="SIGXCPU"
        ;;
    153)
        exit_description="SIGXFSZ"
        ;;
    154)
        exit_description="SIGVTALRM"
        ;;
    155)
        exit_description="SIGPROF"
        ;;
    156)
        exit_description="SIGWINCH"
        ;;
    157)
        exit_description="SIGIO"
        ;;
    158)
        exit_description="SIGPWR"
        ;;
    159)
        exit_description="SIGSYS"
        ;;
    255)
        exit_description="exit status out of range"
        ;;
    *)
        exit_description="non-zero"
        ;;
    esac
    exit_description="${1} ${exit_description}"
}

# break a line of a CSV file into an array
# empty cells are given a gratutious single space
declare -a csv_line
parse_csv_line()
{
    # replace &, ", escaped comma (\,) and space with html entities 
    local line="${1//&/&amp;}"
    line="${line//\"\"/&dquot;}"
    line="${line// /&nbsp;}"

    # now break around ", even numbered cells are quoted cells in CSV!
    local csv_temp=(${line//\"/ })

    # all single " have now been removed
    ix=0
    line=
    for cell in "${csv_temp[@]}" ; do

        # ix % 2 gives non-zero, ie true, for odd numbers
        if ((ix++ % 2))
        then
            # inside a quoted cell,escape the commas
            cell="${cell//,/&comma;}"
        else
            # outside a quoted cell, ,&dquot;, represents an empty cell
            # replace with ,&nbsp;,
            cell="${cell//,&dquot;,/,&nbsp;,}"
        fi
        line="${line}${cell}"
    done

    # now break around , after replacing ,, with ,.,
    # now expend empty fields ,, to be , ,
    line=(${line//,,/,&nbsp;,})
    line=(${line//,,/,&nbsp;,})
    local csv_temp=(${line//,/ })
    ix=0
    csv_line=(" ")
    for cell in "${csv_temp[@]}" ; do

        cell="${cell//&dquot;/\"}"
        cell="${cell//&comma;/,}"
        cell="${cell//&nbsp;/ }"
        cell="${cell//&amp;/&}"

        if [ ${#cell} -eq 0 ] ; then cell=" " ; fi
        csv_line[((ix++))]="${cell}"
    done
}

# no 2D arrays to one array for each element of a test
declare -a tests_id
declare -a tests_runit
declare -a tests_path
declare -a tests_cwd
declare -a tests_program
declare -a tests_arguments
declare -a tests_output
declare -a tests_error
declare -a tests_status
declare -a tests_kind
declare -a tests_cat
declare -a tests_shortd
declare -a tests_longd

# parse our tests.csv file
parse_test_lists()
{
    local full_tests=`cd tests;/bin/pwd`
    local full_websub_tests=`test -d websub-tests && cd websub-tests && /bin/pwd`
    while [ ${#@} -gt 0 ]
    do
        tests_csv="${1}"
        shift 1

        # check the .csv file exists - it should
        if [ ! -f "${tests_csv}" ]
        then
            echo -e "${colorboldred} Warning: cannot find ${tests_csv} ! ${undocolor}"
            continue
        fi

        # check the tests directory is really inside tests/ or websub-tests/
        local csvtests=`dirname ${tests_csv}`
        local full_csvtests=`cd ${csvtests};pwd`
        if [ "x${full_csvtests#${full_tests}}" == "x${full_csvtests}" -a "x${full_csvtests#${full_websub_tests}}" == "x${full_csvtests}" ]
        then
            echo -e "${colorboldred} Warning: ${tests_csv} must be in tests/.... ! ${undocolor}"
            continue
        fi

        # read test.csv, ignore any line starting with ID, it should be the title row
        # prefix path with directory name containing the tests
        local lineno=0
        local col_id=0
        local col_path=0
        local col_cwd=0
        local col_prog=0
        local col_args=0
        local col_output=0
        local col_error=0
        local col_status=0
        local col_kind=0
        local col_cat=0
        local col_shortd=0
        local col_longd=0
        while IFS= read -r line || [ -n "${line}" ]; do
            parse_csv_line "${line}"
            # line 0 - 
            if ((lineno++ == 0))
            then
                local c=0
                for col in ${csv_line[@]} ; do
                    case "${col}" in
                    ID)
                        ((col_id=c++))
                        ;;
                    Path)
                        ((col_path=c++))
                        ;;
                    Program)
                        ((col_prog=c++))
                        ;;
                    Arguments)
                        ((col_args=c++))
                        ;;
                    Output)
                        ((col_output=c++))
                        ;;
                    Error)
                        ((col_error=c++))
                        ;;
                    Status)
                        ((col_status=c++))
                        ;;
                    Kind)
                        ((col_kind=c++))
                        ;;
                    Category)
                        ((col_cat=c++))
                        ;;
                    Short-Description)
                        ((col_shortd=c++))
                        ;;
                    Long-Description)
                        ((col_longd=c++))
                        ;;
                    Directory)
                        ((col_cwd=c++))
                        ;;
                    *)
                        ((c++))
                        ;;
                    esac
                done
                continue
            fi

            # ignore bad lines - test id of ID should only be in line 1
            local testid="${csv_line[$col_id]}"
            if [ "x${testid}" == "xID" ] ; then continue ; fi

            # check we have a path or ignore this test
            local path="${csv_line[$col_path]}" ;
            if [ "x${path}" == "x " -o ${#path} -eq 0 ] ; then continue ; fi

            # prefix path with tests/... if it does not start with ./
            if [ "x${path:0:2}" != "x./" ] ; then path="${csvtests}/${path}" ; fi

            # check we have a program or ignore this test
            local program="${csv_line[$col_prog]}" ;
            if [ "x${program}" == "x " -o ${#program} -eq 0 ] ; then continue ; fi

            # check we have a sensible kind?
            local kind="${csv_line[$col_kind]}"
            if [ "x${kind}" == "x " -o ${#kind} -eq 0 ] ; then kind=filter ; fi
            to_lower_case "${kind}"
            kind="${lowercase}"

            # check exit status, do not check if "no" or " ", "yes" => 0, 0 - 255
            local xstatus="${csv_line[$col_status]}" ;
            if [ "x${xstatus}" == "x " -o "x${xstatus}" == "xno" -o ${#xstatus} -eq 0 ]
            then
                xstatus="no"
            else
                if [ "x${xstatus}" != "x${xstatus##*[^0-9]}" ] ; then xstatus=0 ; fi
            fi

            tests_id+=("${testid}")
            tests_runit+=("no")
            tests_path+=("${path}")
            tests_cwd+=("${csv_line[$col_cwd]}")
            tests_program+=("${program}")
            tests_arguments+=("${csv_line[$col_args]}")
            tests_output+=("${csv_line[$col_output]}")
            tests_error+=("${csv_line[$col_error]}")
            tests_status+=("${xstatus}")
            tests_kind+=("${kind}")
            tests_cat+=("${csv_line[$col_cat]}")
            tests_shortd+=("${csv_line[$col_shortd]}")
            tests_longd+=("${csv_line[$col_longd]}")
        # following does not work on MacOS pre 11
        #done < <(sed -e 's/\\r//g' "${tests_csv}")
        done < <(sed -e 's///g' "${tests_csv}")
    done
}

# add a test - update longest test, program name and description
longest_program=Program
longest_testname=Test
longest_path=tests/
longest_description=Description
add_test()
{
    while [ ${#@} -gt 0 ] ; do
        t="${1}"
        shift 1

        local testname="${tests_id[$t]}"
        local path="${tests_path[$t]}"
        local program="${tests_program[$t]}"
        local arguments="${tests_arguments[$t]}"
        local shortd="${tests_shortd[$t]}"

        tests_runit[$t]="yes"
        alltests+=($t)

        # update the longest description
        if [ ${#longest_description} -lt ${#shortd} ] ; then longest_description="${shortd}" ; fi

        # update longest program 
        program="${program} ${arguments}"
        if [ ${#longest_program} -lt ${#program} ] ; then longest_program="${program}" ; fi

        # update longest testname 
        if [ ${#longest_testname} -lt ${#testname} ] ; then longest_testname="${testname}" ; fi

        # update longest path 
        path="${path}-trace-log-exit-status"
        if [ ${#longest_path} -lt ${#path} ] ; then longest_path="${path}" ; fi
    done
}

# find all requested tests
declare -a alltests
find_tests()
{
    # for every known test - check if its covered by the request
    local t=0
    for id in "${tests_id[@]}" ; do

        # extract the test ID and its path
        testid="${tests_id[$t]}"
        testpath="${tests_path[$t]}"

        # if no tests required - we run everything
        if [ ${#@} -eq 0 ] ; then

            add_test $t

        else

            # check each request in turn
            for prefix in "${@}" ; do

                # if we exact match the testid, run the test
                if [ "x${prefix}" == "x${testid}" ]
                then
                    add_test $t
                    continue
                fi

                # if we have a prefix of the test path, run the test
                if [ "x${testpath#${prefix}}" != "x${testpath}" -o "x${testpath#tests/${prefix}}" != "x${testpath}" ]
                then
                    add_test $t
                fi
            done
        fi

        ((t++))
    done
}

# check a test by comparing two files
check_test_result=X
check_hdl_test()
{
    cmp="${1}"
    out="${2}"
    status="${3}"

    # give up if test is missing ...
    if [ ! -f "${cmp}" -o ! -f "${out}" ] ; then return ; fi

    # compare tested and expected results
    if diff -q --strip-trailing-cr "${cmp}" "${out}" > /dev/null 2> /dev/null ; then
        check_test_result=P
    else
        if [ ${#status} -ne 0 ]
        then
            cmpl=$(wc -l "${cmp}" | sed -e 's/^ *//' -e 's/ .*//')
            outl=$(wc -l "${out}" | sed -e 's/^ *//' -e 's/ .*//')
            if ((cmpl==outl))
            then
                if ((status==0)) ; then check_test_result=P ; return ; fi
            fi
        fi
        check_test_result=F
    fi

}

show_hdl_test_result()
{
    testname="${1}"
    expected="${testname}-1-output"
    tested="${testname}-tested-1-output"
    error="${testname}-tested-2-error"

    if [ -s "${expected}" ] ; then
        echo -n " Expected test output:  "
        cat "${expected}"
    fi

    if [ -s "${tested}" ] ; then
        echo -n " Last test output:      "
        cat "${tested}"
    fi
    if [ -s "${error}" ] ; then
        echo -ne " Last test errors:      ${colorboldred}"
        cat "${error}"
        echo -ne "${undocolor}"
    fi
}

check_filter_test()
{
    local testname="${1}"
    local dowecare="${2}"
    local category="${3}"
    local expected="${testname}-${category}"
    local tested="${testname}-tested-${category}"
    local tracelog="${testname}-trace-log-${category}"

    # if we don't care there is no expected results to check, ignore errors too
    if [ "x${dowecare}" == "xno" ]
    then
        echo -n "   ?    "
        return ;
    fi

    # first compare trace-log and tested versions of the test output
    # give up if tracelog is missing - it must not be
    if [ ! -f "${tracelog}" ]
    then
        check_test_result=F
        echo -ne "${colorboldred} no TL? ${undocolor}"
        return ;
    fi

    # give up if test is missing ...
    if [ ! -f "${tested}" ]
    then
        check_test_result=F
        echo -ne "${colorboldred}missing?${undocolor}"
        return ;
    fi

    # compare tested and trace-log results
    if diff -q "${tested}" "${tracelog}" > /dev/null 2> /dev/null ; then
        TL=.
    else
        TL=X
    fi

    # we care so compare tested and expected results
    # compare tested and expected results
    if diff -q "${tested}" "${expected}" > /dev/null 2> /dev/null ; then
        if [ "${TL}" == "X" ] ; then TL="${colorboldmagenta}*${colorboldgreen}" ; fi
        echo -ne "${colorboldgreen}   P${TL}   ${undocolor}"
        if [ "x${check_test_result}" != "xF" ] ; then check_test_result=P ; fi
    else
        if [ "${TL}" == "X" ] ; then TL="${colorboldmagenta}*${colorboldred}" ; fi
        echo -ne "${colorboldred}   F${TL}   ${undocolor}"
        check_test_result=F
    fi

}

show_filter_test_result()
{
    local banner="${1}"
    local testname="${2}"
    local dowecare="${3}"
    local category="${4}"
    local expected="${testname}-${category}"
    local tested="${testname}-tested-${category}"
    local tracelog="${testname}-trace-log-${category}"

    # workout where in colpad to start
    local padstart=${#banner}
    ((padstart++))
    local padding="${colpad:${padstart}}"

    # if we don't care stop
    if [ "x${dowecare}" == "xno" ]
    then
        if [ ${test_details} == "YES" ]
        then
            if [ ${category} == "exit-status" -a -f "${tested}" ] ; then
                describe_exit_status `cat "${tested}"`
                echo " ${banner}${padding}${exit_description}"
            else
                echo " ${banner}${padding}Not checked"
            fi

            return
        fi
    fi

    # if we have a trace-log and tested - show diffs
    if [ ${test_details} == "YES" ]
    then
        if [ -f "${tracelog}" -a -f "${tested}" ]
        then
            # compare tested and trace-log results
            if diff -q "${tracelog}" "${tested}" > /dev/null 2> /dev/null ; then
                x=0
            else
                echo -e "${colorboldmagenta} ${banner}${padding}Trace-Log and Tested are different ${undocolor}"
                if [ ${category} == "exit-status" ] ; then
                    describe_exit_status `cat "${tracelog}"`
                    echo -e "${colorboldmagenta} ${banner}${padding}tracelog: ${exit_description} ${undocolor}"
                else
                    echo "${diffcmd} ${tracelog} ${tested}"
                    ${diffcmd} "${tracelog}" "${tested}" | bin/diff-colours ${diff_colours_flag}
                fi
            fi
        fi
    fi

    # if we have an expected and tested - show diffs
    if [ -f "${tested}" -a -f "${expected}" ]
    then
        # compare tested and expected results
        if diff -q "${tested}" "${expected}" > /dev/null 2> /dev/null ; then
            if [ ${category} == "exit-status" ] ; then
                describe_exit_status `cat "${expected}"`
                echo -e "${colorboldgreen} ${banner}${padding}${exit_description} ${undocolor}"
            else
                echo -e "${colorboldgreen} ${banner}${padding}Is correct ${undocolor}"
            fi
        else
            if [ ${category} == "exit-status" ] ; then
                describe_exit_status `cat "${tested}"`
                echo -e "${colorboldred} ${banner}${padding}tested: ${exit_description} ${undocolor}"
                if [ ${test_details} == "YES" ]
                then
                    describe_exit_status `cat "${expected}"`
                    echo -e "${colorboldred} ${banner}${padding}expected: ${exit_description} ${undocolor}"
                fi
            else
                if diff -qb "${tested}" "${expected}" > /dev/null 2> /dev/null ; then
                    echo -e "${colorboldred} ${banner}${padding}Has incorrect whitespace ${undocolor}"
                    echo "${diffcmd} ${tested} ${expected}"
                    ${diffcmd} "${tested}" "${expected}" | bin/diff-colours ${diff_colours_flag}
                else
                    echo -e "${colorboldred} ${banner}${padding}Is incorrect ${undocolor}"
                    echo "${diffcmd} -b ${tested} ${expected}"
                    ${diffcmd} -b "${tested}" "${expected}" | bin/diff-colours ${diff_colours_flag}
                fi
            fi
        fi
    fi
}

# move tested to expected to delete both
regenerate_expected()
{
    path="${1}"
    keep="${2}"
    category="${3}"

    if [ "x${keep}" == "xyes" ]
    then
        echo -n " Y "
        mv "${path}-tested-${category}" "${path}-${category}" 
    else
        echo -n " . "
        rm -f "${path}-tested-${category}" "${path}-${category}" 
    fi
}

# regenerate an individual test - we are passed a test index
# generate success output file, no input or exit status required
# this is a no-op
regenerate_hdl_test()
{
    # lookup the required tests info
    t="${1}"
    testname="${tests_id[$t]}"
    path="${tests_path[$t]}"
    program="${tests_program[$t]}"
    description="${tests_shortd[$t]}"
    arguments="${tests_arguments[$t]}"

    # Test Results
    program="${program} ${arguments}"
    echo -n " ${testname}${testname_padding:${#testname}} |"
    echo -n " ${program}${program_padding:${#program}} |"
    echo "  .  .  .  | ${description} "
}

# regenerate an individual test - we are passed a test index
# generate success output file, no input or exit status required
regenerate_bin_testit()
{
    # lookup the required tests info
    t="${1}"
    testname="${tests_id[$t]}"
    path="${tests_path[$t]}"
    program="${tests_program[$t]}"
    description="${tests_shortd[$t]}"
    arguments="${tests_arguments[$t]}"

    # is there an expected output?
    expect_output="${tests_output[$t]}"
    if [ "x${expect_output}" != "xno" ] ; then expect_output=yes ; fi

    # is there an expected error output?
    expect_error="${tests_error[$t]}"
    if [ "x${expect_error}" != "xyes" -a "x${expect_error}" != "x1" ] ; then expect_error=no ; else expect_error=yes ; fi

    # input filename, if file missing use /dev/null
    input="${path}-0-input"
    if [ ! -f "${input}" ] ; then input=/dev/null ; fi

    # clean up potential output files before starting the tests
    rm -f "${path}-1-output" "${path}-2-error" "${path}-exit-status"
    rm -f "${path}-tested-1-output" "${path}-tested-2-error" "${path}-tested-exit-status"
    rm -f "${path}-trace-log-1-output" "${path}-trace-log-2-error" "${path}-trace-log-exit-status"

    # now run in testing mode
    (
        # buffered IO but no trace or log writes
        export CSTOOLS_IOBUFFER_OUTPUT=iob_buffer
        export CSTOOLS_IOBUFFER_ERRORS=iob_buffer_why:iob_no_context
        export CSTOOLS_IOBUFFER_TRACES=iob_disable
        export CSTOOLS_IOBUFFER_LOGS=iob_disable

        cat "${input}" | ${program} ${arguments} fake-it > "${path}-tested-1-output" 2> "${path}-tested-2-error"
        echo $? > "${path}"-tested-exit-status
    )

    # Test Results
    #testname="${testname#tests/${program}/}"
    program="${program} ${arguments}"
    echo -n " ${testname}${testname_padding:${#testname}} |"
    echo -n " ${program}${program_padding:${#program}} | "
    regenerate_expected "${path}" "${expect_output}" 1-output
    regenerate_expected "${path}" "${expect_error}" 2-error
    rm -f "${path}-tested-exit-status"
    echo " .  | ${description} "
}

# regenerate an individual test - we are passed an test index
regenerate_filter_test()
{
    # lookup the required tests info
    t="${1}"
    testname="${tests_id[$t]}"
    path="${tests_path[$t]}"
    program="${tests_program[$t]}"
    description="${tests_shortd[$t]}"
    arguments="${tests_arguments[$t]}"

    # is there an expected output?
    expect_output="${tests_output[$t]}"
    if [ "x${expect_output}" != "xno" ] ; then expect_output=yes ; fi

    # is there an expected error output?
    expect_error="${tests_error[$t]}"
    if [ "x${expect_error}" != "xyes" -a "x${expect_error}" != "x1" ] ; then expect_error=no ; else expect_error=yes ; fi

    # input filename, if file missing use /dev/null
    input="${path}-0-input"
    if [ ! -f "${input}" ] ; then input=/dev/null ; fi

    # fixup program to ./ or bin/ or bash scriptname.bash and check it exists
    if [ "${program%.bash}" != "${program}" ]
    then
        if [ ! -f "${program}" ] ; then echo "Cannot find bash script: ${program}" ; return ; fi
        xprogram="/bin/bash ${program}"

    # use VMEmulator.sh as is
    elif [ "${program}" == "VMEmulator.sh" ]
    then
        xprogram="${program}"

    # do not add prefix if it is already bin/.... or ./....
    elif [ "${program#bin/}" != "${program}" -o "${program#./}" != "${program}" ]
    then
        xprogram="${program}"
        if [ ! -x "${xprogram}" ] ; then echo "Cannot find executable: ${xprogram}" ; return ; fi
    else
        xprogram="${binprefix}${program}"
        if [ ! -x "${xprogram}" ] ; then echo "Cannot find executable: ${xprogram}" ; return ; fi
    fi

    # clean up potential output files before starting the tests
    rm -f "${path}-1-output" "${path}-2-error" "${path}-exit-status"
    rm -f "${path}-tested-1-output" "${path}-tested-2-error" "${path}-tested-exit-status"
    rm -f "${path}-trace-log-1-output" "${path}-trace-log-2-error" "${path}-trace-log-exit-status"

    # now run in testing mode
    (
        # buffered IO but no trace or log writes
        export CSTOOLS_IOBUFFER_OUTPUT=iob_buffer
        export CSTOOLS_IOBUFFER_ERRORS=iob_buffer_why:iob_no_context
        export CSTOOLS_IOBUFFER_TRACES=iob_disable
        export CSTOOLS_IOBUFFER_LOGS=iob_disable

        cat "${input}" | ${xprogram} ${arguments} > "${path}-tested-1-output" 2> "${path}-tested-2-error"
        echo $? > "${path}"-tested-exit-status
    )

    # Test Results
    #testname="${testname#tests/${program}/}"
    program="${program} ${arguments}"
    echo -n " ${testname}${testname_padding:${#testname}} |"
    echo -n " ${program}${program_padding:${#program}} | "
    regenerate_expected "${path}" "${expect_output}" 1-output
    regenerate_expected "${path}" "${expect_error}" 2-error
    rm -f "${path}-tested-exit-status"
    echo " .  | ${description} "
}

# regenerate an individual test - we are passed an test index
regenerate_test()
{
    # lookup the required tests info
    t="${1}"
    program="${tests_program[$t]}"
    case "${program}" in
    HardwareSimulator.sh | CPUEmulator.sh)
        regenerate_hdl_test "${t}"
        ;;
    bin/testit)
        regenerate_bin_testit "${t}"
        ;;
    *)
        regenerate_filter_test "${t}"
        ;;
    esac
}

# regenerate all requested tests
regenerate_tests()
{
    # setup column padding ...
    setpadding "${longest_program}"
    program_padding="${padding}"
    setpadding "${longest_testname}"
    testname_padding="${padding}"
    setpadding "${longest_description}"
    description_padding="${padding}"

    # print the header
    echo -e "${colorboldblack} Test${testname_padding:4} | Program${program_padding:7} | Generated | Description${description_padding:11} ${undocolor}"

    # if no tests - just stop
    if [ ${#alltests[@]} -eq 0 ] ; then return ; fi

    # now run the tests
    for thetest in "${alltests[@]}" ; do
        regenerate_test "${thetest}"
    done
}

# show an individual test
filter_tests=X
show_hdl_test()
{
    # lookup the required tests info
    t="${1}"
    testname="${tests_id[$t]}"
    path="${tests_path[$t]}"
    program="${tests_program[$t]}"
    arguments="${tests_arguments[$t]}"
    shortd="${tests_shortd[$t]}"
    longd="${tests_longd[$t]}"

    colpad="                        "
    header=" Test: ${testname}"
    header="${header}${colpad:${#header}}${shortd}"
    echo -e "${colorboldblue} ${header}${pad:${#header}} ${undocolor}"
    if [ ${#longd} -gt 1 -a "x${longd}" != "x${shortd}" ]
    then
        # replace all \r and \n with \r\n to suit fmt
        header=" Detailed Description:"
        longd="${longd//\\r\\n/\\n}"
        longd="${longd//\\r/\\n}"
        longd="${longd//\\n/\r\n}"
        p="${#pad}"
        cp="${#colpad}"
        ((p=p-cp))
        longd=$(printf "${longd}" | fmt -w ${p} | sed "2,$$s/^/${colpad}/")
        echo "${header}${colpad:${#header}}${longd}"
    fi
    echo " Test command:          ${program} ${arguments}"
    show_hdl_test_result "${path}"
    echo -e "${colorboldblue} ${pad} ${undocolor}"
}

show_filter_test()
{
    # lookup the required tests info
    t="${1}"
    testname="${tests_id[$t]}"
    path="${tests_path[$t]}"
    program="${tests_program[$t]}"
    arguments="${tests_arguments[$t]}"
    shortd="${tests_shortd[$t]}"
    longd="${tests_longd[$t]}"

    # input filename
    input="${path}-0-input"
    ex_output="${path}-1-output"
    ex_error="${path}-2-error"
    ex_status="${path}-exit-status"
    tl_output="${path}-trace-log-1-output"
    tl_error="${path}-trace-log-2-error"
    tl_status="${path}-trace-log-exit-status"
    td_output="${path}-tested-1-output"
    td_error="${path}-tested-2-error"
    td_status="${path}-tested-exit-status"

    # if input file missing use /dev/null
    if [ ! -f "${input}" ] ; then input=/dev/null ; fi

    # is there an expected output?
    expect_output="${tests_output[$t]}"
    if [ "x${expect_output}" != "xno" ]
    then
        expect_output=yes
        if [ ! -f "${ex_output}" ] ; then ex_output="${colorboldred} file missing? ${undocolor}" ; fi
    else
        ex_output="not checked - no file"
    fi

    # is there an expected error output?
    expect_error="${tests_error[$t]}"
    if [ "x${expect_error}" != "xyes" -a "x${expect_error}" != "x1" ]
    then
        expect_error=no
        ex_error="not checked - no file"
    else
        if [ ! -f "${ex_error}" ] ; then ex_error="${colorboldred} file missing? ${undocolor}" ; fi
        expect_error="yes"
    fi

    # is there an expected exit status, if there is write it to the -exit-status file?
    expect_status="${tests_status[$t]}"
    case "x${expect_status}" in
        x1 | xyes)
            echo 0 > "${path}-exit-status"
            ex_status=0
            expect_status=1
            ;;
        x[0-9] | x[0-9][0-9] | x[0-2][0-9][0-9])
            ex_status=${expect_status}
            echo ${ex_status} > "${path}-exit-status"
            expect_status=1
            ;;
        *)
            ex_status="not checked"
            expect_status=0
            ;;
    esac

    colpad="                        "
    header="Test: ${testname}"
    header="${header}${colpad:${#header}}${shortd}"
    echo -e "${colorboldblue} ${header}${pad:${#header}} ${undocolor}"
    if [ ${#longd} -gt 1 -a "x${longd}" != "x${shortd}" ]
    then
        # replace all \r and \n with \r\n to suit fmt
        header=" Detailed Description:"
        longd="${longd//\\r\\n/\\n}"
        longd="${longd//\\r/\\n}"
        longd="${longd//\\n/\r\n}"
        p="${#pad}"
        cp="${#colpad}"
        ((p=p-cp))
        longd=$(printf "${longd}" | fmt -w ${p} | sed "2,$$s/^/${colpad}/")
        echo "${header}${colpad:${#header}}${longd}"
    fi

    if [ ${test_details} == "YES" ]
    then
        header="Program, Arguments and All Pathnames"
        echo -e "${colorboldblack} ${header}${pad:${#header}} ${undocolor}"
        echo " Program:               ${program}"
        if [ ${#arguments} -gt 1 ]
        then
            echo " Command line arguments:${arguments}"
        fi
        echo " Test Input:            ${input}"
        echo -e " Expected Output:       ${ex_output}"
        echo -e " Expected Error Output: ${ex_error}"
        echo " Expected Exit Status:  ${ex_status}"
        echo " Trace Log Output:      ${tl_output}"
        echo " Trace Log Error Output:${tl_error}"
        echo " Trace Log Exit Status: ${tl_status}"
        echo " Tested Output:         ${td_output}"
        echo " Tested Error Output:   ${td_error}"
        echo " Tested Exit Status:    ${td_status}"
    fi

    header="Last Test Results"
    echo -e "${colorboldblack} ${header}${pad:${#header}} ${undocolor}"
    show_filter_test_result "Output:"                "${path}" "${expect_output}" 1-output
    show_filter_test_result "Error Output:"          "${path}" "${expect_error}" 2-error
    show_filter_test_result "Exit Status:"           "${path}" "${expect_status}" exit-status
    echo -e "${colorboldblue} ${pad} ${undocolor}"
}

# select correct show_test function to use
show_test()
{
    # lookup the required tests info
    t="${1}"
    program="${tests_program[$t]}"
    case "${program}" in
    HardwareSimulator.sh | CPUEmulator.sh)
        show_hdl_test "${t}"
        ;;
    *)
        show_filter_test "${t}"
        ;;
    esac
}

# show all requested tests
show_tests()
{
    # if no tests - just stop
    if [ ${#alltests[@]} -eq 0 ]
    then
        echo -e "${colorboldred}No tests found${undocolor}"
        return
    fi


    # padding for longest path
    forty="1234567890123456789012345678901234567890"
    thirtytwo="12345678901234567890123456789012"
    setpadding "Trace Log Error Status: ${longest_path}" "Tested - Buffered Output, No Trace Writes and No Log Writes" "${forty}${thirtytwo}"
    pad="${padding}"

    # now show the tests
    filter_tests=X
    for thetest in "${alltests[@]}" ; do
        if [ "${run_show_tests}" == "YES" ] ; then run_test ${thetest} ; fi
        show_test ${thetest}
    done

    if [ "x${filter_tests}" == "xY" ] ; then
        header="Trace Log Environment Variables:"
        echo -e "${colorboldmagenta} ${header}${pad:${#header}} ${undocolor}"
        echo "export CSTOOLS_IOBUFFER_OUTPUT=iob_immediate"
        echo "export CSTOOLS_IOBUFFER_ERRORS=iob_immediate:iob_context"
        echo "export CSTOOLS_IOBUFFER_TRACES=iob_enable"
        echo "export CSTOOLS_IOBUFFER_LOGS=iob_enable"

        header="Tested Environment Variables:"
        echo -e "${colorboldmagenta} ${header}${pad:${#header}} ${undocolor}"
        echo "export CSTOOLS_IOBUFFER_OUTPUT=iob_buffer"
        echo "export CSTOOLS_IOBUFFER_ERRORS=iob_buffer_why:iob_no_context"
        echo "export CSTOOLS_IOBUFFER_TRACES=iob_disable"
        echo "export CSTOOLS_IOBUFFER_LOGS=iob_disable"
    fi
    echo
}

# run an individual hdl test
run_hdl_test()
{
    # lookup the required tests info
    local t="${1}"
    local testname="${tests_id[$t]}"
    local path="${tests_path[$t]}"
    local program="${tests_program[$t]}"
    local description="${tests_shortd[$t]}"
    local arguments="${tests_arguments[$t]}"

    # actual pathnames - must comment out compare line in the .tst files
    local base="${path#tests/}"
    local tst="${base}.tst"
    local cmp="${base}.cmp"
    local out="${base}.out"

    # first check that the arguments is a single test script that matches the path
    if [ ${#arguments} -eq 0 ] ; then echo exit_with_error "HDL/CPU test ${t} has empty arguments" ; fi

    # check we have the .tst script
    if [ ! -f "${tst}" ] ; then echo "HDL/CPU test ${t} cannot find ${tst}" ; exit -1 ; fi

    # check we have the .cmp file
    if [ ! -f "${cmp}" ] ; then echo "HDL/CPU test ${t} cannot find ${cmp}" ; exit -1 ; fi

    # remove .out if present, bad hdl may not produce output so previous output will be compared by mistake ??
    rm -f "${out}"

    # now run test
    (
        rm -f "${path}-trace-log-1-output" "${path}-trace-log-2-error" "${path}-trace-log-exit-status"
        rm -f "${path}-tested-1-output" "${path}-tested-2-error" "${path}-tested-exit-status"
        3>&2 2>&3- "${program}" "${tst}" > "${path}-tested-1-output" 2> "${path}-tested-2-error"
        echo $? > "${path}-tested-exit-status"
    )

    # Test Results
    program="${program} ${arguments}"
    echo -n " ${testname}${testname_padding:${#testname}} | ${program}${program_padding:${#program}} |"
    check_test_result=X
    touch "${out}"
    local status=$(cat "${path}-tested-exit-status")
    check_hdl_test "${cmp}" "${out}" "${status}"

    if [ "x${check_test_result}" == "xP" ]
    then
        if ((have_filters))
        then 
            echo -ne "${colorboldgreen}   P    |        |        | Test Passed ${undocolor}"
        fi
        echo -ne "${colorboldgreen} Test Passed ${undocolor}"
    elif [ "x${check_test_result}" == "xF" ]
    then
        if ((have_filters))
        then 
            echo -ne "${colorboldred}   F    |        |        | Test Failed ${undocolor}"
        else
            echo -ne "${colorboldred} Test Failed ${undocolor}"
        fi
    else
        if ((have_filters))
        then 
            echo -n "   ?    |        |        | Not Checked "
        else
            echo -n " Not Checked "
        fi
    fi
    echo "| ${description} "
}

# run an individual filter test
run_filter_test()
{
    # lookup the required tests info
    t="${1}"
    testname="${tests_id[$t]}"
    path="${tests_path[$t]}"
    program="${tests_program[$t]}"
    description="${tests_shortd[$t]}"
    arguments="${tests_arguments[$t]}"

    # is there an expected output?
    expect_output="${tests_output[$t]}"
    if [ "x${expect_output}" != "xno" ] ; then expect_output=yes ; fi

    # is there an expected error output?
    expect_error="${tests_error[$t]}"
    if [ "x${expect_error}" != "xyes" -a "x${expect_error}" != "x1" ] ; then expect_error="no" ; else expect_error="yes" ; fi

    # is there an expected exit status, if there is write it to the -exit-status file?
    expect_status="${tests_status[$t]}"
    case "x${expect_status}" in
        x1 | xyes)
            echo 0 > "${path}-exit-status"
            expect_status=1
            ;;
        x[0-9] | x[0-9][0-9] | x[0-2][0-9][0-9])
            echo ${expect_status} > "${path}-exit-status"
            expect_status=1
            ;;
        *)
            expect_status=0
            ;;
    esac

    # input filename, if file missing use /dev/null
    input="${path}-0-input"
    if [ ! -f "${input}" ] ; then input=/dev/null ; fi

    # fixup program to ./ or bin/ or bash scriptname.bash and check it exists
    if [ "${program%.bash}" != "${program}" ]
    then
        if [ ! -f "${program}" ] ; then echo "Cannot find bash script: ${program}" ; return ; fi
        xprogram="/bin/bash ${program}"

    # use VMEmulator.sh as is
    elif [ "${program}" == "VMEmulator.sh" ]
    then
        xprogram="${program}"

    # do not add prefix if it is already bin/.... or ./....
    elif [ "${program#bin/}" != "${program}" -o "${program#./}" != "${program}" ]
    then
        xprogram="${program}"
        if [ ! -x "${xprogram}" ] ; then echo "Cannot find executable: ${xprogram}" ; return ; fi
    else
        xprogram="${binprefix}${program}"
        if [ ! -x "${xprogram}" ] ; then echo "Cannot find executable: ${xprogram}" ; return ; fi
    fi

    fake_it=
    if [ "${xprogram}" == "bin/testit" -a "${working_tests}" == "YES" ]
    then
        fake_it=fake-it
    fi

    # clean up potential output files before starting the tests
    rm -f "${path}-tested-1-output" "${path}-tested-2-error" "${path}-tested-exit-status"
    rm -f "${path}-trace-log-1-output" "${path}-trace-log-2-error" "${path}-trace-log-exit-status"

    # now run in testing mode
    (
        # buffered IO but no trace or log writes
        export CSTOOLS_IOBUFFER_OUTPUT=iob_buffer
        export CSTOOLS_IOBUFFER_ERRORS=iob_buffer_why:iob_no_context
        export CSTOOLS_IOBUFFER_TRACES=iob_disable
        export CSTOOLS_IOBUFFER_LOGS=iob_disable

        cat "${input}" | ${xprogram} ${arguments} ${fake_it} > "${path}-tested-1-output" 2> "${path}-tested-2-error"
        echo $? > "${path}"-tested-exit-status
    )

    # now run as a normal user
    if [ "${working_tests}" != "YES" ]
    then
    (
        # immediate IO plus trace and log writes
        export CSTOOLS_IOBUFFER_OUTPUT=iob_immediate
        export CSTOOLS_IOBUFFER_ERRORS=iob_immediate:iob_context
        export CSTOOLS_IOBUFFER_TRACES=iob_enable
        export CSTOOLS_IOBUFFER_LOGS=iob_enable

        cat "${input}" | "${xprogram}" ${arguments} ${fake_it} > "${path}-trace-log-1-output" 2> "${path}-trace-log-2-error"
        echo $? > "${path}"-trace-log-exit-status
    )
    else
        cp "${path}-tested-1-output" "${path}-trace-log-1-output"
        cp "${path}-tested-2-error" "${path}-trace-log-2-error"
        cp "${path}"-tested-exit-status "${path}"-trace-log-exit-status
    fi

    # Test Results
    program="${program} ${arguments}"
    echo -n " ${testname}${testname_padding:${#testname}} | ${program}${program_padding:${#program}} |"
    check_test_result=X
    check_filter_test "${path}" "${expect_output}" 1-output
    echo -n "|"
    check_filter_test "${path}" "${expect_error}" 2-error
    echo -n "|"
    check_filter_test "${path}" "${expect_status}" exit-status
    echo -n "|"

    if [ "x${check_test_result}" == "xP" ]
    then
        echo -ne "${colorboldgreen} Test Passed ${undocolor}"
    elif [ "x${check_test_result}" == "xF" ]
    then
        echo -ne "${colorboldred} Test Failed ${undocolor}"
    else
        echo -n " Not Checked "
    fi
    echo "| ${description} "

    # if live output, rerun and display all output directly to the user
    if [ "x${live_output}" == "xYES" ] ; then
    (
        # immediate IO plus trace and log writes
        export CSTOOLS_IOBUFFER_OUTPUT=iob_immediate
        export CSTOOLS_IOBUFFER_ERRORS=iob_immediate:iob_context
        export CSTOOLS_IOBUFFER_TRACES=iob_enable
        export CSTOOLS_IOBUFFER_LOGS=iob_enable

        cat "${input}" | "${xprogram}" ${arguments}
    )
    fi
}

# select correct run_test function to use
run_test()
{
    # lookup the required tests info
    t="${1}"
    program="${tests_program[$t]}"
    case "${program}" in
    HardwareSimulator.sh | CPUEmulator.sh)
        run_hdl_test "${t}"
        ;;
    *)
        run_filter_test "${t}"
        ;;
    esac
}

# run all requested tests
run_tests()
{
    # setup column padding ...
    setpadding "${longest_program}"
    program_padding="${padding}"
    setpadding "${longest_testname}"
    testname_padding="${padding}"
    setpadding "${longest_description}"
    description_padding="${padding}"

    # if no tests - just stop
    if [ ${#alltests[@]} -eq 0 ]
    then
        echo -e "${colorboldred} No tests found ${undocolor}"
        return
    fi

    # only run live output if a single test is selectd
    if [ "x${live_output}" == "xYES" -a ${#alltests[@]} -ne 1 ]
    then
        echo -e "${colorboldgreen} Live output disabled: you must provide the name of a single test ${undocolor}"
        live_output="NO"
    fi

    # check if we have any filter programs
    have_filters=0
    for kind in "${tests_kind[@]}" ; do
        case "${kind}" in
            cat | filter | jack | vm-asm)
                have_filters=1
                ;;
                *)
                ;;
        esac
    done
    # print the header
    echo -ne "${colorboldblack} Test${testname_padding:4} | Program${program_padding:7} |"
    if ((have_filters)) ; then echo -n " Output | Errors | Status |" ; fi
    echo -e " Test Result | Description${description_padding:11} ${undocolor}"

    # now run the tests
    for thetest in "${alltests[@]}" ; do
        run_test ${thetest}
    done
}

# check for controls, live, regenerate, show, test or working
working_tests="NO"
less_tests="NO"
live_output="NO"
regenerate="NO"
show_tests="NO"
run_show_tests="NO"
test_details="NO"
binprefix=./

# update controls based on first argument
case $1 in
less)
    # show tests details - do not run any tests
    less_tests="YES"
    show_tests="YES"
    shift 1
	;;
Less)
    # show tests details - do not run any tests
    less_tests="YES"
    show_tests="YES"
    test_details="YES"
    shift 1
	;;
live)
    # run the student versions of the programs
    # if only one test is selected, also output the program output directly
    live_output="YES"
    shift 1
	;;
regenerate)
    # regenerate the expected test output, error output and exit status for all known tests
    binprefix=bin/
    regenerate="ALL"
    shift 1
	;;
show)
    # show tests summary - do not run any tests
    show_tests="YES"
    shift 1
	;;
Show)
    # show tests details - do not run any tests
    show_tests="YES"
    test_details="YES"
    shift 1
	;;
test)
    # run the student versions of the programs
    shift 1
	;;
Test)
    # run the student versions of the programs
    less_tests="YES"
    show_tests="YES"
    run_show_tests="YES"
    test_details="YES"
    shift 1
	;;
working)
    # run the working versions of the programs - a sanity check?
    working_tests="YES"
    binprefix=bin/
    shift 1
	;;
*)
    # run the student versions of the programs
	;;
esac

# enforce 10s time limit so we do not get stuck
ulimit -t 10

# parse the list of known tests
parse_test_lists $(find tests -type f -a -name 'tests*.csv' | sort)
if [ -d websub-tests -a "${regenerate}" == "ALL" ] ; then
    parse_test_lists $(find websub-tests -type f -a -name 'tests*.csv' | sort)
fi

# we either regenerate all tests, or selectively run them
if [ "${show_tests}" == "YES" ] ; then
    find_tests $@
    if [ "${less_tests}" == "YES" ] ; then
        show_tests | less -R
    else
        show_tests
    fi
else
    if [ "${regenerate}" == "ALL" ] ; then
        find_tests
        regenerate_tests
    else
        find_tests $@
        run_tests
    fi
fi
