# Garbage fire script to get the exports from the engine so that it can be dynamically loaded

import os
import subprocess
import sys

if len(sys.argv) < 2:
    print("Missing DLL name and output file")
    exit(1)
elif len(sys.argv) < 3:
    print("Missing output file")
    exit(1)

print(f"Writing exports from {sys.argv[1]} to {sys.argv[2]}")

try:
    dumpbin = subprocess.run(f"dumpbin /exports {sys.argv[1]}", capture_output=True, encoding="utf-8")
except:
    print("Failed to run dumpbin. Make sure to run this from a Visual Studio Developer Command Prompt")
    exit(1)

start_str = "ordinal hint RVA      name"
start = dumpbin.stdout.find(start_str) + len(start_str) + 1
end = dumpbin.stdout.find("Summary")
out = dumpbin.stdout[start:end]

names = []
for x in out.split("\n"):
    # The columns are padded so that the names always start at position 26
    name = x[26:]
    end = name.find(" ")
    end = len(name) if end == -1 else end
    name = name[:end]
    if len(x.split()):
        print(name)
        names.append(name)

f = open(sys.argv[2], "wb+")
for name in names:
    f.write(bytes(f"void (*__imp_{name})(void) = (void *)0;\n", encoding="utf-8"))

f.write(bytes("\nvoid init_ptrs(void)\n{\n", encoding="utf-8"))

for name in names:
    f.write(bytes(f"\t__imp_{name} = (void *)GetProcAddress(engine_dll, \"{name}\");\n", encoding="utf-8"))

f.write(bytes("}\n", encoding="utf-8"))
f.close()

print("Done")

