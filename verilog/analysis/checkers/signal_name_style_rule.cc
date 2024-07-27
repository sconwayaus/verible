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

#include "verilog/analysis/checkers/signal_name_style_rule.h"

#include <memory>
#include <set>
#include <string>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/matcher/bound_symbol_manager.h"
#include "common/analysis/matcher/matcher.h"
#include "common/text/concrete_syntax_leaf.h"
#include "common/text/config_utils.h"
#include "common/text/symbol.h"
#include "common/text/syntax_tree_context.h"
#include "common/text/token_info.h"
#include "common/util/logging.h"
#include "re2/re2.h"
#include "verilog/CST/data.h"
#include "verilog/CST/net.h"
#include "verilog/CST/verilog_matchers.h"
#include "verilog/analysis/descriptions.h"
#include "verilog/analysis/lint_rule_registry.h"

namespace verilog {
namespace analysis {

VERILOG_REGISTER_LINT_RULE(SignalNameStyleRule);

using verible::LintRuleStatus;
using verible::LintViolation;
using verible::SyntaxTreeContext;
using verible::matcher::Matcher;

static constexpr absl::string_view style_default_regex = "[a-z_0-9]+";

SignalNameStyleRule::SignalNameStyleRule()
    : style_regex_(
          std::make_unique<re2::RE2>(style_default_regex, re2::RE2::Quiet)) {}

const LintRuleDescriptor &SignalNameStyleRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "signal-name-style",
      .topic = "signal-conventions",
      .desc =
          "Checks that signal names conform to a naming convention defined by "
          "a RE2 regular expression. Signals are defined as \"a net or "
          "variable within a SystemVerilog design\". The default regex pattern "
          "expects \"lower_snake_case\". Refer to "
          "https://github.com/chipsalliance/verible/tree/master/verilog/tools/"
          "lint#readme for more detail on verible regex patterns.",
      .param = {{"style_regex", std::string(style_default_regex),
                 "A regex used to check signal names style."}},
  };
  return d;
}

static const Matcher &NetMatcher() {
  static const Matcher matcher(NodekNetDeclaration());
  return matcher;
}

static const Matcher &DataMatcher() {
  static const Matcher matcher(NodekDataDeclaration());
  return matcher;
}

std::string SignalNameStyleRule::CreateViolationMessage() {
  return absl::StrCat("Signal name does not match the naming convention ",
                      "defined by regex pattern: ", style_regex_->pattern());
}

void SignalNameStyleRule::HandleSymbol(const verible::Symbol &symbol,
                                       const SyntaxTreeContext &context) {
  verible::matcher::BoundSymbolManager manager;
  if (NetMatcher().Matches(symbol, &manager)) {
    const auto identifier_leaves = GetIdentifiersFromNetDeclaration(symbol);
    for (const auto *leaf : identifier_leaves) {
      const auto name = leaf->text();
      if (!RE2::FullMatch(name, *style_regex_)) {
        violations_.insert(
            LintViolation(*leaf, CreateViolationMessage(), context));
      }
    }
  } else if (DataMatcher().Matches(symbol, &manager)) {
    const auto identifier_leaves = GetIdentifiersFromDataDeclaration(symbol);
    for (const auto *leaf : identifier_leaves) {
      const auto name = leaf->text();
      if (!RE2::FullMatch(name, *style_regex_)) {
        violations_.insert(
            LintViolation(*leaf, CreateViolationMessage(), context));
      }
    }
  }
}

absl::Status SignalNameStyleRule::Configure(absl::string_view configuration) {
  using verible::config::SetRegex;
  absl::Status s = verible::ParseNameValues(
      configuration, {{"style_regex", SetRegex(&style_regex_)}});
  return s;
}

LintRuleStatus SignalNameStyleRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
