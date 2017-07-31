# Datacenter Transport and Flow Completion Metrics

This repository contains code to reproduce simulation in the INFOCOM 2018 submission "Datacenter Transport and Flow Completion Metrics".

Simulation are based on code of YAPS simulator(https://github.com/NetSys/simulator)

## Run and compile: 

`CXX=g++-5 cmake . -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-std=c++14"` then `make`. To simulate all algorithms copy target `simulator` to `runner/` and run `python run_experiments.py`.


## Parameters modification:
To run different experiments modify different params in `run_expriments.py`:

* `big_switch: 0 ? 1` - pfabric-topology or big-switch topology
* `permutation_tm: 0 ? 1` - if `permutation_tm` equals zero thеn each host sends all flows to exactly one another host, otherwise each host send flows to different hosts.
* `flow_trace` - filename for CDF on flow sizes.


Range for `comr` corresponds to variation of compression and range for `split_mode` corresponds to variation of flow intensity. 

## Experiment details:

In experiments with variation of compression we fix `split_mode = 5` and select range for `comr = [1, 10]`. 

In experiments with variation of intensity we fix `comr = 2` and select range for `split_mode = [1, 10]`.

In exepriments with `flow_trace : CDF_dctcp.txt` we set `comr_scale = 0.02`.
 

