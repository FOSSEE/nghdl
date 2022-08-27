library ieee;
package sock_pkg is
	function sock_port_fun return integer;
	function sock_ip_fun return string;
end;

package body sock_pkg is
	function sock_port_fun return integer is
		variable sock_port : integer;
			begin
				sock_port := ;
				return sock_port;
			end function;

	function sock_ip_fun return string is
		type string_ptr is access string;
		variable sock_ip : string_ptr;
			begin
				sock_ip := new string'("");
				return sock_ip.all;
			end function;

		end package body;
