
#pragma once

namespace NS_SSTL
{
	template <bool _test>
	using enable_if_t = typename enable_if<_test, void>::type;

	template <bool _test>
	using enable_ifnot_t = enable_if_t<!_test>;

	template<template<typename...> class _check, typename... _types>
	using enableIf_t = enable_if_t<_check<_types...>::value>;

	template<template<typename...> class _check, typename... _types>
	using enableIfNot_t = enable_ifnot_t<_check<_types...>::value>;

	template <typename _check1, typename _check2>
	using enableIf_and_t = enable_if_t<_check1::value && _check2::value>;

	template <typename _check1, typename _check2>
	using enableIf_or_t = enable_if_t<_check1::value || _check2::value>;


	template <typename T>
	using removeConst_t = typename std::remove_const<T>::type;

	template <typename T>
	using removeConstRef_t = removeConst_t<typename std::remove_reference<T>::type>;


	template <typename T1, typename T2>
	using checkSameType_t = enableIf_t<is_same, T1, T2>;

	template <typename T1, typename T2>
	using checkNotSameType_t = enableIfNot_t<is_same, T1, T2>;

	template <typename T1, typename T2>
	using checkSameDecayType_t = checkSameType_t<typename decay<T1>::type, typename decay<T2>::type>;


	template <typename CB, typename... Paras>
	using cbRet_t = decltype(declval<CB>()(declval<Paras>()...));

	template <typename CB, typename Ret, typename... Paras>
	using checkCBRet_t = checkSameType_t<cbRet_t<CB, Paras...>, Ret>;

	template <typename CB, typename... Paras>
	using checkCBVoid_t = checkSameType_t<decltype(declval<CB>()(declval<Paras>()...)), void>;

	template <typename CB, typename... Paras>
	using checkCBNotVoid_t = checkNotSameType_t<decltype(declval<CB>()(declval<Paras>()...)), void>;
	
	template <typename CB, typename... Paras>
	using checkCBBool_t = checkSameType_t<decltype(declval<CB>()(declval<Paras>()...)), bool>;

	template <typename T1, typename T2>
	using checkClass_t = enableIf_and_t<is_class<T1>, is_class<T2>>;

	template <typename T>
	using checkIsConst_t = enableIf_t<is_const, T>;

	template <typename _Base, typename T>
	using checkIsBase_t = enableIf_t<is_base_of, _Base, T>;

	template <typename _Base, typename T>
	using checkNotIsBase_t = enableIfNot_t<is_base_of, _Base, T>;


	template <typename T, typename... others>
	struct tagCheckArgs0
	{
		typedef T type;
	};
	template <typename... args>
	using args0Type_t = typename tagCheckArgs0<args...>::type;

	template <typename T, typename... args>
	using checkArgs0Type_t = checkSameType_t<T, args0Type_t<args...>>;

    template<class _Iter> using checkIter_t =
#ifdef _MSC_VER
    enableIf_t<_Is_iterator, _Iter>;
#else
    typename enable_if<is_convertible<typename
          iterator_traits<_Iter>::iterator_category, input_iterator_tag>::value>::type;
#endif

	template <typename C>
	struct _tagCheckContainer
	{
		template<typename T, typename = checkIter_t<decltype(declval<T&>().begin())>>
		static auto itr(const T& t)->decltype(declval<T&>().begin());

		template<typename T>
		static void itr(...);

		static constexpr bool value = !std::is_same<decltype(itr<C>(declval<C>())), void>::value;
	};

	template<typename T>
	using checkContainer_t = enableIf_t<_tagCheckContainer, T>;

	template<typename T>
	using checkNotContainer_t = enableIfNot_t<_tagCheckContainer, T>;

	template <typename C, typename = void>
	struct tagCheckContainer
	{
	};

	template <typename C>
	struct tagCheckContainer<C, enable_if_t<_tagCheckContainer<C>::value>>
	{
		typedef decltype(declval<C&>().begin()) Itr_Type;

		typedef decltype(declval<const C&>().begin()) CItr_Type;

		typedef decltype(*declval<C&>().begin()) Ref_Type;

		typedef removeConstRef_t<Ref_Type> Data_Type;
	};

	template<class T>
	using containerRefType_t = typename tagCheckContainer<T>::Ref_Type;

	template<class T>
	using containerDataType_t = typename tagCheckContainer<T>::Data_Type;

	template<class T>
	using containerItrType_t = typename tagCheckContainer<T>::Itr_Type;
	template<class T>
	using containerCItrType_t = typename tagCheckContainer<T>::CItr_Type;

	template<class T>
	using containerRItrType_t = typename tagCheckContainer<T>::RItr_Type;
	template<class T>
	using containerCRItrType_t = typename tagCheckContainer<T>::CRItr_Type;
		
	template<class T, typename DATA>
	using checkContainerData_t = checkSameType_t<containerDataType_t<T>, DATA>;
};
