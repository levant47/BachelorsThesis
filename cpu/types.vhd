library IEEE;
use IEEE.std_logic_1164.all;

package types is
    type T_ALU_STATE is (normal, awaiting_push_argument);

    type T_MEMORY is ARRAY(natural range <>) of STD_ULOGIC_VECTOR(7 downto 0);
end types;
