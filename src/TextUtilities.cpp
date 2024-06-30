//
//////////////////////////////////////////////////////////////////
//
//  Windows Mbox Viewer is a free tool to view, search and print mbox mail archives.
//
// Source code and executable can be downloaded from
//  https://sourceforge.net/projects/mbox-viewer/  and
//  https://github.com/eneam/mboxviewer
//
//  Copyright(C) 2019  Enea Mansutti, Zbigniew Minciel
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the version 3 of GNU Affero General Public License
//  as published by the Free Software Foundation; 
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU
//  Library General Public License for more details.
//
//  You should have received a copy of the GNU Library General Public
//  License along with this program; if not, write to the
//  Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
//  Boston, MA  02110 - 1301, USA.
//
//////////////////////////////////////////////////////////////////
//

#include "stdafx.h"

#ifdef _MSC_VER
#pragma warning (disable : 4786)
#endif

#include "ctype.h"
#include <algorithm>
#include <map>
#include "TextUtilities.h"
#include "TextUtilsEx.h"
#include "HtmlUtils.h"

TextUtilities	g_tu;

#ifdef _MSC_VER
#ifdef _DEBUG
#undef THIS_FILE
#define THIS_FILE __FILE__
#define new DEBUG_NEW
#endif
#endif

CString TextUtilities::m_strW;
SimpleString TextUtilities::m_strA;
SimpleString TextUtilities::m_workBuff;

static UINT32 _crc32Table[256] = 
{
	0x00000000, 0xBC5C9CBF, 0x9F32D42D, 0x236E4892, 0xD9EE4509, 0x65B2D9B6,
	0x46DC9124, 0xFA800D9B, 0x54576741, 0xE80BFBFE, 0xCB65B36C, 0x77392FD3,
	0x8DB92248, 0x31E5BEF7, 0x128BF665, 0xAED76ADA, 0xA8AECE82, 0x14F2523D,
	0x379C1AAF, 0x8BC08610, 0x71408B8B, 0xCD1C1734, 0xEE725FA6, 0x522EC319,
	0xFCF9A9C3, 0x40A5357C, 0x63CB7DEE, 0xDF97E151, 0x2517ECCA, 0x994B7075,
	0xBA2538E7, 0x0679A458, 0xB6D67057, 0x0A8AECE8, 0x29E4A47A, 0x95B838C5,
	0x6F38355E, 0xD364A9E1, 0xF00AE173, 0x4C567DCC, 0xE2811716, 0x5EDD8BA9,
	0x7DB3C33B, 0xC1EF5F84, 0x3B6F521F, 0x8733CEA0, 0xA45D8632, 0x18011A8D,
	0x1E78BED5, 0xA224226A, 0x814A6AF8, 0x3D16F647, 0xC796FBDC, 0x7BCA6763,
	0x58A42FF1, 0xE4F8B34E, 0x4A2FD994, 0xF673452B, 0xD51D0DB9, 0x69419106,
	0x93C19C9D, 0x2F9D0022, 0x0CF348B0, 0xB0AFD40F, 0x8A270DFD, 0x367B9142,
	0x1515D9D0, 0xA949456F, 0x53C948F4, 0xEF95D44B, 0xCCFB9CD9, 0x70A70066,
	0xDE706ABC, 0x622CF603, 0x4142BE91, 0xFD1E222E, 0x079E2FB5, 0xBBC2B30A,
	0x98ACFB98, 0x24F06727, 0x2289C37F, 0x9ED55FC0, 0xBDBB1752, 0x01E78BED,
	0xFB678676, 0x473B1AC9, 0x6455525B, 0xD809CEE4, 0x76DEA43E, 0xCA823881,
	0xE9EC7013, 0x55B0ECAC, 0xAF30E137, 0x136C7D88, 0x3002351A, 0x8C5EA9A5,
	0x3CF17DAA, 0x80ADE115, 0xA3C3A987, 0x1F9F3538, 0xE51F38A3, 0x5943A41C,
	0x7A2DEC8E, 0xC6717031, 0x68A61AEB, 0xD4FA8654, 0xF794CEC6, 0x4BC85279,
	0xB1485FE2, 0x0D14C35D, 0x2E7A8BCF, 0x92261770, 0x945FB328, 0x28032F97,
	0x0B6D6705, 0xB731FBBA, 0x4DB1F621, 0xF1ED6A9E, 0xD283220C, 0x6EDFBEB3,
	0xC008D469, 0x7C5448D6, 0x5F3A0044, 0xE3669CFB, 0x19E69160, 0xA5BA0DDF,
	0x86D4454D, 0x3A88D9F2, 0xF3C5F6A9, 0x4F996A16, 0x6CF72284, 0xD0ABBE3B,
	0x2A2BB3A0, 0x96772F1F, 0xB519678D, 0x0945FB32, 0xA79291E8, 0x1BCE0D57,
	0x38A045C5, 0x84FCD97A, 0x7E7CD4E1, 0xC220485E, 0xE14E00CC, 0x5D129C73,
	0x5B6B382B, 0xE737A494, 0xC459EC06, 0x780570B9, 0x82857D22, 0x3ED9E19D,
	0x1DB7A90F, 0xA1EB35B0, 0x0F3C5F6A, 0xB360C3D5, 0x900E8B47, 0x2C5217F8,
	0xD6D21A63, 0x6A8E86DC, 0x49E0CE4E, 0xF5BC52F1, 0x451386FE, 0xF94F1A41,
	0xDA2152D3, 0x667DCE6C, 0x9CFDC3F7, 0x20A15F48, 0x03CF17DA, 0xBF938B65,
	0x1144E1BF, 0xAD187D00, 0x8E763592, 0x322AA92D, 0xC8AAA4B6, 0x74F63809,
	0x5798709B, 0xEBC4EC24, 0xEDBD487C, 0x51E1D4C3, 0x728F9C51, 0xCED300EE,
	0x34530D75, 0x880F91CA, 0xAB61D958, 0x173D45E7, 0xB9EA2F3D, 0x05B6B382,
	0x26D8FB10, 0x9A8467AF, 0x60046A34, 0xDC58F68B, 0xFF36BE19, 0x436A22A6,
	0x79E2FB54, 0xC5BE67EB, 0xE6D02F79, 0x5A8CB3C6, 0xA00CBE5D, 0x1C5022E2,
	0x3F3E6A70, 0x8362F6CF, 0x2DB59C15, 0x91E900AA, 0xB2874838, 0x0EDBD487,
	0xF45BD91C, 0x480745A3, 0x6B690D31, 0xD735918E, 0xD14C35D6, 0x6D10A969,
	0x4E7EE1FB, 0xF2227D44, 0x08A270DF, 0xB4FEEC60, 0x9790A4F2, 0x2BCC384D,
	0x851B5297, 0x3947CE28, 0x1A2986BA, 0xA6751A05, 0x5CF5179E, 0xE0A98B21,
	0xC3C7C3B3, 0x7F9B5F0C, 0xCF348B03, 0x736817BC, 0x50065F2E, 0xEC5AC391,
	0x16DACE0A, 0xAA8652B5, 0x89E81A27, 0x35B48698, 0x9B63EC42, 0x273F70FD,
	0x0451386F, 0xB80DA4D0, 0x428DA94B, 0xFED135F4, 0xDDBF7D66, 0x61E3E1D9,
	0x679A4581, 0xDBC6D93E, 0xF8A891AC, 0x44F40D13, 0xBE740088, 0x02289C37,
	0x2146D4A5, 0x9D1A481A, 0x33CD22C0, 0x8F91BE7F, 0xACFFF6ED, 0x10A36A52,
	0xEA2367C9, 0x567FFB76, 0x7511B3E4, 0xC94D2F5B
};

UINT32 TextUtilities::CalcCRC32(const char* buf, const UINT length)
{
	register UINT32 crc32 = 0;
	register UINT32* crc32Table = _crc32Table;

	register UINT32 len = length;
	while (len-- > 0)
		crc32 = crc32Table[(crc32 ^ *buf++) & 0xFF] ^ (crc32 >> 8);

	return crc32;
}

int TextUtilities::StrSearch(unsigned char* srcText, int n, UINT textCP, unsigned char* pat, int m, BOOL bCaseSens, unsigned char* skipTable)   /* Search Search pat[0..m-1] in text[0..n-1] */
{
	DWORD error;
	int ret = -1;
	unsigned char* text = srcText;
	BOOL retCP2W = FALSE;

	UINT CP_ASCII = 20127;
	if (textCP == CP_ASCII)
		textCP = 0;  // CodePage2WStr will fail if textCP == CP_ASCII but 8bit charcaters are in text

	BOOL checkIsNormalizedString = FALSE;
	if ((bCaseSens == FALSE) || ((bCaseSens == TRUE) && (textCP != CP_UTF8)) ||
		(checkIsNormalizedString && ((bCaseSens == TRUE) && (textCP == CP_UTF8))))
	{
		int lenW = m_strW.GetAllocLength();
		m_strW.LockBuffer();
		m_strW.Empty();
		//void Preallocate(int nLength);
		lenW = m_strW.GetAllocLength();

		retCP2W = TextUtilsEx::CodePage2WStr((char*)text, n, textCP, &m_strW, error);
		if (retCP2W && checkIsNormalizedString)
		{
#if (NTDDI_VERSION >= NTDDI_VISTA)
			NORM_FORM form = NormalizationC;
			if (IsNormalizedString(form, (LPCWSTR)m_strW, m_strW.GetLength()))
			{
				//TRACE(L"Already normalized in form C\n");
				int deb = 1;
			}
			else
			{
				TRACE(L"Text not normalized in form C !!!\n");
				int deb = 1;
			}
#else
			int deb = 1;
#endif
		}
	}

	// Pattern string and Text/haystack must be converted to UTF8, both lower or upper case
	// Converting strings to lower case is the best effort because it is done as language independent
	// Converting strings to lower case works quite well but matching may result in false negative matches in few cases
	if (bCaseSens == FALSE)
	{
		if (retCP2W)
		{
			CString &loweStr = m_strW.MakeLower();
			BOOL retW2A = TextUtilsEx::WStr2UTF8((LPCWSTR)m_strW, m_strW.GetLength(), &m_strA, error);
			if (retW2A)
			{
				text = (unsigned char*)m_strA.Data();
				n = m_strA.Count();
			}
		}
		bCaseSens = TRUE; // FIXMEFIXME
	}
	else if (textCP != CP_UTF8)
	{
		if (retCP2W)
		{
			BOOL retW2A = TextUtilsEx::WStr2UTF8((LPCWSTR)m_strW, m_strW.GetLength(), &m_strA, error); 
			if (retW2A)
			{
				text = (unsigned char*)m_strA.Data();
				n = m_strA.Count();
			}
		}
	}
	else  // textCP == CP_UTF8
	{
		int deb = 1;
	}

	if (n < 1024)  // 1024 selected without any investigation to help to select the optimal length
	{
		char* found = 0;
		if (bCaseSens)
			found = strstr((char* const)text, (char* const)pat);
		else
			_ASSERTE(FALSE);
			// found = StrStrIA((char* const)text, (char* const)pat);  // doesn't work anyway
		if (found)
			return (int)(found - (char* const)text);
		else
			return -1;
	}
	else
	{
		ret = TextUtilities::BMHSearch(text, n, pat, m, bCaseSens, skipTable);
	}
	return ret;
}

int TextUtilities::StrSearchW(unsigned char* srcText, int n, UINT textCP, unsigned char* pat, int m, BOOL bCaseSens, unsigned char* skipTable)   /* Search Search pat[0..m-1] in text[0..n-1] */
{
	DWORD error;
	int ret = -1;
	unsigned char* text = srcText;
	BOOL retCP2W = FALSE;

	UINT CP_ASCII = 20127;
	if (textCP == CP_ASCII)
		textCP = 0;  // CodePage2WStr will fail if textCP == CP_ASCII but 8bit charcaters are in text

	BOOL checkIsNormalizedString = TRUE;
	if ((bCaseSens == FALSE) || ((bCaseSens == TRUE) && (textCP != CP_UTF8)) ||
		(checkIsNormalizedString && ((bCaseSens == TRUE) && (textCP == CP_UTF8))))
	{
		int lenW = m_strW.GetAllocLength();
		m_strW.LockBuffer();
		m_strW.Empty();
		//void Preallocate(int nLength);
		lenW = m_strW.GetAllocLength();

		retCP2W = TextUtilsEx::CodePage2WStr((char*)text, n, textCP, &m_strW, error);
		if (retCP2W && checkIsNormalizedString)
		{
#if (NTDDI_VERSION >= NTDDI_VISTA)
			NORM_FORM form = NormalizationC;
			if (IsNormalizedString(form, (LPCWSTR)m_strW, m_strW.GetLength()))
			{
				//TRACE(L"Already normalized in form C\n");
				int deb = 1;
			}
			else
			{
				TRACE(L"Text not normalized in form C  !!!!\n");
				int deb = 1;
			}
#else
			int deb = 1;
#endif
		}
	}

	// Pattern string and Text/haystack must be converted to UTF8, both lower or upper case
	// Converting strings to lower case is the best effort because it is done as language independent
	// Converting strings to lower case works quite well but matching may result in false negative matches in few cases
	if (bCaseSens == FALSE)
	{
		if (retCP2W)
		{
			CString& loweStr = m_strW.MakeLower();
			BOOL retW2A = TextUtilsEx::WStr2UTF8((LPCWSTR)m_strW, m_strW.GetLength(), &m_strA, error);
			if (retW2A)
			{
				text = (unsigned char*)m_strA.Data();
				n = m_strA.Count();
			}
		}
		bCaseSens = TRUE; // FIXMEFIXME
	}
	else if (textCP != CP_UTF8)
	{
		if (retCP2W)
		{
			BOOL retW2A = TextUtilsEx::WStr2UTF8((LPCWSTR)m_strW, m_strW.GetLength(), &m_strA, error);
			if (retW2A)
			{
				text = (unsigned char*)m_strA.Data();
				n = m_strA.Count();
			}
		}
	}
	else  // textCP == CP_UTF8
	{
		int deb = 1;
	}

	if (n < m)
		return -1;

	if (n < 1024)  // 1024 selected without any investigation to decide on the optimal length
	{
		int strl = n;
		char* textBegin = (char*)text;
		char* textEnd = (char*)text + strl;
		char *wordLastPlus;

		while (strl > 0)
		{
			char* found = strstr((char* const)text, (char* const)pat);
			if (found == 0)
			{
				return -1;
			}

			if ((found != (char*)textBegin) && !IsWordSeparator(*(found - 1)))
			{
				strl = (int)(textEnd - found);
				if (strl > m)
				{
					text = (unsigned char*)++found;
					//text = (unsigned char*)found + m; // // not valid since  search pattern may contain separators
					_ASSERTE(text <= (unsigned char*)textEnd);
					continue;
				}
				else
					return -1;
			}
			wordLastPlus = found + m;
			_ASSERTE(wordLastPlus <= textEnd);
			if ((wordLastPlus != textEnd) && !IsWordSeparator(*wordLastPlus))
			{
				strl = (int)(textEnd - found);
				if (strl > m)
				{
					text = (unsigned char*)++found;
					//text = (unsigned char*)found + m; // not valid since search pattern may contain separators
					_ASSERTE(text <= (unsigned char*)textEnd);
					continue;
				}
				else
					return -1;
			}
			else
			{
				int pos = (int)(found - textBegin);
				return pos;
			}
		}
		return -1;
	}
	else
	{
		ret = TextUtilities::BMHSearchW(text, n, pat, m, bCaseSens, skipTable);
	}
	return ret;
}

int TextUtilities::BMHSearchW(unsigned char* text, int n, unsigned char* pat, int m, BOOL bCaseSens, unsigned char* skipTable)   /* Search Search pat[0..m-1] in text[0..n-1] */
{
	unsigned char dd[256];  // skip table
	unsigned char* d = &dd[0];
	int i, j, k, lim;
	int res = -1;

	if ((n < m) || (m <= 0))
	{
		_ASSERTE(FALSE);
		return -1;
	}

	if (skipTable == 0)
	{
			// Populate skip table. Precompute and pass as argument . Any Locality concerns ??
		memset(dd, m + 1, 256);  // why not memset(d, m, 256)  optymization, see comment below // FIXMEFIXME
		for (k = 0; k < m; k++)
			dd[pat[k]] = m - k;

		d = &dd[0];
	}
	else
		d = skipTable;

    pat[m] = 1;          /* To avoid having code     */ /* for special case n-k+1=m */ /* null terminator overwrite, restore before exit */
	// TODO original lim = n - m + 1; // // out of range and crash under debugger when searching RAW message
	// TODO corrected  lim = n - m; doesn't crash but matching fails in some cases, restored original lim = n-m+1; 
	/* for special case n-k+1=m */
	lim = n - m + 1; // // out of range and may crash under debugger

	if ( bCaseSens )
	{
		for ( k=0; k < lim; k += d[(unsigned char)(text[k+m])] ) /* Searching */
		{
			i=k;
			for ( j=0; (unsigned char)(text[i]) == pat[j]; j++ ) {
				i++; /* Could be optimal order */
			}
			if ( j == m )
			{
				if ( k != 0 ) {
					if (!IsWordSeparator(text[k-1]) )
						continue;
				}
				if ( k+m < n ) {
					if (!IsWordSeparator(text[k+m]) )
						continue;
				}
				res = k;
				break;
			}
		}
	}
	else
	{
		_ASSERTE(FALSE);   // Delete below, no longer used // FIXME
		for( k=0; k < lim; k += d[(unsigned char)tolower(text[k+m])] ) /* Searching */
		{
			i=k;
			for ( j=0; (unsigned char)tolower(text[i]) == pat[j]; j++ ) {
				i++; /* Could be optimal order */
			}
			if ( j == m )
			{
				if ( k != 0 ) {
					if (!IsWordSeparator(text[k-1]) )
						continue;
				}
				if( k+m < n ) {
					if (!IsWordSeparator(text[k+m]) )
						continue;
				}
				res = k;
				break;
			}
		}
	}
    pat[m] = 0;  // restore null terminator
    return res;
}

// There are many variations of heuristic implementation of the Boyer Moore algorithm, and simplest one is Horspool variation.
// Like Boyer-Moore algorithm, worst-case scenario time complexity is O(m * n) while average complexity is O(n). 
// Space usage doesn't depend on the size of the pattern, but only on the size of the alphabet which is 256 
// since that is the maximum value of ASCII character in English alphabet
// int BoyerMooreHorspoolSearch(char[] pattern, char[] text)
//
// n is text length, m is pattern length
//
int TextUtilities::BMHSearch( unsigned char *text, int n, unsigned char *pat, int m, BOOL bCaseSens, unsigned char *skipTable)   /* Search Search pat[0..m-1] in text[0..n-1] */
{
    unsigned char dd[256];  // skip table
	register unsigned char* d = &dd[0];
    int i, j, k, lim;
    int res = -1;

	if (n < m)
	{
		_ASSERTE(FALSE);
		return -1;
	}

	if (skipTable == 0)
	{

		// Populate skip table. Precompute and pass as argument . Any Locality concern ??
		memset(d, m + 1, 256);  // why not memset(d, m, 256);  optymization, see comment below // FIXMEFIXME  
		for (k = 0; k < m; k++)
			d[pat[k]] = m - k;
	}
	else
		d = skipTable;

    pat[m] = 1;          /* To avoid having code     */  /* for special case n-k+1=m */
	// TODO original lim = n - m + 1; // // out of range and crash under debugger when searching RAW message
	// TODO corrected  lim = n - m; doesn't crash but matching fails in some cases, restored original lim = n-m+1; 
    /* for special case n-k+1=m */
    lim = n - m + 1; // // out of range and may crash under debugger

 	if( bCaseSens )
	{
		for ( k=0; k < lim; k += d[(unsigned char)(text[k+m])] ) /* Searching */
		{
			i=k;
			for ( j=0; (unsigned char)(text[i]) == pat[j]; j++ ) {
				i++; /* Could be optimal order */
			}
			if ( j == m ) {
				res = k; 
				break;
			}
		}
	}
	else
	{
		_ASSERTE(FALSE);  // Delete below, no longer used // FIXME
		for ( k=0; k < lim; k += d[(unsigned char)tolower(text[k+m])] ) /* Searching */
		{
			i=k;
			for (j = 0; (unsigned char)tolower(text[i]) == pat[j]; j++) {
				i++; /* Could be optimal order */
			}
			if ( j == m ) {
				res = k; 
				break;
			}
		}
	}
    pat[m] = 0; // restore null terminator
    return res;
}

static const char WordSeparatorChars[] = "0123456789abcdefghijklmnopqrstuvwxyz_ABCDEFGHIJKLMNOPQRSTUVWXYZ";
static bool WordSeparatorCharsTable[256];

static const char LowerCaseChars[] = "abcdefghijklmnopqrstuvwxyz�������������������������������";
static bool LowerCaseCharsTable[256];
static const char UpperCaseChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ�������������������������������";
static bool UpperCaseCharsTable[256];
static char UpperToLowerCharsTable[256];

//
//
// All characters are from the ISO 8859-1 character set mapped to 7-bit ASCII.
//
static char const iso8859_map[] = {
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  0
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  8
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  16
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	//  24
	' ', '!', '"', '#', '$', '%', '&', '\'',//  32
	'(', ')', '*', '+', ',', '-', '.', '/',	//  40
	'0', '1', '2', '3', '4', '5', '6', '7',	//  48
	'8', '9', ':', ';', '<', '=', '>', '?',	//  56
	'@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',	//  64
	'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',	//  72
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',	//  80
	'X', 'Y', 'Z', '[', '\\', ']', '^', '_',//  88
	'`', 'a', 'b', 'c', 'd', 'e', 'f', 'g',	//  96
	'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',	// 104
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w',	// 112
	'x', 'y', 'z', '{', '|', '}', '~', ' ',	// 120
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	// 128
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	// 136
	' ', '\'','\'', '"', '"', ' ', ' ', ' ',	// 144
	' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',	// 152
	' ', '!', ' ', '#', ' ', ' ', '|', ' ',	// 160
	' ', ' ', ' ', '<', ' ', '-', ' ', ' ',	// 168
	' ', ' ', '2', '3','\'', ' ', ' ', '.', // 176
	' ', '1', ' ', '>', ' ', ' ', ' ', '?',	// 184
	'�', '�', '�', '�', '�', '�', '�', '�',	// 192
	'�', '�', '�', '�', '�', '�', '�', '�',	// 200
	'�', '�', '�', '�', '�', '�', '�', ' ',	// 208
	'�', '�', '�', '�', '�', '�', '�', '�',	// 216
	'�', '�', '�', '�', '�', '�', '�', '�',	// 224
	'�', '�', '�', '�', '�', '�', '�', '�',	// 232
	'�', '�', '�', '�', '�', '�', '�', ' ',	// 240
	'�', '�', '�', '�', '�', '�', '�', '�',	// 248
};

void TextUtilities::MakeWordSeparatorLookupTable(const char* s, bool* l)
{
	memset(l+128, false, 128 * sizeof(*l));
	memset(l, true, 128 * sizeof(*l));
	for (; *s; s++)
		l[(int)((unsigned char)*s)] = false;
}

void TextUtilities::MakeLookupTable(const char* s, bool* l)
{
	memset(l, false, 256 * sizeof(*l));
	for (; *s; s++)
		l[(int)((unsigned char)*s)] = true;
}

void TextUtilities::MakeConversionTable(const char* from, const char* to, char* dest)
{
	ASSERT(strlen(from) == strlen(to));
	for (int i = 0; i < 256; i++)
		dest[i] = i;
	for (; *from; from++, to++)
		dest[(int)((unsigned char)*from)] = *to;
}

static std::vector<CString>* CreateArray(const char *arr[])
{
	std::vector<CString>* cm = new std::vector<CString>;
	const char **mp = arr;
	while (**mp) {
		cm->push_back(*mp);
		mp++;
	}
	return cm;
}

bool TextUtilities::Init()
{
	MakeWordSeparatorLookupTable(WordSeparatorChars, WordSeparatorCharsTable);
	MakeLookupTable(LowerCaseChars, LowerCaseCharsTable);
	MakeConversionTable(UpperCaseChars, LowerCaseChars, UpperToLowerCharsTable);

	return true;
}

TextUtilities::~TextUtilities() {

}

#define HASH_LOOKUP(c, tbl) (tbl[(int) (0x000000ff & c)])

bool TextUtilities::IsWordSeparator(const char c) {
	return HASH_LOOKUP(c, WordSeparatorCharsTable);
}

char TextUtilities::ToLower(char c) {
	return HASH_LOOKUP(c, UpperToLowerCharsTable);
}

#define N_CHARS		16			/* number of bytes printed in one line */
#define	MAX_BUFF	4094		/* size of print buffer for sprintf() and max TRACE can accept */

void TextUtilities::hexdump(char *title, char *area, int length)
{
	char	buff[MAX_BUFF + 1];
	char	tmp[128];
	int	cnt, n, len;
	int	m_length = length;
	int	jj, ii;
	char	ch;

	cnt = 0;

	if (strlen(title) > 0)
	{
		sprintf(tmp, "%s (length=%d)", title, length);
		sprintf(&buff[cnt], "%-22s = \n", tmp);
		n = istrlen(&buff[cnt]);
		if (n > 0) cnt += n;
	}

	for (jj = 0; jj < m_length; jj += N_CHARS) /* print block of 16 bytes */
	{
		len = m_length - jj;			/* bytes left */
		if (len > N_CHARS)
			len = N_CHARS;

		sprintf(&buff[cnt], "%04d  ", (jj % 10000));
		n = istrlen(&buff[cnt]);
		if (n > 0) cnt += n;

		for (ii = 0; ii < len; ++ii)			/* N_CHARS bytes hex */
		{
			if (ii == N_CHARS / 2)
			{
				sprintf(&buff[cnt], "| ");
				n = istrlen(&buff[cnt]);
				if (n > 0) cnt += n;
			}
			sprintf(&buff[cnt], "%02x ", (unsigned char)area[jj + ii]);
			n = istrlen(&buff[cnt]);
			if (n > 0) cnt += n;
		}

		for (ii = len; ii < N_CHARS; ii++)		/* padd with spaces */
		{
			if (ii == N_CHARS / 2)
			{
				sprintf(&buff[cnt], "  ");
				n = istrlen(&buff[cnt]);
				if (n > 0) cnt += n;
			}
			sprintf(&buff[cnt], "   ");
			n = istrlen(&buff[cnt]);
			if (n > 0) cnt += n;
		}

		sprintf(&buff[cnt], "      ");		/* 6 spaces */
		n = istrlen(&buff[cnt]);
		if (n > 0) cnt += n;

		for (ii = 0; ii < len; ii++)			/* N_CHARS bytes ascii */
		{
			ch = area[jj + ii];
			if (ch < ' ' || ch > '~')
				ch = '_';

			sprintf(&buff[cnt], "%c", ch);
			n = istrlen(&buff[cnt]);
			if (n > 0) cnt += n;
		}

		sprintf(&buff[cnt], "\n");
		n = istrlen(&buff[cnt]);
		if (n > 0) cnt += n;

		/* print accumulated characters */
		if (cnt > (MAX_BUFF - (N_CHARS*20)))
		{
			buff[cnt] = '\0';
			TRACE(L"%s", buff);
			cnt = 0;
			sprintf(&buff[cnt], "\n");
			n = istrlen(&buff[cnt]);
			if (n > 0) cnt += n;
		}
	}
	/*
		(void *) sprintf(&buff[cnt],"\n");
		n = strlen(&buff[cnt]);
		if ( n > 0 ) cnt += n;
	*/

	if (cnt >= 0)
	{
		buff[cnt] = '\0';
		TRACE(L"%s", buff);
	}
}

bool TextUtilities::TestAll()
{
	char *s1 = "mbox viewer bigniewZxyz";
	s1 = "mboxview";
	int s1Len = istrlen(s1);

	char *pat1 = "ewer";
	char *pat2 = "zbigniewZ";
	pat1 = s1;
	pat2 = "z";

	int pat1Len = istrlen(pat1);
	int pat2Len = istrlen(pat2);

	char *s11 = new char[s1Len];
	memcpy(s11, s1, s1Len);

	BOOL caseSensitive = TRUE;

	char *pat11 = new char[pat1Len + 1];
	strcpy(pat11, pat1);

	char *pat22 = new char[pat2Len + 1];
	strcpy(pat22, pat2);

	TRACE(L"pat1=%s pat2=%s\n", pat11, pat22);

	UINT inCodePage = CP_UTF8; // FIXMEFIXME

	int pos0 = g_tu.StrSearch((unsigned char*)s11, s1Len, inCodePage, (unsigned char*)pat11, pat1Len, caseSensitive, 0);

	int pos1 = g_tu.StrSearch((unsigned char*)s1, s1Len, inCodePage, (unsigned char*)pat11, pat1Len, caseSensitive, 0);

	int pos2 = g_tu.StrSearch((unsigned char*)s1, s1Len, inCodePage, (unsigned char*)pat22, pat2Len, caseSensitive, 0);

	caseSensitive = FALSE;
	int pos11 = g_tu.StrSearch((unsigned char*)s1, s1Len, inCodePage, (unsigned char*)pat11, pat1Len, caseSensitive, 0);


	//int StrSearchW(unsigned char *text, int n, unsigned char *pat, int m, BOOL bCaseSens = FALSE);

	delete[] pat11;
	delete[] pat22;

	int deb = 1;
	return true;
}

// class MyCArray
template<class T> void MyCArray<T>::SetSizeKeepData(INT_PTR nNewSize, INT_PTR nGrowBy)
{
	if (nNewSize == 0)
		m_nSize = 0;
	else
		SetSize(nNewSize, nGrowBy);
}

template<class T> void MyCArray<T>::CopyKeepData(const MyCArray<T>& src)
{
	if (src.GetSize() == 0)
		m_nSize = 0;
	else
		Copy(src);
}



#if 0

int test_enc()
{
	char *wordEncoded = "=?UTF-8?Q?St=c3=a9phane_Scudeller?= <sscudeller@gmail.com>";
	char *word = "St=c3=a9phane_Scudeller?= <sscudeller@gmail.com>";
	const unsigned char *in = (const unsigned char*)word;

	guint32 save = 0;
	int state = 0;
	unsigned char buff[10000];
	unsigned char *out = buff;
	size_t len = strlen((char*)in);


	size_t ret = quoted_decode(in, len, out, &state, &save);

	return 1;
}

#endif