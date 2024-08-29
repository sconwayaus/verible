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

#include "verilog/CST/directive.h"

#include <vector>

#include "common/analysis/syntax_tree_search.h"
#include "common/analysis/syntax_tree_search_test_utils.h"
#include "gtest/gtest.h"
#include "verilog/CST/match_test_utils.h"

// #undef ASSERT_OK
// #define ASSERT_OK(value) ASSERT_TRUE((value).ok())
#undef EXPECT_OK
#define EXPECT_OK(value) EXPECT_TRUE((value).ok())

namespace verilog {
namespace {

using verible::SyntaxTreeSearchTestCase;
using verible::TextStructureView;
using verible::TreeSearchMatch;

TEST(FindAllTopLevelDirectives, CountMatches) {
  constexpr int kTag = 1;  // value doesn't matter
  const SyntaxTreeSearchTestCase kTestCases[] = {
      {""},
      {"module m;\nendmodule\n"},

      // DR_unconnected_drive pull01
      {{kTag, "`unconnected_drive pull1"}, "\nmodule a; endmodule"},
      {{kTag, "`unconnected_drive pull0"}, "\nmodule a; endmodule"},

      // DR_default_nettype net_type_or_none
      {{kTag, "`default_nettype wire"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype tri"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype tri0"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype tri1"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype wand"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype triand"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype wor"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype trior"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype trireg"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype uwire"}, "\n module m;\nendmodule\n"},
      {{kTag, "`default_nettype none"}, "\n module m;\nendmodule\n"},

      // DR_default_decay_time decay_value_simple
      {{kTag, "`default_decay_time 100"}, "\nmodule a; endmodule"},
      {{kTag, "`default_decay_time infinite"}, "\nmodule a; endmodule"},

      // DR_default_trireg_strength TK_DecNumber
      {{kTag, "`default_trireg_strength 0"}, "\nmodule a; endmodule"},
      {{kTag, "`default_trireg_strength 125"}, "\nmodule a; endmodule"},
      {{kTag, "`default_trireg_strength 250"}, "\nmodule a; endmodule"},

      // DR_begin_keywords TK_StringLiteral
      {{kTag, "`begin_keywords \"1800-2023\""}, "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1800-2017\""}, "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1800-2012\""}, "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1800-2009\""}, "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1800-2005\""}, "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1364-2005\""}, "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1364-2001\""}, "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1364-2001-noconfig\""},
       "\nmodule a; endmodule"},
      {{kTag, "`begin_keywords \"1364-1995\""}, "\nmodule a; endmodule"},
  };
  for (const auto &test : kTestCases) {
    TestVerilogSyntaxRangeMatches(
        __FUNCTION__, test, [](const TextStructureView &text_structure) {
          const auto &root = text_structure.SyntaxTree();
          return FindAllTopLevelDirectives(*ABSL_DIE_IF_NULL(root));
        });
  }
}

}  // namespace
}  // namespace verilog
