// Copyright 2017-2020 The Verible Authors.
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

#include "verilog/analysis/checkers/forbid_port_type_rule.h"

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
using verible::RunLintTestCases;

// Tests defaut configuration expected behaviour
TEST(ForbidPortTypeRuleTest, DefaultAcceptTests) {
  const std::initializer_list<LintTestCase> kTestCases = {
        // {"module foo(input logic a);endmodule"},
        {"module foo(output logic a);endmodule"},

        {"module foo(input wire a);endmodule"},
        {"module foo(output wire a);endmodule"},
        {"module foo(inout wire a);endmodule"},

        // {"module foo(input var a);endmodule"},
        {"module foo(output var a);endmodule"},

        // {"module foo(input reg a);endmodule"},
        {"module foo(output reg a);endmodule"},

        {"module foo(input some_data_t a);endmodule"},
        {"module foo(output some_data_t a);endmodule"},

        {"module foo(input wire some_data_t a);endmodule"},
        {"module foo(output wire some_data_t a);endmodule"},

        // {"module foo(input var some_data_t a);endmodule"},
        {"module foo(output var some_data_t a);endmodule"},

        // FIXME: I want these to violate
        {"module foo(input a);endmodule"},
        {"module foo(output a);endmodule"},

        {"module foo(input [7:0] a);endmodule"},
        {"module foo(output [7:0] a);endmodule"},
  };
  RunLintTestCases<VerilogAnalyzer, ForbidPortTypeRule>(kTestCases);
}

// Tests defaut configuration reject behaviour
TEST(ForbidPortTypeRuleTest, DefaultRejectTests) {
  const std::initializer_list<LintTestCase> kTestCases = {
        {"module foo(input ", {TK_logic, "logic"}, " a);endmodule"},
        // {"module foo(output ", {TK_logic, "logic"}, " a);endmodule"},

        // {"module foo(input ", {TK_wire, "wire"}, " a);endmodule"},
        // {"module foo(output ", {TK_wire, "wire"}, " a);endmodule"},
        // {"module foo(inout ", {TK_wire, "wire"}, " a);endmodule"},

        {"module foo(input ", {TK_var, "var"}, " a);endmodule"},
        // {"module foo(output ", {TK_var, "var"}, " a);endmodule"},

        {"module foo(input ", {TK_reg, "reg"}, " a);endmodule"},
        // {"module foo(output ", {TK_reg, "reg"}, " a);endmodule"},

        {"module foo(input some_data_t a);endmodule"},
        // {"module foo(output some_data_t a);endmodule"},

        // {"module foo(input ", {TK_wire, "wire"}, " some_data_t a);endmodule"},
        // {"module foo(output ", {TK_wire, "wire"}, " some_data_t a);endmodule"},

        {"module foo(input ", {TK_var, "var"}, " some_data_t a);endmodule"},
        // {"module foo(output ", {TK_var, "var"}, " some_data_t a);endmodule"},

        // FIXME: I want these to violate
        // {"module foo(input a);endmodule"},
        // {"module foo(output a);endmodule"},

        // {"module foo(input [7:0] a);endmodule"},
        // {"module foo(output [7:0] a);endmodule"},

    
    //   {"module foo (input ", {TK_wire, "wire"}, " a); endmodule"},
    //   {"module foo (input ", {TK_reg, "reg"}, " a); endmodule"},

    //   {"module foo (output ", {TK_wire, "wire"}, " a); endmodule"},
    //   {"module foo (output ", {TK_reg, "reg"}, " a); endmodule"},

    //   {"module foo (inout ", {TK_logic, "logic"}, " a); endmodule"},
  };
  RunLintTestCases<VerilogAnalyzer, ForbidPortTypeRule>(kTestCases);
}

}  // namespace
}  // namespace analysis
}  // namespace verilog
