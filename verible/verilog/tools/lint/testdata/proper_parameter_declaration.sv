// 'parameters' should only be declared within packages or parameterized modules/classes.
// verilog_lint: waive default-nettype
module proper_parameter_declaration;
  parameter int Bar = 1;
endmodule
