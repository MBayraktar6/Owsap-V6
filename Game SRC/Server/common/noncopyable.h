#ifndef __INC_COMMON_NON_COPYABLE_TEMPLATE_H__
#define __INC_COMMON_NON_COPYABLE_TEMPLATE_H__

class noncopyable
{
protected:
	noncopyable() {}
	~noncopyable() {}

private:
	noncopyable(const noncopyable&);
	noncopyable& operator = (const noncopyable&);
};

#endif // __INC_COMMON_NON_COPYABLE_TEMPLATE_H__
