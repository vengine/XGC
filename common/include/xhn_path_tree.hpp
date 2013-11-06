#ifndef XHN_PATH_TREE_HPP
#define XHN_PATH_TREE_HPP
#include "xhn_string.hpp"
#include "xhn_smart_ptr.hpp"
#include "xhn_garbage_collector.hpp"
#define USE_SMART_PTR
namespace xhn
{
wstring get_upper_directory(const wstring& path);
void force_open_file(const wstring& path);
class path_node;
#ifdef USE_SMART_PTR
typedef SmartPtr<path_node> path_node_ptr;
#else
typedef garbage_collector::mem_handle<path_node> path_node_ptr;
#endif
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