#pragma once
///////////////////////////////////////////////////////////////////////////
// Converter.h   : defines source code conversion to webpage functions   //
// ver 2.0                                                               //
//                                                                       // 
// Application   : OOD-S19 Instructor Solution                           //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Author        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines CodeConverter class which accepts DependencyTable
*  and uses its relationships to create linked webpages that point to each
*  other based on dependencies. It can also convert single files if needed.
*  The conversion process filters HTML special characters before printing
*  them into output files. The resulting output of this converter is a list
*  of files (vector<string>) of the created webpages. 
*
*  Required Files:
* =======================
*  Converter.h Converter.cpp DependencyTable.h DependencyTable.cpp
*  Logger.h Logger.cpp FileSystem.h FileSystem.cpp
*  FileProcessor.h (only for ScopeIdenitifier enum and aliases)
*
*  Maintainence History:
* =======================
*  ver 2.0 - 27 Mar 2019
*  - support for handling single line comments
*  - support for handling multi- line comments
*  - support for handling scopes (function and class begin and end)
*  - uses information extracted from FileProcessor and DependencyAnalysis
*  ver 1.0 - 11 Feb 2019
*  - first release
*/

#include "../DependencyTable/DependencyTable.h"
#include "../FileProcessor/FileProcessor.h"
#include <vector>
#include <string>
#include <fstream>

class CodeConverter
{
public:
  using LineInfo = FileProcessor::LineInfo;
  using FileLineInfo = FileProcessor::FileLineInfo;

  CodeConverter();
  CodeConverter(const DependencyTable& dt);

  const std::string& outputDir() const;
  void outputDir(const std::string& dir);
  bool createOutpurDir();

  void setDepTable(const DependencyTable& dt);
  void setFileLineInfo(const FileLineInfo& info);

  std::vector<std::string> convert();
  std::string convert(const std::string& filepath);
  std::vector<std::string> convert(const std::vector<std::string>& files);
  
  const std::vector<std::string> convertedFiles() const;

  void clear();

private:
  bool convertFile(std::string file);
  void addPreCodeHTML(const std::string& title);
  void addPreTag();
  void addDependencyLinks(std::string file);
  void addClosingTags();
  
  void skipSpecialChars(std::string& line);
  void handleSLComment(std::string& line);
  void handleMLComment(std::string& line);
  void handleScopeInfo(std::string filepath, size_t lineCount, std::string& line);

private:
  DependencyTable dt_;
  std::string outputDir_ = "..\\ConvertedWebpages\\";
  std::vector<std::string> convertedFiles_;
  std::ofstream out_;

  FileLineInfo fileLineInfo_;
};

