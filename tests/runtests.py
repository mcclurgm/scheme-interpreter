#!/usr/bin/env python3
import os
import re
import subprocess
import filecmp
import pprint

allTestsPass = True
for filename in sorted(os.listdir()):
    if filename.endswith('.rkt'):
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
