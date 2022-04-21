# Garbage fire script to get the exports from the engine so that it can be dynamically loaded on Windows

import os
import subprocess
import sys

if len(sys.argv) < 2:
    print("Missing DLL name and output file")
    exit(1)
elif len(sys.argv) < 3:
    print("Missing output file")
    exit(1)

if not os.path.exists(sys.argv[1]):
    print(f"File \"{sys.argv[1]}\" doesn't exist, exiting")
    exit(1)

print(f"Writing exports from {sys.argv[1]} to {sys.argv[2]}")

try:
    dumpbin = subprocess.run(["dumpbin", "/exports", sys.argv[1]], capture_output=True, check=True, encoding="utf-8")
except:
    print("""
Failed to run dumpbin. Make sure to run this from a Visual Studio Developer Command Prompt.
The best way to do this is to use the buildenv.bat script in the tools directory from an
existing Command Prompt, or run buildcmd.bat to open one for you.
          """)
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
f.write(bytes(f"#ifdef __cplusplus\n#define EXTERN_C extern \"C\"\n#else // __cplusplus\n#define EXTERN_C\n#endif // __cplusplus\n\n", encoding="utf-8"))
for name in names:
    f.write(bytes(f"EXTERN_C void (*__imp_{name})(void) = (void (*)(void))0;\n", encoding="utf-8"))

dll_name = os.path.basename(sys.argv[1]).replace("-", "_").replace(".", "_")
f.write(bytes(f"\nEXTERN_C void init_{dll_name}_ptrs(HMODULE dll)\n", encoding="utf-8"))
f.write(bytes("{\n", encoding="utf-8"))

for name in names:
    f.write(bytes(f"\t*(void **)(&__imp_{name}) = GetProcAddress(dll, \"{name}\");\n", encoding="utf-8"))

f.write(bytes("}\n", encoding="utf-8"))
f.close()

print(f"Done. Include {sys.argv[2]} and call init_{dll_name}_ptrs to use the library once it's loaded.")

