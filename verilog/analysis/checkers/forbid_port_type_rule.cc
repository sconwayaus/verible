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

#include <map>
#include <set>
#include <string>
#include <vector>

#include "absl/strings/match.h"
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
#include "verilog/CST/port.h"
#include "verilog/CST/verilog_matchers.h"
#include "verilog/analysis/descriptions.h"
#include "verilog/analysis/lint_rule_registry.h"

namespace verilog {
namespace analysis {

using verible::LintRuleStatus;
using verible::LintViolation;
using verible::Symbol;
using verible::SyntaxTreeContext;
using verible::TokenInfo;
using verible::matcher::Matcher;

// Register ForbidPortTypeRule.
VERILOG_REGISTER_LINT_RULE(ForbidPortTypeRule);

const LintRuleDescriptor &ForbidPortTypeRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "forbid-port-type-rule",
      .topic = "port-declaration",
      .desc = "Checks port declarations for invalid types.\n",
      .param =
          {{"forbid_input_type_regex", "reg|wire",
            "A regex match pattern that defines forbidden input port types."},
           {"forbid_output_type_regex", "reg|wire",
            "A regex match pattern that defines forbidden output port types."},
           {"forbid_inout_type_regex", "reg|logic",
            "A regex match pattern that defines forbidden inout port types."},
           {"allow_port_implicit_data_type", "false",
            "true: Port are allowed to use an implicit data type.\n"
            "false: Ports with implcicit data types are not allowed."}},
  };
  return d;
}

static const Matcher &PortMatcher() {
  static const Matcher matcher(NodekPortDeclaration());
  return matcher;
}

ForbidPortTypeRule::ForbidPortTypeRule() : verible::SyntaxTreeLintRule() {
  // Do nothing
}

// void ForbidPortTypeRule::Violation(absl::string_view direction,
//                                    const TokenInfo &token,
//                                    const SyntaxTreeContext &context) {
//   if (direction == "input") {
//     violations_.insert(LintViolation(token, kMessageIn, context));
//   } else if (direction == "output") {
//     violations_.insert(LintViolation(token, kMessageOut, context));
//   } else if (direction == "inout") {
//     violations_.insert(LintViolation(token, kMessageInOut, context));
//   } else if (direction == "interface") {
//     violations_.insert(LintViolation(token, "Missing modport", context));
//   } else {
//     // std::cout << "Unknown direction: " << direction << "\n";
//   }
// }

void ForbidPortTypeRule::HandleSymbol(const Symbol &symbol,
                                      const SyntaxTreeContext &context) {
  verible::matcher::BoundSymbolManager manager;
  if (PortMatcher().Matches(symbol, &manager)) {
    const auto *direction_leaf = GetDirectionFromPortDeclaration(symbol);

    absl::string_view suffix_type;

    std::set<absl::string_view> suffix_list;
    if (!direction_leaf) {
      return;
    }

    absl::string_view direction = direction_leaf->get().text();

    // Net port, wire and var
    const auto *signal_type = GetSignalTypeFromPortDeclaration(symbol);
    const auto *data_type = GetDataTypePrimitiveFromPortDeclaration(symbol);

    if (signal_type == nullptr && data_type == nullptr) {
      return;
    }

    const auto *token = signal_type ? signal_type : data_type;
    const absl::string_view type = token->get().text();

    if(direction == "input") {
      if(type == "logic") {
        violations_.insert(LintViolation(*token, "'input logic' invalid. Use 'input wire' instead.", context));
      } else if(type == "var") {
        violations_.insert(LintViolation(*token, "'input var' invalid. Use 'input wire' instead.", context));
      } else if(type == "reg") {
        violations_.insert(LintViolation(*token, "'input reg' invalid. Use 'input wire' instead.", context));
      }
    }
    else if(direction == "inout") {
      if(type == "logic") {
        violations_.insert(LintViolation(*token, "'inout logic' invalid. Use 'inout wire' instead.", context));
      } else if(type == "var") {
        violations_.insert(LintViolation(*token, "'inout var' invalid. Use 'inout wire' instead.", context));
      } else if(type == "reg") {
        violations_.insert(LintViolation(*token, "'inout reg' invalid. Use 'inout wire' instead.", context));
      }
    }
  }
}

absl::Status ForbidPortTypeRule::Configure(absl::string_view configuration) {
  // using verible::config::SetBool;
  // using verible::config::SetStringSetOr;

  // auto status = verible::ParseNameValues(
  //     configuration, {{"input_suffixes", SetStringSetOr(&input_suffixes)},
  //                     {"output_suffixes", SetStringSetOr(&output_suffixes)},
  //                     {"inout_suffixes", SetStringSetOr(&inout_suffixes)},
  //                     {"enable_interface_modport_suffix",
  //                      SetBool(&enable_interface_modport_suffix)}});

  // suffixes.clear();
  // suffixes["input"] = input_suffixes;
  // suffixes["output"] = output_suffixes;
  // suffixes["inout"] = inout_suffixes;

  // // Create some violation messages
  // std::string input_str;
  // std::string output_str;
  // std::string inout_str;

  // ParseViloationString("input", input_suffixes, &kMessageIn);
  // ParseViloationString("output", output_suffixes, &kMessageOut);
  // ParseViloationString("inout", inout_suffixes, &kMessageInOut);

  // if (!status.ok()) return status;

  return absl::OkStatus();
}

LintRuleStatus ForbidPortTypeRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
