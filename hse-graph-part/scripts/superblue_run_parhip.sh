#!/bin/bash
ROOT_FOLDER=""
KAHIP_MASTER_FOLDER=$ROOT_FOLDER"/algorithms/KaHIP-master"
# KAHIP_MASTER_FOLDER=$ROOT_FOLDER"/algorithms/KaHIP-updated"
KAHIP_UPDATED_FOLDER=$ROOT_FOLDER"../../KaHIP"

DATA_ROOT_FOLDER="../../data"
GRAPH_DATA_ROOT_FOLDER=$DATA_ROOT_FOLDER"/benchmark_set_graph"
BGF_DATA_ROOT_FOLDER=$DATA_ROOT_FOLDER"/benchmark_set_bgf"

TEST_FILE_PATH=$BGF_DATA_ROOT_FOLDER"/dac2012_superblue19.bgf"

PARHIP_EXECUTABLE=$KAHIP_MASTER_FOLDER"/build/parallel/parallel_src/parhip"

# for i in 2 3 6 7 9 11 12 14 16
# do
#     ./scripts/hgr_to_graph < ./$DATA_ROOT_FOLDER/benchmark_set/dac2012_superblue$i.hgr > ./$GRAPH_DATA_ROOT_FOLDER/dac2012_superblue$i.graph
# done

# for i in 2 3 6 7 9 11 12 14 16 19
# do
#     echo $i
#     ./$KAHIP_MASTER_FOLDER/build/parallel/parallel_src/graph2binary ./$GRAPH_DATA_ROOT_FOLDER/dac2012_superblue$i.graph ./$BGF_DATA_ROOT_FOLDER/dac2012_superblue$i.bgf
# done

for k in 16 
do
    for i in 11 12 14 16 19
    do
        echo $k $i
        mpirun -n 6 ./$KAHIP_MASTER_FOLDER/build/parallel/parallel_src/parhip ./$BGF_DATA_ROOT_FOLDER/dac2012_superblue$i.bgf --k $k --preconfiguration=ecomesh > parhip_master_${i}_n_6_k_${k}_ecomesh 

        mpirun -n 6 ./$KAHIP_UPDATED_FOLDER/build/parallel/parallel_src/parhip ./$BGF_DATA_ROOT_FOLDER/dac2012_superblue$i.bgf --k $k --preconfiguration=ecomesh > parhip_updated_${i}_n_6_k_${k}_ecomesh 
    done
done

for k in 64 
do
    for i in 2 3 6 7 9 11 12 14 16 19
    do
        echo $k $i
        mpirun -n 6 ./$KAHIP_MASTER_FOLDER/build/parallel/parallel_src/parhip ./$BGF_DATA_ROOT_FOLDER/dac2012_superblue$i.bgf --k $k --preconfiguration=ecomesh > parhip_master_${i}_n_6_k_${k}_ecomesh 

        mpirun -n 6 ./$KAHIP_UPDATED_FOLDER/build/parallel/parallel_src/parhip ./$BGF_DATA_ROOT_FOLDER/dac2012_superblue$i.bgf --k $k --preconfiguration=ecomesh > parhip_updated_${i}_n_6_k_${k}_ecomesh 
    done
done

# for i in 2 3 6 7 9 11 12 14 16 19
# do
#     echo $i
#     mpirun -n 6 ./$KAHIP_MASTER_FOLDER/build/parallel/parallel_src/parhip ./$BGF_DATA_ROOT_FOLDER/dac2012_superblue$i.bgf --k 4 --preconfiguration=fastmesh > parhip_master_${i}_n_6_k_4_fastmesh 

#     mpirun -n 6 ./$KAHIP_UPDATED_FOLDER/build/parallel/parallel_src/parhip ./$BGF_DATA_ROOT_FOLDER/dac2012_superblue$i.bgf --k 4 --preconfiguration=fastmesh > parhip_updated_${i}_n_6_k_4_fastmesh 
# done
