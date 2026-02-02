// Copyright 2017-2021 The Verible Authors.
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

#include "verible/verilog/analysis/checkers/literal-digit-seperator-rule.h"

#include <cctype>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "absl/status/status.h"
#include "absl/strings/numbers.h"
#include "absl/strings/str_cat.h"
#include "verible/common/analysis/lint-rule-status.h"
#include "verible/common/analysis/matcher/bound-symbol-manager.h"
#include "verible/common/analysis/matcher/matcher.h"
#include "verible/common/text/concrete-syntax-leaf.h"
#include "verible/common/text/concrete-syntax-tree.h"
// #include "verible/common/text/config-utils.h"
#include "verible/common/text/symbol.h"
#include "verible/common/text/syntax-tree-context.h"
#include "verible/common/text/token-info.h"
#include "verible/common/text/tree-utils.h"
#include "verible/common/util/logging.h"
#include "verible/verilog/CST/numbers.h"
#include "verible/verilog/CST/verilog-matchers.h"
#include "verible/verilog/analysis/descriptions.h"
#include "verible/verilog/analysis/lint-rule-registry.h"

namespace verilog {
namespace analysis {

using verible::AutoFix;
using verible::down_cast;
using verible::LintRuleStatus;
using verible::LintViolation;
using verible::SyntaxTreeContext;
using verible::SyntaxTreeLeaf;
using verible::SyntaxTreeNode;
using verible::matcher::Matcher;

// Register LiteralDigitSeperatorRule
VERILOG_REGISTER_LINT_RULE(LiteralDigitSeperatorRule);

const LintRuleDescriptor& LiteralDigitSeperatorRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "literal-digit-seperator",
      .topic = "number-literals",
      .desc =
          "",
      .param = {
          // {"bin", "true", "Checking binary 'b literals."},
          // {"oct", "false", "Checking octal 'o literals."},
          // {"hex", "false", "Checking hexadecimal 'h literals."},
          // {"lint_zero", "false",
          //  "Also generate a lint warning for value zero such as `32'h0`; "
          //  "autofix suggestions would be to zero-expand or untype `'0`."},
          // {"autofix", "true",
          //  "Provide autofix suggestions, e.g. "
          //  "32'hAB provides suggested fix 32'h000000AB."},
      }};
  return d;
}

std::string* LiteralDigitSeperatorRule::GetExpectedLiteral(std::string *literal, 
int digit_spacing, int min_digits_to_seperate, std::string *expected_literal) {
  // // Filter out underscores.

  // int literal_length = literal->length();
  // if(literal_length <= min_digits_to_seperate) {
  //   expected_literal = literal;
  // } else {
  //   int reserve_length = literal_length + std::ceil(literal_length / (float)digit_spacing);

  //   expected_literal->reserve(reserve_length);

  //   int i = 0;
  //   for (auto literal_ch = literal->rbegin(); literal_ch < literal->rend(); literal_ch++) {
  //     if(i == 0 && literal_ch != literal->rbegin()) {
  //       *expected_literal = '_' + *expected_literal;
  //     }
  //     *expected_literal = *literal_ch + *expected_literal;
  //     i = (i + 1) % digit_spacing;
  //   }
  // }

  return expected_literal;
}

void LiteralDigitSeperatorRule::HandleNumberDigitSeperation(const verible::TokenInfo &token, 
int digit_spacing, int min_digits_to_seperate, std::string_view base) {

  std::string_view literal = token.text();
  // std::string literal;
  // literal.reserve(digits.length());
  // std::remove_copy(digits.begin(), digits.end(), std::back_inserter(literal),'_');

  // std::string *expected_literal;
  // GetExpectedLiteral(&literal, digit_spacing, min_digits_to_seperate, expected_literal);

  // if(token.text() != *expected_literal) {
  //   auto autofix =
  //     verible::AutoFix("Update literal", {token.text(), *expected_literal});

  //   verible::LintViolation violation = verible::LintViolation(
  //       token, absl::StrCat("Literals should use underscore to seperate groups of digits. Expected: ", *expected_literal),
  //       {autofix});
    
  //   violations_.insert(violation);
  // }

  // No underscores when literal length is <= min_digits_to_seperate
  if(literal.length() <= min_digits_to_seperate) {
    bool contains_underscore = false;
    for(char ch: literal) {
      if(ch == '_') {
        contains_underscore = true;
        break;
      }
    }

    if(!contains_underscore) {
      // Ignore the literal
      return;
    }
  }

  // 
  int index = 0;
  int mod = digit_spacing + 1;
  int invalid_format = false;
  for (auto it = literal.rbegin(); it != literal.rend(); ++it) 
  {
    if(index == digit_spacing) {
      if(*it != '_') {
        invalid_format = true;
        break;
      }
    } else {
      if(*it == '_') {
        invalid_format = true;
        break;
      }
    }
    index = (index + 1) % mod;
  }

  // Literals should not start with an underscore
  if(literal[0] == '_') {
    invalid_format = true;
  }

  if(invalid_format) {
    verible::LintViolation violation = verible::LintViolation(
        token, absl::StrCat("Literals should use underscore to seperate groups of digits.")); // Expected: ", *expected_literal),
        //{autofix});
    
    violations_.insert(violation);
  }
}

void LiteralDigitSeperatorRule::HandleToken(const verible::TokenInfo &token) {
  switch(token.token_enum()) {
    case TK_BinDigits:
      VLOG(2) << "TK_BinDigits: " << token.text();
      HandleNumberDigitSeperation(token, 4, 0, "Binary");
      break;
    case TK_OctDigits:
      VLOG(2) << "TK_OctDigits: " << token.text();
      HandleNumberDigitSeperation(token, 3, 0, "Octal");
      break;
    case TK_DecDigits:
    case TK_DecNumber:
      VLOG(2) << "TK_DecDigits or TK_DecNumber: " << token.text();
      HandleNumberDigitSeperation(token, 3, 4, "Decimal");
      break;
    case TK_HexDigits:
      VLOG(2) << "TK_HexDigits: " << token.text();
      // FIXME: Also want 2 digit seperators
      HandleNumberDigitSeperation(token, 4, 0, "Binary");
      break;
    default:
      return;
  }

  // if(NumberHasBinaryDigits(token)) {
  //   VLOG(0) << "Binary";
  // }

  // if(NumberHasBinaryDigits(token)) {
  //   VLOG(0) << "Binary";
  // }


  // VLOG(0) << "HERE";
  // if (!NumberMatcher().Matches(symbol, &manager)) return;
  
  // VLOG(0) << symbol.Tag().tag;
  // const auto *leaf = SymbolCastToLeaf(symbol);
  // const auto *unwrap = AutoUnwrapIdentifier(*leaf);
  
}

// // Generate string representation of why lint error occurred at leaf
// std::string LiteralDigitSeperatorRule::FormatReason(
//     std::string_view width, std::string_view base_text, char base,
//     std::string_view literal) {
//   std::string_view base_describe;
//   switch (base) {
//     case 'b':
//       base_describe = "Binary";
//       break;
//     case 'h':
//       base_describe = "Hex";
//       break;
//     case 'o':
//       base_describe = "Octal";
//       break;
//     default:
//       LOG(FATAL) << "Unexpected base";
//   }
//   return absl::StrCat(base_describe, " literal ", width, base_text, literal,
//                       " has less digits than expected for ", width, " bits.");
// }

absl::Status LiteralDigitSeperatorRule::Configure(
    std::string_view configuration) {
  return absl::OkStatus();
  // using verible::config::SetBool;
  // return verible::ParseNameValues(configuration,
  //                                 {{"bin", SetBool(&check_bin_numbers_)},
  //                                  {"hex", SetBool(&check_hex_numbers_)},
  //                                  {"oct", SetBool(&check_oct_numbers_)},
  //                                  {"lint_zero", SetBool(&lint_zero_)},
  //                                  {"autofix", SetBool(&autofix_)}});
}

LintRuleStatus LiteralDigitSeperatorRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
