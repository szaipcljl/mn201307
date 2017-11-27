#!/usr/bin/env python
# coding=utf-8

#https://github.com/angr/angr-doc/blob/master/docs/analyses/cfg_accurate.md

import angr
# load your project
b = angr.Project('/bin/true', load_options={'auto_load_libs': False})

# generate an accurate CFG
cfg = b.analyses.CFGAccurate(keep_state=True)

#Using the CFG
print "This is the graph:", cfg.graph
print "It has %d nodes and %d edges" % (len(cfg.graph.nodes()), len(cfg.graph.edges()))

# this grabs *any* node at a given location:
entry_node = cfg.get_any_node(b.entry)

# on the other hand, this grabs all of the nodes
print "There were %d contexts for the entry block" % len(cfg.get_all_nodes(b.entry))

# we can also look up predecessors and successors
print "Predecessors of the entry point:", entry_node.predecessors
print "Successors of the entry point:", entry_node.successors
print "Successors (and type of jump) of the entry point:", [ jumpkind + " to " + str(node.addr) for node,jumpkind in cfg.get_successors_and_jumpkind(entry_node) ]

#Function Manager
entry_func = cfg.kb.functions[b.entry]
print entry_func.block_addrs
print entry_func.name

# Get the addresses and count of functions identified by the angr analysis engine
function_addrs = cfg.functions
print function_addrs
methods_identified = len(function_addrs)
print methods_identified

for function_addr in function_addrs:
    function = cfg.functions.function(function_addr)    # Get the method instance
    print function
    first_node_of_function = cfg.get_node(function_addr)    # Get the node containing the function entry point
    calling_nodes = cfg.get_predecessors(first_node_of_function)    # Get the nodes containing a call to the function (entry point)
    #print calling_nodes


print "----------------------------"
target_func = cfg.kb.functions.function(name="__libc_start_main")
print target_func
