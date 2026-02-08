// Forbid parameter names starting with 'disable', use 'enable' instead.
// verilog_lint: waive default-nettype
module positive_meaning_parameter_name #(parameter int DISABLE_FOO = 1); endmodule
