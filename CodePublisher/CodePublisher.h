#pragma once
///////////////////////////////////////////////////////////////////////////
// CodePublisher.h : defines facade/executive for OOD Pr1 S19            //
// ver 2.0                                                               //
//                                                                       // 
// Application   : OOD-S19                                               //
// Platform      :HP intel core i7                                       //
// Author        : Arjun Kalyanasundaram, Syracuse University            //
// Source        : Ammar Salman(instructor solution)                     //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
* =======================
*  This package defines CodePublisher which uses different components of
*  CodeConverter, Display and DirectoryN to put together the entire 
*  functionality of the CodePublisher. The mentioned packages are indep-
*  ndent of each other and their only interaction is through this facade
*
*  NOTE: there are TWO modes for compiling this file:
*   1) Preprocessor DEMO_CP : runs in full demo mode for Pr1.
*   2) Preprocessor USE_CP  : runs normal functionality. 
*  Required Files:
* =======================
*  CodePublisher.h CodePublisher.cpp CodeUtilities.h
*  Converter.h Converter.cpp DirExplorerN.h DirExplorerN.cpp
*  Display.h Display.cpp Logger.h Logger.cpp 
*
*  Maintainence History:
* =======================
*  ver 2.1 - 30 April 2019
*  - added file parsing support to extract information
*  - added automated dependency analysis support 
*  - function 'extractFiles' is no longer public, it's replaced with 'configure()'
*  ver 1.0 - 14 Feb 2019
*  - first release
*/

#include <string>
#include <vector>

#include "../Converter/Converter.h"
#include "../Display/Display.h"
#include "../DirExplorer-Naive/DirExplorerN.h"
#include "../Utilities/CodeUtilities/CodeUtilities.h"

class CodePublisher
{
public:
  CodePublisher();
  bool processCommandLineArgs(int argc, char ** argv);
  
  void workingDirectory(const std::string& dir);
  const std::string& workingDirectory() const;

  void outputDirectory(const std::string& dir);
  const std::string& outputDirectory() const; 

  bool configure();

  std::vector<std::string> publish();
  void publish(const std::string& file);
  void publish(const std::vector<std::string>& files);

  Utilities::DisplayMode displayMode() const;
  bool extractFiles();
  void analyzeFiles();
  std::vector<std::string>Conv_send( int argc, char **argv);
  int demoReq45(CodePublisher& cp, int argc, char ** argv);
  std::vector<std::string> demoReq67(CodePublisher& cp);
  int checkn(CodePublisher& cp, int argc, char ** argv);
private:
  Utilities::ProcessCmdLine *pcl_;
  CodeConverter cconv_;
  Display display_;

  std::string dirIn_; 
  std::string dirOut_;

  std::vector<std::string> files_;

 
};

