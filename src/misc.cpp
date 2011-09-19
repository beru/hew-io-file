#include "misc.h"


std::vector<uint32_t> GetLinePositions(const char* buff, size_t len)
{
	std::vector<uint32_t> ret;

	if (len == 0) {
		return ret;
	}
	
	ret.push_back(0);
	if (len == 1) {
		return ret;
	}
	
	char c = buff[0];
	bool bSkip = false;
	for (size_t i=0; i<len-1; ++i) {
		char nc = buff[i+1];
		if (bSkip) {
			ret.push_back(i+1);
			bSkip = false;
		}else {
			if (c == '\r') {
				if (nc == '\n') {
					bSkip = true;
				}else {
					ret.push_back(i+1);
				}
			}else if (c == '\n') {
				ret.push_back(i+1);
			}
		}
		c = nc;
	}
	
	
	return ret;
}

