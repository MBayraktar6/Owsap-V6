#define DEF_STR(x) #x

/*----- atoi function -----*/

inline bool str_to_number(bool& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (strtol(in, NULL, 10) != 0);
	return true;
}

inline bool str_to_number(char& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (char)strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number(unsigned char& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (unsigned char)strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number(short& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (short)strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number(unsigned short& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (unsigned short)strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number(int& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (int)strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number(unsigned int& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (unsigned int)strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number(long& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (long)strtol(in, NULL, 10);
	return true;
}

inline bool str_to_number(unsigned long& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (unsigned long)strtoul(in, NULL, 10);
	return true;
}

inline bool str_to_number(long long& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (long long)strtoull(in, NULL, 10);
	return true;
}

inline bool str_to_number(unsigned long long& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (unsigned long long) strtoull(in, NULL, 10);
	return true;
}

inline bool str_to_number(float& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (float)strtof(in, NULL);
	return true;
}

inline bool str_to_number(double& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (double)strtod(in, NULL);
	return true;
}

inline bool safe_str_to_number(int& out, const char* in)
{
	if (0 == in || 0 == in[0])
		return false;

	char* end;
	int val = strtol(in, &end, 10);

	if (end == in || *end != '\0')
	{
		if (val == INT_MAX)
			out = INT_MAX;
		else if (val == INT_MIN)
			out = INT_MIN;
		else
			return false;
	}
	else
		out = val;

	return true;
}

inline bool safe_str_to_number(long& out, const char* in)
{
	if (0 == in || 0 == in[0])
		return false;

	char* end;
	long val = strtol(in, &end, 10);

	if (end == in || *end != '\0')
	{
		if (val == LONG_MAX)
			out = LONG_MAX;
		else if (val == LONG_MIN)
			out = LONG_MIN;
		else
			return false;
	}
	else
		out = val;

	return true;
}

inline bool safe_str_to_number(long long& out, const char* in)
{
	if (0 == in || 0 == in[0])
		return false;

	char* end;
	long long val = strtoll(in, &end, 10);

	if (end == in || *end != '\0')
	{
		if (val == LLONG_MAX)
			out = LLONG_MAX;
		else if (val == LLONG_MIN)
			out = LLONG_MIN;
		else
			return false;
	}
	else
		out = val;

	return true;
}

#ifdef __FreeBSD__
inline bool str_to_number(long double& out, const char* in)
{
	if (0 == in || 0 == in[0])	return false;

	out = (long double)strtold(in, NULL);
	return true;
}
#endif

/*----- atoi function -----*/
