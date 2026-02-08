// verilog_lint: waive default-nettype
module dff_name_style ();
  always_ff @(posedge clk) begin
    data_q <= data;
  end
endmodule
