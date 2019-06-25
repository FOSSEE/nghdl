library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
 
entity trial_ha is
  port (
    i_bit  : in std_logic_vector(1 downto 0);
    o_sum   : out std_logic_vector(0 downto 0);
    o_carry : out std_logic_vector(0 downto 0)
    );
end trial_ha;
 
architecture rtl of trial_ha is
begin
  o_sum   <= i_bit(0) xor i_bit(1);
  o_carry <= i_bit(0) and i_bit(1);
end rtl;