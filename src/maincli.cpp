
#include <iostream>
#include <fstream>
#include <filesystem>
#include "parser/dbcscanner.h"
#include "codegen/c-main-generator.h"
#include "codegen/c-util-generator.h"
#include "codegen/fs-creator.h"
#include "codegen/version.h"

#define GEN_UTIL_CODE

char verstr[128] = {0};

const char* helptext =
  "https://github.com/astand/c-coderdbc        (project source code)\n\n"
  "https://coderdbc.com                        (web application)\n\n"
  "To use utility you need to provide 3 arguments:\n\n"
  "1. dbc file path\n"
  "2. directory for generated source files (existable)\n"
  "3. prefix (driver name) which will be used for naming dirver parts\n\n"
  "Usage example:\n\n./dbccoder /home/user/docs/driveshaft.dbc /home/user/docs/gen/ drivedb\n\n";

DbcScanner* scanner;
CiMainGenerator* cigen;
CiUtilGenerator* ciugen;
FsCreator* fscreator;

std::string source_files_out_path;
std::string dbc_file_path;
std::string dbc_driver_name;

int main(int argc, char* argv[])
{
  scanner = new DbcScanner;
  cigen = new CiMainGenerator;
  ciugen = new CiUtilGenerator;
  fscreator = new FsCreator;

  std::snprintf(verstr, 128, "\nDbccoder v%u.%u\n\n", CODEGEN_LIB_VERSION_MAJ, CODEGEN_LIB_VERSION_MIN);
  std::cout << verstr;

  if (argc == 4)
  {
    std::ifstream reader;
    // copy dbc file name to string variable
    dbc_file_path = argv[1];
    source_files_out_path = argv[2];
    dbc_driver_name = argv[3];

    std::cout << "dbc file : " << argv[1] << std::endl;
    std::cout << "gen path : " << argv[2] << std::endl;
    std::cout << "drv name : " << argv[3] << std::endl;

    if (std::filesystem::exists(dbc_file_path) == false)
    {
      std::cout << "DBC file is not exists!" << std::endl;
      return -1;
    }

    reader.open(dbc_file_path);

    std::istream& s = reader;

    scanner->TrimDbcText(s);

    std::string info("");

    auto ret = fscreator->PrepareDirectory(dbc_driver_name.c_str(), source_files_out_path.c_str(), true, info);

    if (ret)
    {
      cigen->Generate(scanner->dblist, fscreator->FS);
    }
    else
    {
      std::cout << "One or both are invalid\n";
    }

#if defined (GEN_UTIL_CODE)

    ret = fscreator->PrepareDirectory(dbc_driver_name.c_str(), (source_files_out_path).c_str(), true, info);

    MsgsClassification groups;

    for (size_t i = 0; i < scanner->dblist.msgs.size(); i++)
    {
      groups.Rx.push_back(scanner->dblist.msgs[i]->MsgID);
    }

    if (ret)
    {
      ciugen->Generate(scanner->dblist, fscreator->FS, groups, dbc_driver_name);
    }

#endif

  }
  else
  {
    std::cout << helptext;
  }
}
