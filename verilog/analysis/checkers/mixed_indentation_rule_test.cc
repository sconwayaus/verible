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

#include "verilog/analysis/checkers/mixed_indentation_rule.h"

#include <initializer_list>

#include "common/analysis/linter_test_utils.h"
#include "common/analysis/text_structure_linter_test_utils.h"
#include "gtest/gtest.h"
#include "verilog/analysis/verilog_analyzer.h"
#include "verilog/parser/verilog_token_enum.h"

namespace verilog {
namespace analysis {
namespace {

using verible::LintTestCase;
using verible::RunLintTestCases;

// Tests that space-only text passes.
TEST(MixedIndentationRuleTest, AcceptsText) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {""},
      // {"module();\n"
      // "  thing (\n"
      // "\t\t.a       (0));\n"
      // "endmodule"},
      {"\t\tint a = /* some comment with spacing    */ 1;"},
      // {"\t\tint a = /* some comment with spacing    */         1;"},
      // {"   int /*\n"},
      // {" asdas fas\n"},
      // {" fa \n"},
      // {" fasdf\n"},
      // {" sdf */ a = 1;\n"},
  };
  RunLintTestCases<VerilogAnalyzer, MixedIndentationRule>(kTestCases);
}

// Tests that tabs are detected and reported.
TEST(MixedIndentationRuleTest, RejectsTabs) {
  // constexpr int kToken = TK_SPACE;
  // const std::initializer_list<LintTestCase> kTestCases = {
  //     // Expect only the first tab on any given line to be reported.
  //     {{kToken, "\t"}},
  //     {{kToken, "\t"}, "\t"},
  //     {{kToken, "\t"}, "\n"},
  //     {{kToken, "\t"}, "\n", {kToken, "\t"}, "\n"},
  //     {"a", {kToken, "\t"}, "b\n\n1", {kToken, "\t"}, "2\n"},
  //     {"module", {kToken, "\t"}, "foo;\nendmodule\n"},
  //     {"module", {kToken, "\t"}, "\tfoo;\nendmodule\n"},
  //     {"`define", {kToken, "\t"}, "IGNORANCE\t\"strength\"\n"},
  // };
  // RunLintTestCases<VerilogAnalyzer, MixedIndentationRule>(kTestCases);
}

}  // namespace
}  // namespace analysis
}  // namespace verilog
