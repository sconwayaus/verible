// Copyright 2021 The Verible Authors.
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
//

#include "verilog/tools/ls/symbol-table-handler.h"

#include <filesystem>

#include "common/strings/line_column_map.h"

namespace verilog {

static constexpr absl::string_view fileschemeprefix = "file://";

absl::string_view LSPUriToPath(absl::string_view uri) {
  if (!absl::StartsWith(uri, fileschemeprefix)) return "";
  return uri.substr(fileschemeprefix.size());
}

std::string PathToLSPUri(absl::string_view path) {
  std::filesystem::path p(path.begin(), path.end());
  return absl::StrCat(fileschemeprefix, std::filesystem::absolute(p).string());
}

void SymbolTableHandler::setProject(
    absl::string_view root, const std::vector<std::string> &include_paths,
    absl::string_view corpus) {
  currproject = std::make_unique<VerilogProject>(root, include_paths, corpus);
  resetSymbolTable();
}

void SymbolTableHandler::resetSymbolTable() {
  checkedfiles.clear();
  symboltable = std::make_unique<SymbolTable>(currproject.get());
}

void SymbolTableHandler::buildSymbolTableFor(VerilogSourceFile &file) {
  auto result = BuildSymbolTable(file, symboltable.get(), currproject.get());
}

void SymbolTableHandler::buildProjectSymbolTable() {
  resetSymbolTable();
  if (!currproject) {
    return;
  }
  LOG(INFO) << "Parsing project files...";
  for (const auto &file : *currproject) {
    auto status = file.second->Parse();
    if (!status.ok()) {
      LOG(ERROR) << "Failed to parse file:  " << file.second->ReferencedPath();
      return;
    }
    LOG(INFO) << "Successfully parsed:  " << file.second->ReferencedPath();
    auto result =
        BuildSymbolTable(*file.second, symboltable.get(), currproject.get());
  }
  LOG(INFO) << "Parsed project files";
  LOG(INFO) << "Symbol table for the project";
  symboltable->PrintSymbolDefinitions(std::cerr);
}

const SymbolTableNode *SymbolTableHandler::ScanSymbolTreeForDefinition(
    const SymbolTableNode *context, absl::string_view symbol) {
  if (!context) {
    return nullptr;
  }
  if (context->Key() && *context->Key() == symbol) {
    return context;
  }
  for (const auto &child : context->Children()) {
    auto res = ScanSymbolTreeForDefinition(&child.second, symbol);
    if (res) {
      return res;
    }
  }
  return nullptr;
}

std::vector<verible::lsp::Location> SymbolTableHandler::findDefinition(
    const verible::lsp::DefinitionParams &params,
    const verilog::BufferTrackerContainer &parsed_buffers) {
  absl::string_view filepath = LSPUriToPath(params.textDocument.uri);
  if (filepath.empty()) {
    std::cerr << "Could not convert URI " << params.textDocument.uri
              << " to filesystem path." << std::endl;
    return {};
  }
  std::string relativepath = currproject->GetRelativePathToSource(filepath);
  // TODO add checking parsed buffers / raw buffers to get the newest state of
  // files and fallback to reading files from filesystem only when necessary.
  if (checkedfiles.find(relativepath) == checkedfiles.end()) {
    // File hasn't been tracked yet in the symbol table, add it
    auto openedfile = currproject->OpenTranslationUnit(relativepath);
    // TODO check parse status
    auto parsestatus = (*openedfile)->Parse();
    if (!openedfile.ok()) {
      LOG(WARNING) << "Could not open [" << filepath << "] in project ["
                   << currproject->TranslationUnitRoot() << "]";
      return {};
    }
    std::vector<absl::Status> buildstatus =
        BuildSymbolTable(**openedfile, symboltable.get(), currproject.get());
  }
  const verilog::ParsedBuffer *parsedbuffer =
      parsed_buffers.FindBufferTrackerOrNull(params.textDocument.uri)
          ->current();
  if (!parsedbuffer) {
    LOG(ERROR) << "Buffer not found among opened buffers:  "
               << params.textDocument.uri;
    return {};
  }
  const verible::LineColumn cursor{params.position.line,
                                   params.position.character};
  const verible::TextStructureView &text = parsedbuffer->parser().Data();

  const verible::TokenInfo cursor_token = text.FindTokenAt(cursor);
  auto symbol = cursor_token.text();
  auto reffile = currproject->LookupRegisteredFile(relativepath);
  if (!reffile) {
    LOG(ERROR) << "Unable to lookup " << params.textDocument.uri;
    return {};
  }

  auto &root = symboltable->Root();

  auto node = ScanSymbolTreeForDefinition(&root, symbol);
  if (!node) {
    LOG(INFO) << "Symbol " << symbol << " not found in symbol table:  " << node;
    return {};
  }
  // TODO add iterating over multiple definitions?
  verible::lsp::Location location;
  const verilog::SymbolInfo &symbolinfo = node->Value();
  if (!symbolinfo.file_origin) {
    LOG(ERROR) << "Origin file not available";
    return {};
  }
  location.uri = PathToLSPUri(symbolinfo.file_origin->ResolvedPath());
  auto *textstructure = symbolinfo.file_origin->GetTextStructure();
  if (!textstructure) {
    LOG(ERROR) << "Origin file's text structure is not parsed";
    return {};
  }
  verible::LineColumnRange symbollocation =
      textstructure->GetRangeForText(*node->Key());
  location.range.start = {.line = symbollocation.start.line,
                          .character = symbollocation.start.column};
  location.range.end = {.line = symbollocation.end.line,
                        .character = symbollocation.end.column};
  return {location};
}

};  // namespace verilog
