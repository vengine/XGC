#ifndef XHN_PATH_TREE_HPP
#define XHN_PATH_TREE_HPP
#include "xhn_string.hpp"
#include "xhn_smart_ptr.hpp"
namespace xhn
{
class path_node : public RefObject
{
public:
	wstring path_name;
	SmartPtr<path_node> next;
    SmartPtr<path_node> children;
public:
	path_node();
	~path_node();
	void search(const wchar_t* path);
	void get_paths(xhn::vector<xhn::wstring>& result);
};
typedef SmartPtr<path_node> path_node_ptr;
class path_tree : public MemObject
{
public:
	path_node_ptr root;
public:
    path_tree(const wchar_t* dir);
	void get_all_paths(xhn::vector<xhn::wstring>& result);
};
}
#endif