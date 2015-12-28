#pragma once

#include "EzCoreLibType.h"


#define PATH_DELIMITER_STR "\\"
#define PATH_DELIMITER_CHAR '\\'
#define PATH_DELIMITER_INCORRECT_STR "/"
#define PATH_DELIMITER_INCORRECT_CHAR '/'

class EZCORE_ENTRY EzPath
{
public:
	EzPath(void);
	~EzPath(void);

	static bool makeStandardize(char* pcPath);

	static bool getExecutableDirectory(char* pcPath, size_t stDestSize);
	static bool getCurrentWorkingDirectory(char* pcPath, size_t stDestSize);
};

