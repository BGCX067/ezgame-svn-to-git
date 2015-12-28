#include "EzPath.h"

#include <direct.h>

EzPath::EzPath(void)
{
}


EzPath::~EzPath(void)
{
}


bool EzPath::getCurrentWorkingDirectory(char* pcPath, size_t stDestSize)
{
    return (_getcwd(pcPath, stDestSize) != NULL);
}

bool EzPath::makeStandardize(char* pcPath)
{
    bool bModified = false;
    if (!pcPath)
        return bModified;

    unsigned int uiLen = strlen(pcPath);
    bool bLastCharWasSlash = false;

    unsigned int uiWrite = 0;
    for (unsigned int uiRead = 0; uiRead < uiLen; uiRead++)
    {
        char c = pcPath[uiRead];

        //  convert NI_PATH_DELIMITER_INCORRECT_CHAR to NI_PATH_DELIMITER_CHAR
        if (c == PATH_DELIMITER_INCORRECT_CHAR)
        {
            bModified = true;
            c = PATH_DELIMITER_CHAR;
        }

        bool bThisCharIsSlash = (c == PATH_DELIMITER_CHAR);

        // Drop duplicate slashes
        if (bThisCharIsSlash && bLastCharWasSlash) 
        {
            // Unless this is a network resource name, e.g. "\\share"
            // which is true only in first two characters of string.
            if (uiRead > 1)
            {
                bModified = true;
                continue; // skip this char
            }
        }            

        bLastCharWasSlash = bThisCharIsSlash;

        // write char
        pcPath[uiWrite++] = c;
    }

    // Terminate (string may have shrunk)
    pcPath[uiWrite] = NULL;
    return bModified;
}

bool EzPath::getExecutableDirectory(char* pcPath, size_t stDestSize)
{
    size_t stWrittenChars = GetModuleFileName(GetModuleHandle(NULL), pcPath,
        stDestSize);

    if (stWrittenChars != NULL && stWrittenChars != stDestSize)
    {
        char* pcLastDirSlash = strrchr(pcPath, '\\');
        if (pcLastDirSlash == NULL)
            pcLastDirSlash = strrchr(pcPath, '/');

        if (pcLastDirSlash)
        {
            pcLastDirSlash[1] = '\0';
            makeStandardize(pcPath);
            return true;
        }
    }

    return false;
}
