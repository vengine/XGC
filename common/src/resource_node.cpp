#include "pch.h"
#include "resource_node.h"
#include "xhn_vector.hpp"
#include "elog.h"
xhn::set<ResourceNode*> ResourceNode::s_deletedResNodes;

ResourceNode::ResourceNode(vptr mem)
: m_mem(mem)
, m_isRootNode(false)
, m_isDeleted(false)
{
	elog("new child resource node\n");
}
ResourceNode::ResourceNode()
: m_mem(NULL)
, m_isRootNode(true)
, m_isDeleted(false)
{
	elog("new root resource node\n");
}
ResourceNode::~ResourceNode()
{
	elog("dest resource node\n");
	if (m_mem)
		Mfree(m_mem);
}

bool ResourceNode::_TrackBack(ResourceNode* node, xhn::set<ResourceNode*>& trackBuffer)
{
	if (m_isRootNode)
		return true;
	m_isDeleted = true;

	xhn::set<ResourceNode*>::iterator iter = node->m_inputs.begin();
	for (; iter != node->m_inputs.end(); iter++)
	{
		ResourceNode* i = *iter;
		if (i->m_isDeleted)
			continue;
		if (trackBuffer.find(i) != trackBuffer.end())
			continue;
		trackBuffer.insert(i);
		if (_TrackBack(i, trackBuffer)) {
			m_isDeleted = false;
			return true;
		}
	}
	iter = m_outputs.begin();
	for (; iter != m_outputs.end(); iter++)
	{
		ResourceNode* node = *iter;
		if (node->m_isDeleted)
			continue;
		node->m_inputs.erase(this);
	}
	MarkDeleted();
	return false;
}
void ResourceNode::TrackBack()
{
	xhn::set<ResourceNode*> trackBuffer;
    _TrackBack(this, trackBuffer);
}
void ResourceNode::Update()
{
	while (s_deletedResNodes.size()) {
		xhn::vector<ResourceNode*> tmp;
		xhn::set<ResourceNode*>::iterator iter = s_deletedResNodes.begin();
		for (; iter != s_deletedResNodes.end(); iter++)
		{
            tmp.push_back(*iter);
		}
		s_deletedResNodes.clear();
		for (euint i = 0; i < tmp.size(); i++)
		{
			ResourceNode* node = tmp[i];
			delete node;
		}
	}
}
