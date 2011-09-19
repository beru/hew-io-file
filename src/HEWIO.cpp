#include "HEWIO.h"

#include <algorithm>

#include "misc.h"


namespace HEWIO {

enum ParseMode {
	ParseMode_Null,
	ParseMode_Modules,
	ParseMode_Module,
	ParseMode_BitFields,
	ParseMode_Register,

};

bool Data::Parse(const char* str, size_t len)
{
	const std::vector<size_t> linePositions = GetLinePositions(str, len);
	size_t nLines = linePositions.size();
	if (nLines < 2) {
		return 0;
	}
	size_t linePos = linePositions[0];
	const char* sectionName = 0;
	size_t sectionNameLen = 0;
	ParseMode mode = ParseMode_Null;

	ModuleDef* pModule;
	size_t moduleIdx;
	BitFieldsDef* pBitFields = 0;
	size_t bitFieldsIdx;


	for (size_t i=0; i<nLines; ++i) {
		size_t nextLinePos = (i<nLines-1) ? linePositions[i+1] : len;
		const char* line = &str[linePos];
		size_t lineLen = nextLinePos - linePos;
		linePos = nextLinePos;
		if (line[0] == ';') {
			continue;
		}else if (line[0] == '[') {
			const char* pr = std::find(&line[1], &line[lineLen-1], ']');
			if (!pr) {
				printf("section name close letter ] was not found at line %d\n", i);
				return 0;
			}
			sectionName = &line[1];
			sectionNameLen = pr - sectionName;

			// switch
			switch (mode) {
			case ParseMode_Null:
				mode = ParseMode_Modules;
				break;
			case ParseMode_Modules:
				mode = ParseMode_Module;
				moduleIdx = 0;
				pModule = &moduleDefs_[0];
				break;
			case ParseMode_Module:
				if (strncmp(sectionName, "Bit", 3) == 0) {
					mode = ParseMode_BitFields;
					bitFieldsIdx = 0;
				}else {
					int regCount = pModule->registerSlotPos + pModule->registerCount;
					++moduleIdx;
					pModule = &moduleDefs_[moduleIdx];
					pModule->registerSlotPos = regCount;
				}
				break;
			case ParseMode_BitFields:
				if (strncmp(sectionName, "Reg", 3) == 0) {
					mode = ParseMode_Register;
				}else {
					++bitFieldsIdx;
				}
				break;
			case ParseMode_Register:
				
				break;
			}

			// regist
			switch (mode) {
			case ParseMode_Module:
				break;
			case ParseMode_BitFields:
				{
					int pos = 0;
					if (bitFieldsIdx) {
						pos = pBitFields->bitSlotPos + pBitFields->fieldCount;
					}
					BitFieldsDef def;
					bitFieldsDefs_.push_back(def);
					pBitFields = &bitFieldsDefs_[bitFieldsIdx];
					pBitFields->bitSlotPos = pos;
				}
				break;
			case ParseMode_Register:
				break;
			}
		}else {
			const char* eqPos = (const char*) memchr(line, '=', len);
			if (!eqPos) {
				continue;
			}
			size_t ipos = eqPos - line;
			StrProxy left(line, ipos);
			StrProxy right(eqPos+1, lineLen - 2 - (ipos+1));
			int hoge = 1;
			switch (mode) {
			case ParseMode_Modules:
				if (strncmp("Module", left.s_, 6) == 0) {
					ModuleDef def;
					def.name = right;
					moduleDefs_.push_back(def);
				}else if (strncmp("FileVersion", left.s_, 11) == 0) {
					fileVersion_ = atoi(right.s_);
				}
				break;
			case ParseMode_Module:
				++pModule->registerCount;
				break;
			case ParseMode_BitFields:
				{
					BitFieldDef def;
					def.bitNo = atoi(left.s_+3);
					const char* pSpace = (const char*) memchr(right.s_, ' ', right.len_);
					if (pSpace) {
						def.name = StrProxy(right.s_, pSpace-right.s_);
						def.bitLen = atoi(pSpace+1);
					}else {
						def.name = right;
						def.bitLen = 1;
					}
					bitFieldDefs_.push_back(def);
				}
				++pBitFields->fieldCount;
				break;
			case ParseMode_Register:
				if (strncmp(left.s_, "id", 2) == 0) {
					RegisterDef def;
					const char* pSpace;
					pSpace = (const char*) memchr(right.s_, ' ', right.len_);
					if (!pSpace) {
						break;
					}
					def.id = StrProxy(right.s_, pSpace - right.s_);
					const char* pAddr = pSpace + 1;
					if (pAddr[0] != '0' || pAddr[1] != 'x') {
						break;
					}
					pSpace = (const char*) memchr(pAddr, ' ', right.len_ - 1 - def.id.len_);
					if (!pSpace) {
						break;
					}
					def.address = strtoul(pAddr, 0, 16);
					switch (pSpace[1]) {
					case 'W': def.size = SizeType_Word; break;
					case 'B': def.size = SizeType_Byte; break;
					case 'L': def.size = SizeType_Long; break;
					}
					def.isAbsolute = (pSpace[2] == 'A');
					switch (pSpace[3]) {
					case 'H': def.format = FormatType_Hex; break;
					case 'D': def.format = FormatType_Decimal; break;
					case 'B': def.format = FormatType_Binary; break;
					}
					pSpace = (const char*) memchr(pSpace+3, ' ', (right.s_+right.len_) - (pSpace+3));
					if (pSpace) {
						if (strncmp(pSpace+1, "Bit", 3) == 0) {
							def.bitFieldsIdx = atoi(pSpace+4);
						}
					}
					registerDefs_.push_back(def);
				}
				break;
			}
		}
		int hoge = 1;
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
	if (registerIdx >= regCount || pReg->bitFieldsIdx == 0) {
		count = 0;
		return 0;
	}
	const BitFieldsDef& fields = bitFieldsDefs_[pReg->bitFieldsIdx];
	count = fields.fieldCount;
	return &bitFieldDefs_[fields.bitSlotPos];
}

} // namespace HEWIO
