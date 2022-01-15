library IEEE;
use IEEE.std_logic_1164.all;

entity cpu is
    port (
        clock : in STD_ULOGIC;
        output_0 : out STD_ULOGIC
    );
end cpu;

architecture cpu_architecture of cpu is
    signal instruction : STD_ULOGIC_VECTOR(7 downto 0) := (others => '0');
    signal instruction_address : STD_ULOGIC_VECTOR(7 downto 0) := "00000000";

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
