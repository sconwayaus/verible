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

#include <set>
#include <string>
#include <vector>

#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/matcher/bound_symbol_manager.h"
#include "common/analysis/matcher/matcher.h"
#include "common/text/concrete_syntax_leaf.h"
#include "common/text/concrete_syntax_tree.h"
#include "common/text/symbol.h"
#include "common/text/syntax_tree_context.h"
#include "common/text/tree_utils.h"
#include "verilog/CST/statement.h"
#include "verilog/CST/verilog_matchers.h"  // IWYU pragma: keep
#include "verilog/analysis/descriptions.h"
#include "verilog/analysis/lint_rule_registry.h"

namespace verilog {
namespace analysis {

using verible::AutoFix;
using verible::LintRuleStatus;
using verible::LintViolation;
using verible::SyntaxTreeContext;
using verible::matcher::Matcher;

// Register AlwaysBlockRule
VERILOG_REGISTER_LINT_RULE(AlwaysBlockRule);

static constexpr absl::string_view kMessage =
    "Use 'always_ff', 'always_comb', or 'always_latch' instead of 'always @'.";

const LintRuleDescriptor &AlwaysBlockRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "always-block",
      .topic = "logic",
      .desc =
          "Checks that there are no occurrences of `always @`. Use "
          "`always_comb`, `always_ff` or `always_latch` instead.",
  };
  return d;
}

static const Matcher &AlwaysBlockMatcher() {
  static const Matcher matcher(
      NodekAlwaysStatement(AlwaysKeyword(), AlwaysBlockStatement()));
  return matcher;
}

static const Matcher &AlwaysStarMatcher() {
  static const Matcher matcher(NodekAlwaysStatement(
      AlwaysKeyword(), AlwaysStatementHasEventControlStar()));
  return matcher;
}

static const Matcher &AlwaysStarMatcherWithParentheses() {
  static const Matcher matcher(NodekAlwaysStatement(
      AlwaysKeyword(), AlwaysStatementHasEventControlStarAndParentheses()));
  return matcher;
}

void AlwaysBlockRule::HandleSymbol(const verible::Symbol &symbol,
                                   const SyntaxTreeContext &context) {
  verible::matcher::BoundSymbolManager manager;

  bool always_block = AlwaysBlockMatcher().Matches(symbol, &manager);

  if (!always_block) {
    return;
  }

  bool always_no_paren = AlwaysStarMatcher().Matches(symbol, &manager);
  bool always_paren =
      AlwaysStarMatcherWithParentheses().Matches(symbol, &manager);
  if (always_no_paren || always_paren) {
    return;
  }

  violations_.insert(LintViolation(symbol, kMessage, context));
}

LintRuleStatus AlwaysBlockRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
