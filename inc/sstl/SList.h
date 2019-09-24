
#pragma once

namespace NS_SSTL
{
#undef __Super__
#define __Super__ SContainerT<std::list<__DataType>>

	template<typename __DataType>
	class SListT : public __Super__
	{
	protected:
		__UsingSuper(__Super__)

		typedef decltype(declval<__ContainerType&>().rbegin()) __RItrType;
		typedef decltype(declval<const __ContainerType&>().rbegin()) __CRItrType;

	public:
		SListT() = default;

		template<typename... args>
		explicit SListT(__DataConstRef data, const args&... others)
			: __Super(data, others...)
		{
		}

		explicit SListT(__ContainerType&& container)
		{
			__Super::swap(container);
		}

		SListT(SListT&& lst)
		{
			__Super::swap(lst);
		}

		SListT(const SListT& lst)
			: __Super(lst)
		{
		}

		explicit SListT(__InitList initList)
			: __Super(initList)
		{
		}

		template<typename T, typename = checkContainerData_t<T, __DataType>>
		explicit SListT(const T& container)
			: __Super(container)
		{
		}

		SListT& operator=(__ContainerType&& container)
		{
			__Super::swap(container);
			return *this;
		}

		SListT& operator=(SListT&& lst)
		{
			__Super::swap(lst);
			return *this;
		}

		SListT& operator=(const SListT& lst)
		{
			__Super::assign(lst);
			return *this;
		}

		SListT& operator=(__InitList initList)
		{
			__Super::assign(initList);
			return *this;
		}

		template <typename T>
		SListT& operator=(const T&t)
		{
			__Super::assign(t);
			return *this;
		}

	public:
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

		int indexOf(__DataConstRef data) const
		{
			int nIdx = 0;
			for (auto& item : m_data)
			{
				if (tagTryCompare<__DataType>::compare(item, data))
				{
					return nIdx;
				}
				nIdx++;
			}

			return -1;
		}

		int lastIndexOf(__DataConstRef data) const
		{
			int nIdx = m_data.size() - 1;
			auto itrEnd = m_data.rend();
			for (auto itr = m_data.rbegin(); itr != itrEnd; ++itr, nIdx--)
			{
				if (tagTryCompare<const __DataType>::compare(*itr, data))
				{
					return nIdx;
				}
			}

			return -1;
		}

		__DataRef add(__DataConstRef data)
		{
			_add(data);
			return m_data.back();
		}

		template<typename... args>
		void add(__DataConstRef data, const args&... others)
		{
			__Super::add(data, others...);
		}

		void add(__InitList initList)
		{
			__Super::add(initList);
		}

		template<typename T, typename = checkContainer_t<T>>
		void add(const T& container)
		{
			__Super::add(container);
		}

		template<typename T>
		void addFront(const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				m_data.insert(m_data.begin(), container.begin(), container.end());
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
				return true;
			}, data, others...);
		}

		__DataRef addFront(__DataConstRef data)
		{
			m_data.push_front(data);
			return m_data.frnt();
		}

		bool popBack()
		{
			if (m_data.empty())
			{
				return false;
			}

			m_data.pop_back();

			return true;
		}

		bool popBack(__CB_Ref_void cb)
		{
			if (m_data.empty())
			{
				return false;
			}
			
			cb(m_data.back());

			m_data.pop_back();

			return true;
		}

		bool popBack(__DataRef data)
		{
			if (m_data.empty())
			{
				return false;
			}

			data = m_data.back();
			m_data.pop_back();

			return true;
		}
		
	public:
		void sort()
		{
			m_data.sort(tagTrySort<__DataType>());
		}

		void sort(__CB_Sort_T<__DataType> cb)
		{
			m_data.sort(tagSort<__DataType>(cb));
		}

		void Reverse()
		{
			reverse(m_data.begin(), m_data.end());
		}

	public:
		template <typename T>
		SListT<T> map(CB_T_Ret<__DataConstRef, T> cb) const
		{
			SListT<T> lst;

			for (auto&data : m_data)
			{
				lst.add(cb(data));
			}

			return lst;
		}

		template <typename CB, typename RET = decltype(declval<CB>()(__DataType()))>
		SListT<RET> map(const CB& cb) const
		{
			return map<RET>(cb);
		}

		void sum(SMap<__DataType, size_t>& mapItemSum) const
		{
			for (auto&data : m_data)
			{
				mapItemSum[data]++;
			}
		}

		void sum(SMap<__DataType, size_t>& mapItemSum, SMap<size_t, SListT>& mapSumItem) const
		{
			sum(mapItemSum);

			for (auto& pr : mapItemSum)
			{
				mapSumItem[pr.second].add(pr.first);
			}
		}

		void sum(SMap<size_t, SListT>& mapSumItem) const
		{
			SMap<__DataType, size_t> mapItemSum;
			sum(mapItemSum, mapSumItem);
		}

	protected:
		void _add(__DataConstRef data) override
		{
			m_data.push_back(data);
		}

		bool _popFront(__CB_Ref_void cb) override
		{
			if (m_data.empty())
			{
				return false;
			}

			if (cb)
			{
				cb(m_data.front());
			}

			m_data.pop_front();

			return true;
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

		bool _includes(__DataConstRef data) const override
		{
			for (auto& t_data : m_data)
			{
				if (tagTryCompare<__DataConstRef>::compare(t_data, data))
				{
					return true;
				}
			}

			return false;
		}

	private:
		template <class T = __ContainerType>
		class CAdaptor
		{
		public:
			CAdaptor(T& data)
				: m_data(data)
			{
			}

		private:
			T& m_data;

		public:
			template <typename CB, typename = checkCBBool_t<CB, __DataRef, size_t>>
			void forEach(const CB& cb)
			{
				size_t idx = 0;
				for (auto& data : m_data)
				{
					if (!cb(data, idx++))
					{
						break;
					}
				}
			}
		};

		CAdaptor<> m_adaptor = CAdaptor<>(m_data);
		inline CAdaptor<>& adaptor()
		{
			return m_adaptor;
		}
		inline CAdaptor<const __ContainerType>& adaptor() const
		{
			return (CAdaptor<const __ContainerType>&)m_adaptor;
		}
	};
}
