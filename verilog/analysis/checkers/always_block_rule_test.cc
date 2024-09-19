// Copyright 2017-2023 The Verible Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "verilog/analysis/checkers/always_block_rule.h"

#include <initializer_list>

#include "common/analysis/linter_test_utils.h"
#include "common/analysis/syntax_tree_linter_test_utils.h"
#include "gtest/gtest.h"
#include "verilog/analysis/verilog_analyzer.h"
#include "verilog/parser/verilog_token_enum.h"

namespace verilog {
namespace analysis {
namespace {

using verible::LintTestCase;
using verible::RunApplyFixCases;
using verible::RunLintTestCases;

TEST(AlwaysBlockTest, FunctionFailures) {
  constexpr int kToken = TK_always;
  const std::initializer_list<LintTestCase> kAlwaysBlockTestCases = {
      {""},
      {"module m;\nendmodule\n"},
      {"module m;\ninitial begin end\nendmodule"},

      // These lines are covered by the always-comb-rule
      {"module m;always @* begin end\nendmodule"},
      {"module m;always @(*) begin end\nendmodule"},
      {"module m;always @( *) begin end\nendmodule"},
      {"module m;always @(* ) begin end\nendmodule"},
      {"module m;always @( * ) begin end\nendmodule"},
      {"module m;always @(/*t*/*) begin end\nendmodule"},
      {"module m;always @(*/*t*/) begin end\nendmodule"},
      {"module m;\nalways @(/*t*/*/*t*/) begin end\nendmodule"},

      {"module m;\nalways_ff begin a <= b; end\nendmodule"},
      {"module m;\nalways_comb begin a = b; end\nendmodule"},

      {"module m;\nalways_ff @(posedge clk) begin a <= b; end\nendmodule"},
      {"module m;\nalways_latch begin if(a) b = c; end\nendmodule"},
      {"module m;\n",
       {kToken, "always"},
       " @(posedge clk) begin a = b; end\nendmodule"},
      {"module m;\n",
       {kToken, "always"},
       " @(negedge clk) begin a = b; end\nendmodule"},
      {"module m;\n",
       {kToken, "always"},
       " @(clk) begin a = b; end\nendmodule"},
      {"module m;\n",
       {kToken, "always"},
       " @(a or b) begin a = b; end\nendmodule"},
      {"module m;\nalways #10 clk <= !clk;\nendmodule"},
      {"module m;\nalways_comb begin a = b; end\nendmodule"},
      {"module m;\nalways_comb begin a = b; end\nendmodule"},
  };

  RunLintTestCases<VerilogAnalyzer, AlwaysBlockRule>(kAlwaysBlockTestCases);
}

}  // namespace
}  // namespace analysis
}  // namespace verilog
