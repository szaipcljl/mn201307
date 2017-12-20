#!/usr/bin/env python
# coding=utf-8

#You can use a global variable in other functions by declaring it as global in
#each function that assigns to it:

globvar = 0

def set_globvar_to_one():
    global globvar    # Needed to modify global copy of globvar
    globvar = 1

def print_globvar():
    print(globvar)     # No need for global declaration to read value of globvar

set_globvar_to_one()
print_globvar()       # Prints 1

#I imagine the reason for it is that, since global variables are so dangerous,
#Python wants to make sure that you really know that's what you're playing with
#by explicitly requiring the global keyword.

#What's going on here is that Python assumes that any name that is assigned to,
#anywhere within a function, is local to that function unless explicitly told
#otherwise. If it is only reading from a name, and the name doesn't exist
#locally, it will try to look up the name in any containing scopes
