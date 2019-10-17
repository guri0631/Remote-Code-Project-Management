////////////////////////////////////////////////////////////////////////////
// FileProcessor.cpp : uses Parser to get scope information for Publisher //
// ver 1.0                                                                //
//                                                                        //
// Application   : OOD-S19 Instructor Solution                            //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64       //
// Author        : Ammar Salman, Syracuse University                      //
//                 313/788-4694, assalman@syr.edu                         //
////////////////////////////////////////////////////////////////////////////

#include "FileProcessor.h"
#include "../CppParser/Parser/ConfigureParser.h"


using namespace CodeAnalysis;

// -----< default ctor - empty >------------------------------------
FileProcessor::FileProcessor(){}

// -----< default dtor - empty >------------------------------------
FileProcessor::~FileProcessor(){}

// -----< filelist - accepts and stores file list >-----------------
void FileProcessor::filelist(const std::vector<std::string> & list)
{
  filelist_ = list;
}

// -----< process files - uses Parser to extract needed info >------
void FileProcessor::processFiles()
{
  for (std::string fileSpec : filelist_) {
    try
    {
      ConfigParseForCodeAnal configure;
      Parser* pParser = configure.Build();

      std::string name;
      if (pParser)
      {
        name = FileSystem::Path::getName(fileSpec);
        if (!configure.Attach(fileSpec))
        {
          std::cout << "\n  could not open file " << name << std::endl;
          continue;
        }
      }
      
      Repository* pRepo = Repository::getInstance();
      pRepo->package() = name;

      while (pParser->next())
      {
        pParser->parse();
      }
      
      ASTNode* pGlobalScope = pRepo->getGlobalScope();

      extractIncludes(pGlobalScope, fileSpec); // get dependencies
      treeWalk(pGlobalScope, fileSpec);   // get scope information
    }
    catch (...) {

    }
  }
}

// -----< get file info - returns extracted scope info >-----------------
const FileProcessor::FileLineInfo & FileProcessor::fileLineInfo() const
{
  return fileInfo_;
}

// -----< get include info - returns #include statments info >-----------
const FileProcessor::FileIncludes & FileProcessor::fileIncludes() const
{
  return fileIncludes_;
}


#ifdef TEST_FPROCESSOR

int main(int argc, char ** argv) {
  std::string filename = "../CodePublisher/CodePublisher.h";

  std::vector<std::string> list;
  list.push_back(filename);

  FileProcessor fp;
  fp.filelist(list);
  fp.processFiles();
  return 0;
}

#endif