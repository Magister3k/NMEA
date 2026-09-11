# MSVC

MSVC-specific instructions belong in this directory. Generated Visual Studio
projects and build artifacts belong in `../build/msvc`.

Configure from the repository root:

```powershell
cmake -S . -B build/msvc -G "Visual Studio 18 2026" -A x86
cmake --build build/msvc --config Release
```

Executables and DLLs are written to `build/msvc/release`.
Import libraries are written to `build/msvc/archive`.
Static libraries are written to `build/msvc/lib`.