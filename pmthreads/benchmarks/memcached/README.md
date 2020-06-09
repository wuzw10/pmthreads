
### 1. Build and install memtier\_benchmark 

Clone and install memtier\_benchmark. 

```
git clone https://github.com/RedisLabs/memtier_benchmark.git
cd ./memtier_benchmark 
autoreconf -ivf
./configure
make
make install
```


### 2. Compile benchmarks

```
cd ./memcached-1.2.4 
./build.sh 
```

### 3. Run benchmarks

```
cd dir #(./figure-7, ./figure-8, ./figure-9, ./table-2, e.g.)  
./run.sh 
```
