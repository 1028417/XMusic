
#pragma once

#include "_define.h"

#include "_check.h"

#include "_util.h"

namespace NS_SSTL
{
	template <typename __ContainerType__, typename __KeyType__ = 
		removeConstRef_t<typename tagCheckContainer<__ContainerType__>::Ref_Type>> class SContainerT;

	template <typename __DataType, template<typename...> class __BaseType> class SArrayT;
	template <typename __DataType, template<typename...> class __BaseType = std::vector>
	using SArray = SArrayT<__DataType, __BaseType>;
	template <typename __DataType> using SVector = SArray<__DataType, std::vector>;
	template <typename __DataType> using SDeque = SArrayT<__DataType, std::deque>;

    template <typename __Type, template<typename...> class __BaseType> class PtrArrayT;
    template <typename __PtrType> using ptrvectorT = ptrcontainerT<vector, __PtrType>;
	template <typename __Type> using PtrArray = PtrArrayT<__Type, ptrvectorT>;
	template <typename __Type> using ConstPtrArray = PtrArray<const __Type>;

	template <typename __DataType> class SListT;
	template <typename __DataType> using SList = SListT<__DataType>;

	template <typename __DataType> class ArrListT;
	template <typename __DataType> using ArrList = ArrListT<__DataType>;
	
	template <typename __FirstType, typename __SecondType> class PairListT;
	template <typename __FirstType, typename __SecondType> using PairList = PairListT<__FirstType, __SecondType>;

	template <typename __DataType, template<typename...> class __BaseType> class SSetT;
	template <typename __DataType> using SSet = SSetT<__DataType, std::set>;
	template <typename __DataType> using SHashSet = SSetT<__DataType, std::unordered_set>;
	
	template <typename __KeyType, typename __ValueType, template<typename...> class __BaseType> class SMapT;
	template <typename __KeyType, typename __ValueType>	using SMap = SMapT<__KeyType, __ValueType, std::map>;
	template <typename __KeyType, typename __ValueType>	using SHashMap = SMapT<__KeyType, __ValueType, std::unordered_map>;
	template <typename __KeyType, typename __ValueType>	using SMultiMap = SMapT<__KeyType, __ValueType, std::multimap>;
	template <typename __KeyType, typename __ValueType> using SMultiHashMap = SMapT<__KeyType, __ValueType, std::unordered_multimap>;
}

#include "SContainer.h"

#include "SArray.h"
#include "PtrArray.h"

#include "SList.h"
#include "ArrList.h"
#include "PairList.h"

#include "SSet.h"

#include "SMap.h"
