entity cpu is
    generic (code : work.types.T_MEMORY);

    port (
        clock : in BIT
    );
end cpu;

architecture cpu_architecture of cpu is
    signal instruction : BIT_VECTOR(7 downto 0);
    signal instruction_address : BIT_VECTOR(7 downto 0);
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
        next_instruction_address => instruction_address
    );
end cpu_architecture;
