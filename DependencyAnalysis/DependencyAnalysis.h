#pragma once
///////////////////////////////////////////////////////////////////////////
// DependencyAnalysis.h : performs dependency analysis using #include    //
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
*  This package defines DependencyAnalysis functionality which builds up
*  DependencyTable instance (already in use by CodeConverter) to hold the
*  extracted relationships. It accepts a list of files, a map holding a
*  vector which contains the #include statements for the corresponding
*  File. Then it performs the analysis and produces the final set of files.
*
*  Note: if a file is in the #include vector but not in the file list, it 
*  will NOT be added as a relationship. 
*
*  Required Files:
* =======================
*  DependencyAnalysis.h DependencyAnalysis.cpp 
*  DependencyTable.h DependencyTable.cpp 
*  FileSystem.h FileSystem.cpp
*
*  Maintainence History:
* =======================
*  ver 1.0 - 11 Feb 2019
*  - first release
*/

#include <vector>
#include <string>
#include "../DependencyTable/DependencyTable.h"

class DependencyAnalysis
{
public:
  using FileIncludes = std::unordered_map<std::string, std::vector<std::string>>;

  DependencyAnalysis();
  ~DependencyAnalysis();
  void filelist(const std::vector<std::string>& list);
  void includeList(const FileIncludes& includes);
  void performAnalysis();
  const DependencyTable& analysisResult() const;

private:
  std::vector<std::string> filelist_;
  FileIncludes includeList_;
  DependencyTable dt_;
};

