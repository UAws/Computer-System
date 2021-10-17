from pathlib import Path
import os
import subprocess
from threading import Lock

# https://gist.github.com/Querela/77b7506b2ce735416e4d77c3bfe954df
###############################################################################
# shell command execution

def run_command(command):
    # run command (can be an array (for parameters))
    p = subprocess.Popen(command, shell=True, \
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    # capture output and error
    (output, err) = p.communicate()
    # wait for command to end
    # TODO: really long running?
    status = p.wait()

    # decode output from byte string
    if output is not None:
        output = output.decode('utf-8')
    if err is not None:
        err = err.decode('utf-8')
    # return stdout, stderr, status code
    return (output, err, status)

def execute(command):
    debug('run command:', command)
    out, err, status = run_command(command)
    debug('-> exit status:', status)
    debug('output:', out)
    debug('error:', err)
    return (out, err, status)


###############################################################################
# logging

log_lock = Lock()

def log(level, msg, *args, **kwargs):
    log_lock.acquire(True)
    print('[' + level + ']', msg, end=' ')
    for arg in args:
        print(arg, end=' ')
    for k, w in kwargs:
        print('[', k, '=', w, ']', end=' ')
    print()
    log_lock.release()

def info(msg, *args, **kwargs):
    log('INFO   ', msg, *args, **kwargs)

def verbose(msg, *args, **kwargs):
    log('VERBOSE', msg, *args, **kwargs)

def debug(msg, *args, **kwargs):
    log('DEBUG  ', msg, *args, **kwargs)

def error(msg, *args, **kwargs):
    log('ERROR  ', msg, *args, **kwargs)

def warning(msg, *args, **kwargs):
    log('WARNING', msg, *args, **kwargs)

    


inputFiles = [];

cur_dir = os.getcwd() # Dir from where search 
# test_dir = cur_dir + "/tests/0parser"
test_dir = cur_dir + "/local_test/optimiser_evaluate/"

count = 0;
errors = 0;

execute("make notest");

for path in Path(test_dir).rglob("*jack"):
    inputFiles.append(str(path))
    
for path in inputFiles:

    count += 1
    execute("rm -rf output1.xml")
    execute("rm -rf output2.xml")

    # executive own written 
    execute("cat " + path + "| ./parser | ./optimiser-r  &> output1.xml")
    # # executive provide executable 
    execute("cat " + path + "| ./lib/macos/bin/parser | ./lib/macos/bin/optimiser-r   &> output2.xml")

    out, err, status = execute("diff -u output1.xml output2.xml | diff-so-fancy")

    if err: 
        errors += 1
    # if there have different between previous compare
    if out:
        print("STOP STOP STOP")
        break

print("count = ",count, "; input size:", len(inputFiles), "; errors :",errors)

