#include "strutil.h"

#include <windows.h>

std::string ToMultiByte(const wchar_t* val)
{
	int length( ::WideCharToMultiByte( CP_ACP, 0, val, -1, NULL, 0, NULL, NULL ) );
	std::string result( length - 1, 0 );
	::WideCharToMultiByte( CP_ACP, 0, &val[ 0 ], -1, &result[ 0 ], length, NULL, NULL );
	return result;
}

std::wstring ToWideChar(const char* val)
{
	int length( ::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, &val[ 0 ], -1, NULL, 0 ) );
	std::wstring result( length - 1, 0 );
	::MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, &val[ 0 ], -1, &result[ 0 ], length );
	return result;
}

std::basic_string<TCHAR> ToT(const char* str)
{
#ifdef UNICODE
	return ToWideChar(str);
#else
	return str;
#endif
}

std::basic_string<TCHAR> ToT(const std::string& str)
{
	return ToT(str.c_str());
}

std::basic_string<TCHAR> ToT(const wchar_t* str)
{
#ifdef UNICODE
	return str;
#else
	return ToMultiByte(str);
#endif
}

std::basic_string<TCHAR> ToT(const std::wstring& str)
{
	return ToT(str.c_str());
}

