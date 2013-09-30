#include "pch.h"
#include "tree.h"
#include "container.h"
#include "estring.h"
#include "elog.h"
#include "eassert.h"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4706)
#endif

_INLINE_ void tree_node_Init(struct _tree* _tree, struct tree_node* _node, var _key, var _data)
{
    _node->ower = _tree;
    _node->left_node = NULL;
    _node->right_node = NULL;
    _node->parent_node = NULL;
    _node->iter_prev = NULL;
    _node->iter_next = NULL;
    if (_tree->key_type != String)
    {
        _node->key = _key;
    }
    else
    {
        var key;
        key.str_var = EString_new((EString)_key.str_var);
        _node->key = key;
    }
    if (_tree->value_type != String)
    {
        _node->value = _data;
    }
    else
    {
        var value;
        value.str_var = EString_new((EString)_data.str_var);
        _node->value = value;
    }

    _node->color = Black;
}

_INLINE_ void var_assign(Tree _tree, var* _dst, var _src)
{
    if (_tree->value_type != String)
    {
        *_dst = _src;
    }
    else
    {
        var value;
        value.str_var = EString_new((char*)_src.str_var);
        *_dst = value;
    }
}

Tree Tree_Init(tree* _tree, etype _key_type, etype _value_type, MALLOC _alc, MFREE _fre)
{
    _tree->alloc_proc = _alc;
    _tree->free_proc = _fre;
    _tree->key_compare_proc = NULL;
    _tree->key_type = _key_type;
    _tree->value_type = _value_type;
    _tree->root = NULL;
    _tree->head = NULL;
    _tree->tail = NULL;
    _tree->count = 0;
    return _tree;
}

Tree Tree_new(etype _key_type, etype _value_type, MALLOC _alc, MFREE _fre)
{
    Tree ret = (Tree)_alc(sizeof(tree));
    Tree_Init(ret, _key_type, _value_type, _alc, _fre);
    return ret;
}

void Tree_Dest(Tree _tree)
{
    MFREE free_proc = _tree->free_proc;
    struct tree_node* node = _tree->head;
    while (node)
    {
        struct tree_node* tmp = NULL;
        if (_tree->key_type == String)
        {
            char* key_str = (char*)node->key.str_var;
            EString_delete(key_str);
        }
        if (_tree->value_type == String)
        {
            char* value_str = (char*)node->value.str_var;
            EString_delete(value_str);
        }
        tmp = node->iter_next;
        _tree->free_proc(node);
        node = tmp;
    }
    free_proc(_tree);
}

struct tree_node* _rotate_left(struct tree_node* node, struct tree_node* root)
{
    struct tree_node* right = node->right_node;
    if ((node->right_node = right->left_node))
    {
        right->left_node->parent_node = node;
    }
    right->left_node = node;
    if ((right->parent_node = node->parent_node))
    {
        if (node == node->parent_node->right_node)
        {
            node->parent_node->right_node = right;
        }
        else
        {
            node->parent_node->left_node = right;
        }
    }
    else
    {
        root = right;
    }
    node->parent_node = right;
    return root;
}

struct tree_node* _rotate_right(struct tree_node* node, struct tree_node* root)
{
    struct tree_node* left = node->left_node;
    if ((node->left_node = left->right_node))
    {
        left->right_node->parent_node = node;
    }
    left->right_node = node;
    if ((left->parent_node = node->parent_node))
    {
        if (node == node->parent_node->right_node)
        {
            node->parent_node->right_node = left;
        }
        else
        {
            node->parent_node->left_node = left;
        }
    }
    else
    {
        root = left;
    }
    node->parent_node = left;
    return root;
}

struct tree_node* _insert_rebalance(struct tree_node* node, struct tree_node* root)
{
    struct tree_node *parent, *gparent, *uncle, *tmp;
    while ((parent = node->parent_node) && parent->color == Red)
    {
        gparent = parent->parent_node;
        if (parent == gparent->left_node)
        {
            uncle = gparent->right_node;
            if (uncle && uncle->color == Red)
            {
                uncle->color = Black;
                parent->color = Black;
                gparent->color = Red;
                node = gparent;
            }
            else
            {
                if (parent->right_node == node)
                {
                    root = _rotate_left(parent, root);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->color = Black;
                gparent->color = Red;
                root = _rotate_right(gparent, root);
            }
        }
        else
        {
            uncle = gparent->left_node;
            if (uncle && uncle->color == Red)
            {
                uncle->color = Black;
                parent->color = Black;
                gparent->color = Red;
                node = gparent;
            }
            else
            {
                if (parent->left_node == node)
                {
                    root = _rotate_right(parent, root);
                    tmp = parent;
                    parent = node;
                    node = tmp;
                }
                parent->color = Black;
                gparent->color = Red;
                root = _rotate_left(gparent, root);
            }
        }
    }
    root->color = Black;
    return root;
}

struct tree_node* _remove_rebalance(struct tree_node *node, struct tree_node *parent, struct tree_node *root)
{
    struct tree_node *other, *o_left, *o_right;
    while ((!node || node->color == Black) && node != root)
    {
        if (parent->left_node == node)
        {
            other = parent->right_node;
            if (other->color == Red)
            {
                other->color = Black;
                parent->color = Red;
                root = _rotate_left(parent, root);
                other = parent->right_node;
            }
            if ((!other->left_node || other->left_node->color == Black) &&
                    (!other->right_node || other->right_node->color == Black))
            {
                other->color = Red;
                node = parent;
                parent = node->parent_node;
            }
            else
            {
                if (!other->right_node || other->right_node->color == Black)
                {
                    if ((o_left = other->left_node))
                    {
                        o_left->color = Black;
                    }
                    other->color = Red;
                    root = _rotate_right(other, root);
                    other = parent->right_node;
                }
                other->color = parent->color;
                parent->color = Black;
                if (other->right_node)
                {
                    other->right_node->color = Black;
                }
                root = _rotate_left(parent, root);
                node = root;
                break;
            }
        }
        else
        {
            other = parent->left_node;
            if (other->color == Red)
            {
                other->color = Black;
                parent->color = Red;
                root = _rotate_right(parent, root);
                other = parent->left_node;
            }
            if ((!other->left_node || other->left_node->color == Black) &&
                    (!other->right_node || other->right_node->color == Black))
            {
                other->color = Red;
                node = parent;
                parent = node->parent_node;
            }
            else
            {
                if (!other->left_node || other->left_node->color == Black)
                {
                    if ((o_right = other->right_node))
                    {
                        o_right->color = Black;
                    }
                    other->color = Red;
                    root = _rotate_left(other, root);
                    other = parent->left_node;
                }
                other->color = parent->color;
                parent->color = Black;
                if (other->left_node)
                {
                    other->left_node->color = Black;
                }
                root = _rotate_right(parent, root);
                node = root;
                break;
            }
        }
    }
    if (node)
    {
        node->color = Black;
    }
    return root;
}

struct tree_node* _search_auxiliary(Tree _tree, var key, struct tree_node* root, struct tree_node** save)
{
    struct tree_node *node = root, *parent = NULL;
    while (node)
    {
        euint cmp = 0;
        parent = node;

        less_then(_tree, _tree->key_type, key, node->key, cmp);
        if (cmp)
        {
            node = node->left_node;
            continue;
        }
        greater_then(_tree, _tree->key_type, key, node->key, cmp);
        if (cmp)
        {
            node = node->right_node;
            continue;
        }
        return node;
    }
    if (save)
    {
        *save = parent;
    }
    return NULL;
}

void Tree_remove(Tree _tree, var _key)
{
    struct tree_node *child, *parent, *old, *left, *node, *root = _tree->root;
    node_color color;
    var key;
    if (unlikely(!_tree->root))
    {
        return;
    }

    if (_tree->key_type != String)
    {
        key = _key;
    }
    else
    {
        key.str_var = EString_new((char*)_key.str_var);
    }
    if (!(node = _search_auxiliary(_tree, key, root, NULL)))
    {
        if (_tree->key_type == String)
        {
            EString_delete((char*)key.str_var);
        }
        return;
    }
    if (_tree->key_type == String)
    {
        EString_delete((char*)key.str_var);
    }
    old = node;
    if (node->left_node && node->right_node)
    {
        node = node->right_node;
        while ((left = node->left_node) != NULL)
        {
            node = left;
        }
        child = node->right_node;
        parent = node->parent_node;
        color = node->color;

        if (child)
        {
            child->parent_node = parent;
        }
        if (parent)
        {
            if (parent->left_node == node)
            {
                parent->left_node = child;
            }
            else
            {
                parent->right_node = child;
            }
        }
        else
        {
            root = child;
        }
        if (node->parent_node == old)
        {
            parent = node;
        }
        node->parent_node = old->parent_node;
        node->color = old->color;
        node->right_node = old->right_node;
        node->left_node = old->left_node;
        if (old->parent_node)
        {
            if (old->parent_node->left_node == old)
            {
                old->parent_node->left_node = node;
            }
            else
            {
                old->parent_node->right_node = node;
            }
        }
        else
        {
            root = node;
        }
        old->left_node->parent_node = node;
        if (old->right_node)
        {
            old->right_node->parent_node = node;
        }
    }
    else
    {
        if (!node->left_node)
        {
            child = node->right_node;
        }
        else if (!node->right_node)
        {
            child = node->left_node;
        }
		else
			child = NULL;
        parent = node->parent_node;
        color = node->color;
        if (child)
        {
            child->parent_node = parent;
        }
        if (parent)
        {
            if (parent->left_node == node)
            {
                parent->left_node = child;
            }
            else
            {
                parent->right_node = child;
            }
        }
        else
        {
            root = child;
        }
    }
    if (old != _tree->head)
    {
        if (old != _tree->tail)
        {
            old->iter_prev->iter_next = old->iter_next;
            old->iter_next->iter_prev = old->iter_prev;
        }
        else
        {
            old->iter_prev->iter_next = NULL;
            _tree->tail = old->iter_prev;
        }
    }
    else
    {
        if (old != _tree->tail)
        {
            old->iter_next->iter_prev = NULL;
            _tree->head = old->iter_next;
        }
        else
        {
            _tree->root = _tree->head = _tree->tail = NULL;
            _tree->count = 0;
			/// 这里可能有bug。。。
			_tree->free_proc(old);
            return;
        }
    }
    _tree->count--;
	/// 这里可能有bug。。。
    _tree->free_proc(old);
    if (color == Black)
    {
        root = _remove_rebalance(child, parent, root);
    }
    _tree->root = root;
}

Iterator Tree_force_find(Tree _tree, var _key)
{
    var key, data;
    memset(&data, 0, sizeof(data));
    struct tree_node *parent, *node, *root = NULL;
    if (_tree->key_type != String)
    {
        key = _key;
    }
    else
    {
        key.str_var = EString_new((char*)_key.str_var);
    }
    if (unlikely(!_tree->root))
    {
        _tree->root = _tree->head = _tree->tail = (struct tree_node*)_tree->alloc_proc(sizeof(struct tree_node));
        tree_node_Init(_tree, _tree->root, key, data);
        _tree->count = 1;
        if (_tree->key_type == String)
        {
            EString_delete((char*)key.str_var);
        }
        return (Iterator)_tree->root;
    }
    node = root = _tree->root;
    if ((node = _search_auxiliary(_tree, key, root, &parent)))
    {
        if (_tree->key_type == String)
        {
            EString_delete((char*)key.str_var);
        }
        return (Iterator)node;
    }
    node = (struct tree_node*)_tree->alloc_proc(sizeof(struct tree_node));
    tree_node_Init(_tree, node, key, data);
    node->parent_node = parent;
    node->left_node = node->right_node = NULL;
    node->color = Red;

    if (likely(parent))
    {
        euint cmp = 0;
        less_then(_tree, _tree->key_type, key, parent->key, cmp)
        if (cmp)
        {
            parent->left_node = node;
            if (parent != _tree->head)
            {
                parent->iter_prev->iter_next = node;
                node->iter_next = parent;
                node->iter_prev = parent->iter_prev;
                parent->iter_prev = node;
            }
            else
            {
                parent->iter_prev = node;
                node->iter_next = parent;
                _tree->head = node;
            }
        }
        else
        {
            parent->right_node = node;
            if (parent != _tree->tail)
            {
                parent->iter_next->iter_prev = node;
                node->iter_prev = parent;
                node->iter_next = parent->iter_next;
                parent->iter_next = node;
            }
            else
            {
                parent->iter_next = node;
                node->iter_prev = parent;
                _tree->tail = node;
            }
        }
    }
    else
    {
        root = node;
    }
    _tree->count++;
    _tree->root = _insert_rebalance(node, root);
    if (_tree->key_type == String)
    {
        EString_delete((char*)key.str_var);
    }
    return (Iterator)node;
}

Iterator Tree_insert(Tree _tree, var _key, var _data)
{
    return Tree_replace(_tree, _key, _data);
}

Iterator Tree_replace(Tree _tree, var _key, var _data)
{
    Iterator iter = Tree_force_find(_tree, _key);
    struct tree_node* node = (struct tree_node*)iter;
    if (_tree->value_type == String && node->value.str_var)
    {
        EString_delete((char*)node->value.str_var);
    }
    var_assign(_tree, &node->value, _data);
    return iter;
}

Iterator Tree_find(Tree _tree, var _key, var* _to)
{
    var key;
    struct tree_node* node = NULL;
    if (_tree->key_type != String)
    {
        key = _key;
    }
    else
    {
        key.str_var = EString_new((char*)_key.str_var);
    }
    node = _search_auxiliary(_tree, key, _tree->root, NULL);
    if (_tree->key_type == String)
    {
        EString_delete((char*)key.str_var);
    }
    if (!node)
    {
        return NULL;
    }
    *_to = node->value;
    return node;
}

var Tree_get_key(Iterator _i)
{
    return ((struct tree_node*)_i)->key;
}

var Tree_get_value(Iterator _i)
{
    return ((struct tree_node*)_i)->value;
}

void Tree_set_value(Iterator _i, var _data)
{
    if (((struct tree_node*)_i)->ower->value_type == String && ((struct tree_node*)_i)->value.str_var)
    {
        EString_delete( (char*)((struct tree_node*)_i)->value.str_var );
    }
    ///((struct tree_node*)_i)->value = _data;
    var_assign(((struct tree_node*)_i)->ower, &((struct tree_node*)_i)->value, _data);
}

Iterator Tree_next(Iterator _i)
{
    return ((struct tree_node*)_i)->iter_next;
}

Iterator Tree_prev(Iterator _i)
{
    return ((struct tree_node*)_i)->iter_prev;
}

Iterator Tree_begin(Tree _tree)
{
    return _tree->head;
}

Iterator Tree_end(Tree _tree)
{
    return _tree->tail;
}

euint Tree_count(Tree _tree)
{
    return _tree->count;
}

void Tree_set_key_compare_proc(Tree _tree, KEY_COMPARE _proc)
{
    EAssert(_tree->key_type == Vptr, "Tree_set_key_compare_proc fail");
    _tree->key_compare_proc = _proc;
}
#ifdef _MSC_VER
#pragma warning(pop)
#endif