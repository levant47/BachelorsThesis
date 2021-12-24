entity cpu_test is
end cpu_test;

architecture cpu_test_architecture of cpu_test is
    signal clock : BIT := '0';
begin
    cpu_instance : entity work.cpu
        port map (clock => clock);

    clock <= not clock after 5 ns;
end cpu_test_architecture;
