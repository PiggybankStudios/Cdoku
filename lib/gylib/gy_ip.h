/*
File:   gy_ip.h
Author: Taylor Robbins
Date:   07\09\2023
Description:
	** Holds functions that help us deal with Internet Protocol things (like ipv4, ipv6, urls, etc)
*/

#ifndef _GY_IP_H
#define _GY_IP_H

// +--------------------------------------------------------------+
// |                           Defines                            |
// +--------------------------------------------------------------+
#define IPV4_NUM_PARTS   4
#define IPV6_NUM_PARTS   8
#define IPV4_PART_SIZE   sizeof(u8)
#define IPV6_PART_SIZE   sizeof(u16)
#define IPV4_SEP_CHAR    '.' //period
#define IPV6_SEP_CHAR    ':' //colon

#define IPV4_MAX_STR_LENGTH      15 //3*4 + 3 = 3 characters per part, 4 parts, 3 periods in-between
#define IPV6_MAX_STR_LENGTH      39 //4*8 + 7 4 characters per part, 8 parts, 7 colons in-between
#define IPADDRESS_MAX_STR_LENGTH 39

#define DEFAULT_HTTP_PORT   80  //port #
#define DEFAULT_HTTPS_PORT  443 //port #

#define IP_PORT_RANGE_COMMON_SERVICES_START 0     //Used for common, well-known services
#define IP_PORT_RANGE_COMMON_SERVICES_END   1023  //(On Unix-like operating systems, using one of these ports requires superuser operating permission)
#define IP_PORT_RANGE_REGISTERED_START      1024  //The registered ports used for IANA-registered services
#define IP_PORT_RANGE_REGISTERED_END        49151
#define IP_PORT_RANGE_DYNAMIC_START         49152 //Not officially designated for any specific service and may be used for any purpose
#define IP_PORT_RANGE_DYNAMIC_END           65535

// +--------------------------------------------------------------+
// |                          Structures                          |
// +--------------------------------------------------------------+
typedef u16 IpPort_t;

typedef u8  Ipv4Part_t;
typedef u16 Ipv6Part_t;

union IpAddress4_t
{
	u8 parts[IPV4_NUM_PARTS];
	struct
	{
		u8 part0; u8 part1; u8 part2; u8 part3;
	};
};
union IpAddress6_t
{
	u16 parts[IPV6_NUM_PARTS];
	struct
	{
		u16 part0; u16 part1; u16 part2; u16 part3;
		u16 part4; u16 part5; u16 part6; u16 part7;
	};
};
struct IpAddress_t
{
	bool isIpv6;
	union
	{
		IpAddress4_t ipv4;
		IpAddress6_t ipv6;
	};
};
struct IpAddressAndPort_t
{
	IpAddress_t address;
	IpPort_t port;
};

// +--------------------------------------------------------------+
// |                            Macros                            |
// +--------------------------------------------------------------+
#define IpAddress_Zero      NewIpAddress(0, 0, 0, 0)
#define IpAddress_LocalHost NewIpAddress(127, 0, 0, 1)

// +--------------------------------------------------------------+
// |                         Header Only                          |
// +--------------------------------------------------------------+
#ifdef GYLIB_HEADER_ONLY
	IpAddress4_t NewIpAddress4(u8 part0, u8 part1, u8 part2, u8 part3);
	IpAddress6_t NewIpAddress6(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7);
	IpAddress_t NewIpAddress(u8 part0, u8 part1, u8 part2, u8 part3);
	IpAddress_t NewIpAddress(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7);
	IpAddressAndPort_t NewIpAddress4AndPort(u8 part0, u8 part1, u8 part2, u8 part3, IpPort_t port);
	IpAddressAndPort_t NewIpAddress6AndPort(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7, IpPort_t port);
	IpAddressAndPort_t NewIpAddressAndPort(IpAddress_t address, IpPort_t port);
	inline bool AreIpAddress4sEqual(IpAddress4_t left, IpAddress4_t right);
	inline bool AreIpAddress6sEqual(IpAddress6_t left, IpAddress6_t right);
	inline bool AreIpAddressesEqual(IpAddress_t left, IpAddress_t right);
	inline bool IsZero(IpAddress_t address);
	inline bool AreIpAddressAndPortsEqual(IpAddressAndPort_t left, IpAddressAndPort_t right);
	MyStr_t GetIpAddress4String(IpAddress4_t address, MemArena_t* memArena);
	MyStr_t GetIpAddress6String(IpAddress6_t address, MemArena_t* memArena);
	MyStr_t GetIpAddressString(IpAddress_t address, MemArena_t* memArena);
	bool TryParseIpAddress4(MyStr_t str, IpAddress4_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr);
	bool TryParseIpAddress6(MyStr_t str, IpAddress6_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr);
	bool TryParseIpAddress(MyStr_t str, IpAddress_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr);
	void SplitUrlHostAndPath(MyStr_t* fullUrl, MyStr_t* hostNameOut = nullptr, MyStr_t* pathNameOut = nullptr);
	MyStr_t GetUrlHostNamePart(MyStr_t* fullUrl);
	MyStr_t GetUrlPathPart(MyStr_t* fullUrl);
#else

// +--------------------------------------------------------------+
// |                        New Functions                         |
// +--------------------------------------------------------------+
IpAddress4_t NewIpAddress4(u8 part0, u8 part1, u8 part2, u8 part3)
{
	IpAddress4_t result;
	result.part0 = part0;
	result.part1 = part1;
	result.part2 = part2;
	result.part3 = part3;
	return result;
}

IpAddress6_t NewIpAddress6(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7)
{
	IpAddress6_t result;
	result.part0 = part0;
	result.part1 = part1;
	result.part2 = part2;
	result.part3 = part3;
	result.part4 = part4;
	result.part5 = part5;
	result.part6 = part6;
	result.part7 = part7;
	return result;
}

IpAddress_t NewIpAddress(u8 part0, u8 part1, u8 part2, u8 part3)
{
	IpAddress_t result;
	result.isIpv6 = false;
	result.ipv4 = NewIpAddress4(part0, part1, part2, part3);
	return result;
}
IpAddress_t NewIpAddress(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7)
{
	IpAddress_t result;
	result.isIpv6 = true;
	result.ipv6 = NewIpAddress6(part0, part1, part2, part3, part4, part5, part6, part7);
	return result;
}

IpAddressAndPort_t NewIpAddress4AndPort(u8 part0, u8 part1, u8 part2, u8 part3, IpPort_t port)
{
	IpAddressAndPort_t result;
	result.address.isIpv6 = false;
	result.address.ipv4 = NewIpAddress4(part0, part1, part2, part3);
	result.port = port;
	return result;
}
IpAddressAndPort_t NewIpAddress6AndPort(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7, IpPort_t port)
{
	IpAddressAndPort_t result;
	result.address.isIpv6 = true;
	result.address.ipv6 = NewIpAddress6(part0, part1, part2, part3, part4, part5, part6, part7);
	result.port = port;
	return result;
}
IpAddressAndPort_t NewIpAddressAndPort(IpAddress_t address, IpPort_t port)
{
	IpAddressAndPort_t result;
	result.address = address;
	result.port = port;
	return result;
}

// +--------------------------------------------------------------+
// |                Operator Overload Equivalents                 |
// +--------------------------------------------------------------+
inline bool AreIpAddress4sEqual(IpAddress4_t left, IpAddress4_t right)
{
	return (
		left.part0 == right.part0 &&
		left.part1 == right.part1 &&
		left.part2 == right.part2 &&
		left.part3 == right.part3
	);
}
inline bool AreIpAddress6sEqual(IpAddress6_t left, IpAddress6_t right)
{
	return (
		left.part0 == right.part0 &&
		left.part1 == right.part1 &&
		left.part2 == right.part2 &&
		left.part3 == right.part3 &&
		left.part4 == right.part4 &&
		left.part5 == right.part5
	);
}
inline bool AreIpAddressesEqual(IpAddress_t left, IpAddress_t right)
{
	if (left.isIpv6 != right.isIpv6) { return false; }
	if (left.isIpv6)
	{
		return AreIpAddress6sEqual(left.ipv6, right.ipv6);
		
	}
	else
	{
		return AreIpAddress4sEqual(left.ipv4, right.ipv4);
	}
}
inline bool IsZero(IpAddress_t address)
{
	return AreIpAddressesEqual(address, IpAddress_Zero);
}

inline bool AreIpAddressAndPortsEqual(IpAddressAndPort_t left, IpAddressAndPort_t right)
{
	if (left.port != right.port) { return false; }
	return AreIpAddressesEqual(left.address, right.address);
}

// +--------------------------------------------------------------+
// |                       String Functions                       |
// +--------------------------------------------------------------+
MyStr_t GetIpAddress4String(IpAddress4_t address, MemArena_t* memArena)
{
	return PrintInArenaStr(memArena, "%u.%u.%u.%u",
		address.part0, address.part1, address.part2, address.part3
	);
}
MyStr_t GetIpAddress6String(IpAddress6_t address, MemArena_t* memArena)
{
	return PrintInArenaStr(memArena, "%04X:%04X:%04X:%04X:%04X:%04X:%04X:%04X",
		address.part0, address.part1, address.part2, address.part3,
		address.part4, address.part5, address.part6, address.part7
	);
}
MyStr_t GetIpAddressString(IpAddress_t address, MemArena_t* memArena)
{
	if (address.isIpv6) { return GetIpAddress6String(address.ipv6, memArena); }
	else { return GetIpAddress4String(address.ipv4, memArena); }
}

// +--------------------------------------------------------------+
// |                       Parse Functions                        |
// +--------------------------------------------------------------+
bool TryParseIpAddress4(MyStr_t str, IpAddress4_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr)
{
	NotNullStr(&str);
	
	u64 numPeriods = 0;
	u64 periodIndices[IPV4_NUM_PARTS-1];
	for (u64 cIndex = 0; cIndex < str.length; cIndex++)
	{
		if (str.chars[cIndex] == IPV4_SEP_CHAR)
		{
			if (numPeriods >= ArrayCount(periodIndices))
			{
				SetOptionalOutPntr(reasonOut, TryParseFailureReason_TooManyCommas);
				return false;
			}
			periodIndices[numPeriods] = cIndex;
			numPeriods++;
		}
	}
	if (numPeriods < ArrayCount(periodIndices))
	{
		SetOptionalOutPntr(reasonOut, TryParseFailureReason_NotEnoughCommas);
		return false;
	}
	
	MyStr_t part0Str = StrSubstring(&str, 0, periodIndices[0]);
	MyStr_t part1Str = StrSubstring(&str, periodIndices[0]+1, periodIndices[1]);
	MyStr_t part2Str = StrSubstring(&str, periodIndices[1]+1, periodIndices[2]);
	MyStr_t part3Str = StrSubstring(&str, periodIndices[2]+1, str.length);
	
	if (part0Str.length > 3) { SetOptionalOutPntr(reasonOut, TryParseFailureReason_TooLong); return false; }
	if (part1Str.length > 3) { SetOptionalOutPntr(reasonOut, TryParseFailureReason_TooLong); return false; }
	if (part2Str.length > 3) { SetOptionalOutPntr(reasonOut, TryParseFailureReason_TooLong); return false; }
	if (part3Str.length > 3) { SetOptionalOutPntr(reasonOut, TryParseFailureReason_TooLong); return false; }
	
	u8 part0 = 0; if (!TryParseU8(part0Str, &part0, reasonOut, false, false)) { return false; }
	u8 part1 = 0; if (!TryParseU8(part1Str, &part1, reasonOut, false, false)) { return false; }
	u8 part2 = 0; if (!TryParseU8(part2Str, &part2, reasonOut, false, false)) { return false; }
	u8 part3 = 0; if (!TryParseU8(part3Str, &part3, reasonOut, false, false)) { return false; }
	
	if (valueOut != nullptr) { *valueOut = NewIpAddress4(part0, part1, part2, part3); }
	return true;
}
bool TryParseIpAddress6(MyStr_t str, IpAddress6_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr)
{
	NotNullStr(&str);
	NotNullStr(&str);
	
	u64 numColons = 0;
	u64 periodIndices[IPV6_NUM_PARTS-1];
	for (u64 cIndex = 0; cIndex < str.length; cIndex++)
	{
		if (str.chars[cIndex] == IPV6_SEP_CHAR)
		{
			if (numColons >= ArrayCount(periodIndices))
			{
				SetOptionalOutPntr(reasonOut, TryParseFailureReason_TooManyCommas);
				return false;
			}
			periodIndices[numColons] = cIndex;
			numColons++;
		}
	}
	if (numColons < ArrayCount(periodIndices))
	{
		SetOptionalOutPntr(reasonOut, TryParseFailureReason_NotEnoughCommas);
		return false;
	}
	
	MyStr_t part0Str = StrSubstring(&str, 0, periodIndices[0]);
	MyStr_t part1Str = StrSubstring(&str, periodIndices[0]+1, periodIndices[1]);
	MyStr_t part2Str = StrSubstring(&str, periodIndices[1]+1, periodIndices[2]);
	MyStr_t part3Str = StrSubstring(&str, periodIndices[2]+1, periodIndices[3]);
	MyStr_t part4Str = StrSubstring(&str, periodIndices[3]+1, periodIndices[4]);
	MyStr_t part5Str = StrSubstring(&str, periodIndices[4]+1, periodIndices[5]);
	MyStr_t part6Str = StrSubstring(&str, periodIndices[5]+1, periodIndices[6]);
	MyStr_t part7Str = StrSubstring(&str, periodIndices[6]+1, str.length);
	
	if (part0Str.length != 4) { SetOptionalOutPntr(reasonOut, part0Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	if (part1Str.length != 4) { SetOptionalOutPntr(reasonOut, part1Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	if (part2Str.length != 4) { SetOptionalOutPntr(reasonOut, part2Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	if (part3Str.length != 4) { SetOptionalOutPntr(reasonOut, part3Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	if (part4Str.length != 4) { SetOptionalOutPntr(reasonOut, part4Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	if (part5Str.length != 4) { SetOptionalOutPntr(reasonOut, part5Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	if (part6Str.length != 4) { SetOptionalOutPntr(reasonOut, part6Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	if (part7Str.length != 4) { SetOptionalOutPntr(reasonOut, part7Str.length > 4 ? TryParseFailureReason_TooLong : TryParseFailureReason_TooShort); return false; }
	
	u16 part0 = 0; if (!TryParseU16(part0Str, &part0, reasonOut, true, false, false)) { return false; }
	u16 part1 = 0; if (!TryParseU16(part1Str, &part1, reasonOut, true, false, false)) { return false; }
	u16 part2 = 0; if (!TryParseU16(part2Str, &part2, reasonOut, true, false, false)) { return false; }
	u16 part3 = 0; if (!TryParseU16(part3Str, &part3, reasonOut, true, false, false)) { return false; }
	u16 part4 = 0; if (!TryParseU16(part4Str, &part4, reasonOut, true, false, false)) { return false; }
	u16 part5 = 0; if (!TryParseU16(part5Str, &part5, reasonOut, true, false, false)) { return false; }
	u16 part6 = 0; if (!TryParseU16(part6Str, &part6, reasonOut, true, false, false)) { return false; }
	u16 part7 = 0; if (!TryParseU16(part7Str, &part7, reasonOut, true, false, false)) { return false; }
	
	if (valueOut != nullptr) { *valueOut = NewIpAddress6(part0, part1, part2, part3, part4, part5, part6, part7); }
	return true;
}
bool TryParseIpAddress(MyStr_t str, IpAddress_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr)
{
	NotNullStr(&str);
	if (TryParseIpAddress6(str, (valueOut != nullptr) ? &valueOut->ipv6 : nullptr, reasonOut))
	{
		if (valueOut != nullptr) { valueOut->isIpv6 = true; }
		return true;
	}
	else if (TryParseIpAddress4(str, (valueOut != nullptr) ? &valueOut->ipv4 : nullptr, reasonOut))
	{
		if (valueOut != nullptr) { valueOut->isIpv6 = false; }
		return true;
	}
	else { return false; }
}

// +--------------------------------------------------------------+
// |                        Url Functions                         |
// +--------------------------------------------------------------+
void SplitUrlHostAndPath(MyStr_t* fullUrl, MyStr_t* hostNameOut = nullptr, MyStr_t* pathNameOut = nullptr)
{
	for (u32 cIndex = 0; cIndex < fullUrl->length; cIndex++)
	{
		if (fullUrl->chars[cIndex] == '/' || fullUrl->chars[cIndex] == '\\')
		{
			SetOptionalOutPntr(hostNameOut, StrSubstring(fullUrl, 0, cIndex));
			SetOptionalOutPntr(pathNameOut, StrSubstring(fullUrl, cIndex));
			return;
		}
	}
	SetOptionalOutPntr(hostNameOut, *fullUrl);
	SetOptionalOutPntr(pathNameOut, MyStr_Empty);
}
MyStr_t GetUrlHostNamePart(MyStr_t* fullUrl)
{
	MyStr_t hostNameStr = MyStr_Empty;
	SplitUrlHostAndPath(fullUrl, &hostNameStr, nullptr);
	return hostNameStr;
}
MyStr_t GetUrlPathPart(MyStr_t* fullUrl)
{
	MyStr_t pathStr = MyStr_Empty;
	SplitUrlHostAndPath(fullUrl, nullptr, &pathStr);
	return pathStr;
}

#endif //GYLIB_HEADER_ONLY

#endif //  _GY_IP_H

// +--------------------------------------------------------------+
// |                   Autocomplete Dictionary                    |
// +--------------------------------------------------------------+
/*
@Defines
IPV4_NUM_PARTS
IPV6_NUM_PARTS
IPV4_PART_SIZE
IPV6_PART_SIZE
IPV4_SEP_CHAR
IPV6_SEP_CHAR
IPV4_MAX_STR_LENGTH
IPV6_MAX_STR_LENGTH
IPADDRESS_MAX_STR_LENGTH
DEFAULT_HTTP_PORT
DEFAULT_HTTPS_PORT
IP_PORT_RANGE_COMMON_SERVICES_START
IP_PORT_RANGE_COMMON_SERVICES_END
IP_PORT_RANGE_REGISTERED_START
IP_PORT_RANGE_REGISTERED_END
IP_PORT_RANGE_DYNAMIC_START
IP_PORT_RANGE_DYNAMIC_END
IpAddress_Zero
IpAddress_LocalHost
@Types
IpPort_t
Ipv4Part_t
Ipv6Part_t
IpAddress4_t
IpAddress6_t
IpAddress_t
IpAddressAndPort_t
@Functions
IpAddress4_t NewIpAddress4(u8 part0, u8 part1, u8 part2, u8 part3)
IpAddress6_t NewIpAddress6(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7)
IpAddress_t NewIpAddress(u8 part0, u8 part1, u8 part2, u8 part3)
IpAddressAndPort_t NewIpAddress4AndPort(u8 part0, u8 part1, u8 part2, u8 part3, IpPort_t port)
IpAddressAndPort_t NewIpAddress6AndPort(u16 part0, u16 part1, u16 part2, u16 part3, u16 part4, u16 part5, u16 part6, u16 part7, IpPort_t port)
IpAddressAndPort_t NewIpAddressAndPort(IpAddress_t address, IpPort_t port)
inline bool AreIpAddress4sEqual(IpAddress4_t left, IpAddress4_t right)
inline bool AreIpAddress6sEqual(IpAddress6_t left, IpAddress6_t right)
inline bool AreIpAddressesEqual(IpAddress_t left, IpAddress_t right)
inline bool IsZero(IpAddress_t address)
inline bool AreIpAddressAndPortsEqual(IpAddressAndPort_t left, IpAddressAndPort_t right)
MyStr_t GetIpAddress4String(IpAddress4_t address, MemArena_t* memArena)
MyStr_t GetIpAddress6String(IpAddress6_t address, MemArena_t* memArena)
MyStr_t GetIpAddressString(IpAddress_t address, MemArena_t* memArena)
bool TryParseIpAddress4(MyStr_t str, IpAddress4_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr)
bool TryParseIpAddress6(MyStr_t str, IpAddress6_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr)
bool TryParseIpAddress(MyStr_t str, IpAddress_t* valueOut = nullptr, TryParseFailureReason_t* reasonOut = nullptr)
void SplitUrlHostAndPath(MyStr_t* fullUrl, MyStr_t* hostNameOut = nullptr, MyStr_t* pathNameOut = nullptr)
MyStr_t GetUrlHostNamePart(MyStr_t* fullUrl)
MyStr_t GetUrlPathPart(MyStr_t* fullUrl)
*/
