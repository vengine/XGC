#ifndef XHN_LIST_H
#define XHN_LIST_H
#include "common.h"
#include "etypes.h"
#include "xhn_iterator.hpp"
#include "emem.hpp"
#ifndef __APPLE__
#include <pthread.h>
#else
#define _SCHED_H
#include </usr/include/sched.h>
#include </usr/include/pthread.h>
#endif
namespace xhn
{
template<typename T>
class list : public RefObject
{
public:
    class list_node : public MemObject
    {
    public:
        list_node *m_iter_prev;
        list_node *m_iter_next;
        ALIGN_FLAG ( 16 ) T m_value;
        list_node ( const T &data )
            : m_iter_prev ( NULL )
            , m_iter_next ( NULL )
            , m_value ( data )
        {}
    };
    list_node *m_head;
    list_node *m_tail;
    euint m_count;
    list() {
        m_head = NULL;
        m_tail = NULL;
        m_count = 0;
    }
	list (const list& lt)
	{
		m_head = NULL;
		m_tail = NULL;
		m_count = 0;
		const_iterator iter = lt.begin();
		const_iterator end = lt.end();
		for (; iter != end; iter++)
		{
            push_back(*iter);
		}
	}
    ~list() {
        clear();
    }
    void clear() {
		list_node *node = m_head;

		while ( node ) {
			list_node *tmp = node;
			node = node->m_iter_next;
			delete tmp;
		}

		m_head = NULL;
		m_tail = NULL;
		m_count = 0;
    }

	list* clone() {
		list* ret = ENEW list(*this);
		return ret;
	}

    bool empty() const {
        return m_count == 0;
    }

    template<typename N>
    struct FReadProc {
        void operator() ( N *from, N &to ) {
            to = *from;
        }
    };
    template<typename N>
    struct FWriteProc {
        void operator() ( N *to, N &from ) {
            *to = from;
        }
    };
    template<typename N>
    struct FNextProc {
        void operator() ( N *from, N *&to, euint ele_real_size ) {
            to = from->m_iter_next;
        }
    };
    template<typename N>
    struct FPrevProc {
        void operator() ( N *from, N *&to, euint ele_real_size ) {
            to = from->m_iter_prev;
        }
    };

    class iterator : public bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FNextProc<list_node>, FPrevProc<list_node> >
    {
    public:
        typedef bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FNextProc<list_node>, FPrevProc<list_node> > base_type;
        iterator ( list_node *a, euint ele_real_size )
            : base_type (
                a, ele_real_size, FReadProc<list_node>(), FWriteProc<list_node>(), FNextProc<list_node>(), FPrevProc<list_node>() )
        {}
		iterator (  )
			: base_type (
			NULL, 0, FReadProc<list_node>(), FWriteProc<list_node>(), FNextProc<list_node>(), FPrevProc<list_node>() )
		{}
        inline iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline iterator operator++ ( int ) {
            iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline iterator operator-- ( int ) {
            iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline T &operator*() {
            return base_type::m_ptr->m_value;
        }
        inline T *operator->() {
            return &base_type::m_ptr->m_value;
        }
    };
    class const_iterator : public const_bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FNextProc<list_node>, FPrevProc<list_node> >
    {
    public:
        typedef const_bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FNextProc<list_node>, FPrevProc<list_node> > base_type;
        const_iterator ( list_node *a, euint ele_real_size )
            : base_type (
                a, ele_real_size, FReadProc<list_node>(), FWriteProc<list_node>(), FNextProc<list_node>(), FPrevProc<list_node>() )
        {}
		const_iterator (  )
			: base_type (
			NULL, 0, FReadProc<list_node>(), FWriteProc<list_node>(), FNextProc<list_node>(), FPrevProc<list_node>() )
		{}
        inline const_iterator &operator++() {
            base_type::next();
            return *this;
        }
        inline const_iterator operator++ ( int ) {
            const_iterator tmp = *this;
            ++*this;
            return tmp;
        }
        inline const_iterator &operator--() {
            base_type::prev();
            return *this;
        }
        inline const_iterator operator-- ( int ) {
            const_iterator tmp = *this;
            --*this;
            return tmp;
        }
        inline const T &operator*() {
            return base_type::m_ptr->m_value;
        }
        inline const T *operator->() {
            return &base_type::m_ptr->m_value;
        }
    };
	class reverse_iterator : public bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FPrevProc<list_node>, FNextProc<list_node> >
	{
	public:
		typedef bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FPrevProc<list_node>, FNextProc<list_node> > base_type;
		reverse_iterator ( list_node *a, euint ele_real_size )
			: base_type (
			a, ele_real_size, FReadProc<list_node>(), FWriteProc<list_node>(), FPrevProc<list_node>(), FNextProc<list_node>() )
		{}
		reverse_iterator (  )
			: base_type (
			NULL, 0, FReadProc<list_node>(), FWriteProc<list_node>(), FPrevProc<list_node>(), FNextProc<list_node>() )
		{}
		inline reverse_iterator &operator++() {
			base_type::next();
			return *this;
		}
		inline reverse_iterator operator++ ( int ) {
			reverse_iterator tmp = *this;
			++*this;
			return tmp;
		}
		inline reverse_iterator &operator--() {
			base_type::prev();
			return *this;
		}
		inline reverse_iterator operator-- ( int ) {
			reverse_iterator tmp = *this;
			--*this;
			return tmp;
		}
		inline T &operator*() {
			return base_type::m_ptr->m_value;
		}
		inline T *operator->() {
			return &base_type::m_ptr->m_value;
		}
	};
	class const_reverse_iterator : public bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FPrevProc<list_node>, FNextProc<list_node> >
	{
	public:
		typedef bidirectional_readwrite_iterator<list_node, FReadProc<list_node>, FWriteProc<list_node>, FPrevProc<list_node>, FNextProc<list_node> > base_type;
		const_reverse_iterator ( list_node *a, euint ele_real_size )
			: base_type (
			a, ele_real_size, FReadProc<list_node>(), FWriteProc<list_node>(), FPrevProc<list_node>(), FNextProc<list_node>() )
		{}
		const_reverse_iterator (  )
			: base_type (
			NULL, 0, FReadProc<list_node>(), FWriteProc<list_node>(), FPrevProc<list_node>(), FNextProc<list_node>() )
		{}
		inline const_reverse_iterator &operator++() {
			base_type::next();
			return *this;
		}
		inline const_reverse_iterator operator++ ( int ) {
			const_reverse_iterator tmp = *this;
			++*this;
			return tmp;
		}
		inline const_reverse_iterator &operator--() {
			base_type::prev();
			return *this;
		}
		inline const_reverse_iterator operator-- ( int ) {
			const_reverse_iterator tmp = *this;
			--*this;
			return tmp;
		}
		inline const T &operator*() {
			return base_type::m_ptr->m_value;
		}
		inline const T *operator->() {
			return &base_type::m_ptr->m_value;
		}
	};
public:
    iterator push_back ( const T &data ) {
        list_node *node = ENEW list_node ( data );
       
        if ( m_tail ) {
            m_tail->m_iter_next = node;
            node->m_iter_prev = m_tail;
            node->m_iter_next = NULL;
            m_tail = node;
        } else {
            m_head = m_tail = node;
            m_tail->m_iter_prev = NULL;
            m_tail->m_iter_next = NULL;
        }

        m_count++;
        iterator ret ( m_tail, 0 );
        return ret;
    }

    iterator push_front ( const T &data ) {
        list_node *node = ENEW list_node ( data );

        if ( m_head ) {
            m_head->m_iter_prev = node;
            node->m_iter_next = m_head;
            node->m_iter_prev = NULL;
            m_head = node;
        } else {
            m_tail = m_head = node;
            m_head->m_iter_prev = NULL;
            m_head->m_iter_next = NULL;
        }

        m_count++;
        iterator ret ( m_head, 0 );
        return ret;
    }
    iterator insert_after ( iterator i, const T &data ) {
        list_node *node = i.m_ptr;
        list_node *n = ENEW list_node ( data );

        if ( node->m_iter_next ) {
            n->m_iter_next = node->m_iter_next;
            node->m_iter_next->m_iter_prev = n;
            n->m_iter_prev = node;
            node->m_iter_next = n;
            m_count++;
            return iterator ( n, 0 );
        } else {
            node->m_iter_next = n;
            m_tail = node->m_iter_next;
            m_tail->m_iter_prev = node;
            m_tail->m_iter_next = NULL;
            m_count++;
            iterator ret ( m_tail, 0 );
            return ret;
        }
    }

    iterator insert_before ( iterator i, const T &data ) {
        list_node *node = i.m_ptr;
        list_node *n = ENEW list_node ( data );

        if ( node->m_iter_prev ) {
            n->m_iter_prev = node->m_iter_prev;
            node->m_iter_prev->m_iter_next = n;
            n->m_iter_next = node;
            node->m_iter_prev = n;
            m_count++;
            return iterator ( n, 0 );
        } else {
            node->m_iter_prev = n;
            m_head = node->m_iter_prev;
            m_head->m_iter_prev = NULL;
            m_head->m_iter_next = node;
            m_count++;
            iterator ret ( m_head, 0 );
            return ret;
        }
    }

    iterator remove ( iterator i ) {
        if ( !m_count ) {
            return iterator ( NULL, 0 );
        } else {
            list_node *node = i.m_ptr;
            m_count--;

            if ( node != m_head && node != m_tail ) {
                list_node *ret = node->m_iter_next;
                node->m_iter_prev->m_iter_next = node->m_iter_next;
                node->m_iter_next->m_iter_prev = node->m_iter_prev;
                node->~list_node();
                Mfree ( node );
                return iterator ( ret, NULL );
            } else if ( node != m_head && node == m_tail ) {
                node->m_iter_prev->m_iter_next = NULL;
                m_tail = node->m_iter_prev;
                node->~list_node();
                Mfree ( node );
                return iterator ( NULL, 0 );
            } else if ( node == m_head && node != m_tail ) {
                list_node *ret = node->m_iter_next;
                node->m_iter_next->m_iter_prev = NULL;
                m_head = node->m_iter_next;
                node->~list_node();
                Mfree ( node );
                return iterator ( ret, 0 );
            } else {
                list_node *tmp = m_head;
                m_head = m_tail = NULL;
                tmp->~list_node();
                Mfree ( tmp );
                return iterator ( NULL, 0 );
            }
        }
    }
    iterator begin() {
        return iterator ( m_head, 0 );
    }
    iterator end() {
        return iterator ( NULL, 0 );
    }
    const_iterator begin() const {
        return const_iterator ( m_head, 0 );
    }
    const_iterator end() const {
        return const_iterator ( NULL, 0 );
    }
	reverse_iterator rbegin() {
		return reverse_iterator ( m_tail, 0 );
	}
	reverse_iterator rend() {
		return reverse_iterator ( NULL, 0 );
	}
	const_reverse_iterator rbegin() const {
		return const_reverse_iterator ( m_tail, 0 );
	}
	const_reverse_iterator rend() const {
		return const_reverse_iterator ( NULL, 0 );
	}
    euint size() const {
        return m_count;
    }
    list<T>& operator =(const list<T>& rts) {
        clear();
        list<T>::const_iterator iter = rts.begin();
        list<T>::const_iterator iter_end = rts.end();
        for (; iter != iter_end; iter++) {
            push_back(*iter);
        }
        return *this;
    }
    T& back() {
        return m_tail->m_value;
    }
    T& front() {
        return m_head->m_value;
    }
    void pop_front() {
        if (m_head) {
            list_node* tmp = m_head;
            m_head = m_head->m_iter_next;
            if (!m_head) {
                m_tail = NULL;
            }
            tmp->~list_node();
            Mfree ( tmp );
            m_count--;
        }
    }
    void pop_back() {
        if (m_tail) {
            list_node* tmp = m_tail;
            m_tail = m_head->m_iter_prev;
            if (!m_tail) {
                m_head = NULL;
            }
            tmp->~list_node();
            Mfree ( tmp );
            m_count--;
        }
    }
	void throw_front(iterator i) {
		if (!m_count)
			return;
		{
			list_node* node = i.m_ptr;

			if (node != m_head && node != m_tail)
			{
				node->m_iter_prev->m_iter_next = node->m_iter_next;
				node->m_iter_next->m_iter_prev = node->m_iter_prev;
			}
			else if (node != m_head && node == m_tail)
			{
				node->m_iter_prev->m_iter_next = NULL;
				m_tail = node->m_iter_prev;
			}
			else
			{
				return;
			}

			m_head->m_iter_prev = node;
			node->m_iter_next = m_head;
			node->m_iter_prev = NULL;
			m_head = node;
		}
	}
	void throw_back(iterator i) {
		if (!m_count)
			return;
		{
			list_node* node = i.m_ptr;

			if (node != m_head && node != m_tail)
			{
				node->m_iter_prev->m_iter_next = node->m_iter_next;
				node->m_iter_next->m_iter_prev = node->m_iter_prev;
			}
			else if (node == m_head && node != m_tail)
			{
				node->m_iter_next->m_iter_prev = NULL;
				m_head = node->m_iter_next;
			}
			else
			{
				return;
			}

			m_tail->m_iter_next = node;
			node->m_iter_prev = m_tail;
			node->m_iter_next = NULL;
			m_tail = node;
		}
	}
};

}
#endif
