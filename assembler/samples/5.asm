push 5

push add_10
call

push add_2
call

push add_10
call

# halt
halt:
push halt
jmp

## functions

add_10:
push 10
add
ret

add_2:
push 2
add
ret
