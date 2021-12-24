package types is
    type T_ALU_STATE is (normal, awaiting_push_argument);

    type T_MEMORY is ARRAY(natural range <>) of BIT_VECTOR(7 downto 0);
end types;
