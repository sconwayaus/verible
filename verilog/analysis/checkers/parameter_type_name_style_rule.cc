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

#include "verilog/analysis/checkers/parameter_type_name_style_rule.h"

#include <memory>
#include <set>
#include <string>

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

namespace verilog {
namespace analysis {

VERILOG_REGISTER_LINT_RULE(ParameterTypeNameStyleRule);

using verible::LintRuleStatus;
using verible::LintViolation;
using verible::SyntaxTreeContext;
using verible::matcher::Matcher;

#define STYLE_DEFAULT_REGEX "[a-z_0-9]+(_t)"
static std::string style_default_regex = STYLE_DEFAULT_REGEX;

ParameterTypeNameStyleRule::ParameterTypeNameStyleRule() {
  style_regex_ =
      std::make_unique<re2::RE2>(style_default_regex, re2::RE2::Quiet);

  kMessage =
      absl::StrCat("Parameter type name does not match the naming convention ",
                   "defined by regex pattern: ", style_regex_->pattern());
}

const LintRuleDescriptor &ParameterTypeNameStyleRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "parameter-type-name-style",
      .topic = "parametrized-objects",
      .desc =
          "Checks that parameter type names conform to a naming convention "
          "defined by "
          "a RE2 regular expression.\n"
          "Example common regex patterns:\n"
          "  lower_snake_case: \"[a-z_0-9]+\"\n"
          "  UPPER_SNAKE_CASE: \"[A-Z_0-9]+\"\n"
          "  Title_Snake_Case: \"[A-Z]+[a-z0-9]*(_[A-Z0-9]+[a-z0-9]*)*\"\n"
          "  Sentence_snake_case: \"([A-Z0-9]+[a-z0-9]*_?)([a-z0-9]*_*)*\"\n"
          "  camelCase: \"([a-z0-9]+[A-Z0-9]*)+\"\n"
          "  PascalCaseRegexPattern: \"([A-Z0-9]+[a-z0-9]*)+\"\n"
          "RE2 regular expression syntax documentation can be found at "
          "https://github.com/google/re2/wiki/syntax\n",
      .param = {{"style_regex", STYLE_DEFAULT_REGEX,
                 "A regex used to check parameter type name style."}},
  };
  return d;
}

static const Matcher &ParamDeclMatcher() {
  static const Matcher matcher(NodekParamDeclaration());
  return matcher;
}

void ParameterTypeNameStyleRule::HandleSymbol(
    const verible::Symbol &symbol, const SyntaxTreeContext &context) {
  verible::matcher::BoundSymbolManager manager;
  if (ParamDeclMatcher().Matches(symbol, &manager)) {
    const verible::TokenInfo *param_name_token = nullptr;
    if (!IsParamTypeDeclaration(symbol)) return;

    param_name_token = GetSymbolIdentifierFromParamDeclaration(symbol);
    const auto param_name = param_name_token->text();

    if (!RE2::FullMatch(param_name, *style_regex_)) {
      violations_.insert(LintViolation(*param_name_token, kMessage, context));
    }
  }
}

absl::Status ParameterTypeNameStyleRule::Configure(
    absl::string_view configuration) {
  using verible::config::SetRegex;
  absl::Status s = verible::ParseNameValues(
      configuration, {{"style_regex", SetRegex(&style_regex_)}});
  if (!s.ok()) return s;

  kMessage =
      absl::StrCat("Parameter type name does not match the naming convention ",
                   "defined by regex pattern: ", style_regex_->pattern());

  return absl::OkStatus();
}

LintRuleStatus ParameterTypeNameStyleRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
