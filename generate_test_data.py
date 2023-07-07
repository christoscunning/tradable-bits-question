import random
import uuid
import sys

NUM_LINES = 1_000

if (len(sys.argv) == 2):
    NUM_LINES = int(sys.argv[1])

# file to write test data
fileName = "./testing/INPUT_test_data_" + str(NUM_LINES) + ".txt"

lines = []
numDupes = 0
dupes = []

# write a uuid to each line, with a chance to write the uuid to two lines
# for now limit duplicates to max two instances of the same line
line = ""
prevLine = ""
for i in range(NUM_LINES):
    # 20% chance to write a duplicate line
    if prevLine != "" and random.random() <= 0.2:
        line = prevLine
        numDupes += 1
        dupes.append(line)
        prevLine = ""
    else:
        line = str(uuid.uuid4()) + "\n"
        prevLine = line
    # save line
    lines.append(line)
    

# write lines to file in random order
r = list(range(NUM_LINES))
random.shuffle(r)

# write lines to file
file = open(fileName, "w")
for i in r:
    file.write(lines[i])
file.close()


# print number of duplicate lines
print("Num dupes: " + str(numDupes))

# Create solution file
dupes.sort()
file = open("./testing/SOLN_test_data_" + str(NUM_LINES) + ".txt", "w")

# write number of duplicates to first line of solution file
#file.write(str(numDupes) + "\n\n")

# write duplicate lines to solution file
for d in dupes:
    file.write(d)
file.close()