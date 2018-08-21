#ifndef _DUI_HELP_IMAGE_H_
#define _DUI_HELP_IMAGE_H_

namespace duihelp
{
	bool BaseSaveBitmap(HBITMAP hbitmap, const std::wstring save_path);

	bool CreateImageObject(void **ppv);
	String GetIconByFile(String file);
	bool FilePathExtension(const String &filepath_in, String &extension_out);


}
#endif