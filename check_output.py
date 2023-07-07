import sys

NUM_LINES = 30000000

if (len(sys.argv) > 1):
    NUM_LINES = sys.argv[1]
    
print("Testing output vs solution file for NUM_LINES = " + str(NUM_LINES))

basePath = "./testing/"

outputFileName = basePath + "OUTPUT_test_data_" + str(NUM_LINES) + ".txt"
solutionFileName = basePath + "SOLN_test_data_" + str(NUM_LINES) + ".txt"

output = open(outputFileName, "r")
soln = open(solutionFileName, "r")


match = True
for outputLine in output:
    solnLine = soln.readline()
    if outputLine != solnLine:
        print(outputLine)
        print(solnLine)
        match = False
        break
        
if match:
    print("Test passed: files match")
else:
    print("Test failed: files do not match")


output.close()
soln.close()