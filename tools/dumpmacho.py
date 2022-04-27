#!/usr/bin/env python3
# Garbage fire script to get the exports from the engine so that it can be dynamically loaded on macOS

import os
import subprocess
import sys

if len(sys.argv) < 2:
    print("Missing dylib name and output file")
    exit(1)
elif len(sys.argv) < 3:
    print("Missing output file")
    exit(1)

if not os.path.exists(sys.argv[1]):
    print(f"File \"{sys.argv[1]}\" doesn't exist, exiting")
    exit(1)

print(f"Writing exports from {sys.argv[1]} to {sys.argv[2]}")

try:
    nm = subprocess.run(["nm", "-g", sys.argv[1]], capture_output=True, check=True, encoding="utf-8")
except:
    print(f"Failed to run nm, exiting")
    exit(1)

out = nm.stdout

names = []
for x in out.split("\n"):
    # The columns are padded so, this always works if nm ran correctly
    name = x[19:]
    end = name.find(" ")
    end = len(name) if end == -1 else end
    name = name[:end]
    if len(x.split()) and x.split()[1] == "T":
        print(name)
        names.append(name)

f = open(sys.argv[2], "wb+")
f.write(bytes(f"#ifdef __cplusplus\n#define EXTERN_C extern \"C\"\n#else // __cplusplus\n#define EXTERN_C\n#endif // __cplusplus\n\n", encoding="utf-8"))
for name in names:
    f.write(bytes(f"EXTERN_C uint64_t (*{name})(...) = (uint64_t (*)(...))0;\n", encoding="utf-8"))
    f.write(bytes(f"EXTERN_C uint64_t {name[1:]}(...) {{ return {name}(); }}\n", encoding="utf-8"))

so_name = os.path.basename(sys.argv[1]).replace("-", "_").replace(".", "_")
f.write(bytes(f"\nvoid init_{so_name}_ptrs(void *so)\n{{\n", encoding="utf-8"))

for name in names:
    f.write(bytes(f"\t*(void **)(&{name}) = dlsym(so, \"{name[1:]}\");\n", encoding="utf-8"))

f.write(bytes("}\n", encoding="utf-8"))
f.close()

print(f"Done. Include {sys.argv[2]} and call init_{so_name}_ptrs to use the library once it's loaded.")

