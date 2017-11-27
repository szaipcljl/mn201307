#!/usr/bin/env python
# coding=utf-8
import angr

# get all the matches
p = angr.Project("/home/nma1x/angr_dir/binaries/tests/i386/identifiable")
#p = angr.Project("./a.out", load_options={'auto_load_libs':False})
idfer = p.analyses.Identifier()

# note that .run() yields results so make sure to iterate through them or call
# list() etc
for addr, symbol in idfer.run():
    print hex(addr), symbol
