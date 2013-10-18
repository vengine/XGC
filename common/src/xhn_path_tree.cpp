#include "xhn_path_tree.hpp"
#include <io.h>
struct FWCharFormat
{
	wchar_t operator() (wchar_t wc) {
		if (wc == L'/')
			return L'\\';
		else
			return wc;
	}
};

int StrCmpW(const wchar_t* ws0, const wchar_t* ws1)
{
	int count = 0;
	while (ws0[count] && ws1[count]) {
		if (ws0[count] > ws1[count])
			return 1;
		else if (ws0[count] < ws1[count])
			return -1;
		count++;
	}
	if (ws0[count] && !ws1[count])
		return 1;
	else if (!ws0[count] && ws1[count])
		return -1;
	else
		return 0;
}

xhn::path_node::path_node()
{
}

xhn::path_node::~path_node()
{
}

void xhn::path_node::search(const wchar_t* path)
{
	path_name = path;
	transform(path_name.begin(), path_name.end(), path_name.begin(), FWCharFormat());
	wstring search_path = path_name;
	if (search_path.size() && search_path[search_path.size() - 1] != L'\\') {
		search_path += L'\\';
	}
	wstring tmp = search_path +  L"*";

	struct _wfinddata_t fd;
	long h = _wfindfirst(tmp.c_str(), &fd);
	int ret = 0;
	while (h != -1L && !ret)
	{
		if (StrCmpW(fd.name, L".") != 0 && StrCmpW(fd.name, L"..") != 0)
		{
			path_node_ptr child;
			child = GC_ALLOC(path_node);
			child->search((search_path + fd.name).c_str());
			child->next = children;
			children = child;
		}
		ret = _wfindnext(h, &fd);
	}
	_findclose(h);
}

void xhn::path_node::get_paths(xhn::vector<xhn::wstring>& result)
{
	if (!children) {
		result.push_back(path_name);
		return;
	}
    path_node_ptr child;
	child = children;
	while (child) {
		child->get_paths(result);
		child = child->next;
	}
}

xhn::path_tree::path_tree(const wchar_t* dir)
{
	root = xhn::garbage_collector::get()->alloc<path_node>(__FILE__, __LINE__, "root");
	root->search(dir);
}

void xhn::path_tree::get_all_paths(xhn::vector<xhn::wstring>& result)
{
    root->get_paths(result);
}