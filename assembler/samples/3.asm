push 0

blink:
dup
push 1
cmp
jeq turn_on_led
jmp turn_off_led

turn_on_led:
push 0
push 1
out
pop
push 0
jmp end_if

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
jeq loop_3_end

push 100
loop_2:
dup
push 0
cmp
jeq loop_2_end

push 100
loop_1:
dup
push 0
cmp
jeq loop_1_end
push -1
add
jmp loop_1
loop_1_end:
pop

push -1
add
jmp loop_2
loop_2_end:
pop

push -1
add
jmp loop_3
loop_3_end:
pop

jmp blink
