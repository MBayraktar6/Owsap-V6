#ifndef __INC_LOCALE_H__
#define __INC_LOCALE_H__

extern "C"
{
	void locale_init(const char* filename);
	const char* locale_find(const char* string);

	extern int g_iUseLocale;

#define LC_TEXT(str) locale_find(str)
};

#if defined(__LOCALE_CLIENT__)
#include <sstream>

template<typename... Args>
std::string FormatLC(std::string token, uint32_t vnum, Args&& ... args)
{
	std::stringstream stream;
	stream << '[';
	stream << token << ";" << vnum;
	((stream << ';' << std::forward<Args>(args)), ...);
	stream << ']';
	return stream.str();
}

template<typename Arg>
decltype(auto) convert_arg(Arg&& arg)
{
	if constexpr (std::is_same<std::decay_t<decltype(arg)>, BYTE>::value)
		return static_cast<int>(arg);
	else
		return std::forward<decltype(arg)>(arg);
}

template<typename... Args>
std::string ReplaceLocaleString(std::string format, Args&& ... args)
{
	try
	{
		std::string str = locale_find(format.c_str());
		if (!str.empty() && std::all_of(str.begin(), str.end(), ::isdigit))
		{
			std::stringstream stream;
			stream << '[';
			stream << "LS" << ";" << str;
			((stream << ';' << convert_arg(std::forward<Args>(args))), ...);
			stream << ']';
			return stream.str();
		}
		else // if child is not a number, return a normal formated string
		{

			int buflen = snprintf(NULL, 0, str.c_str(), args...) + 1;
			if (buflen <= 0)
				throw std::runtime_error("Error during formatting.");

			std::size_t bufsize = static_cast<std::size_t>(buflen);
			std::unique_ptr<char[]> bufify(new char[bufsize]);
			std::snprintf(bufify.get(), bufsize, str.c_str(), convert_arg(args)...);

			return std::string(bufify.get(), bufify.get() + bufsize - 1);
		}
	}
	catch (const std::exception& ex)
	{
		printf("ReplaceLocaleString: Error: %s\n", ex.what());
		return format;
	}
}

#if !defined(_WIN32)
#	define LC_STRING(str, args...) ReplaceLocaleString(str, ##args).c_str()
#else 
#	define LC_STRING(str, ...) ReplaceLocaleString(str, __VA_ARGS__).c_str()
#endif // _WIN32

#define LC_ITEM(vnum) FormatLC("IN", vnum).c_str()
#if defined(__SET_ITEM__)
#	define LC_PRE_ITEM(set_value, vnum) FormatLC("PRE_IN", set_value, vnum).c_str()
#endif
#define LC_MOB(vnum) FormatLC("MN", vnum).c_str()
#define LC_SKILL(vnum) FormatLC("SN", vnum).c_str()
#define LC_PETSKILL(vnum) FormatLC("PSN", vnum).c_str()
#define LC_OX(vnum) FormatLC("LOX", vnum).c_str()

#else
#	if !defined(_WIN32)
#		define LC_STRING(str, args...) locale_find(str)
#	else 
#		define LC_STRING(str, ...) locale_find(str)
#	endif // _WIN32
#endif // __LOCALE_CLIENT__

#endif // __INC_LOCALE_H__
