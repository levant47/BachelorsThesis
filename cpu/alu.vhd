library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

entity alu is
    port (
        clock : in STD_ULOGIC;
        instruction : in STD_ULOGIC_VECTOR(7 downto 0);
        next_instruction_address : out STD_ULOGIC_VECTOR(7 downto 0);
        output_0 : out STD_ULOGIC
    );
end alu;

architecture alu_architecture of alu is
    type T_FLAGS_REGISTER is ('l', 'e', 'g');
    signal flags_register : T_FLAGS_REGISTER;

    signal instruction_register : STD_ULOGIC_VECTOR(7 downto 0) := "00000000";

    signal stack : work.types.T_MEMORY(255 downto 0) := (others => "00000000");
    -- TODO: rename to stack_size
    signal stack_index : integer := 0;

    signal is_awaiting_second_byte : STD_ULOGIC := '0';
    -- TODO: rename to previous_byte
    signal previous_instruction : STD_ULOGIC_VECTOR(7 downto 0) := "00000000";

    signal output_0_register : STD_ULOGIC := '0';
begin
    next_instruction_address <= instruction_register;
    output_0 <= output_0_register;

    process (clock) begin
        if rising_edge(clock) then
            instruction_register <= std_ulogic_vector(unsigned(instruction_register) + 1);

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
                    stack(stack_index-2) <= std_ulogic_vector(unsigned(stack(stack_index-2)) + unsigned(stack(stack_index-1)));
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
                    if flags_register = 'l' then
                        instruction_register <= stack(stack_index - 1);
                    end if;
                    stack_index <= stack_index - 1;
                -- jle
                elsif instruction = "00000110" then
                    if flags_register = 'l' or flags_register = 'e' then
                        instruction_register <= stack(stack_index - 1);
                    end if;
                    stack_index <= stack_index - 1;
                -- jeq
                elsif instruction = "00000111" then
                    if flags_register = 'e' then
                        instruction_register <= stack(stack_index - 1);
                    end if;
                    stack_index <= stack_index - 1;
                -- jge
                elsif instruction = "00001000" then
                    if flags_register = 'e' or flags_register = 'g' then
                        instruction_register <= stack(stack_index - 1);
                    end if;
                    stack_index <= stack_index - 1;
                -- jg
                elsif instruction = "00001001" then
                    if flags_register = 'g' then
                        instruction_register <= stack(stack_index - 1);
                    end if;
                    stack_index <= stack_index - 1;
                -- jne
                elsif instruction = "00001010" then
                    if flags_register /= 'e' then
                        instruction_register <= stack(stack_index - 1);
                    end if;
                    stack_index <= stack_index - 1;
                -- jmp
                elsif instruction = "00001011" then
                    instruction_register <= stack(stack_index - 1);
                    stack_index <= stack_index - 1;
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
                -- push_nothing
                elsif instruction = "00001110" then
                    stack_index <= stack_index + 1;
                end if;
                -- there is a definition for a no-op instruction,
                -- but in the implementation everything that isn't a valid instruction
                -- is interpreted as a no-op as well
            else -- handle the second byte
                -- push
                if previous_instruction = "00000001" then
                    stack(stack_index) <= instruction;
                    stack_index <= stack_index + 1;
                end if;
                is_awaiting_second_byte <= '0';
            end if;
        end if;
    end process;
end alu_architecture;
