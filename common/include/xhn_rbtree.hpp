#ifndef XHN_RBTREE_H
#define XHN_RBTREE_H
#include "common.h"
#include "etypes.h"
#include "xhn_utility.hpp"
#include "xhn_pair.hpp"
namespace xhn
{
template <typename K>
struct FSpecProc
{
	bool operator () (rbtree_node<K>* node) const { return false; }
};
template<typename K, typename V, typename Lessproc, typename NodeAllocproc, typename NodeSpecproc = FSpecProc<K> >
class rbtree
{
public:
    rbtree_node<K> *root;
    rbtree_node<K> *head;
    rbtree_node<K> *tail;
    euint count;

    ///NodeCreateproc nodeCreateProc;
	///NodeDeleteproc nodeDeleteProc;
    ///NodeFreeproc nodeFreeProc;
	NodeAllocproc nodeAllocProc;
	Lessproc lessProc;
	NodeSpecproc specProc;

    euint key_real_size;
    euint value_real_size;

    rbtree()
        : root ( NULL )
        , head ( NULL )
        , tail ( NULL )
        , count ( 0 )
        , key_real_size ( 0 )
        , value_real_size ( 0 ) {
    }
    ~rbtree() {
        clear();
    }
    void clear() {
        rbtree_node<K> *node = head;

        while ( node ) {
            rbtree_node<K> *tmp = NULL;
            tmp = node->iter_next;
			nodeAllocProc.destroy(node);
            nodeAllocProc.deallocate ( node, 0 );
            node = tmp;
        }

        root = NULL;
        head = NULL;
        tail = NULL;
        count = 0;
    }

    static inline rbtree_node<K> *_rotate_left ( rbtree_node<K> *node, rbtree_node<K> *root ) {
        rbtree_node<K> *right = node->right_node;

        if ( ( node->right_node = right->left_node ) ) {
            right->left_node->parent_node = node;
        }

        right->left_node = node;

        if ( ( right->parent_node = node->parent_node ) ) {
            if ( node == node->parent_node->right_node ) {
                node->parent_node->right_node = right;
            } else {
                node->parent_node->left_node = right;
            }
        } else {
            root = right;
        }

        node->parent_node = right;
        return root;
    }

    static inline rbtree_node<K> *_rotate_right ( rbtree_node<K> *node, rbtree_node<K> *root ) {
        rbtree_node<K> *left = node->left_node;

        if ( ( node->left_node = left->right_node ) ) {
            left->right_node->parent_node = node;
        }

        left->right_node = node;

        if ( ( left->parent_node = node->parent_node ) ) {
            if ( node == node->parent_node->right_node ) {
                node->parent_node->right_node = left;
            } else {
                node->parent_node->left_node = left;
            }
        } else {
            root = left;
        }

        node->parent_node = left;
        return root;
    }

    static inline rbtree_node<K> *_insert_rebalance ( rbtree_node<K> *node, rbtree_node<K> *root ) {
        rbtree_node<K> *parent, *gparent, *uncle, *tmp;

        while ( ( parent = node->parent_node ) && parent->color == Red ) {
            gparent = parent->parent_node;

            if ( parent == gparent->left_node ) {
                uncle = gparent->right_node;

                if ( uncle && uncle->color == Red ) {
                    uncle->color = Black;
                    parent->color = Black;
                    gparent->color = Red;
                    node = gparent;
                } else {
                    if ( parent->right_node == node ) {
                        root = _rotate_left ( parent, root );
                        tmp = parent;
                        parent = node;
                        node = tmp;
                    }

                    parent->color = Black;
                    gparent->color = Red;
                    root = _rotate_right ( gparent, root );
                }
            } else {
                uncle = gparent->left_node;

                if ( uncle && uncle->color == Red ) {
                    uncle->color = Black;
                    parent->color = Black;
                    gparent->color = Red;
                    node = gparent;
                } else {
                    if ( parent->left_node == node ) {
                        root = _rotate_right ( parent, root );
                        tmp = parent;
                        parent = node;
                        node = tmp;
                    }

                    parent->color = Black;
                    gparent->color = Red;
                    root = _rotate_left ( gparent, root );
                }
            }
        }

        root->color = Black;
        return root;
    }

    static inline rbtree_node<K> *_remove_rebalance ( rbtree_node<K> *node, rbtree_node<K> *parent, rbtree_node<K> *root ) {
        rbtree_node<K> *other, *o_left, *o_right;

        while ( ( !node || node->color == Black ) && node != root ) {
            if ( parent->left_node == node ) {
                other = parent->right_node;

                if ( other->color == Red ) {
                    other->color = Black;
                    parent->color = Red;
                    root = _rotate_left ( parent, root );
                    other = parent->right_node;
                }

                if ( ( !other->left_node || other->left_node->color == Black ) &&
                     ( !other->right_node || other->right_node->color == Black ) ) {
                    other->color = Red;
                    node = parent;
                    parent = node->parent_node;
                } else {
                    if ( !other->right_node || other->right_node->color == Black ) {
                        if ( ( o_left = other->left_node ) ) {
                            o_left->color = Black;
                        }

                        other->color = Red;
                        root = _rotate_right ( other, root );
                        other = parent->right_node;
                    }

                    other->color = parent->color;
                    parent->color = Black;

                    if ( other->right_node ) {
                        other->right_node->color = Black;
                    }

                    root = _rotate_left ( parent, root );
                    node = root;
                    break;
                }
            } else {
                other = parent->left_node;

                if ( other->color == Red ) {
                    other->color = Black;
                    parent->color = Red;
                    root = _rotate_right ( parent, root );
                    other = parent->left_node;
                }

                if ( ( !other->left_node || other->left_node->color == Black ) &&
                     ( !other->right_node || other->right_node->color == Black ) ) {
                    other->color = Red;
                    node = parent;
                    parent = node->parent_node;
                } else {
                    if ( !other->left_node || other->left_node->color == Black ) {
                        if ( ( o_right = other->right_node ) ) {
                            o_right->color = Black;
                        }

                        other->color = Red;
                        root = _rotate_left ( other, root );
                        other = parent->left_node;
                    }

                    other->color = parent->color;
                    parent->color = Black;

                    if ( other->left_node ) {
                        other->left_node->color = Black;
                    }

                    root = _rotate_right ( parent, root );
                    node = root;
                    break;
                }
            }
        }

        if ( node ) {
            node->color = Black;
        }

        return root;
    }

    static inline rbtree_node<K> *_search_auxiliary ( const rbtree *_tree, const K &key, rbtree_node<K> *root, rbtree_node<K> **save ) {
        rbtree_node<K> *node = root, *parent = NULL;

        while ( node ) {
			if (_tree->specProc(node))
				return node;

            parent = node;

            if ( _tree->lessProc(key, node->first) ) {
                node = node->left_node;
                continue;
            }

            if ( _tree->lessProc(node->first, key) ) {
                node = node->right_node;
                continue;
            }

            return node;
        }

        if ( save ) {
            *save = parent;
        }

        return NULL;
    }

    static inline euint _remove ( rbtree *_tree, const K &key ) {
        rbtree_node<K> *node, *root = _tree->root;

        if ( unlikely ( !_tree->root ) ) {
            return 0;
        }

        if ( ! ( node = _search_auxiliary ( _tree, key, root, NULL ) ) ) {
            return 0;
        }

        return _erase(_tree, node);
    }

    static inline euint _erase ( rbtree *_tree, rbtree_node<K> *_node ) {
        rbtree_node<K> *child, *parent, *old, *left, *node, *root = _tree->root;
        node_color color;

        if ( unlikely ( !_tree->root ) ) {
            return 0;
        }

        old = node = _node;

        if ( node->left_node && node->right_node ) {
            node = node->right_node;

            while ( ( left = node->left_node ) != NULL ) {
                node = left;
            }

            child = node->right_node;
            parent = node->parent_node;
            color = node->color;

            if ( child ) {
                child->parent_node = parent;
            }

            if ( parent ) {
                if ( parent->left_node == node ) {
                    parent->left_node = child;
                } else {
                    parent->right_node = child;
                }
            } else {
                root = child;
            }

            if ( node->parent_node == old ) {
                parent = node;
            }

            node->parent_node = old->parent_node;
            node->color = old->color;
            node->right_node = old->right_node;
            node->left_node = old->left_node;

            if ( old->parent_node ) {
                if ( old->parent_node->left_node == old ) {
                    old->parent_node->left_node = node;
                } else {
                    old->parent_node->right_node = node;
                }
            } else {
                root = node;
            }

            old->left_node->parent_node = node;

            if ( old->right_node ) {
                old->right_node->parent_node = node;
            }
        } else {
            if ( !node->left_node ) {
                child = node->right_node;
            } else if ( !node->right_node ) {
                child = node->left_node;
            } else {
                child = NULL;
            }

            parent = node->parent_node;
            color = node->color;

            if ( child ) {
                child->parent_node = parent;
            }

            if ( parent ) {
                if ( parent->left_node == node ) {
                    parent->left_node = child;
                } else {
                    parent->right_node = child;
                }
            } else {
                root = child;
            }
        }

        if ( old != _tree->head ) {
            if ( old != _tree->tail ) {
                old->iter_prev->iter_next = old->iter_next;
                old->iter_next->iter_prev = old->iter_prev;
            } else {
                old->iter_prev->iter_next = NULL;
                _tree->tail = old->iter_prev;
            }
        } else {
            if ( old != _tree->tail ) {
                old->iter_next->iter_prev = NULL;
                _tree->head = old->iter_next;
            } else {
                _tree->root = _tree->head = _tree->tail = NULL;
                _tree->count = 0;
				///_tree->nodeDeleteProc(old);
                ///_tree->nodeFreeProc ( old );
				_tree->nodeAllocProc.destroy(old);
				_tree->nodeAllocProc.deallocate(old, 0);
                return 1;
            }
        }

        _tree->count--;

        ///_tree->nodeDeleteProc(old);
        ///_tree->nodeFreeProc ( old );
		_tree->nodeAllocProc.destroy(old);
		_tree->nodeAllocProc.deallocate(old, 0);

        if ( color == Black ) {
            root = _remove_rebalance ( child, parent, root );
        }

        _tree->root = root;
        return 1;
    }

    static inline void tree_node_Init ( rbtree *tree, rbtree_node<K> *node, K &key ) {
        node->left_node = NULL;
        node->right_node = NULL;
        node->parent_node = NULL;
        node->iter_prev = NULL;
        node->iter_next = NULL;
        node->first = key;
        node->color = Black;
    }

    static inline rbtree_node<K> *_find ( rbtree *_tree, K &key, bool is_force ) {
        rbtree_node<K> *parent, *node, *root = NULL;

        if ( unlikely ( !_tree->root ) ) {
            if ( is_force ) {
                ///_tree->root = _tree->head = _tree->tail = _tree->nodeCreateProc();
				rbtree_node<K> *n = _tree->root = _tree->head = _tree->tail = _tree->nodeAllocProc.allocate(1);
				_tree->nodeAllocProc.construct(n);
                tree_node_Init ( _tree, _tree->root, key );
                _tree->count = 1;
                return _tree->root;
            } else {
                return NULL;
            }
        }

        node = root = _tree->root;

        if ( ( node = _search_auxiliary ( _tree, key, root, &parent ) ) ) {
            return node;
        } else {
            if ( !is_force ) {
                return NULL;
            }
        }

        node = _tree->nodeAllocProc.allocate(1);
		_tree->nodeAllocProc.construct(node);
        tree_node_Init ( _tree, node, key );
        node->parent_node = parent;
        node->left_node = node->right_node = NULL;
        node->color = Red;

        if ( likely ( parent ) ) {
            if ( _tree->lessProc(key, parent->first) ) {
                parent->left_node = node;

                if ( parent != _tree->head ) {
                    parent->iter_prev->iter_next = node;
                    node->iter_next = parent;
                    node->iter_prev = parent->iter_prev;
                    parent->iter_prev = node;
                } else {
                    parent->iter_prev = node;
                    node->iter_next = parent;
                    _tree->head = node;
                }
            } else {
                parent->right_node = node;

                if ( parent != _tree->tail ) {
                    parent->iter_next->iter_prev = node;
                    node->iter_prev = parent;
                    node->iter_next = parent->iter_next;
                    parent->iter_next = node;
                } else {
                    parent->iter_next = node;
                    node->iter_prev = parent;
                    _tree->tail = node;
                }
            }
        } else {
            root = node;
        }

        _tree->count++;
        _tree->root = _insert_rebalance ( node, root );
        return node;
    }
    static inline const rbtree_node<K> *_find ( const rbtree *_tree, const K &key ) {
        rbtree_node<K> *parent, *node, *root = NULL;

        if ( unlikely ( !_tree->root ) ) {
            return NULL;
        }

        node = root = _tree->root;

        if ( ( node = _search_auxiliary ( _tree, key, root, &parent ) ) ) {
            return node;
        } else {
            return NULL;
        }
    }
};
}
#endif
