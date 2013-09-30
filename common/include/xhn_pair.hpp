#ifndef XHN_PAIR_HPP
#define XHN_PAIR_HPP
#include "common.h"
#include "etypes.h"
#include "emem.h"
#include "emem.hpp"
#include <new>
namespace xhn
{
enum node_color {
    Red,
    Black,
};
template<typename F>
class rbtree_node
{
public:
    ALIGN_FLAG ( 16 ) F first;
    rbtree_node *left_node;
    rbtree_node *right_node;
    rbtree_node *parent_node;
    rbtree_node *iter_prev;
    rbtree_node *iter_next;
    node_color color;

    rbtree_node ( const F &f )
        : first ( f )
        , left_node(NULL)
        , right_node(NULL)
        , parent_node(NULL)
        , iter_prev(NULL)
        , iter_next(NULL)
        , color(Black)
    {}
    rbtree_node ( const rbtree_node &p )
        : first ( p.first )
        , left_node(p.left_node)
        , right_node(p.right_node)
        , parent_node(p.parent_node)
        , iter_prev(p.iter_prev)
        , iter_next(p.iter_next)
        , color(p.color)
    {}
    rbtree_node()
        : left_node(NULL)
        , right_node(NULL)
        , parent_node(NULL)
        , iter_prev(NULL)
        , iter_next(NULL)
        , color(Black)
    {}
    rbtree_node &operator = ( const rbtree_node &p ) {
        first = p.first;
        left_node = p.left_node;
        right_node = p.right_node;
        parent_node = p.parent_node;
        iter_prev = p.iter_prev;
        iter_next = p.iter_next;
        color = p.color;
        return *this;
    }
};
template<typename F>
inline rbtree_node<F> *to_rbtree_node ( F *f )
{
    return ( rbtree_node<F> * ) ( ( ( char * ) f ) - ( esint ) ( & ( ( rbtree_node<F> * ) 0 )->first ) );
}
template<typename F, typename S>
class pair : public rbtree_node<F>
{
public:
    ALIGN_FLAG ( 16 ) S second;
    pair ( const F &f, const S &s )
        : rbtree_node<F> ( f )
        , second ( s )
    {}
    pair ( const pair &p )
        : rbtree_node<F> ( p.first )
        , second ( p.second )
    {}
    pair()
    {}
    pair &operator = ( const pair &p ) {
        rbtree_node<F>::first = p.first;
        second = p.second;
        return *this;
    }
    bool operator < ( const pair &p ) const {
        if (rbtree_node<F>::first < p.first)
            return true;
        else if (rbtree_node<F>::first > p.first)
            return false;
        else {
            if (second < p.second)
                return true;
            else
                return false;
        }
    }
};
template<typename F, typename S>
inline pair<F, S> *to_pair ( F *f )
{
	return ( pair<F, S> * ) ( ( ( char * ) f ) - ( esint ) ( & ( ( pair<F, S> * ) 0 )->first ) );
}
template<typename F, typename S>
pair<F, S> make_pair ( const F &f, const S &s )
{
    return pair<F, S> ( f, s );
}
}
#endif
