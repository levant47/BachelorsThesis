## MAIN

push 1 # initial LED value
store # store it on the general stack

loop:
# set LED to current value
push 0
load
dup
store
out

# pause for 3,333,333 cycles (approx. equivalent to 1 second of CPU time)
push 50
push 220
push 213
push sleep_3
call

# toggle LED value
load
push logical_not
call
store

push loop
jmp


## FUNCTION
sleep_1:
dup
push 0
cmp
push sleep_1_end
jeq

push -1
add
push sleep_1
jmp

sleep_1_end:
pop
ret


## FUNCTION
sleep_2:
push sleep_1
call

sleep_2_loop:
dup
push 0
cmp
push sleep_2_end
jeq

push 255
push sleep_1
call

push -1
add
push sleep_2_loop
jmp

sleep_2_end:
pop
ret



## FUNCTION
sleep_3:
push sleep_2
call

sleep_3_loop:
dup
push 0
cmp
push sleep_3_end
jeq

push 255
push 255
push sleep_2
call

push -1
add
push sleep_3_loop
jmp

sleep_3_end:
pop
ret


## FUNCTION
logical_not:
push 0
cmp
push logical_not_return_1
jeq

# otherwise, we need to return 0
push 0
ret

logical_not_return_1:
push 1
ret

