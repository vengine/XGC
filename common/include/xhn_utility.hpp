#ifndef XHN_UTILITY_HPP
#define XHN_UTILITY_HPP
#include "common.h"
#include "etypes.h"
#include "hash.h"
#include "emem.h"
inline euint _strlen(const char* s)
{
    euint count = 0;
    while (s[count])
        count++;
    return count;
}
#define to_struct_addr(t, member, addr) \
	(t*)( ((char*)addr) - (esint)(&((t*)0)->member) )
namespace xhn
{
class buffer
{
private:
    vptr m_buf;
public:
    inline buffer(euint size) {
        m_buf = Malloc(size);
    }
    inline ~buffer() {
        Mfree(m_buf);
    }
    inline vptr get() {
        return m_buf;
    }
};
template<typename T>
euint _real_size()
{
    euint num_qwords = sizeof ( T ) / 16;
    euint mod = sizeof ( T ) % 16;

    if ( mod ) {
        num_qwords++;
    }

    return num_qwords * 16;
}

inline euint32 _hash ( char *&key )
{
    return calc_hashnr ( key, _strlen ( key ) );
}

inline euint32 _hash ( wchar_t *&key )
{
    int count = 0;
    while (key[count]) {
        count++;
    }
    return calc_hashnr ( (const char*)key, count * sizeof(wchar_t) );
}

template < typename InputIterator, typename OutputIterator, typename UnaryOperator >
OutputIterator transform ( InputIterator first1, InputIterator last1,
                           OutputIterator result, UnaryOperator op )
{
    while ( first1 != last1 ) {
        *result = op ( *first1 ); // or: *result=binary_op(*first1,*first2++);
        ++result;
        ++first1;
    }

    return result;
}

template < typename T >
struct FCtorProc {
    void operator () (T* ptr) {
        new ( ptr ) T ();
    }
    void operator () (T* dst, T& src) {
        new ( dst ) T ( src );
    }
};

template < typename T >
struct FDestProc {
    void operator () (T* ptr) {
        ptr->~T();
    }
};

template <typename T>
struct FGetElementRealSizeProc {
    euint operator() () {
        euint num_qwords = sizeof ( T ) / 16;
        euint mod = sizeof ( T ) % 16;

        if ( mod ) {
            num_qwords++;
        }

        return num_qwords * 16;
    }
};

template <typename CHAR>
struct FGetCharRealSizeProc {
    euint operator() () {
        return sizeof(CHAR);
    }
};

inline char to_lower(char c)
{
    if (c >= 'A' && c <= 'Z') {
        char d = c - 'A';
        return 'a' + d;
    } else
        return c;
}

template<class T>
struct FLessProc
{
	bool operator()(const T& a, const T& b) const {
		return a < b;
	}
};

template <typename T>
class allocator
{
public:
    typedef euint size_type;
	typedef euint difference_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T value_type;
	typedef T& reference;
	typedef const T& const_reference;
    
	template<typename A>
	struct rebind
	{
		typedef allocator<A> other;
	};

	pointer address(reference v) const
	{	
		return &v;
	}

	const_pointer address(const_reference v) const
	{	
		return &v;
	}

	allocator()
	{
	}

	allocator(const allocator& rth) 
	{
	}

	template<class A>
	allocator(const allocator<A>&)
	{
	}

	template<class A>
	allocator<T>& operator=(const allocator<A>&)
	{
		return (*this);
	}

	void deallocate(pointer ptr, size_type)
	{	
		Mfree(ptr);
	}

	pointer allocate(size_type count)
	{
		return (pointer)Malloc(count * sizeof(value_type));
	}

	pointer allocate(size_type count, const void*)
	{
        return (pointer)Malloc(count * sizeof(value_type));
	}

	void construct(pointer ptr, const T& v)
	{	
		new ( ptr ) T ();
	}

	void construct(pointer ptr)
	{	
		new ( ptr ) T ();
	}

	void destroy(pointer ptr)
	{	
		ptr->~T();
	}

	size_type max_size() const {
		return static_cast<size_type>(-1) / sizeof(value_type);
	}
};
}
#endif
