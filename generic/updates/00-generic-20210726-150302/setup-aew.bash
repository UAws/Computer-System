#!/bin/bash

# creates / updates all expected assignment and workshop svn directories

# The Plan:
# . if generic Makefile is run as updates/Makefile it is in the generic assignment
#   . after running the installer.bash and updater.bash scripts
#   . just before it exits
# . this script creates any missing assignments / workshops, AEW
# . if an AEW is missing
#   . its directory and updates directory are both created
#   . an empty updates/log file is created
# . for all AEW's that can be found
#   . Makefile is copied to the AEW's directory if different
#   . generic/installer.bash and generic/updates.bash are copied to the AEW's updates directory if different

# do nothing if not run from a directory named generic
me=$(basename `pwd`)
if [ "x${me}" != "xgeneric" ] ; then exit 0 ; fi

# setup colours for interative output
# if using an xterm - use headings and colour coded test results
if [ "${TERM:0:5}" == "xterm" -o "${TERM:0:2}" == "vt" ] ; then
    colorboldblack="\x1b[1;7;30m"
    colorboldgreen="\x1b[1;7;32m"
    colorboldred="\x1b[1;7;31m"
    colorboldmagenta="\x1b[1;7;35m"
    undocolor="\x1b[0m"
else
    colorboldblack=
    colorboldgreen=
    colorboldred=
    colorboldmagenta=
    undocolor=
fi

echo -e "${colorboldmagenta}  Checking Assignments / Exams and Workshops ...                                ${undocolor}"
echo -e "${colorboldmagenta}                                                                                ${undocolor}"

aew_displayed="no"
aew_padding="                                                                                "
aew_title=""
init_aew_banner()
{
    aew_displayed="no"
    aew_title="  Checking ${1}"
}

display_aew_banner()
{
    if [ "${aew_displayed}" == "no" ]
    then
        aew_displayed="yes"
        echo -e "${colorboldmagenta}${aew_title}${aew_padding:${#aew_title}}${undocolor}"
    fi
    local message="  ${1}"
    #echo -e "${colorboldmagenta}${message}${aew_padding:${#message}}${undocolor}"
    echo "${message}"
}

# create file if it does not exist or update if changed
create_or_update_file()
{
    src="${1}"
    dst="${2}"
    updated_file=

    # create file if it does not exist
    if [ ! -f "${dst}" ]
    then
        display_aew_banner "Creating: ${dst}"
        cp -p "${src}" "${dst}"
        chmod 600 "${dst}"
        updated_file="${dst}"
        return
    fi

    # if file is out of date, update it
    if ! cmp "${src}" "${dst}" > /dev/null 2> /dev/null ;
    then
        display_aew_banner "Updating: ${dst}"
        cp "${src}" "${dst}"
        updated_file="${dst}"
    fi
}

# create an AEW or update the critial files - no svn operations attempted
create_or_update_AEW()
{
    # the AEW should be in the parent directory
    init_aew_banner "${1}"
    local aew="../${1}"

    if [ ! -d "${aew}" -o ! -d "${aew}/updates" ]
    then
        # create the directories just in case
        display_aew_banner "Creating directories"
        mkdir -p "${aew}/updates"
    fi
    chmod 700 "${aew}" "${aew}/updates"

    # copy over initial files
    create_or_update_file Makefile "${aew}/Makefile"
    create_or_update_file generic/installer.bash "${aew}/updates/installer.bash"
    create_or_update_file generic/updater.bash "${aew}/updates/updater.bash"
    touch "${aew}/updates/log"

    # fix permissions
    chmod 600 "${aew}/Makefile" "${aew}/updates/installer.bash" "${aew}/updates/updater.bash" "${aew}/updates/log"
}

# step 1 update installer.bash and updater.bash
create_or_update_file generic/installer.bash updates/installer.bash
create_or_update_file generic/updater.bash updates/updater.bash

# Step 2 - make sure generic is using an up to date Makefile
if [ -f updates/Makefile ]
then
    create_or_update_file updates/Makefile Makefile
else
    updated_file=
fi

# if we just updated the generic Makefile - hangup
if [ "${#updated_file}" -gt 0 ]
then
    echo "The generic Makefile may have been modified - sending self a hangup signal."
    echo "Ignore the \"... Hangup: 1\" error lines, then run make again."
    kill -1 0
    exit 0
fi

# stop if this directory appears to be the top level of an svn working copy
if [ -d .svn ]
then
    echo -e "${colorboldmagenta}                                                                                ${undocolor}"
    echo -e "${colorboldmagenta}  The parent directory has not been checked out, stopping ...                   ${undocolor}"
    echo -e "${colorboldmagenta}                                                                                ${undocolor}"
    exit 0
fi

# create / update assignments
for i in 1 2 3 ; do create_or_update_AEW assignment${i} ; done

# create / update workshops
for i in 01 02 03 04 05 06 07 08 09 10 11 12 ; do create_or_update_AEW workshop${i} ; done

# create / update exams
create_or_update_AEW exam-demo
create_or_update_AEW exam-raa

echo -e "${colorboldmagenta}                                                                                ${undocolor}"
exit 0
