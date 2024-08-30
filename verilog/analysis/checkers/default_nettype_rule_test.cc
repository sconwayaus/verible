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

#include "verilog/analysis/checkers/default_nettype_rule.h"

#include <initializer_list>

#include "common/analysis/linter_test_utils.h"
#include "common/analysis/token_stream_linter_test_utils.h"
#include "gtest/gtest.h"
#include "verilog/analysis/verilog_analyzer.h"
#include "verilog/parser/verilog_token_enum.h"

namespace verilog {
namespace analysis {
namespace {

using verible::LintTestCase;
using verible::RunApplyFixCases;
using verible::RunLintTestCases;

TEST(DefaultNetTypeRuleTest, Accepts) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {"`default_nettype none\nmodule a; endmodule"},
      {"/* Some\nComment\n*/\nimport some_package::*\n`default_nettype "
       "none\nimport another_package\n\nmodule#() a();"},
  };
  RunLintTestCases<VerilogAnalyzer, DefaultNetTypeRule>(kTestCases);
}

TEST(DefaultNetTypeRuleTest, Rejects) {
  const std::initializer_list<LintTestCase> kTestCases = {
      {{TK_module, "module"}, " a; endmodule"},
      {"/* Some\nComment\n*/\nimport some_package::*\nimport "
       "another_package\n\n",
       {TK_module, "module"},
       "#() a();"},
  };
  RunLintTestCases<VerilogAnalyzer, DefaultNetTypeRule>(kTestCases);
}

// TEST(DefaultNetTypeRuleTest, Rejects) {
//   constexpr int kToken = PP_endif;
//   const std::initializer_list<LintTestCase> kTestCases = {
//       {"`ifdef FOO\n", {kToken, "`endif"}, "\n"},
//       {"`ifdef FOO\n", {kToken, "`endif"}, ""},  // missing POSIX newline
//       {"`ifndef FOO\n", {kToken, "`endif"}, "\n"},
//   };
//   RunLintTestCases<VerilogAnalyzer, DefaultNetTypeRule>(kTestCases);
// }

// TEST(DefaultNetTypeRuleTest, ApplyAutoFix) {
//   // Alternatives the auto fix offers
//   constexpr int kEOLComment = 0;
//   constexpr int kBlkComment = 1;
//   const std::initializer_list<verible::AutoFixInOut> kTestCases = {
//       {"`ifdef FOO\n`endif\n", "`ifdef FOO\n`endif  // FOO\n", kEOLComment},
//       {"`ifdef FOO  /*xyz*/\n`endif\n", "`ifdef FOO  /*xyz*/\n`endif  //
//       FOO\n",
//        kEOLComment},
//       {"`ifdef FOO\n`endif\n", "`ifdef FOO\n`endif  /* FOO */\n",
//       kBlkComment},
//   };
//   RunApplyFixCases<VerilogAnalyzer, DefaultNetTypeRule>(kTestCases, "");
// }

}  // namespace
}  // namespace analysis
}  // namespace verilog
