#include "stdafx.h"
#include "KCharset.h"
#include "KString.h"
#include "PlatformLib.h"

#define MAX_HTML_CHAR_NAME_LIST 15

typedef struct tagIntNamePair
{
	int value;
	const char* name;
} IntNamePair;

IntNamePair K2LangNameList[] =
{
	{ KCHARSET_AR, "ar" }, // Arabic
	/*
	{ KCHARSET_BE, "be" }, // Belarusian
	{ KCHARSET_BG, "bg" }, // Bulgarian
	{ KCHARSET_CA, "ca" }, // Catalan
	{ KCHARSET_CS, "cs" }, // Czech
	{ KCHARSET_DA, "da" }, // Danish
	*/
	{ KCHARSET_DE, "de" }, // German
	{ KCHARSET_EL, "el" }, // Greek
	{ KCHARSET_EN, "en" },
	{ KCHARSET_ES, "es" }, // Spanish
	{ KCHARSET_ET, "et" }, // Estonian
	/*
	{ KCHARSET_EU, "eu" }, // Basque
	{ KCHARSET_FA, "fa" }, // Farsi
	{ KCHARSET_FI, "fi" }, // Finnish 
	{ KCHARSET_FO, "fo" }, // Faeroese
	*/
	{ KCHARSET_FR, "fr" }, // French
	/*
	{ KCHARSET_GA, "ga" }, // Irish
	{ KCHARSET_GD, "gd" }, // Gaelic(Scotland)
	{ KCHARSET_HE, "he" }, // Hebrew
	{ KCHARSET_HI, "hi" }, // Hindi
	{ KCHARSET_HR, "hr" }, // Croatian 
	{ KCHARSET_HU, "hu" }, // Hungarian 
	{ KCHARSET_ID, "id" }, // Indonesian
	*/
	{ KCHARSET_IT, "it" }, // Italian
	/*
	{ KCHARSET_JI, "ji" }, // Yiddish 
	*/
	{ KCHARSET_JP, "jp" }, // Japanese
	{ KCHARSET_KO, "ko" },
	/*
	{ KCHARSET_LT, "lt" }, // Lithuanian
	{ KCHARSET_LV, "lv" }, // Latvian
	{ KCHARSET_MK, "mk" }, // Macedonian(FYROM)
	{ KCHARSET_MS, "ms" }, // Malaysian
	{ KCHARSET_MT, "mt" }, // Maltese
	*/
	{ KCHARSET_NL, "nl" }, // Dutch
	/*
	{ KCHARSET_NO, "no" }, // Norwegian (Bokmal)
	{ KCHARSET_PL, "pl" }, // Polish
	*/
	{ KCHARSET_PT, "pt" }, // Portuguese
	/*
	{ KCHARSET_RM, "rm" }, // Rhaeto-Romanic
	{ KCHARSET_RO, "ro" }, // Romanian
	*/
	{ KCHARSET_RU, "ru" }, // Russian
	// { KCHARSET_SB, "sb" }, // Sorbian
	// { KCHARSET_SK, "sk" }, // Slovak
	// { KCHARSET_SL, "sl" }, // Slovenian
	// { KCHARSET_SR, "sr" }, // Serbian (Cyrillic)
	{ KCHARSET_SV, "sv" }, // Swedish
	// { KCHARSET_SX, "sx" }, // Sutu
	// { KCHARSET_SZ, "sz" }, // Sami (Lappish)
	// { KCHARSET_TH, "th" }, // Thai
	// { KCHARSET_TN, "tn" }, // Tswana
	{ KCHARSET_TR, "tr" }, // Turkish
	// { KCHARSET_TS, "ts" }, // Tsonga
	{ KCHARSET_UR, "ur" }, // Urdu
	// { KCHARSET_UK, "uk" }, // Ukrainian
	// { KCHARSET_VI, "vi" }, // Vietnamese
	// { KCHARSET_XH, "xh" }, // Xhosa
	{ KCHARSET_ZH, "zh" } // Chinese
	/*
	{ KCHARSET_ZU, "zu" } // Zulu
	*/
};

IntNamePair K2CharsetNameList[] =
{
	{ KCHARSET_UTF_8, "UTF-8" },
	{ KCHARSET_UTF_16, "UTF-16" },
	{ KCHARSET_8859_1, "ISO_8859-1" },
	{ KCHARSET_8859_2, "ISO_8859-2" },
	{ KCHARSET_8859_3, "ISO_8859-3" },
	{ KCHARSET_8859_4, "ISO_8859-4" },
	{ KCHARSET_8859_5, "ISO_8859-5" },
	{ KCHARSET_8859_6, "ISO_8859-6" },
	{ KCHARSET_8859_7, "ISO_8859-7" },
	{ KCHARSET_8859_8, "ISO_8859-8" },
	{ KCHARSET_8859_9, "ISO_8859-9" },
	{ KCHARSET_8859_15, "ISO_8859-15" },
	{ KCHARSET_AR, "ISO_8859-6" },
	{ KCHARSET_JP, "Shift_JIS" },
	{ KCHARSET_JP, "SJIS" },
	{ KCHARSET_JP, "x-sjis" },
	{ KCHARSET_JP, "jisx0201" },
	{ KCHARSET_JP, "ISO-2022-JP" },
	{ KCHARSET_JP, "ja_JP" },
	{ KCHARSET_JP, "JIS" },
	{ KCHARSET_JP, "EUC-JP" },
	{ KCHARSET_KO, "EUC-KR" },
	{ KCHARSET_KO, "ko_KR" },
	{ KCHARSET_KO, "ksc5601-1987" },
	{ KCHARSET_KO, "ksc5601" },
	{ KCHARSET_LAO, "mulelao-1" },
	{ KCHARSET_THAI, "TIS-620" },
	{ KCHARSET_ZH, "EUC-CN" },
	{ KCHARSET_ZH, "zh_CH" },
	{ KCHARSET_GB2312, "GBK" },
	{ KCHARSET_GB2312, "GB2312" },
	{ KCHARSET_GB18030, "GB18030" },
	{ KCHARSET_BIG5, "Big5" },
	{ KCHARSET_BIG5, "Big5Hkscs-0" },
	{ KCHARSET_BIG5, "HKSCS-1" },
	{ KCHARSET_BIG5, "HKSCS" }
};

KString removeUnserscreDash(const KString& code)
{
	int i, count;
	KString name;
	char ch;

	count = code.length();
	for(i = 0; i < count; i++)
	{
		ch = code.at(i);
		if((ch != '_') && (ch != '-'))
			name += ch;
	}
	// KTRACE("removeUnserscreDash(%s, %s)\n", code.data(), name.data());
	return name;
}

K2Charset getLanguageCode(const char* lang)
{
	KString code(lang);
	KString name;
	int i, count;

	// KTRACE("find  charset : %s\n", lang);

	count = sizeof(K2CharsetNameList) / sizeof(IntNamePair);
	for(i = 0; i < count; i++)
	{
		if(code.compare(K2CharsetNameList[i].name, false) == 0)
		{
			// KTRACE("found charset1 : %s\n", K2CharsetNameList[i].name);
			return (K2Charset) K2CharsetNameList[i].value;
		}
	}

	// compare with ignoring '_', '-'
	code = removeUnserscreDash(code);
	for(i = 0; i < count; i++)
	{
		name.assign(K2CharsetNameList[i].name);
		name = removeUnserscreDash(name);

		if(code.compare(name, false) == 0)
		{
			// KTRACE("found charset2 : %s\n", K2CharsetNameList[i].name);
			return (K2Charset) K2CharsetNameList[i].value;
		}
	}

	if(code.length() > 2)
		code = code.substr(0, 2);
	if(code.length() == 2)
	{
		count = sizeof(K2LangNameList) / sizeof(IntNamePair);

		for(i = 0; i < count; i++)
		{
			if(code.compare(K2LangNameList[i].name, false) == 0)
			{
				// KTRACE("found charset3 : %s\n", K2LangNameList[i].name);
				return (K2Charset) K2LangNameList[i].value;
			}
		}
	}

	KTRACE("cannot found charset : %s\n", lang);
	return KCHARSET_UNDEFINED;
}

const char* getLanguageString( K2Charset langCode )
{
	int i, count;

	count = sizeof(K2CharsetNameList) / sizeof(IntNamePair);
	for(i = 0; i < count; i++)
	{
		if(langCode == K2CharsetNameList[i].value)
		{
			// KTRACE("found charset1 : %s\n", K2CharsetNameList[i].name);
			return K2CharsetNameList[i].name;
		}
	}

	count = sizeof(K2LangNameList) / sizeof(IntNamePair);
	for(i = 0; i < count; i++)
	{
		if(langCode == K2LangNameList[i].value)
		{
			// KTRACE("found charset3 : %s\n", K2LangNameList[i].name);
			return K2LangNameList[i].name;
		}
	}

	return NULL;
}
