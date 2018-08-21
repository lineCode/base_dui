#include "stdafx.h"
#include "file.h"

namespace duihelp
{
	bool FilePathIsExist(const TCHAR *path, bool is_directory)
	{
		const DWORD file_attr = ::GetFileAttributes(path);
		if (file_attr != INVALID_FILE_ATTRIBUTES)
		{
			if (is_directory)
				return (file_attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
			else
				return true;
		}
		return false;
	}

	bool CreateDir(const TCHAR* path)
	{
		if (path == nullptr)
			return false;

		if (FilePathIsExist(path, true))
			return true;

		std::list<String> subpaths;
		ParsePathComponents(path, subpaths);
		if (subpaths.empty())
			return false;

		// Collect a list of all parent directories.
		auto curr = subpaths.begin();
		for (auto prev = curr++; curr != subpaths.end(); curr++) {
			*curr = *prev + *curr;
			prev = curr;
		}

		// Iterate through the parents and create the missing ones.
		for (auto i = subpaths.begin(); i != subpaths.end(); ++i) {
			if (FilePathIsExist(i->c_str(), true))
				continue;
			if (!::CreateDirectoryW(i->c_str(), NULL)) {
				DWORD error = ::GetLastError();
				if (error == ERROR_ALREADY_EXISTS &&
					FilePathIsExist(i->c_str(), true)) {
					continue;
				}
				return false;
			}
		}
		return true;
	}

	bool ParsePathComponents(const TCHAR *path, std::list<String> &components)
	{
		components.clear();
		if (path == NULL)
			return false;
		const TCHAR *seperators = _T("\\/");
		const TCHAR *prev = NULL;
		const TCHAR *next = path;
		const TCHAR *c;
		while (*next)
		{
			prev = next;
			// find the first seperator
			for (;;)
			{
				for (c = seperators; *c && *next != *c; c++);
				if (*c || !*next)
					break;
				next++;
			}
			components.push_back(String(prev, next - prev));
			if (*next)
				components.back().push_back(*seperators);
			// skip duplicated seperators
			for (++next;;)
			{
				for (c = seperators; *c && *next != *c; c++);
				if (!*c)
					break;
				next++;
			}
		}
		return true;
	}

}