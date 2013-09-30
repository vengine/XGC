#ifndef XHN_STATIC_STRING_H
#define XHN_STATIC_STRING_H
#include "common.h"
#include "etypes.h"
#include "xhn_hash_set.hpp"
#include "xhn_string.hpp"
#include "xhn_utility.hpp"
namespace xhn
{
class static_string
{
private:
    static hash_set<string> s_static_string_set;
private:
    const char *m_str;
public:
	static void dest();
    static_string ( const char *str ) {
		euint32 hash_value = calc_hashnr ( str, _strlen ( str ) );
		xhn::hash_set<string>::bucket& b = s_static_string_set.get_bucket(hash_value);
		{
			SpinLock::Instance inst = b.m_lock.Lock();
			xhn::list<xhn::string>::iterator iter = b.begin();
			xhn::list<xhn::string>::iterator end = b.begin();
			for (; iter != end; iter++) {
				if (*iter == str) {
					m_str = (*iter).c_str();
					return;
				}
			}
		}
		string value ( str );
		const string &v = s_static_string_set.insert ( value );
		m_str = v.c_str();
    }
    static_string ( const static_string& str ) {
        m_str = str.m_str;
    }
    static_string () {
        string value ( "" );
        const string &v = s_static_string_set.insert ( value );
        m_str = v.c_str();
    }
    const char *c_str() const {
        return m_str;
    }
    bool operator < ( const static_string &str ) const {
        return m_str < str.m_str;
    }
    bool operator > ( const static_string &str ) const {
        return m_str > str.m_str;
    }
    bool operator == ( const static_string &str ) const {
        return m_str == str.m_str;
    }
    bool operator != ( const static_string &str ) const {
        return m_str != str.m_str;
    }
    euint size() const {
        return strlen(m_str);
    }
};
}
#endif
