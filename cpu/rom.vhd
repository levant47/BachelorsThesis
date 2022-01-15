library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity rom is
    generic (code : work.types.T_MEMORY);

    port (
        address : in STD_ULOGIC_VECTOR(7 downto 0);
        output : out STD_ULOGIC_VECTOR(7 downto 0)
    );
end rom;

architecture rom_architecture of rom is
begin
    output <= code(to_integer(unsigned(address)))
        -- TODO: need this because address is undefined at the very first moment
        -- of the simulation, no idea why; is there a proper solution to this?
        when address /= "UUUUUUUU"
        else (others => '0');
end rom_architecture;
