// Expected localparam type name, "Hello_World" to follow lower_snake_case naming convention and end with _t.
// verilog_lint: waive default-nettype
class foo;
  localparam type Hello_World = logic;
endclass
