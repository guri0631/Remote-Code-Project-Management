///////////////////////////////////////////////////////////////////////////
// Converter.cpp : implements and provides test stub for Converter.h     //
// ver 2.0                                                               //
//                                                                       // 
// Application   : OOD-S19 Instructor Solution                           //
// Platform      : Visual Studio Community 2017, Windows 10 Pro x64      //
// Author        : Ammar Salman, Syracuse University                     //
//                 313/788-4694, assalman@syr.edu                        //
///////////////////////////////////////////////////////////////////////////

#include "Converter.h"
#include "../Logger/Logger.h"
#include "../FileSystem/FileSystem.h"

using namespace FileSystem;
using namespace LoggingNew;

// -----< default ctor >--------------------------------------------------
CodeConverter::CodeConverter()
{
}

// -----< ctor to set dep table >-----------------------------------------
CodeConverter::CodeConverter(const DependencyTable & dt) : dt_(dt)
{
  LoggerDebug::write("\n  CodeConverter initialized with DependencyTable\n");
}

// -----< set dependency table function >---------------------------------
void CodeConverter::setDepTable(const DependencyTable & dt)
{
  dt_ = dt;
}

void CodeConverter::setFileLineInfo(const FileLineInfo & info)
{
  fileLineInfo_ = info;
}

// -----< convert pre-set dependency table >------------------------------
std::vector<std::string> CodeConverter::convert()
{
  if (!createOutpurDir()) {
    LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
    LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
    return std::vector<std::string>();
  }

  LoggerDemo::write("\n\n  Converting files to webpages");
  LoggerDebug::write("\n\n  Converting files to webpages");

  LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
  LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

  for (auto entry : dt_) {
    if (convertFile(entry.first)) {
      LoggerDemo::write("\n  -- Converted: ");
      LoggerDebug::write("\n  -- Converted: ");
    }
    else {
      LoggerDemo::write("\n  -- Failed:    ");
      LoggerDebug::write("\n  -- Failed:    ");
    }

    std::string filename = Path::getName(entry.first);
    LoggerDebug::write(filename);
    LoggerDemo::write(filename);
  }
  LoggerDebug::write("\n");
  LoggerDemo::write("\n");
  return convertedFiles_;
}

// -----< convert single file given path >----------------------------------
std::string CodeConverter::convert(const std::string & filepath)
{
  if (!createOutpurDir()) {
    LoggerDebug::write("\n\n  Failed to start conversion due to inability to create output directory");
    LoggerDebug::write("\n  -- Output Directory: " + Path::getFullFileSpec(outputDir_));
    return "";
  }

  LoggerDemo::write("\n\n  Converting files to webpages");
  LoggerDebug::write("\n\n  Converting files to webpages");

  LoggerDebug::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));
  LoggerDemo::write("\n  Output directory: " + Path::getFullFileSpec(outputDir_));

  if (convertFile(filepath)) {
    LoggerDemo::write("\n  -- Converted: ");
    LoggerDebug::write("\n  -- Converted: ");
  }
  else {
    LoggerDemo::write("\n  -- Failed:    ");
    LoggerDebug::write("\n  -- Failed:    ");
  }

  std::string filename = Path::getName(filepath);
  LoggerDebug::write(filename);
  LoggerDemo::write(filename);

  return filename + ".html";
}

std::vector<std::string> CodeConverter::convert(const std::vector<std::string>& files)
{
  clear();
  //dt_ = DependencyTable(files);
  return convert();
}

// -----< get set output directory >----------------------------------------
const std::string & CodeConverter::outputDir() const
{
  return outputDir_;
}

// -----< set output directory >--------------------------------------------
void CodeConverter::outputDir(const std::string & dir)
{
  outputDir_ = dir;
}

// -----< create output directory >-----------------------------------------
/* returns success of creation. it could fail if dir is sys protected */
bool CodeConverter::createOutpurDir()
{
  if (!Directory::exists(outputDir_)) {
    LoggerDebug::write("\n  Creating output directory. Path: " + Path::getFullFileSpec(outputDir_));
    return Directory::create(outputDir_);
  }
  LoggerDebug::write("\n  Output directory already exists. Proceeding...");
  return true;
}

// -----< get list of converted files >-------------------------------------
const std::vector<std::string> CodeConverter::convertedFiles() const
{
  return convertedFiles_;
}

void CodeConverter::clear()
{
  convertedFiles_.clear();
  //dt_.clear();
}

// -----< private - read file and create webpage >--------------------------
bool CodeConverter::convertFile(std::string file)
{
  std::ifstream in(file);
  if (!in.is_open() || !in.good()) {
    LoggerDebug::write("Error -- unable to read file, path may be invalid.");
    in.close();
    return false;
  }

  std::string filename = Path::getName(file);
  std::string outputPath = outputDir_ + filename + ".html";
  out_.open(outputPath, std::ofstream::out);
  if (!out_.is_open() || !out_.good()) {
    LoggerDebug::write("Error -- unable to open output file for writing.");
    in.close();
    return false;
  }
  
  // write everything to file 
  addPreCodeHTML(filename);
  addDependencyLinks(file);
  addPreTag();

  size_t lineCount = 1;
  while (in.good()) {
    std::string line;
    while (std::getline(in, line)) {
      skipSpecialChars(line);
      handleSLComment(line);
      handleMLComment(line);
      handleScopeInfo(file, lineCount, line);
      out_ << line << std::endl;
      lineCount++;
    }
  }

  addClosingTags();
  out_.close();

  convertedFiles_.push_back(outputPath);
  return true;
}

// -----< private - add generic HTML preliminary markup >-------------------
void CodeConverter::addPreCodeHTML(const std::string& title)
{
  out_ << "<DOCTYPE !HTML>" << std::endl;
  out_ << "<html>" << std::endl;
  out_ << "  <head>" << std::endl;
  out_ << "    <Title>" << title << "</Title>" << std::endl;
  out_ << "    <script src=\"script.js\"></script>" << std::endl;
  out_ << "    <link rel=\"stylesheet\" type=\"text/css\" href=\"style.css\">" << std::endl;
  out_ << "  </head>" << std::endl << std::endl;
  out_ << "  <body>" << std::endl;
  out_ << "    <span class=\"absolute\">" << std::endl;
  out_ << "      <label><input type=\"checkbox\" id =\"hideF\" value=\"hidefuncs\" checked=\"true\" onclick=\"toggleVisibility('function')\">Functions</label><br>" << std::endl;
  out_ << "      <label><input type=\"checkbox\" id =\"hideC\" value=\"hideclass\" checked=\"true\" onclick=\"toggleVisibility('class')\">Classes</label><br>" << std::endl;
  out_ << "      <label><input type=\"checkbox\" id =\"hidec\" value=\"hidecomms\" checked=\"true\" onclick=\"toggleVisibility('comment')\">Comments</label><br>" << std::endl;
  out_ << "    </span>" << std::endl;
}

// -----< private - add pre tag >------------------------------------------
/* seperated into seperate function to allow for dependencies addition
*  before the actual code of the file */
void CodeConverter::addPreTag()
{
  out_ << "    <pre>" << std::endl;
}

// -----< private - add depedency links markup code >----------------------
void CodeConverter::addDependencyLinks(std::string file)
{
  std::string filename = Path::getName(file);
  if (!dt_.has(file)) { // in case of single file conversion
    LoggerDebug::write("\n    No entry found in DependencyTable for [" + filename + "]. Skipping dependency links..");
    return;
  }

  if (dt_[file].size() == 0) { // in case the file has no dependencies
    LoggerDebug::write("\n    No dependencies found for [" + filename + "]. Skipping dependency links..");
    return;
  }

  out_ << "    <h3>Dependencies: " << std::endl;
  for (auto dep : dt_[file]) {
    out_ << "      <a href=\"" << Path::getName(dep) << ".html\">" << Path::getName(dep) << "</a>" << std::endl;
  }
  out_ << "    </h3>";
}

// -----< private - add generic HTML markup closing tags >-----------------
void CodeConverter::addClosingTags()
{
  out_ << "    </pre>" << std::endl;
  out_ << "  </body>" << std::endl;
  out_ << "</html>" << std::endl;
}

// -----< private - replace HTML special chars >---------------------------
/* note: the function uses while loop to make sure ALL special characters
*  are replaced instead of just the first encounter. */
void CodeConverter::skipSpecialChars(std::string & line)
{
  size_t pos = line.npos;
  while((pos = line.find('<')) != line.npos) 
    line.replace(pos, 1, "&lt;");

  while ((pos = line.find('>')) != line.npos)
    line.replace(pos, 1, "&gt;");
}

// -----< private - wrap single line comments with span tag >--------------
/* wraps multiple single-line comments into one tag with 'comments' class*/
void CodeConverter::handleSLComment(std::string & line)
{
  static bool ongoing = false; // to wrap continious SL comments in a single tag
  size_t pos = line.npos;
  pos = line.find('/');
  if (pos == line.npos || pos == line.size() - 1) {
    if (ongoing) { // if the wrap is already started, end the tag
      line = "</span>" + line;
    }
    ongoing = false;
    return;
  }
  
  // if it's the first comment in the sequence, add the tag
  if (line[pos + 1] == '/' && !ongoing) {
    line.insert(pos, "<span class=\"comment\">");
    ongoing = true;
  }
}

// -----< private - handle multi-line comments >---------------------------
/* wraps slash-star to star-slash comments in comment tag
 * while loops are used for output correctness */
void CodeConverter::handleMLComment(std::string & line)
{
  std::string commentTag = "<span class=\"comment\">";
  std::string cTagEnd = "</span>";

  size_t pos = 0;
  pos = line.find("/*");
  while (pos != line.npos) {
    line.insert(pos, commentTag);
    pos = line.find("/*", pos + 2 + commentTag.size());
  }
  
  pos = 0;
  pos = line.find("*/");
  while (pos != line.npos) {
    if (pos + 2 == line.size()) line += cTagEnd;
    else line.insert(pos + 2, cTagEnd);
    pos = line.find("*/", pos + 2 + cTagEnd.size());
  }
}

// ------< private - handle scope info >-----------------------------------
/* marks the begginings and endings of classes and functions with tags*/
void CodeConverter::handleScopeInfo(std::string filepath, size_t lineCount, std::string & line)
{
  if (fileLineInfo_.find(filepath) == fileLineInfo_.end()) return;
  
  LineInfo& info = fileLineInfo_[filepath];
  if (info.find(lineCount) == info.end()) return;

  size_t pos = 0;
  for (auto scopeInfo : info[lineCount]) {
    if (scopeInfo == LineIndentifier::classBegin) {
      pos = line.find('{', pos);
      if (pos != line.npos)
        line.insert(pos, "<span class=\"class\">");
      else pos = 0;
    }
    if (scopeInfo == LineIndentifier::functionBegin) {
      pos = line.find('{', pos);
      if (pos != line.npos)
        line.insert(pos, "<span class=\"function\">");
      else pos = 0;
    }
    if (scopeInfo == LineIndentifier::scopeEnd) {
      pos = line.find('}', pos);
      if (pos != line.npos) {
        if (pos + 1 == line.size()) line += "</span>";
        else line.insert(pos, "</span>");
      }
      else pos = 0;
    }
  }
}


#ifdef TEST_CONVERTER

// -----< test stub for converter class >-----------------------------------
int main() {
  LoggerDebug::attach(&std::cout);
  LoggerDebug::start();

  LoggerDebug::title("Testing CodeConverter functions");

  LoggerDebug::stop(); // do not log DependencyTable logs
  
  DependencyTable dt;
  dt.addDependency("Converter.h", "Converter.cpp");
  dt.addDependency("Converter.cpp", "Converter.h");
  dt.addDependency("A.h", "A.h");
  
  LoggerDebug::start(); // log from now on

  CodeConverter cc(dt);
  dt.display();
  LoggerDebug::write("\n");
  //cc.outputDir("C:\\windows\\system32\\"); // will trigger errors which are handled
  cc.convert();

  LoggerDebug::stop();
  return 0;
}

#endif