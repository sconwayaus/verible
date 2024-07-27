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

#include "verilog/analysis/checkers/port_name_style_rule.h"

#include <map>
#include <set>
#include <string>
#include <vector>

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "common/analysis/lint_rule_status.h"
#include "common/analysis/matcher/bound_symbol_manager.h"
#include "common/analysis/matcher/matcher.h"
#include "common/text/config_utils.h"
#include "common/text/symbol.h"
#include "common/text/syntax_tree_context.h"
#include "common/text/token_info.h"
#include "common/util/logging.h"
#include "re2/re2.h"
#include "verilog/CST/port.h"
#include "verilog/CST/verilog_matchers.h"
#include "verilog/analysis/descriptions.h"
#include "verilog/analysis/lint_rule_registry.h"

namespace verilog {
namespace analysis {

VERILOG_REGISTER_LINT_RULE(PortNameStyleRule);

using verible::LintRuleStatus;
using verible::LintViolation;
using verible::Symbol;
using verible::SyntaxTreeContext;
using verible::TokenInfo;
using verible::matcher::Matcher;

static constexpr absl::string_view input_style_default_regex =
    "[a-z_0-9]+(_i|_ni|_pi)";
static constexpr absl::string_view output_style_default_regex =
    "[a-z_0-9]+(_o|_no|_po)";
static constexpr absl::string_view inout_style_default_regex =
    "[a-z_0-9]+(_io|_nio|_pio)";
static constexpr absl::string_view interface_style_default_regex = "[a-z_0-9]+";

constexpr absl::string_view kInputTitleCase = "Input";
constexpr absl::string_view kOutputTitleCase = "Output";
constexpr absl::string_view kInoutTitleCase = "Inout";
constexpr absl::string_view kInterfaceTitleCase = "Interface";

PortNameStyleRule::PortNameStyleRule()
    : input_style_regex_(std::make_unique<re2::RE2>(input_style_default_regex,
                                                    re2::RE2::Quiet)),
      output_style_regex_(std::make_unique<re2::RE2>(output_style_default_regex,
                                                     re2::RE2::Quiet)),
      inout_style_regex_(std::make_unique<re2::RE2>(inout_style_default_regex,
                                                    re2::RE2::Quiet)),
      interface_style_regex_(std::make_unique<re2::RE2>(
          interface_style_default_regex, re2::RE2::Quiet)),
      enable_interface_modport_suffix(false) {}

const LintRuleDescriptor &PortNameStyleRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "port-name-style",
      .topic = "port-conventions",
      .desc =
          "Checks that port names conform to a naming convention defined by "
          "RE2 regular expressions. The default regex pattern for port "
          "namesexpects \"lower_snake_case\" with inputs ending in "
          "\"_i|_ni|_pi\", output ports ending in \"_o|_no|_po\" and inout "
          "ports ending in \"_io|_nio|_pio\". Refer to "
          "https://github.com/chipsalliance/verible/tree/master/verilog/tools/"
          "lint#readme for more detail on verible regex patterns.",
      .param = {{"input_style_regex", std::string(input_style_default_regex),
                 "A regex used to check input port names style."},
                {"output_style_regex", std::string(output_style_default_regex),
                 "A regex used to check output port names style."},
                {"inout_style_regex", std::string(inout_style_default_regex),
                 "A regex used to check inout port names style."},
                {"interface_style_regex",
                 std::string(interface_style_default_regex),
                 "A regex used to check interface port names style."},
                {"enable_interface_modport_suffix", "false",
                 "Checks that the interface port names ends with the modport "
                 "name."}},
  };
  return d;
}

static const Matcher &PortMatcher() {
  static const Matcher matcher(NodekPortDeclaration());
  return matcher;
}

std::string PortNameStyleRule::CreateViolationMessage(
    absl::string_view direction, const re2::RE2 &regex) {
  return absl::StrCat(direction,
                      " port name does not match the naming convention ",
                      "defined by regex pattern: ", regex.pattern());
}

void PortNameStyleRule::HandleSymbol(const Symbol &symbol,
                                     const SyntaxTreeContext &context) {
  verible::matcher::BoundSymbolManager manager;
  if (PortMatcher().Matches(symbol, &manager)) {
    const auto *interface_header_node =
        GetInterfaceHeaderNodeFromPortDeclaration(symbol);
    const auto *direction_leaf = GetDirectionFromPortDeclaration(symbol);

    const auto *identifier_leaf = GetIdentifierFromPortDeclaration(symbol);
    const auto token = identifier_leaf->get();
    const auto name = token.text();

    if (direction_leaf) {
      absl::string_view direction = direction_leaf->get().text();
      re2::RE2 *style_regex;
      absl::string_view msg_direction;

      if (direction == "input") {
        style_regex = input_style_regex_.get();
        msg_direction = kInputTitleCase;
      } else if (direction == "output") {
        style_regex = output_style_regex_.get();
        msg_direction = kOutputTitleCase;
      } else if (direction == "inout") {
        style_regex = inout_style_regex_.get();
        msg_direction = kInoutTitleCase;
      } else {
        return;
      }

      if (!RE2::FullMatch(name, *style_regex)) {
        violations_.insert(LintViolation(
            token, CreateViolationMessage(msg_direction, *style_regex),
            context));
      }
    } else if (interface_header_node) {
      if (!RE2::FullMatch(name, *interface_style_regex_)) {
        violations_.insert(
            LintViolation(token,
                          CreateViolationMessage(kInterfaceTitleCase,
                                                 *interface_style_regex_),
                          context));
      }

      if (!enable_interface_modport_suffix) {
        return;
      }
      const auto *modport_leaf =
          GetInterfaceModPortFromInterfaceHeaderNode(*interface_header_node);
      if (modport_leaf) {
        absl::string_view modport_suffix = modport_leaf->get().text();
        std::string suffix = absl::AsciiStrToLower(modport_suffix);
        std::string name_lc = absl::AsciiStrToLower(name);
        if (!absl::EndsWith(name_lc, suffix)) {
          violations_.insert(LintViolation(
              token,
              absl::StrCat("Interface dosn't end with modport name \"",
                           modport_suffix, "\""),
              context));
        }
      }
    } else {
      // Unknown
      return;
    }
  }
}

absl::Status PortNameStyleRule::Configure(absl::string_view configuration) {
  using verible::config::SetBool;
  using verible::config::SetRegex;

  absl::Status s = verible::ParseNameValues(
      configuration,
      {{"input_style_regex", SetRegex(&input_style_regex_)},
       {"output_style_regex", SetRegex(&output_style_regex_)},
       {"inout_style_regex", SetRegex(&inout_style_regex_)},
       {"interface_style_regex", SetRegex(&interface_style_regex_)},
       {"enable_interface_modport_suffix",
        SetBool(&enable_interface_modport_suffix)}});

  return s;
}

LintRuleStatus PortNameStyleRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
