#include "stdafx.h"

int R_LetterWidth(unsigned int letter, Font_s* font)
{
	// fuck inline
	// R_GetCharacterGlyph
	return ((Glyph* (__cdecl*)(Font_s*, uint32_t))0x006D2FA0)(font, letter)->dx;
}

int SEH_DecodeLetter(unsigned int firstChar, unsigned int secondChar, int* usedCount, int* pbIsTrailingPunctuation)
{
	return ((int(__cdecl*)(uint32_t, uint32_t, int*, int*))0x00533720)(firstChar, secondChar, usedCount, pbIsTrailingPunctuation);
}

int SEH_DecodeLetterU8(const char* utf8Stream, int* numBytesConsumed)
{
	int u8char = 0;

	*numBytesConsumed = 0;
	char ch = *utf8Stream;
	if (*utf8Stream >= 0)
	{
		*numBytesConsumed = 1;
		return ch;
	}
	if ((ch & 0xE0) == 0xC0u)
	{
		u8char = utf8Stream[1] & 0x7F | ((ch & 0x1F) << 6);
		if (u8char >= 128)
		{
			*numBytesConsumed = 2;
			return u8char;
		}
	}
	else if ((ch & 0xF0) == 0xE0u)
	{
		u8char = utf8Stream[2] & 0x7F | ((utf8Stream[1] & 0x7F | ((ch & 0xF) << 6)) << 6);
		if (u8char - 2048 <= 53247 || u8char >= 57344)
		{
			*numBytesConsumed = 3;
			return u8char;
		}
	}
	else if ((ch & 0xF8) == 0xF0u)
	{
		u8char = utf8Stream[3] & 0x7F | ((utf8Stream[2] & 0x7F | ((utf8Stream[1] & 0x7F | ((ch & 7) << 6)) << 6)) << 6);
		if (u8char - 65536 <= 1048574)
		{
			*numBytesConsumed = 4;
			return u8char;
		}
	}

	// fallback to original SEH_DecodeLetter
	return SEH_DecodeLetter(utf8Stream[0], utf8Stream[1], numBytesConsumed, nullptr);
}

uint32_t SEH_ReadCharFromStringU8(const char** text, int* isTrailingPunctuation)
{
	int usedCount = 0;
	int letter = SEH_DecodeLetterU8(*text, &usedCount);

	*text += usedCount;
	return letter;
}

int __cdecl PrintableCharsCountU8(MessageWindow* msgwnd, MessageLine* line)
{
	int printedCnt = 0;
	int idx = 0;
	while (idx < line->textBufSize)
	{
		int pos = msgwnd->textBufPos + idx;
		int len = line->textBufSize - 1;
		int usedCharCnt = 0;

		auto letter = SEH_DecodeLetterU8(msgwnd->circularTextBuffer + (len & pos), &usedCharCnt);

		idx += usedCharCnt;
		++printedCnt;

		if (letter == '^')
		{
			char c = msgwnd->circularTextBuffer[len & pos];

			if (c != '^' && c >= '0' && c <= '@' || c && c != '^' && c == 'F')
			{
				++idx;
			}
		}
	}

	return printedCnt;
}

int __cdecl R_ConsoleTextWidthU8(const char* textPool, int poolSize, int firstChar, int charCount, Font_s* font)
{
	int indexMask = poolSize - 1;
	int stopPos = (poolSize - 1) & (charCount + firstChar);
	int parsePos = firstChar;
	int width = 0;

	while (parsePos != stopPos)
	{
		int usedCharCount = 0;
		int letter = SEH_DecodeLetterU8(textPool + parsePos, &usedCharCount);
		parsePos = indexMask & (usedCharCount + parsePos);

		// color code or whatever
		if (letter == '^' && textPool[parsePos] != '^' && 
			(textPool[parsePos] >= '0' && textPool[parsePos] <= '@' || 
			textPool[parsePos] != '^' && textPool[parsePos] == 'F'))
		{
			parsePos = indexMask & (parsePos + 1);
		}
		// icons
		else if (letter == '^' && (textPool[parsePos] == 1 || textPool[parsePos] == 2))
		{
			width += (font->pixelHeight * (textPool[indexMask & (parsePos + 1)] - 16) + 16) / 32;
			parsePos = indexMask & (parsePos + 7);
		}
		// normal text
		else
		{
			width += R_LetterWidth(letter, font);
		}
	}

	return width;
}

void __cdecl DB_BuildOSPath(const char* zoneName, const char* ext, int size, char* filename)
{
	((void(__cdecl*)(const char*, const char*, int, char*))0x4644A0)(zoneName, ext, size, filename);

	MessageBoxA(NULL, filename, "INFO", MB_OK);
}

int __cdecl SEH_StrLengthU8(const char* str)
{
	int printedCnt = 0;
	size_t idx = 0;
	while (idx < strlen(str))
	{
		int usedCharCnt = 0;
		auto letter = SEH_DecodeLetterU8(str + idx, &usedCharCnt);

		idx += usedCharCnt;

		if (letter == '^')
		{
			char c = str[idx];

			if (c != '^' && c >= '0' && c <= '@' || c && c != '^' && c == 'F')
			{
				++idx;
			}
		}
		else if (letter == '\n' || letter == '\r')
		{
			continue;
		}
		else
		{
			++printedCnt;
		}
	}

	return printedCnt;
}

languageInfo_t* g_languages = (languageInfo_t*)0xB7AEAC;

void UseUTF8()
{
	// PatchCall(0x7A37B0, (PBYTE)&DB_BuildOSPath);

	// hack
	PatchMemory(0x54EF40, (PBYTE)"\xE9", 1);
	PatchMemory(0x79BBA0, (PBYTE)"\xE9", 1);
	PatchMemory(0x6D3500, (PBYTE)"\xE9", 1);
	PatchMemory(0x50FA90, (PBYTE)"\xE9", 1);

	PatchJump(0x54EF40, (PBYTE)&SEH_ReadCharFromStringU8);
	PatchJump(0x79BBA0, (PBYTE)&PrintableCharsCountU8);
	PatchJump(0x6D3500, (PBYTE)&R_ConsoleTextWidthU8);
	PatchJump(0x50FA90, (PBYTE)&SEH_StrLengthU8);

	// dont replace fulljap with japanese
	// as we replaced it with schinese
	// PatchMemory(0x833B5F, (PBYTE)"\xEB", 1);
	// dont validate localized iwd
	PatchMemory(0x85F1D5, (PBYTE)"\xE9\xEC\x01\x00\x00\x90", 6);
	g_languages[11] = { 0, "schinese", "sc_" };
}
