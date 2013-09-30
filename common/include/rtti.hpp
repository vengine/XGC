#ifndef RTTI_HPP
#define RTTI_HPP
class RTTI
{
public:
	RTTI (const char* typeName, const RTTI* parentRTTI)
		: m_typeName(typeName)
		, m_parentRTTI(parentRTTI)
	{}
	inline const char* GetTypeName() const {return m_typeName;}
	inline const RTTI* GetParentRTTI() const {return m_parentRTTI;}
protected:
	const char* m_typeName;
	const RTTI* m_parentRTTI;
};

#define DeclareRootRTTI \
public: \
static const RTTI s_RTTI; \
virtual const RTTI* GetRTTI() const {return &s_RTTI;} \
bool IsKindOf(const RTTI* rtti) const \
{ \
	const RTTI* tmp = GetRTTI(); \
	while (tmp) \
    { \
	    if (tmp == rtti) return true; \
	    tmp = tmp->GetParentRTTI(); \
    } \
	return false; \
} \
template <typename T> \
bool IsKindOf() const \
{ \
    return IsKindOf(&T::s_RTTI); \
} \
template <typename T> \
T* DynamicCast() \
{ \
    if (IsKindOf(&T::s_RTTI)) return (T*)this; \
	else                      return 0; \
} \
template <typename T> \
const T* DynamicCast() const\
{ \
	if (IsKindOf(&T::s_RTTI)) return (const T*)this; \
	else                      return 0; \
} 

#define DeclareRTTI \
public: \
static const RTTI s_RTTI; \
virtual const RTTI* GetRTTI() const {return &s_RTTI;}

#define ImplementRootRTTI(rootclassname) \
const RTTI rootclassname::s_RTTI(#rootclassname, 0)

#define ImplementRTTI(classname, parentclassname) \
const RTTI classname::s_RTTI(#classname, &parentclassname::s_RTTI)

#endif