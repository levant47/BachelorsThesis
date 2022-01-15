push 0

blink:
dup
push 1
cmp
push turn_on_led
jeq
push turn_off_led
jmp

turn_on_led:
push 0
push 1
out
pop
push 0
push end_if
jmp

turn_off_led:
push 0
push 0
out
pop
push 1

end_if:

push 100
loop_3:
dup
push 0
cmp
push loop_3_end
jeq

push 100
loop_2:
dup
push 0
cmp
push loop_2_end
jeq

push 100
loop_1:
dup
push 0
cmp
push loop_1_end
jeq
push -1
add
push loop_1
jmp
loop_1_end:
pop

push -1
add
push loop_2
jmp
loop_2_end:
pop

push -1
add
push loop_3
jmp
loop_3_end:
pop

push blink
jmp
