-- Basic peripheral VHDL Code for ATtiny series of microcontrollers by ASHUTOSH JHA

-- This VHDL Code was modified by SUMANTO KAR for ATtiny85 microcontroller
-- Latest edit - 12:16 PM, 19/6/2020
-- NOTE :- The function MapToRam and output are linked to the C code of ATTINY85
-- by "ghdl_access.vhdl" file 


library ieee;
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all;

library work;
use work.ghdl_access.all;

entity attiny_85_nghdl is
port(VCC : in std_logic;
     GND : in std_logic;
     clk : in std_logic;
     PB0 : out std_logic_vector(0 downto 0);
     PB1 : out std_logic_vector(0 downto 0);
     PB2 : in std_logic_vector(0 downto 0);
     PB3 : in std_logic_vector(0 downto 0);
     PB4 : in std_logic_vector(0 downto 0);
     PB5 : in std_logic_vector(0 downto 0));
end attiny_85_nghdl;

architecture bhv of attiny_85_nghdl is
signal PINB_Data : integer;
begin
MapToRam(1);
	process(clk)
	begin
		if(rising_edge(clk) and VCC = '1' and GND = '0') then
			PINB_Data <= to_integer(unsigned(PB2))*4+to_integer(unsigned(PB3))*8 + to_integer(unsigned(PB4))*16+ to_integer(unsigned(PB5))*32;
			input(PINB_Data);
			output(1);
			PB0 <= std_logic_vector(to_unsigned(var0.all, PB0'length));
			PB1 <= std_logic_vector(to_unsigned(var1.all, PB1'length));
			--PB2 <= std_logic_vector(to_unsigned(var2.all, PB2'length));
			--PB3 <= std_logic_vector(to_unsigned(var3.all, PB3'length));
			--PB4 <= std_logic_vector(to_unsigned(var4.all, PB4'length));
			--PB5 <= std_logic_vector(to_unsigned(var5.all, PB5'length));
		elsif(VCC = '0') then
			PB0 <= "0";
			PB1 <= "0";
			--PB2 <= "0";
			--PB3 <= "0";
			--PB4 <= "0";
			--PB5 <= "0";
		end if;
	end process;
end bhv;
