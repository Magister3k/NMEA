# MinGW

MinGW project files are generated in this directory. Build outputs are written
to `../build/mingw`.

Configure from the repository root:

```powershell
cmake -S . -B mingw -G "MinGW Makefiles"
cmake --build mingw --config Release
```

Executables and DLLs are written to `build/mingw/release`.
Import libraries are written to `build/mingw/archive`.
Static libraries are written to `build/mingw/lib`.