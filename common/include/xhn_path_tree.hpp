#ifndef XHN_PATH_TREE_HPP
#define XHN_PATH_TREE_HPP
#include "xhn_string.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_garbage_collector.hpp"
namespace xhn
{
class path_node;
typedef garbage_collector::mem_handle<path_node> path_node_ptr;
class path_node : public RefObject
{
public:
	wstring path_name;
	path_node_ptr next;
    path_node_ptr children;
	bool is_folder;
public:
	path_node();
	~path_node();
	void search(const wchar_t* path);
	void get_paths(xhn::vector<xhn::wstring>& result);
};
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