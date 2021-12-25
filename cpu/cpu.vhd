library IEEE;
use IEEE.std_logic_1164.all;

entity cpu is
    port (
        clock : in STD_ULOGIC;
        output_0 : out BIT
    );
end cpu;

architecture cpu_architecture of cpu is
    signal instruction : BIT_VECTOR(7 downto 0);
    signal instruction_address : BIT_VECTOR(7 downto 0);

    constant code : work.types.T_MEMORY := work.program.code;
begin
    rom_instance : entity work.rom
        generic map (code => code)
        port map (
            address => instruction_address,
            output => instruction
        );

    alu_instance : entity work.alu port map (
        clock => clock,
        instruction => instruction,
        next_instruction_address => instruction_address,
        output_0 => output_0
    );
end cpu_architecture;
