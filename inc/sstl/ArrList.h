
#ifndef __ArrList_H
#define __ArrList_H

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
		void clear() override
		{
			m_ptrArray.clear();
			m_data.clear();
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
			int nRetPos = -1;

            (*this)(startPos, [&](__DataConstRef data, size_t pos) {
				if (cb(data))
				{
					nRetPos = pos;
					return false;
				}

				return true;
			});

			return nRetPos;
		}

		bool get(size_t pos, __CB_Ref_void cb)
		{
			return m_ptrArray.get(pos, cb);
		}

		bool get(size_t pos, __CB_ConstRef_void cb) const
		{
			return m_ptrArray.get(pos, cb);
		}

		bool get(size_t pos, __DataRef& data) const
		{
			return m_ptrArray.get(pos, [&](__DataConstRef& t_data) {
				data = t_data;
			});
		}

		bool set(size_t pos, __DataConstRef& data)
		{
            return get(pos, [&](__DataRef m_data) {
				m_data = data;
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
			(void)m_ptrArray.popBack();
			return __Super::popBack();
		}

		bool popBack(__CB_Ref_void cb)
		{
			(void)m_ptrArray.popBack();
			return __Super::popBack(cb);
		}

		bool popBack(__DataRef data)
		{
			(void)m_ptrArray.popBack();
			return __Super::popBack(data);
		}

		ArrListT& qsort()
		{
			tagTrySort<__DataType> trySort;
			return qsort([&](__DataType& lhs, __DataType& rhs) {
				return trySort(lhs, rhs);
			});
		}

		void qsort(__CB_Sort_T<__DataType> cb)
		{
			//__Super::sort(cb);
			//m_ptrArray.assign(m_data);

			m_ptrArray.qsort([&](__DataType& lhs, __DataType& rhs) {
				if (cb(lhs, rhs))
				{
					std::swap(lhs, rhs);
					return true;
				}

				return false;
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

		void _onErase(const __CItrType& itr) override
		{
			m_ptrArray.del(&*itr);
		}

		void _add(__DataConstRef data) override
		{
			m_data.push_back(data);
			m_ptrArray.add(m_data.back());
		}

		bool _popFront(__CB_Ref_void cb) override
		{
			(void)m_ptrArray.popFront();
			return __Super::_popFront(cb);
		}

		size_t _del(__DataConstRef data, CB_Del cb) override
		{
			size_t uRet = 0;

			auto itrPtr = m_ptrArray.begin();
			for (auto itr = m_data.begin(); itr != m_data.end() && itrPtr != m_ptrArray.end(); )
			{
				if (!tagTryCompare<__DataConstRef>::compare(*itr, data))
				{
					++itr;
					++itrPtr;
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
					++itrPtr;
					continue;
				}
				else
				{
					itr = __Super::erase(itr);
					itrPtr = m_ptrArray.erase(itrPtr);
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
        template <typename T = __DataType>
		class CAdaptor
		{
		public:
            CAdaptor(const PtrArray<T>& ptrArray)
				: m_ptrArray(ptrArray)
			{
			}

		private:
            const PtrArray<T>& m_ptrArray;

		public:
			template <typename CB>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1) const
			{
				m_ptrArray(startPos, endPos, cb);
			}
		};

        CAdaptor<> m_adaptor = CAdaptor<>(m_ptrArray);
        CAdaptor<>& adaptor()
		{
			return m_adaptor;
		}
		CAdaptor<const __DataType>& adaptor() const
		{
			return (CAdaptor<const __DataType>&)m_adaptor;
		}
	};
}

#endif //__ArrList_H
