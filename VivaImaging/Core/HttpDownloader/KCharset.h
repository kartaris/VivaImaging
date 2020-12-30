
#ifndef _K_CHARSET_H_
#define _K_CHARSET_H_

typedef enum
{
	KCHARSET_ACTIVE = 0,		// CP_ACP

	KCHARSET_EN = 1252,
	KCHARSET_US = 437,
	KCHARSET_FR = 863,
	KCHARSET_DE = 1141,
	KCHARSET_IT = 1144,
	KCHARSET_NL = 865,
	KCHARSET_EL = 737,
	KCHARSET_ES = 1145,
	KCHARSET_PT = 860,
	KCHARSET_AR = 864,
	KCHARSET_HE = 862,
	KCHARSET_RU = 866,
	KCHARSET_UR = 918,
	KCHARSET_ZH = 936,
	KCHARSET_JP = 932,
	KCHARSET_KO = 949,
	/*
	KCHARSET_HI = ,
	KCHARSET_UR,
	KCHARSET_SA,
	*/

	KCHARSET_UTF_8 = 65001,

	KCHARSET_BE = 1252,
	KCHARSET_BG = 21025,
	KCHARSET_SV = 278,
	KCHARSET_CA = 284,
	KCHARSET_TR = 1254,
	KCHARSET_ET = 922,

	KCHARSET_8859_1 = 28591,
	KCHARSET_8859_2 = 28592,
	KCHARSET_8859_3 = 28593,
	KCHARSET_8859_4 = 28594,
	KCHARSET_8859_5 = 28595,
	KCHARSET_8859_6 = 28596,
	KCHARSET_8859_7 = 28597,
	KCHARSET_8859_8 = 28598,
	KCHARSET_8859_9 = 28599,
	KCHARSET_8859_15 = 28605,

	KCHARSET_LAO = 1258, // Vietnamese
	KCHARSET_THAI = 874,

	KCHARSET_GB2312 = 20936,
	KCHARSET_GB18030 = 54936,

	KCHARSET_BIG5 = 950,

	KCHARSET_UTF_16 = 65500,
	KCHARSET_UNDEFINED = 65535
}K2Charset;

K2Charset getLanguageCode(const char* lang);
const char* getLanguageString( K2Charset langCode );

#endif // _K_CHARSET_H_
