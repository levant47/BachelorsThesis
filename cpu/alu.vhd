entity alu is
    port (
        clock : in BIT;
        instruction : in BIT_VECTOR(7 downto 0);
        next_instruction_address : out BIT_VECTOR(7 downto 0);
        output_0 : out BIT
    );

    function add_bit_vector_8(
        left : bit_vector(7 downto 0);
        right : bit_vector(7 downto 0)
    ) return bit_vector is
        variable result : bit_vector(7 downto 0) := b"0000_0000";
        variable carry : bit := '0';
    begin
        for i in 0 to 7 loop
            result(i) := left(i) xor right(i) xor carry;
            carry := (left(i) and right(i)) or (left(i) and carry) or (right(i) and carry);
        end loop;
        return result;
    end add_bit_vector_8;
end alu;

architecture alu_architecture of alu is
    type T_FLAGS_REGISTER is ('l', 'e', 'g');
    signal flags_register : T_FLAGS_REGISTER;

    signal instruction_register : BIT_VECTOR(7 downto 0) := "00000000";

    signal stack : work.types.T_MEMORY(255 downto 0);
    signal stack_index : integer := 0;

    signal is_awaiting_second_byte : BIT := '0';
    signal previous_instruction : BIT_VECTOR(7 downto 0) := "00000000";

    signal output_0_register : BIT := '0';
begin
    output_0 <= output_0_register;

    process (clock) begin
        if clock'event and clock = '1' then
            instruction_register <= add_bit_vector_8(instruction_register, "00000001");

            if is_awaiting_second_byte = '0' then
                -- push
                if instruction = "00000001" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- pop
                elsif instruction = "00000010" then
                    stack_index <= stack_index - 1;
                -- add
                elsif instruction = "00000011" then
                    stack(stack_index-2) <= add_bit_vector_8(stack(stack_index-2), stack(stack_index-1));
                    stack_index <= stack_index - 1;
                -- cmp
                elsif instruction = "00000100" then
                    if stack(stack_index-2) < stack(stack_index-1) then
                        flags_register <= 'l';
                    elsif stack(stack_index-2) > stack(stack_index-1) then
                        flags_register <= 'g';
                    else
                        flags_register <= 'e';
                    end if;
                    stack_index <= stack_index - 2;
                -- jl
                elsif instruction = "00000101" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- jle
                elsif instruction = "00000110" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- jeq
                elsif instruction = "00000111" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- jge
                elsif instruction = "00001000" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- jg
                elsif instruction = "00001001" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- jne
                elsif instruction = "00001010" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- jmp
                elsif instruction = "00001011" then
                    is_awaiting_second_byte <= '1';
                    previous_instruction <= instruction;
                -- dup
                elsif instruction = "00001100" then
                    stack(stack_index) <= stack(stack_index-1);
                    stack_index <= stack_index + 1;
                -- out
                elsif instruction = "00001101" then
                    if stack(stack_index-2) = "00000000" then
                        output_0_register <= stack(stack_index-1)(0);
                    end if;
                    stack_index <= stack_index - 2;
                end if;
                -- there is a definition for a no-op instruction,
                -- but in the implementation everything that isn't a valid instruction
                -- is interpreted as a no-op as well
            else -- handle the second byte
                -- push
                if previous_instruction = "00000001" then
                    stack(stack_index) <= instruction;
                    stack_index <= stack_index + 1;
                -- jl
                elsif previous_instruction = "00000101" then
                    if flags_register = 'l' then
                        instruction_register <= instruction;
                    end if;
                -- jle
                elsif previous_instruction = "00000110" then
                    if flags_register = 'l' or flags_register = 'e' then
                        instruction_register <= instruction;
                    end if;
                -- jeq
                elsif previous_instruction = "00000111" then
                    if flags_register = 'e' then
                        instruction_register <= instruction;
                    end if;
                -- jge
                elsif previous_instruction = "00001000" then
                    if flags_register = 'e' or flags_register = 'g' then
                        instruction_register <= instruction;
                    end if;
                -- jg
                elsif previous_instruction = "00001001" then
                    if flags_register = 'g' then
                        instruction_register <= instruction;
                    end if;
                -- jne
                elsif previous_instruction = "00001010" then
                    if flags_register /= 'e' then
                        instruction_register <= instruction;
                    end if;
                -- jmp
                elsif previous_instruction = "00001011" then
                    instruction_register <= instruction;
                end if;
                is_awaiting_second_byte <= '0';
            end if;
        end if;
    end process;

    next_instruction_address <= instruction_register;
end alu_architecture;
