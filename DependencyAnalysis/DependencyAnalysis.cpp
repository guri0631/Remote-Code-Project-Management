///////////////////////////////////////////////////////////////////////////
// DependencyAnalysis.cpp : performs dependency analysis using #include  //
// ver 1.0                                                               //
//                                                                       // 
// Application   : OOD-S19 Instructor Solution                           //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Author        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////

#include "DependencyAnalysis.h"
#include "../FileSystem/FileSystem.h"

#include <algorithm>

using namespace FileSystem;

// -----< default ctor - empty >----------------------------------------
DependencyAnalysis::DependencyAnalysis(){}

// -----< default dtor - empty >----------------------------------------
DependencyAnalysis::~DependencyAnalysis(){}

// -----< filelist - accepts and sets private file list >---------------
void DependencyAnalysis::filelist(const std::vector<std::string>& list)
{
  filelist_ = list;
}

// -----< includeList - accepts and sets include map >------------------
void DependencyAnalysis::includeList(const FileIncludes& includes)
{
  includeList_ = includes;
}

// -----< performAnalysis - produces DependencyTable >------------------
void DependencyAnalysis::performAnalysis()
{
  for (std::string filepath : filelist_) {
    dt_.addDependency(filepath, "");

    //add circular dependency between H and CPP file (without real analysis)
    std::string ext = Path::getExt(filepath);
    if (ext == "h") {
      std::string temp = filepath.substr(0, filepath.size() - 1) + "cpp";
      if (std::find(filelist_.begin(), filelist_.end(), temp) != filelist_.end())
        dt_.addDependency(filepath, temp);
    }
    else if (ext == "cpp") {
      std::string temp = filepath.substr(0, filepath.size() - 3) + "h";
      if (std::find(filelist_.begin(), filelist_.end(), temp) != filelist_.end())
        dt_.addDependency(filepath, temp);
    }

    std::string fileDir = Path::getPath(filepath);

    for (std::string include : includeList_[filepath]) {
      // get the absolute path for dependency
      std::string includePath = Path::getFullFileSpec(fileDir + include);
      // verify dependency exists in the given file list 
      if (std::find(filelist_.begin(), filelist_.end(), includePath) != filelist_.end())
        dt_.addDependency(filepath, includePath);
    }
  }
}

// -----< analysis result - returns produced DependencyTable >---------
const DependencyTable & DependencyAnalysis::analysisResult() const
{
  return dt_;
}


#ifdef TEST_DEPANALYSIS

#include "../Logger/Logger.h" 
#include <iostream>

using namespace Logging;

int main(int argc, char** argv) {
  LoggerDebug::attach(&std::cout);
  LoggerDebug::start();

  std::string f1 = Path::getFullFileSpec("DependencyAnalysis.h");
  std::string f2 = Path::getFullFileSpec("DependencyAnalysis.cpp");
  std::string f3 = Path::getFullFileSpec("../DependencyTable/DependencyTable.h");
  std::string f4 = Path::getFullFileSpec("../DependencyTable/DependencyTable.cpp");

  std::vector<std::string> filelist;
  filelist.push_back(f1);
  filelist.push_back(f2);
  filelist.push_back(f3);
  filelist.push_back(f4);

  DependencyAnalysis da;
  da.filelist(filelist);

  DependencyAnalysis::FileIncludes fileIncludes;


  fileIncludes[f1].push_back(f3);
  fileIncludes[f2].push_back(f1);
  fileIncludes[f4].push_back(f3);

  da.includeList(fileIncludes);
  da.performAnalysis;

  da.analysisResult().display();

  LoggerDebug::stop();
}

#endif