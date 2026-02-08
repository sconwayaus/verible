// Expected localparam name, "Hello_World" to follow UpperCamelCase
// naming convention.
// verilog_lint: waive default-nettype
class foo;
  localparam int Hello_World = 1;
endclass
