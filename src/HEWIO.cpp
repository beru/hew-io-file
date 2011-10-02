#include "HEWIO.h"

#include <algorithm>
#include <map>
#include <string>

#include "misc.h"


namespace HEWIO {

enum ParseMode {
	ParseMode_Null,
	ParseMode_Modules,
	ParseMode_Module,
	ParseMode_BitFields,
	ParseMode_Register,

};



void getSectionLinePositions(
	const char* str, size_t len,
	const std::vector<size_t>& linePositions,
	std::vector<size_t>& sectionLineIdxs,
	std::map<std::string, size_t>& idxMap
	)
{
	const size_t nLines = linePositions.size();
	size_t linePos = linePositions[0];
	char buff[256];
	for (size_t i=0; i<nLines; ++i) {
		size_t nextLinePos = (i<nLines-1) ? linePositions[i+1] : len;
		const char* line = &str[linePos];
		size_t lineLen = nextLinePos - linePos;
		linePos = nextLinePos;
		if (line[0] != '[') {
			continue;
		}
		const char* pr = std::find(&line[1], &line[lineLen-1], ']');
		if (!pr) {
			printf("section name close letter ] was not found at line %d\n", i);
			return;
		}
		sectionLineIdxs.push_back(i);
		memcpy(buff, line+1, pr-line-1);
		buff[pr-line-1] = 0;
		std::string str = buff;
		idxMap.insert(std::make_pair(str, sectionLineIdxs.size()-1));
	}
}

bool Data::Parse(const char* str, size_t len)
{
	moduleDefs_.clear();
	registerDefs_.clear();
	bitFieldsDefs_.clear();
	bitFieldDefs_.clear();

	std::vector<size_t> linePositions = GetLinePositions(str, len);
	linePositions.push_back(len);
	std::vector<size_t> lineIdxs;
	std::map<std::string, size_t> idxMap;
	getSectionLinePositions(str, len, linePositions, lineIdxs, idxMap);
	
	std::map<std::string, size_t>::const_iterator idxIt = idxMap.find("Modules");
	if (idxIt == idxMap.end()) {
		return false;
	}
	size_t idx = idxIt->second;
	if (idx >= lineIdxs.size()) {
		return false;
	}
	size_t moduleLineIdx = lineIdxs[idx] + 1;
	size_t modulesCount = lineIdxs[idx+1] - moduleLineIdx;
	char buff[512];
	for (size_t i=0; i<modulesCount; ++i) {
		const char* pLine = &str[linePositions[moduleLineIdx+i]];
		const char* pNextLine = &str[linePositions[moduleLineIdx+i+1]];
		size_t lineLen = pNextLine - pLine;
		if (memcmp(pLine, "Module", 6) != 0) {
			continue;
		}
		const char* pSpace = (const char*) memchr(pLine+6, '=', lineLen-6);
		if (!pSpace) {
			continue;
		}
		moduleDefs_.resize(moduleDefs_.size()+1);
		ModuleDef& moduleDef = moduleDefs_[moduleDefs_.size()-1];
		moduleDef.name = StrProxy(pSpace+1, (pNextLine-2)-(pSpace+1));
		memcpy(buff, moduleDef.name.s_, moduleDef.name.len_);
		buff[moduleDef.name.len_] = 0;
		idxIt = idxMap.find(buff);
		if (idxIt == idxMap.end()) {
			continue;
		}
		size_t regLineIdx = lineIdxs[idxIt->second] + 1;
		size_t regCount = lineIdxs[idxIt->second+1] - regLineIdx;
		moduleDef.registerSlotPos = registerDefs_.size();
		moduleDef.registerCount = regCount;
		registerDefs_.resize(moduleDef.registerSlotPos + regCount);
		for (size_t j=0; j<regCount; ++j) {
			const char* pLine = &str[linePositions[regLineIdx+j]];
			const char* pNextLine = &str[linePositions[regLineIdx+j+1]];
			size_t lineLen = (pNextLine-2) - pLine;
			const char* pEqual = (const char*) memchr(pLine+4, '=', lineLen-4);
			if (!pEqual) {
				continue;
			}
			const char* name = pEqual+1;
			size_t len = (pNextLine-2) - (pEqual+1);
			memcpy(buff, name, len);
			buff[len] = 0;
			idxIt = idxMap.find(buff);
			if (idxIt == idxMap.end()) {
				continue;
			}
			size_t lineIdx = lineIdxs[idxIt->second] + 1;
			pLine = &str[linePositions[lineIdx]];
			pNextLine = &str[linePositions[lineIdx+1]];
			lineLen = (pNextLine-2) - pLine;
			if (memcmp(pLine, "id=", 3) != 0) {
				continue;
			}
			StrProxy right(pLine+3, lineLen-3);
			const char* pSpace = (const char*) memchr(right.s_, ' ', right.len_);
			if (!pSpace) {
				break;
			}
			RegisterDef& registerDef = registerDefs_[moduleDef.registerSlotPos + j];
			registerDef.id.s_ = pLine + 3;
			registerDef.id.len_ = pSpace - right.s_;
			const char* pAddr = pSpace + 1;
			if (pAddr[0] != '0' || pAddr[1] != 'x') {
				break;
			}
			pSpace = (const char*) memchr(pAddr, ' ', right.len_ - 1 - registerDef.id.len_);
			if (!pSpace) {
				break;
			}
			registerDef.address = strtoul(pAddr, 0, 16);
			const char* ps = pSpace + 1;
			switch (*ps++) {
			case 'W': registerDef.size = SizeType_Word; break;
			case 'B': registerDef.size = SizeType_Byte; break;
			case 'L': registerDef.size = SizeType_Long; break;
			}
			if (*ps == ' ') {
				++ps;
			}
			if (*ps++ != 'A') {
				break;
			}
			registerDef.isAbsolute = true;
			if (*ps == ' ') {
				++ps;
			}
			switch (*ps++) {
			case 'H': registerDef.format = FormatType_Hex; break;
			case 'D': registerDef.format = FormatType_Decimal; break;
			case 'B': registerDef.format = FormatType_Binary; break;
			}
			if (*ps++ == ' ') {
				len = lineLen-(ps-pLine);
				memcpy(buff, ps, len);
				buff[len] = 0;
				idxIt = idxMap.find(buff);
				if (idxIt == idxMap.end()) {
					break;
				}
				registerDef.bitFieldsIdx = bitFieldsDefs_.size();
				bitFieldsDefs_.resize(registerDef.bitFieldsIdx+1);
				BitFieldsDef& bitFields = bitFieldsDefs_[registerDef.bitFieldsIdx];
				size_t beginLine = lineIdxs[idxIt->second] + 1;
				size_t endLine = lineIdxs[idxIt->second + 1];
				size_t bfCount = endLine - beginLine;
				size_t bfIdx = bitFieldDefs_.size();
				bitFields.bitSlotPos = bfIdx;
				bitFieldDefs_.resize(bfIdx + bfCount);
				for (size_t k=0; k<bfCount; ++k) {
					pLine = &str[linePositions[beginLine+k]];
					pNextLine = &str[linePositions[beginLine+k+1]];
					lineLen = (pNextLine-2) - pLine;
					if (memcmp(pLine, "bit", 3) != 0) {
						continue;
					}
					BitFieldDef& def = bitFieldDefs_[bfIdx + k];
					def.bitNo = atoi(pLine+3);
					pEqual = (const char*) memchr(pLine+4, '=', lineLen-4);
					if (!pEqual) {
						continue;
					}
					StrProxy right(pEqual+1, lineLen-(pEqual+1-pLine));
					const char* pSpace = (const char*) memchr(right.s_, ' ', right.len_);
					if (pSpace) {
						def.name = StrProxy(right.s_, pSpace-right.s_);
						def.bitLen = atoi(pSpace+1);
					}else {
						def.name = right;
						def.bitLen = 1;
					}
					++bitFields.fieldCount;
				}
			}

		}
	}

}

const ModuleDef* Data::GetModule(size_t& count) const
{
	count = moduleDefs_.size();
	return &moduleDefs_[0];
}

const RegisterDef* Data::GetRegister(size_t moduleIdx, size_t& count) const
{
	const ModuleDef& module = moduleDefs_[moduleIdx];
	count = module.registerCount;
	return &registerDefs_[module.registerSlotPos];
}

const BitFieldDef* Data::GetBitField(size_t moduleIdx, size_t registerIdx, size_t& count) const
{
	size_t regCount;
	const RegisterDef* pReg = GetRegister(moduleIdx, regCount) + registerIdx;
	if (registerIdx >= regCount || pReg->bitFieldsIdx == -1) {
		count = 0;
		return 0;
	}
	const BitFieldsDef& fields = bitFieldsDefs_[pReg->bitFieldsIdx];
	count = fields.fieldCount;
	return &bitFieldDefs_[fields.bitSlotPos];
}

} // namespace HEWIO
