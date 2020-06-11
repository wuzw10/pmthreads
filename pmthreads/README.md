##PMThreads: Persistent Memory Threads Harnessing Versioned Shadow Copies


## Documentation
Please check our PLDI'20 [paper](https://www.research.manchester.ac.uk/portal/files/162076150/PMThreads_PLDI2020_authorversion.pdf). 

## Building PMThreads

`PMTHREADS_HOME=$(pwd)`. 

```
cd $PMTHREADS_HOME/src/compiler-plugin 
./build_plugin 
cd $PMTHREADS_HOME/src 
./build.sh pmthreads 		#build pmthreads-i
./build.sh pmthreads-mpt   #build pmthreads-p
```

## Using PMThreads 

### Setup NVM emulator 
```
mkdir -p /dev/shm/pmthreads
export NVM_LATENCY="100"	#ns
export CPU_FREQ_MHZ="2000" #MHZ
export PERSIST_INTERVAL="100" #ms 
```
### Using PMThreads-I 
`clang++ -Xclang -load -Xclang $PMTHREADS_HOME/src/compiler-plugin/plugin_build/libPMThreadsPass.so -c target.cpp -o target.o`
`clang++ target.o -rdynamic $PMTHREADS_HOME/libpmthreads.so -ldl -o target`  

### Using PMThreads-P 
`clang++ -c target.cpp -o target.o`
`clang++ target.o -rdynamic $PMTHREADS_HOME/src/libpmthreads-mpt.so -ldl -o target`

## Citing PMThreads 
If you use PMThreads, please cite this paper: 
 
```
@inproceedings{10.1145/3385412.3386000,
	author = {Wu, Zhenwei and Lu, Kai and Nisbet, Andrew and Zhang, Wenzhe and Luj\'{a}n, Mikel},
	title = {PMThreads: Persistent Memory Threads Harnessing Versioned Shadow Copies},
	year = {2020},
	isbn = {9781450376136},
	publisher = {Association for Computing Machinery},
	address = {New York, NY, USA},
	url = {https://doi.org/10.1145/3385412.3386000},
	doi = {10.1145/3385412.3386000},
	booktitle = {Proceedings of the 41st ACM SIGPLAN Conference on Programming Language Design and Implementation},
	pages = {623–637},
	numpages = {15},
	keywords = {non-volatile memory, parallel programs, memory persistence},
	location = {London, UK},
	series = {PLDI 2020}
}
```
