push 10
loop:
dup
push 0
cmp
jeq end_loop
push -1
add
jmp loop
end_loop:
