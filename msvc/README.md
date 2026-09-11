# MSVC

MSVC-specific instructions belong in this directory. Generated Visual Studio
projects and build artifacts belong in `../build/msvc`.

Configure from the repository root. The Visual Studio project files are generated
directly in this directory:

```powershell
cmake -S . -B msvc -G "Visual Studio 18 2026" -A x64
cmake --build msvc --config Release
```

Executables and DLLs are written to `build/msvc/release`.
Import libraries are written to `build/msvc/archive`.
Static libraries are written to `build/msvc/lib`.