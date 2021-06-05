#!/bin/bash
ROOT_FOLDER=""
#KAHIP_MASTER_FOLDER=$ROOT_FOLDER"/algorithms/KaHIP-master"
#KAHIP_MASTER_FOLDER=$ROOT_FOLDER"/algorithms/KaHIP-updated"
KAHIP_MASTER_FOLDER=$ROOT_FOLDER"../../KaHIP"

DATA_ROOT_FOLDER="../../data"
GRAPH_DATA_ROOT_FOLDER=$DATA_ROOT_FOLDER"/benchmark_set_graph"
BGF_DATA_ROOT_FOLDER=$DATA_ROOT_FOLDER"/benchmark_set_bgf"

TEST_FILE_PATH=$BGF_DATA_ROOT_FOLDER"/dac2012_superblue19.bgf"

PARHIP_EXECUTABLE=$KAHIP_MASTER_FOLDER"/build/parallel/parallel_src/parhip"

mpirun -n 6 ./$PARHIP_EXECUTABLE $TEST_FILE_PATH --k 4 --preconfiguration=fastmesh
