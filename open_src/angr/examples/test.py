#!/usr/bin/env python
# coding=utf-8

import angr


print "### start"

proj = angr.Project('/bin/true')
print proj.loader
print proj.loader.shared_objects

print proj.loader.main_object # we've loaded several binaries into this project.  Here's the main one!

print proj.loader.main_object.execstack  # sample query: does this binary have an executable stack?

print proj.loader.main_object.pic # sample query: is this binary position-independent?

block = proj.factory.block(proj.entry)

block.pp()                          # pretty-print a disassembly to stdout
print block.instructions                  # how many instructions are there?

print block.instruction_addrs             # what are the addresses of the instructions?


print "# capstone disassembly"
print block.capstone
print block.vex                            # VEX IRSB (that's a python internal address, not a program address)


# States
print "#stats"
state = proj.factory.entry_state()
print state

print state.regs.rip        # get the current instruction pointer
print state.regs.rax
print state.mem[proj.entry].int.resolved  # interpret the memory at the entry point as a C int

bv = state.solver.BVV(0x1234, 32)       # create a 32-bit-wide bitvector with value 0x1234
print bv
print state.solver.eval(bv)                # convert to python int

proj.analyses            # Press TAB here in ipython to get an autocomplete-listing of everything:


print "### end"
