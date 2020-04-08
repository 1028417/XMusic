
#pragma once

namespace NS_SSTL
{
#undef __Super__
#define __Super__ SListT<__DataType>

	template <typename __DataType>
	class ArrListT : public __Super__
	{
	protected:
		__UsingSuper(__Super__)

		__UsingSuperType(__RItrType)
		__UsingSuperType(__CRItrType)

		PtrArray<__DataType> m_ptrArray;
		
	public:
		ArrListT() = default;

		template<typename... args>
		explicit ArrListT(__DataConstRef data, const args&... others)
			: __Super(data, others...)
		{
		}

		explicit ArrListT(__ContainerType&& container)
		{
			__Super::swap(container);
		}

		ArrListT(ArrListT&& lst)
		{
			__Super::swap(lst);
		}

		ArrListT(const ArrListT& lst)
			: __Super(lst)
			, m_ptrArray(m_data)
		{
		}

		explicit ArrListT(__InitList initList)
			: __Super(initList)
			, m_ptrArray(m_data)
		{
		}

		template<typename T, typename = checkContainerData_t<T, __DataType>>
		explicit ArrListT(const T& container)
			: __Super(container)
			, m_ptrArray(m_data)
		{
		}

		ArrListT& operator=(__ContainerType&& container)
		{
			__Super::swap(container);
			return *this;
		}

		ArrListT& operator=(ArrListT&& lst)
		{
			__Super::swap(lst);
			return *this;
		}

		ArrListT& operator=(const ArrListT& lst)
		{
			__Super::assign(lst);
			return *this;
		}

		ArrListT& operator=(__InitList initList)
		{
			__Super::assign(initList);
			return *this;
		}

		template <typename T>
		ArrListT& operator=(const T&t)
		{
			__Super::assign(t);
			return *this;
		}

	public:
		template<typename CB>
		void operator() (int startPos, int endPos, const CB& cb)
		{
			adaptor().forEach(cb, startPos, endPos);
		}

		template<typename CB>
		void operator() (int startPos, int endPos, const CB& cb) const
		{
			adaptor().forEach(cb, startPos, endPos);
		}

		template<typename CB>
		void operator() (int startPos, const CB& cb)
		{
			adaptor().forEach(cb, startPos);
		}

		template<typename CB>
		void operator() (int startPos, const CB& cb) const
		{
			adaptor().forEach(cb, startPos);
		}

		template<typename CB>
		void operator() (const CB& cb)
		{
			adaptor().forEach(cb);
		}

		template<typename CB>
		void operator() (const CB& cb) const
		{
			adaptor().forEach(cb);
		}

	public:
		__ItrType erase(const __CItrType& itr) override
		{
			m_ptrArray.del(&*itr);
			return __Super::erase(itr);
		}

		void clear() override
		{
			m_ptrArray.clear();
			m_data.clear();
		}

		__ItrType insert(const __CItrType& itr, __DataConstRef data) override
        {
            int pos = -1;
            if (itr != m_data.end())
            {
				pos = m_ptrArray.indexOf(&*itr);
            }

            auto itrRet = m_data.insert(itr, data);
            m_ptrArray.insert(pos, *itrRet);

            return itrRet;
        }

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

		__RItrType rbegin()
		{
			return m_data.rbegin();
		}
		__CRItrType rbegin() const
		{
			return m_data.rbegin();
		}

		__RItrType rend()
		{
			return m_data.rend();
		}
		__CRItrType rend() const
		{
			return m_data.rend();
		}

	public:
		int find(__CB_ConstRef_bool cb, size_t startPos = 0) const
		{
			return m_ptrArray.find([&](__DataConstRef data) {
				return cb(data);
			}, startPos);
		}

		bool get(size_t pos, __CB_Ref_void cb)
		{
			return m_ptrArray.get(pos, cb);
		}

		bool get(size_t pos, __CB_ConstRef_void cb) const
		{
			return m_ptrArray.get(pos, cb);
		}

		bool get(size_t pos, __DataRef data) const
		{
			return m_ptrArray.get(pos, [&](__DataConstRef t_data) {
				data = t_data;
			});
		}

		bool set(size_t pos, __DataConstRef data)
		{
            return get(pos, [&](__DataRef t_data) {
				t_data = data;
			});
		}

		template<typename T>
		void addFront(const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				itrReverseVisit(container, [&](__DataConstRef data) {
					m_data.push_front(data);
					m_ptrArray.addFront(m_data.front());
				});
			}
		}

		void addFront(__InitList initList)
		{
			addFront<__InitList>(initList);
		}

		template<typename... args>
		void addFront(__DataConstRef data, const args&... others)
		{
			(void)tagDynamicArgsExtractor<const __DataType>::extractReverse([&](__DataConstRef data) {
				m_data.push_front(data);
				m_ptrArray.addFront(m_data.front());
				return true;
			}, data, others...);
		}

		__DataRef addFront(__DataConstRef data)
		{
			m_data.push_front(data);
			m_ptrArray.addFront(m_data.front());
			return m_data.frnt();
		}

		bool popBack()
		{
			if (__Super::popBack())
			{
				(void)m_ptrArray.popBack();
				return true;
			}

			return false;
		}

		bool popBack(__CB_Ref_void cb)
		{
			if (__Super::popBack(cb))
			{
				(void)m_ptrArray.popBack();
				return true;
			}

			return false;
		}

		bool popBack(__DataRef data)
		{
			if (__Super::popBack(data))
			{
				(void)m_ptrArray.popBack();
				return true;
			}

			return false;
		}

		void qsort(__CB_Sort_T<__DataType> cb)
		{
			m_ptrArray.qsort([&](__DataType& lhs, __DataType& rhs) {
				if (cb(lhs, rhs))
				{
					std::swap(lhs, rhs);
				}

				return false;
			});
		}

		ArrListT& qsort()
		{
			tagTrySort<__DataType> trySort;
			return qsort([&](__DataConstRef lhs, __DataConstRef rhs) {
				return trySort(lhs, rhs);
			});
		}

		void Reverse()
		{
			reverse(m_data.begin(), m_data.end());
			m_ptrArray.Reverse();
		}

		ArrListT slice(int startPos, int endPos=-1) const
		{
			ArrListT lst;

			auto ptrArray = m_ptrArray.slice(startPos, endPos);
			ptrArray([&](__DataConstRef data) {
				lst.add(data);
			});

			return lst;
		}

	protected:
		void _swap(__ContainerType& container) override
		{
			if (&container != &m_data)
			{
				m_data.swap(container);
				m_ptrArray.assign(m_data);
			}
		}

		void _add(__DataConstRef data) override
		{
			m_data.push_back(data);
			m_ptrArray.add(m_data.back());
		}

		bool _popFront(__CB_Ref_void cb) override
		{
			if (__Super::_popFront(cb))
			{
				(void)m_ptrArray.popFront();
				return true;
			}

			return false;
		}

		size_t _del(__DataConstRef data, CB_Del cb) override
		{
			size_t uRet = 0;

			for (auto itr = m_data.begin(); itr != m_data.end(); )
			{
				if (!tagTryCompare<__DataConstRef>::compare(*itr, data))
				{
					++itr;
					continue;
				}

				E_DelConfirm eRet = cb(*itr);
				if (E_DelConfirm::DC_Abort == eRet)
				{
					break;
				}
				else if (E_DelConfirm::DC_No == eRet)
				{
					++itr;
					continue;
				}
				else
				{
					itr = __Super::erase(itr);
					uRet++;

					if (E_DelConfirm::DC_YesAbort == eRet)
					{
						break;
					}
				}
			}

			return uRet;
		}

	private:
		template <class __ContainerType>
		class CAdaptor
		{
		public:
			CAdaptor(__ContainerType& data, const PtrArray<__DataType>& ptrArray)
				: m_data(data)
				, m_ptrArray(ptrArray)
			{
			}

		private:
			__ContainerType& m_data;

			const PtrArray<__DataType>& m_ptrArray;

			using __RefType = decltype(m_data.front())&;

		public:
			template <typename CB, typename = checkCBBool_t<CB, __RefType, size_t>>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				m_ptrArray(startPos, endPos, [&](__RefType ref, size_t pos) {
					return cb(ref, pos);
				});
			}

			template <typename CB, typename = checkCBVoid_t<CB, __RefType, size_t>, typename = void>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				m_ptrArray(startPos, endPos, [&](__RefType ref, size_t pos) {
					cb(ref, pos);
				});
			}

			template <typename CB, typename = checkCBBool_t<CB, __RefType>, typename = void, typename = void>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				m_ptrArray(startPos, endPos, [&](__RefType ref) {
					return cb(ref);
				});
			}

			template <typename CB, typename = checkCBVoid_t<CB, __RefType>, typename = void, typename = void, typename = void>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				m_ptrArray(startPos, endPos, [&](__RefType ref) {
					cb(ref);
				});
			}
		};
#define __Adaptor CAdaptor<__ContainerType>
#define __CAdaptor CAdaptor<const __ContainerType>

		__Adaptor m_adaptor = __Adaptor(m_data, m_ptrArray);
		inline __Adaptor& adaptor()
		{
			return m_adaptor;
		}
		__CAdaptor m_cadaptor = __CAdaptor(m_data, m_ptrArray);
		inline __CAdaptor& adaptor() const
		{
			return (__CAdaptor&)m_cadaptor;
		}
	};
}
