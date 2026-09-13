# MinGW

MinGW-specific generated CMake files belong in this directory. Runtime
artifacts belong in `../build/mingw`.

Configure from the repository root:

```powershell
cmake --preset mingw
cmake --build --preset mingw-release --clean-first
```

Executables are written to `build/mingw/release`.
Dynamic libraries are written to `build/mingw/lib`.
Import and static libraries are written to `build/mingw/archive`.