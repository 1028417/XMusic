
#ifndef __SArray_H
#define __SArray_H

#include <algorithm>

namespace NS_SSTL
{
#undef __Super__
#define __Super__ SContainerT<__BaseType<__DataType>>

	template <typename __DataType, template<typename...> class __BaseType>
	class SArrayT : public __Super__
	{
	protected:
		__UsingSuper(__Super__);

		typedef decltype(declval<__ContainerType&>().rbegin()) __RItrType;
		typedef decltype(declval<const __ContainerType&>().rbegin()) __CRItrType;

	public:
		SArrayT() = default;

		template<typename... args>
		explicit SArrayT(__DataConstRef data, const args&... others)
			: __Super(data, others...)
		{
		}

		explicit SArrayT(__ContainerType&& container)
		{
			__Super::swap(container);
		}

		SArrayT(SArrayT&& arr)
		{
			__Super::swap(arr);
		}

		SArrayT(const SArrayT& arr)
			: __Super(arr)
		{
		}

		explicit SArrayT(__InitList initList)
			: __Super(initList)
		{
		}

		template<typename T, typename = checkContainerData_t<T, __DataType>>
		explicit SArrayT(const T& container)
			: __Super(container)
		{
		}

		SArrayT& operator=(__ContainerType&& container)
		{
			__Super::swap(container);
			return *this;
		}

		SArrayT& operator=(SArrayT&& arr)
		{
			__Super::swap(arr);
			return *this;
		}

		SArrayT& operator=(const SArrayT& arr)
		{
			__Super::assign(arr);
			return *this;
		}

		SArrayT& operator=(__InitList initList)
		{
			__Super::assign(initList);
			return *this;
		}
		
		template <typename T>
		SArrayT& operator=(const T&t)
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

		bool get(size_t pos, __CB_Ref_void cb)
		{
			return adaptor().get(pos, cb);
		}

		bool get(size_t pos, __CB_ConstRef_void cb) const
		{
			return adaptor().get(pos, cb);
		}
		
		bool get(size_t pos, __DataRef& data) const
		{
			return adaptor().get(pos, [&](__DataConstRef& t_data) {
				data = t_data;
			});
		}

		bool set(size_t pos, __DataConstRef& data)
		{
			return get(pos, [&](__DataRef t_data) {
				t_data = data;
			});
		}

		bool del_pos(size_t pos)
		{
			if (pos >= m_data.size())
			{
				return false;
			}

			m_data.erase(m_data.begin() + pos);

			return true;
		}

		bool del_pos(list<size_t> lstPos)
		{
                        for (int nIdx = m_data.size() - 1; nIdx >= 0; nIdx--)
			{
				auto itr = std::find(lstPos.begin(), lstPos.end(), (size_t)nIdx);
				if (itr != lstPos.end())
				{
					m_data.erase(m_data.begin() + nIdx);

					(void)lstPos.erase(itr);

					if (lstPos.empty())
					{
						break;
					}
				}
			}

			return true;
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
			int nIdx = m_data.size()-1;
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

		int find(__CB_ConstRef_bool cb, size_t startPos = 0) const
		{
			int nRetPos = -1;

			(*this)([&](__DataConstRef data, size_t pos) {
				if (cb(data))
				{
					nRetPos = pos;
					return false;
				}

				return true;
			});

			return nRetPos;
		}

		// TODO insert(uPos)

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
				m_data.insert(m_data.begin(), data);
				return true;
			}, data, others...);
		}

		__DataRef addFront(__DataConstRef data)
		{
			m_data.insert(m_data.begin(), data);
			return m_data.front();
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

		void qsort()
		{
			size_t size = m_data.size();
			if (size > 1)
			{
				NS_SSTL::qsort<__DataType>(&m_data.front(), size);
			}
		}

		void qsort(__CB_Sort_T<__DataType> cb)
		{
			size_t size = m_data.size();
			if (size > 1)
			{
				NS_SSTL::qsort<__DataType>(&m_data.front(), size, cb);
			}
		}

		void Reverse()
		{
			reverse(m_data.begin(), m_data.end());
		}

	public:
		template<typename... args>
		void splice(size_t pos, size_t nRemove, __DataConstRef data, const args&... others)
		{
			vector<__DataType> vecData;
			extractDataTypeArgs(vecData, data, others...);
			splice(pos, nRemove, vecData);
		}

		template<typename T>
		void splice(size_t pos, size_t nRemove, const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				auto itr = m_data.end();
				if (pos < m_data.size())
				{
					itr = m_data.begin() + pos;

					if (0 != nRemove)
					{
						itr = m_data.erase(itr, itr + (nRemove - 1));
					}
				}

				m_data.insert(itr, container.begin(), container.end());
			}
		}

		void splice(size_t pos, size_t nRemove, __InitList initList)
		{
            splice(pos, nRemove, initList);
		}

		void splice(size_t pos, size_t nRemove)
		{
			return splice(pos, nRemove, __InitList());
		}

		SArrayT slice(int startPos, int endPos=-1) const
		{
			SArrayT arr;
			
			(*this)(startPos, endPos, [&](__DataConstRef data) {
				arr.add(data);
			});
			
			return arr;
		}

		template <typename T>
		SArrayT<T, __BaseType> map(CB_T_Ret<__DataConstRef, T> cb) const
		{
			SArrayT<T, __BaseType> arr;

			for (auto& data : m_data)
			{
				arr.add(cb(data));
			}

			return arr;
		}

		template <typename CB, typename RET = decltype(declval<CB>()(__DataType()))>
		SArrayT<RET, __BaseType> map(const CB& cb) const
		{
			return map<RET>(cb);
		}

		void sum(SMap<__DataType, size_t>& mapItemSum) const
		{
			for (auto& data : m_data)
			{
				mapItemSum[data]++;
			}
		}

		void sum(SMap<__DataType, size_t>& mapItemSum, SMap<size_t, SArrayT>& mapSumItem) const
		{
			sum(mapItemSum);

			for (auto& pr : mapItemSum)
			{
				mapSumItem[pr.second].add(pr.first);
			}
		}

		void sum(SMap<size_t, SArrayT>& mapSumItem) const
		{
			SMap<__DataType, size_t> mapItemSum;
			sum(mapItemSum, mapSumItem);
		}

	private:
		void _add(__DataConstRef data) override
		{
			m_data.push_back(data);
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
					itr = m_data.erase(itr);
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

			using __RefType = decltype(declval<T&>()[0])&;

		private:
			int _transPos(int pos) const
			{
				auto size = m_data.size();
				if (pos < 0)
				{
					pos = (int)size + pos;
					if (pos < 0)
					{
						pos = -1;
					}
				}
				else
				{
					if (pos >= (int)size)
					{
						pos = -1;
					}
				}
			
				return pos;
			}
			
		public:
			template <typename CB>
			bool get(size_t pos, CB cb)
			{
				if (pos >= m_data.size())
				{
					return false;
				}

				cb(m_data[pos]);

				return true;
			}

			template <typename CB, typename = checkCBBool_t<CB, __RefType, size_t>>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				_forEach(cb, startPos, endPos);
			}

			template <typename CB, typename = checkCBVoid_t<CB, __RefType, size_t>, typename = void>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				_forEach([&](__RefType data, size_t pos) {
					cb(data, pos);
					return true;
				}, startPos, endPos);
			}

			template <typename CB, typename = checkCBVoid_t<CB, __RefType>, typename = void, typename = void>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				_forEach([&](__RefType data, size_t) {
					cb(data);
					return true;
				}, startPos, endPos);
			}

			template <typename CB, typename = checkCBBool_t<CB, __RefType>, typename = void, typename = void, typename = void>
			void forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				_forEach([&](__RefType data, size_t) {
					return cb(data);
				}, startPos, endPos);
			}

		private:
			template <typename CB>
			void _forEach(const CB& cb, int startPos = 0, int endPos = -1)
			{
				int pos = _transPos(startPos);
				if (pos < 0)
				{
					return;
				}

				int targetPos = _transPos(endPos);
				if (targetPos < 0)
				{
					return;
				}

				while (cb(m_data[pos], (size_t)pos))
				{
					if (pos == targetPos)
					{
						break;
					}
					else if (pos < targetPos)
					{
						pos++;
					}
					else
					{
						pos--;
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

#endif // __SArray_H
