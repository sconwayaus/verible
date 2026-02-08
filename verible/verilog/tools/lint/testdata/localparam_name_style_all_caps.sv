// fail, as we expect localparam to follow ALL_CAPS naming convention
// verilog_lint: waive default-nettype
class foo;
  localparam int HelloWorld = 1;  // found Camel case instead
endclass
