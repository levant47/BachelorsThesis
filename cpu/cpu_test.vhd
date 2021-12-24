entity cpu_test is
end cpu_test;

architecture cpu_test_architecture of cpu_test is
    signal clock : BIT := '0';

    constant code : work.types.T_MEMORY := work.program.code;
begin
    cpu_instance : entity work.cpu
        generic map (code => code)
        port map (clock => clock);

    clock <= not clock after 5 ns;
end cpu_test_architecture;
