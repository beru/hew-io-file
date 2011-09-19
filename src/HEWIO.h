#pragma once

#include "strProxy.h"
#include <vector>

namespace HEWIO {

struct ModuleDef
{
	ModuleDef()
		:
		registerSlotPos(0),
		registerCount(0)
//		,registerNumber(0)
	{
	}
	StrProxy name;
	uint16_t registerSlotPos;
	uint16_t registerCount;
//	uint8_t registerNumber;	// ? RegX_n の X ?
};

enum SizeType {
	SizeType_Byte,
	SizeType_Word,
	SizeType_Long,
};

enum FormatType {
	FormatType_Hex,
	FormatType_Decimal,
	FormatType_Binary,
};

struct RegisterDef
{
	RegisterDef()
		:
		bitFieldsIdx(0)
	{
	}
	StrProxy id;
	uint32_t address;
	SizeType size;
	bool isAbsolute;
	FormatType format;
	uint16_t bitFieldsIdx;	// 0 は非ビットフィールド
	
};

struct BitFieldsDef
{
	BitFieldsDef()
		:
		bitSlotPos(0),
		fieldCount(0)
	{
	}
	uint16_t bitSlotPos;
	uint8_t fieldCount;
};

struct BitFieldDef
{
	StrProxy name;
	uint8_t bitNo;
	uint8_t bitLen;
};

struct Data
{
public:
	bool Parse(const char* str, size_t len);
	
	const ModuleDef* GetModule(size_t& count) const;
	const RegisterDef* GetRegister(size_t moduleIdx, size_t& count) const;
	const BitFieldDef* GetBitField(size_t moduleIdx, size_t registerIdx, size_t& count) const;
private:
	uint8_t fileVersion_;
	std::vector<ModuleDef> moduleDefs_;
	std::vector<RegisterDef> registerDefs_;
	std::vector<BitFieldsDef> bitFieldsDefs_;
	std::vector<BitFieldDef> bitFieldDefs_;
};

} // namespace HEWIO
