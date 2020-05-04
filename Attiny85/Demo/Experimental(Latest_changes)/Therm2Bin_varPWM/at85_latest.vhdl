-- By Ashutosh Jha
-- Modified - 2 May 2020

library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;

library work;
use work.ghdl_access.all;

entity at85_latest is
port(VCC : in std_logic;
     GND : in std_logic;
     clk : in std_logic;
     PB1 : in std_logic_vector(0 downto 0);
     PB2 : in std_logic_vector(0 downto 0);
     PB0 : out std_logic_vector(0 downto 0);
     PB3 : out std_logic_vector(0 downto 0);
     PB4 : out std_logic_vector(0 downto 0);
     PB5 : out std_logic_vector(0 downto 0));
end at85_latest;

architecture bhv of at85_latest is
signal PINB_Data : integer;
begin
MapToRam(1);
	process(clk)
	begin
		if(rising_edge(clk) and VCC = '1' and GND = '0') then
			PINB_Data <= to_integer(unsigned(PB2))*2 + to_integer(unsigned(PB1))*4;
			input(PINB_Data);
			output(1);
			PB0 <= std_logic_vector(to_unsigned(var0.all, PB0'length));
			--PB1 <= std_logic_vector(to_unsigned(var1.all, PB1'length));
			--PB2 <= std_logic_vector(to_unsigned(var2.all, PB2'length));
			PB3 <= std_logic_vector(to_unsigned(var3.all, PB3'length));
			PB4 <= std_logic_vector(to_unsigned(var4.all, PB4'length));
			PB5 <= std_logic_vector(to_unsigned(var5.all, PB5'length));
		elsif(VCC = '0') then
			PB0 <= "0";
			--PB1 <= "0";
			--PB2 <= "0";
			PB3 <= "0";
			PB4 <= "0";
			PB5 <= "0";
		end if;
	end process;
end bhv;
