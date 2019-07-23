
#ifndef __Define_H
#define __Define_H

//#include <initializer_list>

#include <vector>
#include <deque>

#include <list>

#include <set>
#include <unordered_set>

#include <map>
#include <unordered_map>

#include <functional>

using namespace std;

namespace NS_SSTL
{
	template <typename T>
	using InitList_T = const initializer_list<T>&;

	template <typename T, typename RET>
	using CB_T_Ret = const function<RET(const T&v)>&;

	template <typename T>
	using CB_T_void = CB_T_Ret<T, void>;

	template <typename T>
	using CB_T_bool = CB_T_Ret<T, bool>;

	template <typename T>
    using __CB_Sort_T = const function<bool(const T&lhs, const T&rhs)>&;

	template <template<typename...> class __BaseType, class __PtrType> class ptrcontainerT;

	template <class __PtrType>
	using ptrvectorT = ptrcontainerT<vector, __PtrType>;

	template <class __Type>
	using ptrvector = ptrvectorT<__Type*>;

	template <class __PtrType>
	using ptrlistT = ptrcontainerT<list, __PtrType>;
	
	enum class E_DelConfirm
	{
		DC_Yes
		, DC_No
		, DC_Abort
		, DC_YesAbort
	};
};

#ifdef _MSC_VER
#define __UsingSuperType(T)
#else
#define __UsingSuperType(T) using T = typename __Super::T;
#endif

#define __UsingSuper(Super) \
	using __Super = Super; \
	\
	__UsingSuperType(__ContainerType) \
	__ContainerType& m_data = __Super::m_data; \
	\
	__UsingSuperType(__DataRef) \
	__UsingSuperType(__DataConstRef) \
	\
	__UsingSuperType(__ItrType) \
	__UsingSuperType(__CItrType) \
	\
	__UsingSuperType(CB_Del) \
	\
	__UsingSuperType(__InitList) \
	\
	__UsingSuperType(__CB_Ref_void) \
	__UsingSuperType(__CB_Ref_bool) \
	\
	__UsingSuperType(__CB_ConstRef_void) \
	__UsingSuperType(__CB_ConstRef_bool) \


#endif // __Define_H
