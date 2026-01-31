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

#include "verible/verilog/analysis/checkers/mixed-indentation-rule.h"

#include <cstdlib>
#include <set>
#include <string_view>
#include <vector>

#include "absl/strings/str_cat.h"
#include "verible/common/analysis/lint-rule-status.h"
#include "verible/common/strings/line-column-map.h"
#include "verible/common/text/text-structure.h"
#include "verible/common/text/token-info.h"
#include "verible/verilog/analysis/descriptions.h"
#include "verible/verilog/analysis/lint-rule-registry.h"
#include "verible/verilog/parser/verilog-token-enum.h"

namespace verilog {
namespace analysis {

using verible::LintRuleStatus;
using verible::LintViolation;
using verible::TokenInfo;

// Register the lint rule
VERILOG_REGISTER_LINT_RULE(MixedIndentationRule);

static constexpr std::string_view kMessage = "Use spaces, not tabs.";

const LintRuleDescriptor &MixedIndentationRule::GetDescriptor() {
  static const LintRuleDescriptor d{
      .name = "mixed-indentation",
      .topic = "indentation",
      .desc =
          "Checks indentation consistancy within the file. The tries to "
          "autodetect the file indentation based on the leading "
          "indentation. ",
  };
  return d;
}

void MixedIndentationRule::Lint(
    const verible::TextStructureView &text_structure,
    std::string_view filename) {
  // First determine the indentation in the file
  FindFileIndentation(text_structure);

  // Now parse the file for violations
  ParseIndentation(text_structure);
}

/**
 * Checks the whitespace string contains only expected characters
 */
bool MixedIndentationRule::isIndentPure(std::string_view whitespace) {
  std::string_view::size_type num_valid_indent_chars =
      whitespace.find_first_not_of(indent_use_spaces ? ' ' : '\t');
  bool isPure = num_valid_indent_chars == std::string_view::npos;
  if (!isPure) {
    const TokenInfo token(TK_SPACE, whitespace);
    if (indent_use_spaces) {
      violations_.insert(LintViolation(
          token, absl::StrCat("Mixed indentation style using tabs and spaces "
                              "(0). Expected indent style: ",
                              num_indent_spaces, " spaces")));
    } else {
      violations_.insert(LintViolation(
          token, absl::StrCat("Mixed indentation style using tabs and spaces "
                              "(1). Expected indent style: tabs")));
    }
  }

  return isPure;
}

bool MixedIndentationRule::CheckLeadingSpacingIndent(
    std::string_view whitespace) {
  if ((whitespace.length() % num_indent_spaces) != 0) {
    const TokenInfo token(TK_SPACE, whitespace);

    violations_.insert(LintViolation(
        token, absl::StrCat("Incorrect number of spaces used for indentation. "
                            "Expected indent style: ",
                            num_indent_spaces, " spaces")));
    return false;
  }

  return true;
}

void MixedIndentationRule::CheckIndentation(
    const verible::TextStructureView &text_structure,
    std::string_view segment) {
  std::string_view::size_type start_pos = 0;
  do {
    if (indent_use_spaces) {
      // Find the first tab character and report that
      start_pos = segment.find('\t', start_pos);
      if (start_pos != std::string_view::npos) {
        std::string_view::size_type end_pos =
            segment.find_first_not_of('\t', start_pos);
        verible::LineColumnRange range = text_structure.GetRangeForText(
            segment.substr(start_pos, end_pos - start_pos));
        TokenInfo token = text_structure.FindTokenAt(range.start);

        if (token.token_enum() == TK_SPACE) {
          violations_.insert(LintViolation(
              token, absl::StrCat("Mixed indentation style using tabs and "
                                  "spaces (2). Expected indent style: ",
                                  num_indent_spaces, " spaces")));
        }
        start_pos = end_pos;
      }
    } else {
      // Indent using tabs, so check for spaces that are 2 or more
      start_pos = segment.find_first_of(" \t", start_pos);
      if (start_pos != std::string_view::npos) {
        std::string_view::size_type end_pos =
            segment.find_first_not_of(" \t", start_pos);

        std::string_view whitespace =
            segment.substr(start_pos, end_pos - start_pos);

        if (whitespace.length() > 1) {
          verible::LineColumnRange range =
              text_structure.GetRangeForText(whitespace);
          TokenInfo token = text_structure.FindTokenAt(range.start);

          if (token.token_enum() == TK_SPACE) {
            isIndentPure(whitespace);
            // violations_.insert(LintViolation(
            //     token, absl::StrCat("Mixed indentation style using tabs and "
            //                         "spaces (3). Expected indent style:
            //                         tabs")));
          }
        }
        start_pos = end_pos;
      }
    }
  } while (start_pos != std::string_view::npos);
}

void MixedIndentationRule::FindFileIndentation(
    const verible::TextStructureView &text_structure) {
  // Find expected whitespace by looking at the leading space
  const std::vector<std::string_view> &lines = text_structure.Lines();
  int num_lines_starting_with_spaces = 0;
  int num_lines_starting_with_tabs = 0;
  constexpr int NUM_HISTOGRAM_BINS = 5;
  int indents_histogram[NUM_HISTOGRAM_BINS] = {0};
  int last_space_width = 0;
  int last_indent = 0;

  for (std::string_view line : lines) {
    if (line.empty()) {
      continue;
    }

    // Determine indentation from leading space
    std::string_view::size_type pos = line.find_first_not_of(" \t");

    if (pos != std::string_view::npos && pos != 0) {
      std::string_view leading_indent = line.substr(0, pos);

      // Is this spacing (not a comment etc...)
      verible::LineColumnRange range =
          text_structure.GetRangeForText(leading_indent);
      TokenInfo token = text_structure.FindTokenAt(range.start);
      if (token.token_enum() == TK_SPACE) {
        if (leading_indent[0] == ' ') {
          num_lines_starting_with_spaces++;
          std::string_view::size_type num_leading_spaces =
              leading_indent.find_first_not_of(' ');

          if (num_leading_spaces == std::string_view::npos) {
            num_leading_spaces = leading_indent.length();

            int indent = num_leading_spaces - last_space_width;
            indent = std::abs(indent);

            // Accumulate histogram, discard if outside bin range
            if (indent < NUM_HISTOGRAM_BINS) {
              if (indent == 0) {
                indents_histogram[last_indent]++;
              } else {
                indents_histogram[indent]++;
              }
              last_space_width = num_leading_spaces;
              last_indent = indent;
            }
          }
        } else if (line[0] == '\t') {
          num_lines_starting_with_tabs++;
        }
      }
    }
  }

  indent_use_spaces =
      num_lines_starting_with_spaces >= num_lines_starting_with_tabs;

  // Determine the number of spaces to indent
  num_indent_spaces = 2;
  int max = 0;
  if (indent_use_spaces) {
    for (int i = 1; i < NUM_HISTOGRAM_BINS; i++) {
      if (indents_histogram[i] > max) {
        max = indents_histogram[i];
        num_indent_spaces = i;
      }
    }
  }
}

void MixedIndentationRule::ParseIndentation(
    const verible::TextStructureView &text_structure) {
  const std::vector<std::string_view> &lines = text_structure.Lines();
  for (std::string_view line : lines) {
    // Ignore empty lines
    if (line.empty()) {
      continue;
    }

    // Check leading space
    std::string_view::size_type pos = line.find_first_not_of(" \t");

    if (pos != std::string_view::npos && pos != 0) {
      std::string_view leading_indent = line.substr(0, pos);

      // Is this spacing? This avoids all the corners like comments, and strings
      verible::LineColumnRange range =
          text_structure.GetRangeForText(leading_indent);
      TokenInfo token = text_structure.FindTokenAt(range.start);

      if (token.token_enum() == TK_SPACE) {
        bool is_pure = isIndentPure(leading_indent);

        if (is_pure && indent_use_spaces) {
          CheckLeadingSpacingIndent(leading_indent);
        }
      }
      line = line.substr(pos);
    }

    CheckIndentation(text_structure, line);
  }
}

LintRuleStatus MixedIndentationRule::Report() const {
  return LintRuleStatus(violations_, GetDescriptor());
}

}  // namespace analysis
}  // namespace verilog
