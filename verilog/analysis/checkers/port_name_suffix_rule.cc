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

#include "verilog/analysis/checkers/port_name_suffix_rule.h"

#include <map>
#include <set>
#include <string>
#include <vector>

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

// Register PortNameSuffixRule.
VERILOG_REGISTER_LINT_RULE(PortNameSuffixRule);

const LintRuleDescriptor &PortNameSuffixRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "port-name-suffix",
      .topic = "suffixes-for-signals-and-types",
      .desc = "Checks that port names and interface ports include a "
              "suffix.\n"
              "Ports with a direction include one of the "
              "listed suffixes in \"input_suffixes\", "
              "\"output_suffixes\" and \"inout_suffixes\", which "
              "contain lists of ORed suffixes with the pipe-symbol(|).\n"
              "Interface port suffix are controlled by "
              "\"interface_suffix_style\". \"_modport\" will check that "
              "the interface name ends an underscore, followed by the "
              "mod port."
              "Empty configuration: no style enforcement.",
      .param = {{"input_suffixes", "_i|_ni|_pi",
                 "A list of allowed input port name suffixes."},
                 {"output_suffixes", "_o|_no|_po",
                 "A list of allowed output port name suffixes."},
                 {"inout_suffixes", "_io|_nio|_pio",
                 "A list of allowed inout port name suffixes."},
                 {"interface_suffix_style", "_modport", 
                 "Sets the interface port name suffix style."}},
  };
  return d;
}

static const Matcher &PortMatcher() {
  static const Matcher matcher(NodekPortDeclaration());
  return matcher;
}

PortNameSuffixRule::PortNameSuffixRule() : verible::SyntaxTreeLintRule() {
  
  // FIXME: This sucks, as we now have 2 sources of truth, here and 
  // in GetDescriptor().param above
  input_suffixes = {"_i", "_ni", "_pi"};
  output_suffixes = {"_o", "_no", "_po"};
  inout_suffixes = {"_io", "_nio", "_pio"};
  interface_suffix_style = "_modport";

  suffixes.clear();
  suffixes["input"] = input_suffixes;
  suffixes["output"] = output_suffixes;
  suffixes["inout"] = inout_suffixes;
}

void PortNameSuffixRule::Violation(absl::string_view direction,
                                   const TokenInfo &token,
                                   const SyntaxTreeContext &context) {
  // std::cout << "Violation token: " << token.text() << "\n";

  if (direction == "input") {
    violations_.insert(LintViolation(token, kMessageIn, context));
  } else if (direction == "output") {
    violations_.insert(LintViolation(token, kMessageOut, context));
  } else if (direction == "inout") {
    violations_.insert(LintViolation(token, kMessageInOut, context));
  } else if (direction == "interface") {
    violations_.insert(LintViolation(token, "Missing modport", context));
  } else {
    // std::cout << "Unknown direction: " << direction << "\n";
  }
}

void PortNameSuffixRule::HandleSymbol(const Symbol &symbol,
                                      const SyntaxTreeContext &context) {
  verible::matcher::BoundSymbolManager manager;
  if (PortMatcher().Matches(symbol, &manager)) {

    // std::cout << "\n\n" << verible::RawTreePrinter(symbol);

    const auto *interface_header_node = GetInterfaceHeaderNodeFromPortDeclaration(symbol);
    const auto *direction_leaf = GetDirectionFromPortDeclaration(symbol);
    
    absl::string_view suffix_type;

    std::set<absl::string_view> suffix_list;
    if(direction_leaf) {
      suffix_type = direction_leaf->get().text();

      // std::cout << "Found Direction Leaf, direction = " << suffix_type << "\n";

      if(suffixes.empty()) {
        // std::cout << "Empty\n";
      }
      else {
        // std::cout << "NOT Empty\n";
      }

      if (suffixes.find(suffix_type) == suffixes.end()) {
        // std::cout << "*** Didn't find suffixes\n";
        return;
      }

      // std::cout << "FOUND SUFFIX\n";

      suffix_list = suffixes.at(suffix_type);
      // std::cout << "FOUND SUFFIX 2\n";
    } else if(interface_header_node) {
      suffix_type = "interface";
      if(interface_suffix_style == "_modport") {
        const auto *modport_leaf = GetInterfaceModPortFromInterfaceHeaderNode(*interface_header_node);
        // std::cout << "Get Leaf\n";
        if(modport_leaf) {
          absl::string_view modport = modport_leaf->get().text();
          std::string modport_suffix = "_";
          modport_suffix.append(std::string(modport));
          suffix_list.insert(modport_suffix);
        } else {
          // std::cout << "no modport found" << "\n";
        }
      }
    } else {
      // Unknown
      // std::cout << "Unknown port" << "\n";
      return;
    }
    
    const auto *identifier_leaf = GetIdentifierFromPortDeclaration(symbol);
    const auto token = identifier_leaf->get();
    const auto name = ABSL_DIE_IF_NULL(identifier_leaf)->get().text();

    if(IsSuffixOk(name, suffix_list)) {
      return;
    }

    // No match found, report a rule violation
    Violation(suffix_type, token, context);
  }
}

bool PortNameSuffixRule::IsSuffixOk(const absl::string_view &name, const std::set<absl::string_view> &suffix_list) {
  if(suffix_list.empty()) {
    // No style enforcement
    // std::cout << "No style enforcment\n";
    return true;
  }

  absl::string_view::size_type name_length = name.length();
  // std::cout << "length: " << name_length << "\n";

  for (const auto& suffix : suffix_list) {
    absl::string_view::size_type idx = name.rfind(suffix);
    
    if(idx == std::string::npos) {
      // std::cout << "IsSuffixOk: npos\n";
      continue;
    }

    absl::string_view::size_type expected_name_length = suffix.length() + idx;
    if(expected_name_length == name_length) {
      
      // if(name == "_io") { 
        // std::cout << "expected_name_length: " << expected_name_length << "\n";
        // std::cout << "name_length: " << name_length << "\n";
        // std::cout << "idx: " << idx << "\n";
      // }

      if(idx == 0) {
        // The string needs to be more than the suffix. This should raise 
        // a different Violation
        return false;
      }

      // Found one of the listed suffix
      // std::cout << suffix << "\n";
      return true;
    }
  }

  return false;
}

void PortNameSuffixRule::ParseViloationString(const absl::string_view direction, 
    std::set<absl::string_view> suffix_list, std::string *msg) const {
  
  msg->assign(std::string(direction));
  msg->append(" port names must end with one of the folowing: ");

  bool first = true;
  for( auto k : suffix_list) {
    if(!first) {
      msg->append(", ");
    }
    msg->append(std::string(k));
    first = false;
  }
}

absl::Status PortNameSuffixRule::Configure(
    absl::string_view configuration) {
  using verible::config::SetStringSetOr;
  using verible::config::SetString;

  auto status = verible::ParseNameValues(
      configuration, {{"input_suffixes", SetStringSetOr(&input_suffixes)},
                      {"output_suffixes", SetStringSetOr(&output_suffixes)},
                      {"inout_suffixes", SetStringSetOr(&inout_suffixes)},
                      {"interface_suffix_style", SetString(&interface_suffix_style)}});

  suffixes.clear();
  suffixes["input"] = input_suffixes;
  suffixes["output"] = output_suffixes;
  suffixes["inout"] = inout_suffixes;

  // Create some violation messages
  std::string input_str;
  std::string output_str;
  std::string inout_str;

  ParseViloationString("input", input_suffixes, &kMessageIn);
  ParseViloationString("output", output_suffixes, &kMessageOut);
  ParseViloationString("inout", inout_suffixes, &kMessageInOut);

  if (!status.ok()) return status;

  return absl::OkStatus();
}

LintRuleStatus PortNameSuffixRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
