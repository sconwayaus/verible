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

#include "verilog/analysis/checkers/parameter_name_style_rule.h"

#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/matcher/bound_symbol_manager.h"
#include "common/analysis/matcher/matcher.h"
#include "common/text/config_utils.h"
#include "common/text/symbol.h"
#include "common/text/syntax_tree_context.h"
#include "common/text/token_info.h"
#include "re2/re2.h"
#include "verilog/CST/parameters.h"
#include "verilog/CST/verilog_matchers.h"
#include "verilog/analysis/descriptions.h"
#include "verilog/analysis/lint_rule_registry.h"
#include "verilog/parser/verilog_token_enum.h"

namespace verilog {
namespace analysis {

VERILOG_REGISTER_LINT_RULE(ParameterNameStyleRule);

using verible::LintRuleStatus;
using verible::LintViolation;
using verible::SyntaxTreeContext;
using Matcher = verible::matcher::Matcher;

// PascalCase, may end in _[0-9]+
#define LOCALPARAM_DEFAULT_REGEX "([A-Z0-9]+[a-z0-9]*)+(_[0-9]+)?"
static constexpr absl::string_view localparam_default_regex =
    LOCALPARAM_DEFAULT_REGEX;

// PascalCase (may end in _[0-9]+) or UPPER_SNAKE_CASE
#define PARAMETER_DEFAULT_REGEX "(([A-Z0-9]+[a-z0-9]*)+(_[0-9]+)?)|([A-Z_0-9]+)"
static constexpr absl::string_view parameter_default_regex =
    PARAMETER_DEFAULT_REGEX;

ParameterNameStyleRule::ParameterNameStyleRule() {
  localparam_style_regex_ =
      std::make_unique<re2::RE2>(localparam_default_regex, re2::RE2::Quiet);

  parameter_style_regex_ =
      std::make_unique<re2::RE2>(parameter_default_regex, re2::RE2::Quiet);

  kLocalparamErrorMessage = absl::StrCat(
      "Localparam name does not match the naming convention ",
      "defined by regex pattern: ", localparam_style_regex_->pattern());

  kParameterErrorMessage = absl::StrCat(
      "Parameter name does not match the naming convention ",
      "defined by regex pattern: ", parameter_style_regex_->pattern());
}

const LintRuleDescriptor &ParameterNameStyleRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "parameter-name-style",
      .topic = "constants",
      .desc =
          "Checks that parameter and localparm names conform to a naming "
          "convention defined by "
          "a RE2 regular expression.\n"
          "Example common regex patterns:\n"
          "  lower_snake_case: \"[a-z_0-9]+\"\n"
          "  UPPER_SNAKE_CASE: \"[A-Z_0-9]+\"\n"
          "  Title_Snake_Case: \"[A-Z]+[a-z0-9]*(_[A-Z0-9]+[a-z0-9]*)*\"\n"
          "  Sentence_snake_case: \"([A-Z0-9]+[a-z0-9]*_?)([a-z0-9]*_*)*\"\n"
          "  camelCase: \"([a-z0-9]+[A-Z0-9]*)+\"\n"
          "  PascalCaseRegexPattern: \"([A-Z0-9]+[a-z0-9]*)+\"\n"
          "  No style enforcement: \".*\"\n"
          "RE2 regular expression syntax documentation can be found at "
          "https://github.com/google/re2/wiki/syntax\n",
      .param = {{"localparam_style_regex", LOCALPARAM_DEFAULT_REGEX,
                 "A regex used to check localparam name style."},
                {"parameter_style_regex", PARAMETER_DEFAULT_REGEX,
                 "A regex used to check parameter name style."}},
  };

  return d;
}

static const Matcher &ParamDeclMatcher() {
  static const Matcher matcher(NodekParamDeclaration());
  return matcher;
}

void ParameterNameStyleRule::HandleSymbol(const verible::Symbol &symbol,
                                          const SyntaxTreeContext &context) {
  verible::matcher::BoundSymbolManager manager;
  if (ParamDeclMatcher().Matches(symbol, &manager)) {
    if (IsParamTypeDeclaration(symbol)) return;

    const verilog_tokentype param_decl_token = GetParamKeyword(symbol);

    auto identifiers = GetAllParameterNameTokens(symbol);

    for (const auto *id : identifiers) {
      const auto name = id->text();
      switch (param_decl_token) {
        case TK_localparam:
          if (!RE2::FullMatch(name, *localparam_style_regex_)) {
            violations_.insert(
                LintViolation(*id, kLocalparamErrorMessage, context));
          }
          break;

        case TK_parameter:
          if (!RE2::FullMatch(name, *parameter_style_regex_)) {
            violations_.insert(
                LintViolation(*id, kParameterErrorMessage, context));
          }
          break;

        default:
          break;
      }
    }
  }
}

absl::Status ParameterNameStyleRule::Configure(
    absl::string_view configuration) {
  using verible::config::SetRegex;
  absl::Status s = verible::ParseNameValues(
      configuration,
      {{"localparam_style_regex", SetRegex(&localparam_style_regex_)},
       {"parameter_style_regex", SetRegex(&parameter_style_regex_)}});

  if (!s.ok()) return s;

  kLocalparamErrorMessage = absl::StrCat(
      "Localparam name does not match the naming convention ",
      "defined by regex pattern: ", localparam_style_regex_->pattern());

  kParameterErrorMessage = absl::StrCat(
      "Parameter name does not match the naming convention ",
      "defined by regex pattern: ", parameter_style_regex_->pattern());

  return absl::OkStatus();
}

LintRuleStatus ParameterNameStyleRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
