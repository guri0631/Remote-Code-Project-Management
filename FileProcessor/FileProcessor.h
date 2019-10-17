#pragma once
///////////////////////////////////////////////////////////////////////////
// FileProcessor.h : uses Parser to get scope information for Publisher  //
// ver 1.0                                                               //
//                                                                       //
// Application   : OOD-S19 Instructor Solution                           //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Author        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines FileProcessor class which uses Parser to create 
*  AST and then walks through the AST to get class/function start/end 
*  information. Furthermore, it also gets the #include statements for each
*  file. The scope-information is stored in one map:
*    filepath -> [ lineNumber -> {classBegin, functionBegin, endScope}]
*  The map has string key and a second map value. The second map uses
*  size_t key (representing line number) and vector value. The vector
*  holds instances of LineIdentifier enum which indicate what this line
*  in particular holds. It's possible for a single line to have multiple
*  identifiers (e.g. empty function {}) which is why we've used vector.
*
*  The #include statements are processed also in a map:
*    filepath -> { dep1, dep2, dep3 ... }
*  The map has filepath as key, and vector of dependencies as value.
*  Only dependencies starting with " character are taken into consideration.
*  This way, DependencyAlaysis package can directly use the info.
*
*  Required Files:
* =======================
*  FileProcessor.h FileProcessor.cpp ConfigureParser.h ConfigureParser.cpp
*
*  Maintainence History:
* =======================
*  ver 1.0 - 11 Feb 2019
*  - first release
*/

#include <unordered_map>
#include <vector>
#include <string>


enum LineIndentifier {
  classBegin,
  functionBegin,
  scopeEnd
};

class FileProcessor
{
public:
  using LineInfo = std::unordered_map<size_t, std::vector<LineIndentifier>>;
  using FileLineInfo = std::unordered_map<std::string, LineInfo>;
  using FileIncludes = std::unordered_map<std::string, std::vector<std::string>>;
  
  FileProcessor();
  ~FileProcessor();

  void filelist(const std::vector<std::string> & list);
  void processFiles();

  const FileLineInfo& fileLineInfo() const; 
  const FileIncludes& fileIncludes() const;

private:
  std::vector<std::string> filelist_;
  FileLineInfo fileInfo_;
  FileIncludes fileIncludes_;

  // templates because we did not want to include AST header file 
  // in here. The reason we opted for this is to reduce the possible
  // confilicts. This package only depends on Parser in its CPP and 
  // not in the header file. 

  template<typename T> void extractIncludes(T t, std::string file);
  template<typename T> void treeWalk(T t, std::string file);
};

// -----< extract includes - accepts ASTNode >-----------------------
/* this function accepts the GlobalScope ASTNode and uses its
 * statements_ vector to get the possible dependencies.
 * statements_ holds ITokCollection, we only need the last one of
 * each collection since that's the only token which can be a
 * dependency .. as in { # include "something.h" } */
template<typename T>
inline void FileProcessor::extractIncludes(T t, std::string file)
{
  for (auto it = t->statements_.begin(); it != t->statements_.end(); ++it) {
    std::string token = (*it)->last();
    // verify if token starts with " which makes it a candidate as dep
    if (token[0] == '\"')
      fileIncludes_[file].push_back(token.substr(1, token.size()-2)); // remove "" marks 
  }
}

// -----< tree walk - accepts ASTNode >-------------------------------
/* this function is recursive and walks the tree using DFS manner.
 * it extracts class and function information from the tree and 
 * stores them in the map described in the interface documentation*/
template<typename T>
inline void FileProcessor::treeWalk(T t, std::string file)
{
  if (t->type_ == "class") {
    size_t begin = t->startLineCount_;
    size_t end = t->endLineCount_;
    fileInfo_[file][begin].push_back(LineIndentifier::classBegin);
    fileInfo_[file][end].push_back(LineIndentifier::scopeEnd);
  }

  if (t->type_ == "function") {
    size_t begin = t->startLineCount_;
    size_t end = t->endLineCount_;
    fileInfo_[file][begin].push_back(LineIndentifier::functionBegin);
    fileInfo_[file][end].push_back(LineIndentifier::scopeEnd);
  }

  for (size_t i = 0; i < t->children_.size(); i++) 
    treeWalk(t->children_[i], file);
}

