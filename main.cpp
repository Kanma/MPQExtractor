#include <StormLib.h>
#include <SimpleOpt.h>
#include <iostream>
#include <string>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>


using namespace std;


struct tSearchResult
{
    string strFileName;
    string strFullPath;
};


/**************************** COMMAND-LINE PARSING ****************************/

// The valid options
enum
{
    OPT_HELP,
    OPT_LISTFILE,
    OPT_PATCHPREFIX,
    OPT_PATCHES,
    OPT_SEARCH,
    OPT_EXTRACT,
    OPT_DEST,
    OPT_FULLPATH,
    OPT_LOWERCASE,
};


const CSimpleOpt::SOption COMMAND_LINE_OPTIONS[] = {
    { OPT_HELP,             "-h",               SO_NONE    },
    { OPT_HELP,             "--help",           SO_NONE    },
    { OPT_LISTFILE,         "-l",               SO_REQ_SEP },
    { OPT_LISTFILE,         "--listfile",       SO_REQ_SEP },
    { OPT_PATCHPREFIX,      "--prefix",         SO_REQ_SEP },
    { OPT_PATCHES,          "-p",               SO_MULTI },
    { OPT_PATCHES,          "--patches",        SO_MULTI },
    { OPT_SEARCH,           "-s",               SO_REQ_SEP },
    { OPT_SEARCH,           "--search",         SO_REQ_SEP },
    { OPT_EXTRACT,          "-e",               SO_REQ_SEP },
    { OPT_EXTRACT,          "--extract",        SO_REQ_SEP },
    { OPT_DEST,             "-o",               SO_REQ_SEP },
    { OPT_DEST,             "--dest",           SO_REQ_SEP },
    { OPT_FULLPATH,         "-f",               SO_NONE    },
    { OPT_FULLPATH,         "--fullpath",       SO_NONE    },
    { OPT_LOWERCASE,        "-c",               SO_NONE    },
    { OPT_LOWERCASE,        "--lowercase",      SO_NONE    },
    
    SO_END_OF_OPTIONS
};


/********************************** FUNCTIONS *********************************/

void showUsage(const std::string& strApplicationName)
{
    cout << "MPQExtractor" << endl
         << "Usage: " << strApplicationName << " [options] <filename>" << endl
         << endl
         << "This program can either:" << endl
         << "  - retrieve the list of files in a MPQ archive" << endl
         << "  - search for the list of files corresponding to a given pattern in a MPQ archive" << endl
         << "  - extract some files from a MPQ archive" << endl
         << "  - apply some patches to a MPQ archive before extracting some files" << endl
         << "  - combine some of the above options" << endl
         << endl
         << "Options:" << endl
         << "    --help, -h:              Display this help" << endl
         << "    --listfile <FILE>," << endl
         << "    -l <FILE>:               Retrieve the list of files in the archive and save it" << endl
         << "                             in FILE" << endl
         << "    --search <PATTERN>," << endl
         << "    -s <PATTERN>:            Search for the list of files corresponding to a PATTERN" << endl
         << "                             in the archive" << endl
         << "    --extract <PATTERN>," << endl
         << "    -e <PATTERN>:            Same as --search, but the found files are also extracted" << endl
         << "    --dest <PATH>," << endl
         << "    -o <PATH>:               The folder where the files are extracted (default: the" << endl
         << "                             current one)" << endl
         << "    --fullpath, -f:          During extraction, preserve the path hierarchy found" << endl
         << "                             inside the archive" << endl
         << "    --patches <FILE-1> <FILE-2> ... <FILE-N>," << endl
         << "    -p <FILE-1> <FILE-2> ... <FILE-N>:" << endl
         << "                             The patches to apply to the archive before extracting files," << endl
         << "                             in their order of application. A per-patch prefix can be " << endl
         << "                             specified with a comma (e.g. -p wow-update-13164.MPQ,base)" << endl
         << "    --prefix <PREFIX>        Path prefix to apply to all patches which do not specify their " << endl
         << "                             own explicit prefix" << endl
         << "    --lowercase, -c:         Convert extracted file paths to lowercase" <<endl
         << endl
         << "Examples:" << endl
         << endl
         << "  1) Retrieve the list of files in a MPQ archive:" << endl
         << endl
         << "       ./MPQExtractor -l list.txt archive.MPQ" << endl
         << endl
         << "  2) Search all the *.M2 files in a MPQ archive:" << endl
         << endl
         << "       ./MPQExtractor -s *.M2 archive.MPQ" << endl
         << endl
         << "  3) Extract a specific file from a MPQ archive:" << endl
         << endl
         << "       ./MPQExtractor -e \"Path\\To\\The\\File\" -o out archive.MPQ" << endl
         << endl
         << "  4) Extract some specific files from a MPQ archive, preserving the path hierarchy" << endl
         << "     found inside the MPQ archive:" << endl
         << endl
         << "       ./MPQExtractor -e \"Path\\To\\Extract\\*\" -f -o out archive.MPQ" << endl
         << endl
         << "  5) Apply some patches before extracting a specific file from a MPQ archive:" << endl
         << endl
         << "       ./MPQExtractor -p patch-*.MPQ --prefix base -e \"Path\\To\\The\\File\" -o out archive.MPQ" << endl
         << endl;
}


int main(int argc, char** argv)
{
    HANDLE hArchive;
    string strListFile;
    string strPatchPrefix;
    vector<string> patches;
    string strSearchPattern;
    string strDestination = ".";
    vector<tSearchResult> searchResults;
    bool bExtraction = false;
    bool bUseFullPath = false;
    bool bLowerCase = false;


    // Parse the command-line parameters
    CSimpleOpt args(argc, argv, COMMAND_LINE_OPTIONS);
    while (args.Next())
    {
        if (args.LastError() == SO_SUCCESS)
        {
            switch (args.OptionId())
            {
                case OPT_HELP:
                    showUsage(argv[0]);
                    return 0;
                
                case OPT_LISTFILE:
                    strListFile = args.OptionArg();
                    break;

                case OPT_PATCHPREFIX:
                    strPatchPrefix = args.OptionArg();
                    break;

                case OPT_PATCHES:
                {
                    unsigned int i = 1;
                    while (true)
                    {
                        char** val = args.MultiArg(i);
                        if (!val)
                            break;

                        patches.push_back(val[i - 1]);
                    }
                    break;
                }

                case OPT_SEARCH:
                    strSearchPattern = args.OptionArg();
                    break;

                case OPT_EXTRACT:
                    strSearchPattern = args.OptionArg();
                    bExtraction = true;
                    break;

                case OPT_DEST:
                    strDestination = args.OptionArg();
                    break;

                case OPT_FULLPATH:
                    bUseFullPath = true;
                    break;

                case OPT_LOWERCASE:
                    bLowerCase = true;
                    break;
            }
        }
        else
        {
            cerr << "Invalid argument: " << args.OptionText() << endl;
            return -1;
        }
    }

    if (args.FileCount() != 1)
    {
        cerr << "No MPQ file specified" << endl;
        return -1;
    }

    
    cout << "Opening '" << args.File(0) << "'..." << endl;
    if (!SFileOpenArchive(args.File(0), 0, MPQ_OPEN_READ_ONLY, &hArchive))
    {
        cerr << "Failed to open the file '" << args.File(0) << "'" << endl;
        return -1;
    }


    // List file extraction
    if (!strListFile.empty())
    {
        if (!SFileExtractFile(hArchive, "(listfile)", strListFile.c_str()))
        {
            cerr << "Failed to extract the list of files" << endl;
            SFileCloseArchive(hArchive);
            return -1;
        }
    }
    
    
    // Apply the patches
    if (!patches.empty())
    {
        vector<string>::iterator iter, iterEnd;

        string prefix;
        string patch;
        size_t idx;

        for (iter = patches.begin(), iterEnd = patches.end(); iter != iterEnd; ++iter)
        {
            prefix = strPatchPrefix;;
            patch = *iter;

            idx = patch.find_first_of(',');
            if (idx != string::npos)
            {
                prefix = patch.substr(idx+1, string::npos);
                patch = patch.substr(0, idx);
            }

            if (prefix.length())
            {
                cout << "Applying patch '" << patch << "' (prefix '" << prefix << "')..." << endl;
            }else{
                cout << "Applying patch '" << patch << "' (no prefix)..." << endl;
            }

            if (!SFileOpenPatchArchive(hArchive, patch.c_str(), prefix.c_str(), 0))
                cerr << "Failed to apply the patch '" << patch << "'" << endl;
        }
    }

    
    // Search the files
    if (!strSearchPattern.empty())
    {
        if ((strSearchPattern.find("*") == string::npos) && (strSearchPattern.find("?") == string::npos))
        {
            tSearchResult r;
            r.strFileName = strSearchPattern.substr(strSearchPattern.find_last_of("\\") + 1);
            r.strFullPath = strSearchPattern;

            searchResults.push_back(r);
        }
        else
        {
            cout << endl;
            cout << "Searching for '" << strSearchPattern << "'..." << endl;

            SFILE_FIND_DATA findData;
            HANDLE handle = SFileFindFirstFile(hArchive, strSearchPattern.c_str(), &findData, 0);
        
            if (handle)
            {
                cout << endl;
                cout << "Found files:" << endl;

                do {
                    cout << "  - " << findData.cFileName << endl;

                    tSearchResult r;
                    r.strFileName = findData.szPlainName;
                    r.strFullPath = findData.cFileName;

                    searchResults.push_back(r);
                } while (SFileFindNextFile(handle, &findData));

                SFileFindClose(handle);
            }
            else
            {
                cout << "No file found!" << endl;
            }
        }
    }

    // Extraction
    if (bExtraction && !searchResults.empty())
    {
        cout << endl;
        cout << "Extracting files..." << endl;
        cout << endl;

        if (strDestination.at(strDestination.size() - 1) != '/')
            strDestination += "/";

        vector<tSearchResult>::iterator iter, iterEnd;
        for (iter = searchResults.begin(), iterEnd = searchResults.end(); iter != iterEnd; ++iter)
        {
            string strDestName = strDestination;
            
            if (bUseFullPath)
            {
                strDestName += iter->strFullPath;
                
                size_t offset = strDestName.find("\\");
                while (offset != string::npos)
                {
                    strDestName = strDestName.substr(0, offset) + "/" + strDestName.substr(offset + 1);
                    offset = strDestName.find("\\");
                }

                offset = strDestName.find_last_of("/");
                if (offset != string::npos)
                {
                    string dest = strDestName.substr(0, offset + 1);

                    size_t start = dest.find("/", 0);
                    while (start != string::npos)
                    {
                        string dirname = dest.substr(0, start);

                        DIR* d = opendir(dirname.c_str());
                        if (!d)
                            mkdir(dirname.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                        else
                            closedir(d);

                        start = dest.find("/", start + 1);
                    }
                }
            }
            else
            {
                strDestName += iter->strFileName;
            }

            if (!SFileExtractFile(hArchive, iter->strFullPath.c_str(), strDestName.c_str(), SFILE_OPEN_PATCHED_FILE))
                cerr << "Failed to extract the file '" << iter->strFullPath << "' in " << strDestName << endl;
        }
    }

    SFileCloseArchive(hArchive);

    return 0;
}
