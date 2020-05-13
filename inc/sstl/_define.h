
#pragma once

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
    using fn_void = function<void()>;
    using cfn_void = const fn_void&;
    template <typename T> using fn_void_t = function<void(T)>;
    template <typename T> using cfn_void_t = const fn_void_t<T>&;
    template <typename T> using fn_t_void = function<T(void)>;
    template <typename T> using cfn_t_void = const fn_t_void<T>&;

    using fn_bool = function<bool()>;
    using cfn_bool = const fn_bool&;
    template <typename T> using fn_bool_t = function<bool(T)>;
    template <typename T> using cfn_bool_t = const fn_bool_t<T>&;

    template <typename RET, typename T> using fn_t = function<RET(T)>;
    template <typename RET, typename T> using cfn_t = const fn_t<RET, T>&;

	template <template<typename...> class __BaseType, class __PtrType> class ptrcontainerT;
    template <typename __Type> using ptrvector = ptrcontainerT<vector, __Type*>;

    template <typename T> using InitList_T = const initializer_list<T>&;

	enum class E_DelConfirm
	{
		DC_Yes
		, DC_No
		, DC_Abort
		, DC_YesAbort
	};

    template <typename T> using __CB_Sort_T = const function<bool(const T&lhs, const T&rhs)>&;
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
	__UsingSuperType(__CB_ConstRef_bool)
