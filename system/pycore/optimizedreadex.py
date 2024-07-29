import collections
import sys

counts = collections.Counter()
remaining = ""
while True:
    chunk = remaining + sys.stdin.read(64 * 1024)
    if not chunk:
        break
    last_if = chunk.rfind("\n")
    if last_if == -1:
        remaining = ""
    else:
        remaining = chunk[last_if + 1 :]
        chunk = chunk[:last_if]
    counts.update(chunk.lower().split())

for word, count in counts.most_common():
    print(word, count)
