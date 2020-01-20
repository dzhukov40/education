import sys
import queue as Q


# IN
dataCount = sys.stdin.readline()
data = sys.stdin.readline().split()


# order
index = 0
q = Q.PriorityQueue()
for value in data:
    q.put((value, index))
    index += 1


# OUT
while not q.empty():
    print(str(q.get()[1]), end =" ")

