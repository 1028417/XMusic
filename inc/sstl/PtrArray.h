
#pragma once

#include "ptrcontainer.h"

namespace NS_SSTL
{
#undef __Super__
#define __Super__ SArrayT<__DataType__*, __BaseType>

	template <typename __DataType__, template<typename...> class __BaseType>
	class PtrArrayT : public __Super__
	{
	private:
		__UsingSuper(__Super__)

		__UsingSuperType(__RItrType)
		__UsingSuperType(__CRItrType)

		using __DataType = __DataType__;
		using __PtrType = __DataType*;
		using __RefType = __DataType&;

		using __ConstPtr = const __DataType*;
		using __ConstRef = const __DataType&;
		
        using __CB_RefType_void = cfn_void_t<__RefType>;
        using __CB_RefType_bool = cfn_bool_t<__RefType>;
				
	public:
		virtual ~PtrArrayT() {} // 解决qt-mac clang告警

		PtrArrayT() {}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>>
		explicit PtrArrayT(T* ptr, args... others)
		{
			add(ptr, others...);
		}
		
		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>
			, typename = checkNotSameType_t<T, __PtrType>, typename = checkNotContainer_t<T>>
		explicit PtrArrayT(T& ref, args&... others)
		{
			add(ref, others...);
		}

		template<typename... args>
		explicit PtrArrayT(__PtrType ptr, args... others)
		{
			add(ptr, others...);
		}

		template<typename... args>
		explicit PtrArrayT(__RefType ref, args&... others)
		{
			add(ref, others...);
		}

		explicit PtrArrayT(__ContainerType&& container)
		{
			__Super::swap(container);
		}

		PtrArrayT(PtrArrayT&& arr)
		{
			__Super::swap(arr);
		}

		PtrArrayT(const PtrArrayT& arr)
			: __Super(arr)
		{
		}

		explicit PtrArrayT(__InitList initList)
		{
			add(initList);
		}

		template<typename T, typename = checkContainer_t<T>>
		explicit PtrArrayT(const T& container)
		{
			add(container);
		}

		template<typename T, typename = checkContainer_t<T>>
		explicit PtrArrayT(T& container)
		{
			add(container);
		}

		PtrArrayT& operator=(__ContainerType&& container)
		{
			__Super::swap(container);
			return *this;
		}

		PtrArrayT& operator=(PtrArrayT&& arr)
		{
			__Super::swap(arr);
			return *this;
		}

		PtrArrayT& operator=(const PtrArrayT& arr)
		{
			assign(arr);
			return *this;
		}

		PtrArrayT& operator=(__InitList initList)
		{
			assign(initList);
			return *this;
		}

		template <typename T>
		PtrArrayT& operator=(const T&t)
		{
			assign(t);
			return *this;
		}

		template <typename T>
		PtrArrayT& operator=(T&t)
		{
			assign(t);
			return *this;
		}

	public:
		template <typename T>
		PtrArrayT& operator+= (T& rhs)
		{
			this->add(rhs);
			return *this;
		}

		template <typename T>
		PtrArrayT& operator+= (const T& rhs)
		{
			this->add(rhs);
			return *this;
		}

		PtrArrayT& operator+= (__InitList rhs)
		{
			this->add(rhs);
			return *this;
		}

		template <typename T>
		PtrArrayT& operator-= (T& rhs)
		{
			this->del(rhs);
			return *this;
		}

		template <typename T>
		PtrArrayT& operator-= (const T& rhs)
		{
			this->del(rhs);
			return *this;
		}
		
		PtrArrayT& operator-= (__InitList rhs)
		{
			this->del(rhs);
			return *this;
		}

		friend PtrArrayT operator+ (const PtrArrayT& lhs, const PtrArrayT& rhs)
		{
			return PtrArrayT(lhs) += rhs;
		}

		friend PtrArrayT operator+ (const PtrArrayT& lhs, __InitList rhs)
		{
			return PtrArrayT(lhs) += rhs;
		}

		friend PtrArrayT operator+ (__InitList lhs, const PtrArrayT& rhs)
		{
			return PtrArrayT(lhs) += rhs;
		}

		template <typename T>
		friend PtrArrayT operator+ (const PtrArrayT& lhs, const T& rhs)
		{
			return PtrArrayT(lhs) += rhs;
		}

		template <typename T, typename = checkNotIsBase_t<PtrArrayT, T>>
		friend PtrArrayT operator+ (const T& lhs, const PtrArrayT& rhs)
		{
			return PtrArrayT(lhs) += rhs;
		}
		
		friend PtrArrayT operator- (const PtrArrayT& lhs, const PtrArrayT& rhs)
		{
			return PtrArrayT(lhs) -= rhs;
		}

		friend PtrArrayT operator- (const PtrArrayT& lhs, __InitList rhs)
		{
			return PtrArrayT(lhs) -= rhs;
		}

		friend PtrArrayT operator- (__InitList lhs, const PtrArrayT& rhs)
		{
			return PtrArrayT(lhs) -= rhs;
		}\

		template <typename T>
		friend PtrArrayT operator- (const PtrArrayT& lhs, const T& rhs)
		{
			return PtrArrayT(lhs) -= rhs;
		}

		template <typename T, typename = checkNotIsBase_t<PtrArrayT, T>>
		friend PtrArrayT operator- (const T& lhs, const PtrArrayT& rhs)
		{
			return PtrArrayT(lhs) -= rhs;
		}

		template <typename T>
		friend PtrArrayT operator& (const PtrArrayT& lhs, const T& rhs)
		{
			PtrArrayT arr;
			for (auto&data : rhs)
			{
				if (lhs.includes(data))
				{
					arr.add(data);
				}
			}

			return arr;
		}

		template <typename T>
		friend PtrArrayT operator& (const PtrArrayT& lhs, T& rhs)
		{
			PtrArrayT arr;
			for (auto&data : rhs)
			{
				if (lhs.includes(data))
				{
					arr.add(data);
				}
			}

			return arr;
		}

		friend PtrArrayT operator& (const PtrArrayT& lhs, __InitList rhs)
		{
			return lhs & PtrArrayT(rhs);
		}

	public:
		const __ContainerType& operator->()
		{
			return m_data;
		}
		const __ContainerType& operator->() const
		{
			return m_data;
		}

		const __ContainerType& operator *()
		{
			return m_data;
		}
		const __ContainerType& operator *() const
		{
			return m_data;
		}

		operator __ContainerType& () = delete;
		operator const __ContainerType& ()
		{
			return m_data;
		}
		operator const __ContainerType& () const
		{
			return m_data;
		}

		__CItrType erase(const __CItrType& itr)
		{
			return m_data.erase((const __ItrType&)itr);
		}

		__CItrType begin() const
		{
			return m_data.cbegin();
		}
		__CItrType end() const
		{
			return m_data.cend();
		}

		__CRItrType rbegin() const
		{
			return m_data.crbegin();
		}
		__CRItrType rend() const
		{
			return m_data.crend();
		}

	public:
		template <typename CB>
		void operator() (int startPos, int endPos, const CB& cb) const
		{
			_forEach(cb, startPos, endPos);
		}

		template <typename CB>
		void operator() (int startPos, const CB& cb) const
		{
			_forEach(cb, startPos);
		}

		template <typename CB>
		void operator() (const CB& cb) const
		{
			_forEach(cb);
		}

		int find(__CB_RefType_bool cb, size_t startPos = 0) const
		{
			int nRetPos = -1;

            (*this)(startPos, [&](__RefType ref, size_t pos) {
				if (cb(ref))
				{
					nRetPos = pos;
					return false;
				}

				return true;
			});

			return nRetPos;
		}

		int indexOf(__ConstPtr ptr) const
		{
			return m_data.indexOf(ptr);
		}

		int indexOf(__ConstRef ref) const
		{
			return m_data.indexOf(ref);
		}

		template <typename T>
		int indexOf(T* ptr) const
		{
			return m_data.indexOf(ptr);
		}

		template <typename T>
		int indexOf(T& ref) const
		{
			return m_data.indexOf(ref);
		}

		bool includes(__ConstPtr ptr) const
		{
			return indexOf(ptr) >= 0;
		}

		bool includes(__ConstRef ref) const
		{
			return indexOf(ref) >= 0;
		}

		template <typename T>
		bool includes(T* ptr) const
		{
			return indexOf(ptr) >= 0;
		}

		template <typename T>
		bool includes(T& ref) const
		{
			return indexOf(ref) >= 0;
		}

		size_t del(__ConstRef ref, bool bDelOnlyOne = true)
		{
			return m_data.del(ref, bDelOnlyOne);
		}

		size_t del(__ConstPtr ptr, bool bDelOnlyOne = true)
		{
			return m_data.del(ptr, bDelOnlyOne);
		}
		
		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>>
		size_t del(T* ptr, args... others)
		{
			size_t uRet = 0;
			(void)tagDynamicArgsExtractor<T*>::extract([&](T* ptr) {
				uRet += m_data.del(ptr);

				return true;
			}, ptr, others...);
			return uRet;
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>
			, typename = checkNotSameType_t<T, __PtrType>, typename = checkNotContainer_t<T>>
		size_t del(T& ref, args&... others)
		{
			size_t uRet = 0;
			(void)tagDynamicArgsExtractor<T>::extract([&](T& ref) {
				uRet += m_data.del(ref);

				return true;
			}, ref, others...);
			return uRet;
		}

		template<typename... args>
		size_t del(__ConstPtr ptr, args... others)
		{
			size_t uRet = 0;
			(void)tagDynamicArgsExtractor<__ConstPtr>::extract([&](__ConstPtr ptr) {
				uRet += m_data.del(ptr);

				return true;
			}, ptr, others...);
			return uRet;
		}

		template<typename... args>
		size_t del(__ConstRef ref, args&... others)
		{
			size_t uRet = 0;
			(void)tagDynamicArgsExtractor<__ConstRef>::extract([&](__ConstRef ref) {
				uRet += m_data.del(ref);
				
				return true;
			}, ref, others...);
			return uRet;
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t del(T& container)
		{
			if (__Super::checkIsSelf(container))
			{
				size_t uRet = m_data.size();
				m_data.clear();
				return uRet;
			}
			
			return m_data.del(container);
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t del(const T& container)
		{
			if (__Super::checkIsSelf(container))
			{
				size_t uRet = m_data.size();
				m_data.clear();
				return uRet;
			}
			
			return m_data.del(container);
		}

		size_t del(__InitList initList)
		{
			return m_data.del(initList);
		}

        size_t del_ex(cfn_t<E_DelConfirm, __RefType> cb)
		{
			return __Super::del_ex([&](const __PtrType ptr) {
				if (NULL != ptr)
				{
					return cb(*ptr);
				}

				return E_DelConfirm::DC_No;
			});
		}

        size_t del(cfn_bool_t<__RefType> cb)
		{
			return __Super::del([&](const __PtrType ptr) {
				if (NULL != ptr)
				{
					return cb(*ptr);
				}

				return false;
			});
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>>
		size_t insert(int nPos, T* ptr, args... others)
		{
			size_t uRet = 0;

			if (nPos < 0 || nPos >= (int)m_data.size())
			{
				(void)tagDynamicArgsExtractor<T*>::extract([&](T* ptr) {
					if (m_data.add(ptr))
					{
						uRet++;
					}

					return true;
				}, ptr, others...);
			}
			else
			{
				(void)tagDynamicArgsExtractor<T*>::extractReverse([&](T* ptr) {
					if (m_data.add(ptr, nPos))
					{
						uRet++;
					}

					return true;
				}, ptr, others...);
			}

			return uRet;
		}
		
		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>
			, typename = checkNotSameType_t<T, __PtrType>, typename = checkNotContainer_t<T>>
		size_t insert(int nPos, T& ref, args&... others)
		{
			return insert(nPos, &ref, &others...);
		}

		template<typename... args>
		size_t insert(int nPos, __PtrType ptr, args... others)
		{
			size_t uRet = 0;

			if (nPos < 0 || nPos >= (int)m_data.size())
			{
				(void)tagDynamicArgsExtractor<__PtrType>::extract([&](__PtrType ptr) {
					if (m_data.add(ptr))
					{
						uRet++;
					}

					return true;
				}, ptr, others...);
			}
			else
			{
				(void)tagDynamicArgsExtractor<__PtrType>::extractReverse([&](__PtrType ptr) {
					if (m_data.add(ptr, nPos))
					{
						uRet++;
					}

					return true;
				}, ptr, others...);
			}

			return uRet;
		}

		template<typename... args>
		void insert(int nPos, __RefType ref, args&... others)
		{
			if (nPos < 0 || nPos >= (int)m_data.size())
			{
				(void)tagDynamicArgsExtractor<__RefType>::extract([&](__RefType ref) {
					m_data.add(ref);
					return true;
				}, ref, others...);
			}
			else
			{
				(void)tagDynamicArgsExtractor<__RefType>::extractReverse([&](__RefType ref) {
					m_data.add(ref, nPos);
					return true;
				}, ref, others...);
			}
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t insert(int nPos, T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				return m_data.add(container, nPos);
			}

			return 0;
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t insert(int nPos, const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				return m_data.add(container, nPos);
			}

			return 0;
		}

		size_t insert(int nPos, __InitList initList)
		{
			return m_data.add(initList, nPos);
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>>
		size_t add(T* ptr, args... others)
		{
			return insert(-1, ptr, others...);
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>
			, typename = checkNotSameType_t<T, __PtrType>, typename = checkNotContainer_t<T>>
		size_t add(T& ref, args&... others)
		{
			return insert(-1, ref, others...);
		}

		template<typename... args>
		size_t add(__PtrType ptr, args... others)
		{
			return insert(-1, ptr, others...);
		}

		template<typename... args>
		void add(__RefType ref, args&... others)
		{
			return insert(-1, ref, others...);
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t add(T& container)
		{
			return insert(-1, container);
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t add(const T& container)
		{
			return insert(-1, container);
		}

		size_t add(__InitList initList)
		{
			return insert(-1, initList);
		}
			
		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>>
		size_t addFront(T* ptr, args... others)
		{
			return insert(0, ptr, others...);
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>
			, typename = checkNotSameType_t<T, __PtrType>, typename = checkNotContainer_t<T>>
		size_t addFront(T& ref, args&... others)
		{
			return insert(0, ref, others...);
		}

		template<typename... args>
		size_t addFront(__PtrType ptr, args... others)
		{
			return insert(0, ptr, others...);
		}

		template<typename... args>
		void addFront(__RefType ref, args&... others)
		{
			return insert(0, ref, others...);
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t addFront(T& container)
		{
			return insert(0, container);
		}

		template<typename T, typename = checkContainer_t<T>>
		size_t addFront(const T& container)
		{
			return insert(0, container);
		}

		size_t addFront(__InitList initList)
		{
			return insert(0, initList);
		}

		bool front(__CB_RefType_void cb) const
		{
			return __Super::front([&](__PtrType ptr) {
				if (NULL != ptr)
				{
					cb(*ptr);
				}
			});
		}

		bool back(__CB_RefType_void cb) const
		{
			return __Super::back([&](__PtrType ptr) {
				if (NULL != ptr)
				{
					cb(*ptr);
				}
			});
		}

		bool popFront()
		{
			return __Super::popFront();
		}

		bool popFront(__CB_RefType_void cb)
		{
			return __Super::popFront([&](__PtrType ptr) {
				if (NULL != ptr)
				{
					cb(*ptr);
				}
			});
		}

		bool popBack()
		{
			return __Super::popBack();
		}

		bool popBack(__CB_RefType_void cb)
		{
			return __Super::popBack([&](__PtrType ptr) {
				if (NULL != ptr)
				{
					cb(*ptr);
				}
			});
		}

		bool get(size_t pos, __CB_RefType_void cb) const
		{
			if (pos >= m_data.size())
			{
				return false;
			}

			__PtrType ptr = m_data[pos];
			if (NULL == ptr)
			{
				return false;
			}

			cb(*ptr);

			return true;
		}

		bool set(size_t pos, __PtrType ptr)
		{
			return m_data.set(pos, ptr);
		}

		bool set(size_t pos, __RefType ref)
		{
			return m_data.set(pos, &ref);
		}

		template<typename... args>
		void assign(__PtrType ptr, args... others)
		{
			m_data.clear();
			add(ptr, others...);
		}

		template<typename... args>
		void assign(__RefType ref, args&... others)
		{
			m_data.clear();
			add(ref, others...);
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>>
		void assign(T* ptr, args... others)
		{
			m_data.clear();
			add(ptr, others...);
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>
			, typename = checkNotSameType_t<T, __PtrType>, typename = checkNotContainer_t<T>>
		void assign(T& ref, args&... others)
		{
			m_data.clear();
			add(ref, others...);
		}
			
		void assign(__ContainerType&& container)
		{
			__Super::swap(container);
		}

		void assign(PtrArrayT&& arr)
		{
			__Super::swap(arr);
		}

		template <typename T>
		void assign(T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				m_data.clear();

				add(container);
			}
		}

		template <typename T>
		void assign(const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				m_data.clear();

				add(container);
			}
		}

		void assign(__InitList initList)
		{
			m_data.clear();
			m_data.add(initList);
		}

		template<typename... args>
		PtrArrayT concat(__PtrType ptr, args... others) const
		{
			PtrArrayT arr(*this);
			arr.add(ptr, others...);
			return arr;
		}

		template<typename... args>
		PtrArrayT concat(__RefType ref, args&... others) const
		{
			PtrArrayT arr(*this);
			arr.add(ref, others...);
			return arr;
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>>
		PtrArrayT concat(T* ptr, args... others) const
		{
			PtrArrayT arr(*this);
			arr.add(ptr, others...);
			return arr;
		}

		template <typename T, typename... args, typename = checkNotSameType_t<T, __DataType>
			, typename = checkNotSameType_t<T, __PtrType>, typename = checkNotContainer_t<T>>
		PtrArrayT concat(T& ref, args&... others) const
		{
			PtrArrayT arr(*this);
			arr.add(ref, others...);
			return arr;
		}

		template<typename T>
		PtrArrayT concat(T& container) const
		{
			PtrArrayT arr(*this);
			arr.add(container);
			return arr;
		}

		template<typename T>
		PtrArrayT concat(const T& container) const
		{
			PtrArrayT arr(*this);
			arr.add(container);
			return arr;
		}

		PtrArrayT concat(__InitList initList) const
		{
			PtrArrayT arr(*this);
			arr.add(initList);
			return arr;
		}

		template<typename... args>
		void splice(size_t pos, size_t nRemove, __PtrType v, args... others)
		{
			__Super::splice(pos, nRemove, v, others...);
		}

		template<typename... args>
		void splice(size_t pos, size_t nRemove, __RefType ref, args&... others)
		{
			vector<__PtrType> vec;
			(void)tagDynamicArgsExtractor<__RefType>::extract([&](__RefType ref) {
				vec.push_back(&ref);
				return true;
			}, ref, others...);

			__Super::splice(pos, nRemove, vec);
		}

		template<typename T>
		void splice(size_t pos, size_t nRemove, T& container)
		{
			__Super::splice(pos, nRemove, container);
		}

		template<typename T>
		void splice(size_t pos, size_t nRemove, const T& container)
		{
			__Super::splice(pos, nRemove, container);
		}

		void splice(size_t pos, size_t nRemove, __InitList initList)
		{
			__Super::splice(pos, nRemove, initList);
		}

		void splice(size_t pos, size_t nRemove)
		{
            __Super::splice(pos, nRemove, __InitList());
		}

		void qsort(__CB_Sort_T<__DataType> cb)
		{
			__Super::qsort([&](__PtrType lhs, __PtrType rhs) {
                if (NULL != lhs && NULL != rhs)
                {
                    return cb(*lhs, *rhs);
                }

                return false;
            });
		}

	public:
		template <typename T>
        SArray<T> map(cfn_t<T, __ConstRef> cb) const
		{
			SArray<T> arr;

			(*this)([&](__RefType ref) {
				arr.add(cb(ref));
			});
			
			return arr;
		}

		template <typename CB, typename RET = decltype(declval<CB>()(declval<__RefType>()))>
		SArray<RET> map(const CB& cb) const
		{
			return map<RET>(cb);
		}

		PtrArrayT filter(__CB_RefType_bool cb) const
		{
			PtrArrayT arr;

			for (auto& ptr : m_data)
			{
				if (NULL != ptr && cb(*ptr))
				{
					arr.add(ptr);
				}
			}

			return arr;
		}

		bool every(__CB_RefType_bool cb) const
		{
			return __Super::every([&](__PtrType ptr) {
				if (NULL == ptr)
				{
					return false;
				}

				return cb(*ptr);
			});
		}

		bool any(__CB_RefType_bool cb) const
		{
			return __Super::any([&](__PtrType ptr) {
				if (NULL == ptr)
				{
					return false;
				}

				return cb(*ptr);
			});
		}

	private:
		void _add(const __PtrType& ptr) override
		{
			m_data.add((__PtrType)ptr);
		}

		template <typename CB>
		void __forEach(const CB& cb, int startPos = 0, int endPos = -1) const
		{
			__Super::operator() (startPos, endPos, [&](__PtrType ptr, size_t pos) {
				if (NULL != ptr)
				{
					return cb(*ptr, pos);
				}

				return true;
			});
		}

		template <typename CB, typename = checkCBBool_t<CB, __RefType, size_t>>
		void _forEach(const CB& cb, int startPos = 0, int endPos = -1) const
		{
			__forEach(cb, startPos, endPos);
		}

		template <typename CB, typename = checkCBVoid_t<CB, __RefType, size_t>, typename = void>
		void _forEach(const CB& cb, int startPos = 0, int endPos = -1) const
		{
			__forEach([&](__RefType ref, size_t pos) {
				cb(ref, pos);
				return true;
			}, startPos, endPos);
		}

		template <typename CB, typename = checkCBBool_t<CB, __RefType>, typename = void, typename = void>
		void _forEach(const CB& cb, int startPos = 0, int endPos = -1) const
		{
			__forEach([&](__RefType ref, size_t) {
				return cb(ref);
			}, startPos, endPos);
		}

		template <typename CB, typename = checkCBVoid_t<CB, __RefType>, typename = void, typename = void, typename = void>
		void _forEach(const CB& cb, int startPos = 0, int endPos = -1) const
		{
			__forEach([&](__RefType ref, size_t) {
				cb(ref);
				return true;
			}, startPos, endPos);
		}
	};
}
