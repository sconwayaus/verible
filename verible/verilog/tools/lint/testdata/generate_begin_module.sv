module generate_begin_module;
  // verilog_lint: waive legacy-generate-region
  generate
    begin : gen_block1
    // verilog_lint: waive always-block
      always @(posedge clk) begin
        foo <= bar;
      end
    end
  endgenerate
endmodule
