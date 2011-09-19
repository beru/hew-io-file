#pragma once

#include <string.h>

struct StrProxy
{
	StrProxy()
		:
		s_(0),
		len_(0)
	{
	}

	StrProxy(const char* s)
		:
		s_(s),
		len_(strlen(s))
	{
	}
	
	StrProxy(const char* s, size_t len)
		:
		s_(s),
		len_(len)
	{
	}
	
	const char* s_;
	size_t len_;
};

inline bool operator == (const StrProxy& lhs, const StrProxy& rhs)
{
	return lhs.len_ == rhs.len_ && memcmp(lhs.s_, rhs.s_, lhs.len_) == 0;
}

