push 10
loop:
dup
push 0
cmp
push end_loop
jeq
push -1
add
push loop
jmp
end_loop:
pop

halt:
push halt
jmp
