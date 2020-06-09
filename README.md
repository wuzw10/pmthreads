## pmthreads-ae 
Artifact evaluation for [PMThreads](https://www.research.manchester.ac.uk/portal/files/162076150/PMThreads_PLDI2020_authorversion.pdf). 

## Setups

### Set up the docker container. 
Run `./install-docker-engine.sh` (will need root privilege) to get Docker Engine. This install instructions are derived from [Get Docker Engine](https://docs.docker.com/install/linux/docker-ce/ubuntu/). 

Run `./kickoff.sh` (will need root privilege) to assemble the docker image, then derive a docker container from it. 

### Build the benchmarks. 
In the bash of the docker container, execute `cd /root/pmthreads` to change to the top directory of the artifacts then perform `./init.sh` to build the runtime libraries, benchmarks, download workloads, etc. 

## Evaluations

### Run the benchmarks.
Execute `./run.sh` to run all the benchmarks. 

Alternatively, one could change to the `scripts/benchname` directory to run benchmarks one-by-one. For example, `cd scripts/fig4a`, then `./run.sh`. 

The `geomeans` appeared in `Table-1` will be reproduced by the console output of `fig4a/run.sh` and `fig4b/run.sh`. 

Figures of `Table-2` will be reproduced by the console output of `fig8/run.sh`. 

## Citing PMThreads 
If you use PMThreads, please cite this paper: 
 
```
@inproceedings{pmthreads2020,
	author = {Wu,Zhenwei and Lu, Kai and Nisbet, Andrew and Zhang, Wenzhe and Luj{\'a}n, Mikel},
	title = {{PMThreads}: Persistent Memory Threads Harnessing Versioned Shadow Copies},
	year = {2020},
	publisher = {ACM},
	address = {New York, NY, USA},
	url = {https://doi.org/10.1145/3385412.3386000},
	doi = {10.1145/3385412.3386000},
	booktitle = {Proceedings of the 41st ACM SIGPLAN International Conference on Programming Language Design and Implementation}, 
	series = {PLDI 2020}
}
```