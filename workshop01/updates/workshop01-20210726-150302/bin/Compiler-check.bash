#!/bin/bash

# go through preferred list of C++ compilers and eventually output path or NOTFOUND
# preferences currently:
# clang++ 9*, 11*, 10*, 8*, 7*, 6*
# g++ 8*, 10*, 9*, 7*

# what kind of system?
System=`test -f /usr/bin/uname && /usr/bin/uname -s`
if [ "${#System}" -eq 0 ] ; then System=Linux ; fi

# function to check C++ compiler and stop if acceptable
check_compiler()
{
    compiler="${1}"
    #echo "Checking ${compiler}"
    compiler=`/usr/bin/which "${compiler}" 2> /dev/null`

    # giveup if which failed, result is "" or it is no executable
    if [ "$?" -ne 0 -o ${#compiler} -eq 0 -o ! -x "${compiler}" ] ; then return ; fi

    # extract major version number
    version=`"${compiler}" --version | head -1 | cut -f "${version_field}" -d ' '| cut -f 1,1 -d '-'| cut -f 1 -d '.'`
    #echo "version: ${version}"

    min_version="${2}"
    if [ "${#version}" -gt 0 -a "${#min_version}" -gt 0 ]
    then
        #echo "minimum version: ${min_version}"
        if [ "${version}" -lt "${min_version}" ] ; then return ; fi
    fi
    echo "${compiler}"
    exit 0
}

# look for C++ compiler in preferred order ...
if [ "${System}" == "Darwin" ]
then
    version_field=4
    # On a Mac just check clang++ min version is 10.0.0, prefer 11.0.3
    # 11.0.3 => clang 9.0.0
    # 11.0.0 => clang 8.0.0
    # 10.0.1 => clang 7.0.0
    # 10.0.0 => clang 6.0.0
    check_compiler `/usr/bin/which clang++ 2> /dev/null` 10
else
    version_field=3
    # On our RHEL7 systems clang++-9 is in /opt/rh/llvm-toolset-9.0/root/usr/bin/clang++-9
    check_compiler /opt/rh/llvm-toolset-9.0/root/usr/bin/clang++-9 9
    check_compiler clang++-9  9
    check_compiler clang++  9
    check_compiler clang++-11  11
    check_compiler clang++-10  10
    check_compiler clang++-8  8
    check_compiler clang++  8
    check_compiler clang++-7  7
    check_compiler clang++  7
    check_compiler clang++-6  6
    check_compiler clang++  6

    # On our RHEL7 systems g++-8 is in /opt/rh/devtoolset-8/root/usr/bin/g++-8
    check_compiler /opt/rh/devtoolset-8/root/usr/bin/g++-8 8
    check_compiler g++-8  8
    check_compiler g++  8
    check_compiler g++-10  10
    check_compiler g++-9  9
    check_compiler g++-7  7
    check_compiler g++  7
fi
