#ifndef _DUI_HELP_FILE_H_
#define _DUI_HELP_FILE_H_

#include <list>

namespace duihelp
{
	bool FilePathIsExist(const TCHAR *path, bool is_directory);

	bool ParsePathComponents(const TCHAR *path, std::list<String> &components);

	bool CreateDir(const TCHAR* path);
}
#endif