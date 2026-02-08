// fail, as we expect localparam to follow CamelCase naming convention
// verilog_lint: waive default-nettype
class foo;
  localparam int HELLO_WORLD = 1;  // ... found ALL_CAPS instead
endclass
