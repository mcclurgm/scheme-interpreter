#!/usr/bin/env python3
import os
import re
import subprocess
import filecmp
import pprint

# Recompile source, so I don't have to redo this every time
subprocess.run(['make', '-C', '..'])

# Try clang scan-build, just for fun.
clangresult = subprocess.run(['scan-build', 'make', '-C', '..'],
                             stdout=subprocess.PIPE, stderr=subprocess.PIPE)
if "No bugs found." not in clangresult.stdout.decode():
    print("Clang static analyzer thinks it found a bug.")
    print(clangresult.stdout.decode())

allTestsPass = True
for filename in sorted(os.listdir()):
    if filename.endswith('.rkt') and filename.startswith('test-in-'):
        # Get files
        m = re.match('test-in-(.*).rkt', filename)
        expectedOutput = 'test-out-'+m.group(1)+'.txt'
        sourcefile = open(filename)

        # Check for correct output
        result = subprocess.run('./valgrind.sh',stdin=open(filename),
                                stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        output = result.stdout
        valgrind_output = result.stderr.decode('utf-8')

        # Check output
        with open(expectedOutput,"rb") as expOut:
            expResults = expOut.read()
        with open(expectedOutput,"r") as expOut:
            expResultsPretty = expOut.read()
        testPassesCorrectness = result.stdout==expResults
        testPassesMemory = "ERROR SUMMARY: 0 errors from 0 contexts" in valgrind_output

        testPasses = testPassesCorrectness and testPassesMemory
        allTestsPass = allTestsPass and testPasses
        print("Input:",filename,"Expected output:",expectedOutput,
              "Success:",testPasses)
        if not testPassesCorrectness:
            print("Correctness error.")
            print("Actual output in bytes:")
            print(output)
            print("Expected output in bytes:")
            print(expResults)
            print()
            print("Actual output:")
            print(output.decode())
            print("Expected output:")
            print(expResultsPretty)
        if not testPassesMemory:
            print("Valgrind error.")
            print("Output:")
            print(valgrind_output)
        sourcefile.close()

if allTestsPass:
    print('All tests passed!')
else:
    print('At least one test failed.')
