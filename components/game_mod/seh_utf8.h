#pragma once

struct MessageLine
{
	int messageIndex;
	int textBufPos;
	int textBufSize;
	int typingStartTime;
	int lastTypingSoundTime;
	int flags;
};

struct Message
{
	int startTime;
	int endTime;
};

struct MessageWindow
{
	MessageLine* lines;
	Message* messages;
	char* circularTextBuffer;
	int textBufSize;
	int lineCount;
	int padding;
	int scrollTime;
	int fadeIn;
	int fadeOut;
	int textBufPos;
	int firstLineIndex;
	int activeLineCount;
	int messageIndex;
};

struct Font_s
{
	const char* fontName;
	int pixelHeight;
	int glyphCount;
	Material* material;
	Material* glowMaterial;
	void* glyphs;
};

struct languageInfo_t
{
	int bPresent;
	const char* pszName;
	const char* pszNameAbbr;
};

void UseUTF8();
