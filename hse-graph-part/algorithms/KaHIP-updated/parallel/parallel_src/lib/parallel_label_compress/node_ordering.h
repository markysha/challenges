/******************************************************************************
 * node_ordering.h
 * *
 * Source of KaHIP -- Karlsruhe High Quality Partitioning.
 * Christian Schulz <christian.schulz.phone@gmail.com>
 *****************************************************************************/

#ifndef NODE_ORDERING_HM1YMLB1
#define NODE_ORDERING_HM1YMLB1

#include <algorithm>

#include "definitions.h"
#include "partition_config.h"
#include "data_structure/parallel_graph_access.h"
#include "tools/random_functions.h"

class node_ordering {
public:
        node_ordering();
        virtual ~node_ordering();

        void order_nodes(const PPartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) {
                forall_local_nodes(G, node) {
                        ordered_nodes[node] = node;
                } endfor

                switch( config.node_ordering ) {
                        case RANDOM_NODEORDERING:
                                order_nodes_random(config, G, ordered_nodes);
                             break;
                        case DEGREE_NODEORDERING:
                                order_nodes_degree(config, G, ordered_nodes);
                             break;
                        case LEASTGHOSTNODESFIRST_DEGREE_NODEODERING:
                                order_leastghostnodes_nodes_degree(config, G, ordered_nodes);
                             break;
                        case DEGREE_LEASTGHOSTNODESFIRST_NODEODERING:
                                order_nodes_degree_leastghostnodes(config, G, ordered_nodes);
                             break;
                 }
        }

        void order_nodes_random(const PPartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) { 
                random_functions::permutate_vector_fast(ordered_nodes, false);
        }

        void order_nodes_degree(const PPartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) {
                static std::vector< size_t > count;
                static std::vector< std::pair< EdgeID, NodeID > > ordered_nodes_with_degrees;
                ordered_nodes_with_degrees.clear();
                for (NodeID node_id : ordered_nodes) {
                        ordered_nodes_with_degrees.emplace_back(G.getNodeDegree(node_id), node_id);
                }
                auto values_range = std::minmax_element(
                        ordered_nodes_with_degrees.begin(), 
                        ordered_nodes_with_degrees.end()
                );
                if (values_range.second->first - values_range.first->first + 1 <= ordered_nodes.size() * 2) {
                        NodeID OFFSET = values_range.first->first;
                        size_t COUNT = values_range.second->first - values_range.first->first + 1;
                        count.assign(COUNT, 0);
                        for (const auto& it: ordered_nodes_with_degrees) {
                                ++count[it.first - OFFSET];
                        }
                        for (int i = 1; i < COUNT; i++) {
                                count[i] += count[i - 1];
                        }
                        for (const auto& it: ordered_nodes_with_degrees) {
                                size_t pos = --count[it.first - OFFSET];
                                ordered_nodes[pos] = it.second;
                        }
                } else {
                        std::sort(ordered_nodes_with_degrees.begin(), ordered_nodes_with_degrees.end());
                        for (int i = 0; i < ordered_nodes.size(); ++i) {
                                ordered_nodes[i] = ordered_nodes_with_degrees[i].second;
                        }
                }          
        }

        void order_leastghostnodes_nodes_degree(const PPartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) { 
                order_nodes_degree(config, G, ordered_nodes);
        }

	void order_nodes_degree_leastghostnodes(const PPartitionConfig & config, parallel_graph_access & G, std::vector< NodeID > & ordered_nodes) { 
                order_nodes_degree(config, G, ordered_nodes);
        }
};


#endif /* end of include guard: NODE_ORDERING_HM1YMLB1 */
