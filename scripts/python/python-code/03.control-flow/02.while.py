#!/usr/bin/env python
# coding=utf-8

# Filename: while.py
number = 23
running = True

while running:
    guess = int(raw_input('Enter an integer : '))
    if guess == number:
        print 'Congratulations, you guessed it.'
        running = False # this causes the while loop to stop
    elif guess < number:
        print 'No, it is a little higher than that'
    else:
        print 'No, it is a little lower than that'
# optional else-block
else:
    print 'The while loop is over.'

# Do anything else you want to do here
print 'Done'

# note: 可以在while循环中使用一个else从句.
