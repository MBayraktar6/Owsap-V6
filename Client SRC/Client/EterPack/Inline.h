#ifndef __INC_ETER_PACK_LIB_INLINE_H__
#define __INC_ETER_PACK_LIB_INLINE_H__

inline void inlinePathCreate(const char* path)
{
	char dir[64];
	const char* p, * k;

	p = path + 3;

	while (NULL != (k = strchr(p, '/')))
	{
		memset(dir, 0, sizeof(dir));
		strncpy(dir, path, k - path);
		CreateDirectory(dir, NULL);
		p = k + 1;
	}
}

inline void inlineConvertPackFilename(char* name)
{
	char* p = name;

	while (*p)
	{
		if (*p == '\\')
			*p = '/';
		else
			*p = (int)tolower(*p);
		p++;
	}
}

#endif // __INC_ETER_PACK_INLINE_H__
