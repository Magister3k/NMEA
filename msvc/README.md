# MSVC

MSVC-specific instructions belong in this directory. Generated Visual Studio
projects and build artifacts belong in `../build/msvc`.

Configure from the repository root:

```powershell
cmake --preset msvc
cmake --build --preset msvc-release --clean-first
```

Executables are written to `build/msvc/release`.
Dynamic libraries are written to `build/msvc/lib`.
Import and static libraries are written to `build/msvc/archive`.