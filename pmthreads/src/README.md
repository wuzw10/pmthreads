## Build Instructions 


### *config* options:
|Config|Persistent data allocation interface|Memory tracking|
|----|----|----|
|pmthreads|nvmalloc|compiler instrumentation|
|pmthreads-mpt|nvmalloc|page protection|
|pmthreads-malloc|malloc|compiler instrumentation|
|pmthreads-mpt-malloc|malloc|page protection|
|pmthreads-zero|-|-|
|pmthreads-verbose|malloc|compiler instrumentation|
```
python build.py config
```
### Compiler instrumentation plugin  
PMThreads's compiler instrumentation plugin is modified from that of [Atlas](https://github.com/HewlettPackard/Atlas). 
Please change to the compiler-plugin subdirector to find instructions for building the compiler instrumentation plugin.  
