
#pragma once

namespace NS_SSTL
{
#define __SMapSuper SContainerT<__BaseType<__KeyType, __ValueType>, __KeyType>

	template<typename __KeyType, typename __ValueType, template<typename...> class __BaseType>
	class SMapT : public __SMapSuper
	{
	private:
		__UsingSuper(__SMapSuper)

		__UsingSuperType(__KeyConstRef)
		__UsingSuperType(__InitList_Key)

		using __ValueRef = __ValueType&;
		using __ValueConstRef = const __ValueType&;

		using __PairConstRef = __DataConstRef;

		const bool m_bMulti = is_same<__ContainerType, std::multimap<__KeyType, __ValueType>>::value
			|| is_same<__ContainerType, std::unordered_multimap<__KeyType, __ValueType>>::value;

	public:
		virtual ~SMapT() {} // ½â¾öqt-mac clang¸æ¾¯

		SMapT() = default;

		template <typename T>
		explicit SMapT(const T& keys, const function<__ValueType(__KeyConstRef)>& cb)
		{
			set(keys, cb);
		}

		explicit SMapT(__InitList_Key keys, const function<__ValueType(__KeyConstRef)>& cb)
		{
			set(keys, cb);
		}

		explicit SMapT(__ContainerType&& container)
		{
			__Super::swap(container);
		}

		SMapT(SMapT&& map)
		{
			__Super::swap(map);
		}

		SMapT(const SMapT& map)
			: __Super(map)
		{
		}

		explicit SMapT(__InitList initList)
			: __Super(initList)
		{
		}

		template<typename T, typename = checkContainer_t<T>>
		explicit SMapT(const T& container)
			: __Super(container)
		{
		}

		//template<typename T, typename = checkContainer_t<T>>
		//explicit SMapT(T& container)
		//	: __Super(container)
		//{
		//}

		SMapT& operator=(__ContainerType&& container)
		{
			__Super::swap(container);
			return *this;
		}

		SMapT& operator=(SMapT&& map)
		{
			__Super::swap(map);
			return *this;
		}

		SMapT& operator=(const SMapT& map)
		{
			__Super::assign(map);
			return *this;
		}

		SMapT& operator=(__InitList initList)
		{
			__Super::assign(initList);
			return *this;
		}

		template<typename T>
		SMapT& operator=(const T&t)
		{
			__Super::assign(t);
			return *this;
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
		bool get(__KeyConstRef key, __ValueRef value) const
		{
			return _find(key, [&](__PairConstRef pr) {
				value = pr.second;
				return false;
			});
		}

		template <typename CB, typename = checkCBVoid_t<CB, __ValueRef>>
		size_t get(__KeyConstRef key, const CB& cb)
		{
			return _find(key, [&](__ItrType& itr) {
				cb(itr->second);

				return true;
			});
		}

		template <typename CB, typename = checkCBBool_t<CB, __ValueRef>, typename = void>
		size_t get(__KeyConstRef key, const CB& cb)
		{
			return _find(key, [&](__ItrType& itr) {
				return cb(itr->second);
			});
		}

		template <typename CB, typename = checkCBVoid_t<CB, __ValueRef>>
		bool get(__KeyConstRef key, const CB& cb) const
		{
			return _find(key, [&](__PairConstRef pr) {
				cb(pr.second);

				return true;
			});
		}

		template <typename CB, typename = checkCBBool_t<CB, __ValueRef>, typename = void>
		bool get(__KeyConstRef key, const CB& cb) const
		{
			return _find(key, [&](__PairConstRef pr) {
				return cb(pr.second);
			});
		}

		__ValueType& insert(__KeyConstRef key, __ValueConstRef value= __ValueType())
		{
			return _insert(key, value);
		}

		__ValueType& set(__KeyConstRef key, __ValueConstRef value)
		{
			auto itr = m_data.find(key);
			if (itr != m_data.end())
			{
				return itr->second = value;
			}
			else
			{
				return _insert(key, value);
			}
		}
		
		template<typename T>
		void set(const T& container)
		{
			if (!__Super::checkIsSelf(container))
			{
				m_data.insert(container.begin(), container.end());
			}
		}

		void set(__InitList initList)
		{
			set<__InitList>(initList);
		}

		void set(__InitList_Key keys, const function<__ValueType(__KeyConstRef)>& cb)
		{
			set<__InitList_Key>(keys, cb);
		}

		template <typename T>
		void set(const T& container, const function<__ValueType(__KeyConstRef)>& cb)
		{
			for (auto& key : container)
			{
				this->set(key, cb(key));
			}
		}

	public:
		template <typename CB>
		SArray<__KeyType> keys(const CB& cb) const
		{
			return adaptor().keys(cb);
		}

		SArray<__KeyType> keys() const
		{
			return adaptor().keys([&](__KeyConstRef) {
				return true;
			});
		}

		template <typename CB>
		SArray<__ValueType> values(const CB& cb) const
		{
			return adaptor().values(cb);
		}

		SArray<__ValueType> values() const
		{
			return adaptor().values([&](__ValueConstRef) {
				return true;
			});
		}

		template <typename CB, typename RET = SMapT<cbRet_t<CB, __KeyConstRef>, __ValueType, __BaseType> >
		RET mapKey(const CB& cb) const
		{
			RET map;

			for (auto& pr : m_data)
			{
				map.set(cb(pr.first), pr.second);
			}

			return map;
		}

		template <typename CB, typename RET = SMapT<__KeyType, cbRet_t<CB, __ValueConstRef>, __BaseType> >
		RET mapValue(const CB& cb) const
		{
			RET map;

			for (auto& pr : m_data)
			{
				map.set(pr.first, cb(pr.second));
			}

			return map;
		}

		template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef, __ValueConstRef> >
		SMapT filter(const CB& cb) const
		{
			SMapT map;

			for (auto& pr : m_data)
			{
				if (cb(pr.first, pr.second))
				{
					map.set(pr.first, pr.second);
				}
			}

			return map;
		}

		template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef> >
		SMapT filterKey(const CB& cb) const
		{
			SMapT map;

			for (auto& pr : m_data)
			{
				if (cb(pr.first))
				{
					map.set(pr.first, pr.second);
				}
			}

			return map;
		}

		template <typename CB, typename = checkCBBool_t<CB, __ValueConstRef> >
		SMapT filterValue(const CB& cb) const
		{
			SMapT map;

			for (auto& pr : m_data)
			{
				if (cb(pr.second))
				{
					map.set(pr.first, pr.second);
				}
			}

			return map;
		}

		template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef> >
		bool everyKey(const CB& cb) const
		{
			for (auto& pr : m_data)
			{
				if (!cb(pr.first))
				{
					return false;
				}
			}

			return true;
		}

		template <typename CB, typename = checkCBBool_t<CB, __ValueConstRef> >
		bool everyValue(const CB& cb) const
		{
			for (auto& pr : m_data)
			{
				if (!cb(pr.second))
				{
					return false;
				}
			}

			return true;
		}

		template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef> >
		bool anyKey(const CB& cb) const
		{
			for (auto& pr : m_data)
			{
				if (cb(pr.first))
				{
					return true;
				}
			}

			return false;
		}

		template <typename CB, typename = checkCBBool_t<CB, __ValueConstRef> >
		bool anyValue(const CB& cb) const
		{
			for (auto& pr : m_data)
			{
				if (cb(pr.second))
				{
					return true;
				}
			}

			return false;
		}

	private:
		void _add(__PairConstRef pr) override
		{
			m_data.insert(pr);
		}

		size_t _del(__KeyConstRef key, CB_Del cb) override
        {
            auto itr = m_data.find(key);
            if (itr == m_data.end())
            {
                return 0;
            }

            size_t uRet = 0;
            do
            {
                E_DelConfirm eRet = cb(*itr);
                if (E_DelConfirm::DC_Abort == eRet)
                {
                    break;
                }
                else if (E_DelConfirm::DC_No == eRet)
                {
                    ++itr;
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
            } while (m_bMulti && itr != m_data.end() && itr->first == key);

            return uRet;
		}

		bool _includes(__KeyConstRef key) const override
		{
			return m_data.find(key) != m_data.end();
		}

		void _toString(stringstream& ss, __PairConstRef pr) const override
		{
			tagSSTryLMove(ss) << '<' << pr.first << ", " << pr.second << '>';
		}

	private:
		size_t _find(__KeyConstRef key, CB_T_Ret<__ItrType&, bool> cb)
		{
			auto itr = m_data.find(key);
			if (itr == m_data.end())
			{
				return 0;
			}

			size_t uRet = 0;
			do
			{
				uRet++;

				auto lpValue = &itr->second;
				if (!cb(itr) || !m_bMulti)
				{
					break;
				}

				if (&itr->second == lpValue)
				{
					++itr;
				}
			} while (itr != m_data.end() && itr->first == key);

			return uRet;
		}

		size_t _find(__KeyConstRef key, CB_T_Ret<__PairConstRef, bool> cb) const
		{
			auto itr = m_data.find(key);
			if (itr == m_data.end())
			{
				return 0;
			}

			size_t uRet = 0;
			do
			{
				uRet++;

				if (!cb(*itr) || !m_bMulti)
				{
					break;
				}

				++itr;
			} while (itr != m_data.end() && itr->first == key);

			return uRet;
		}

		template <typename _V>
		auto _insert(__KeyConstRef key, const _V& value)->decltype(m_data.insert({ key, value }).first->second)&
		{
			return m_data.insert({ key, value }).first->second;
		}

		template <typename _V, typename = void>
		auto _insert(__KeyConstRef key, const _V& value)->decltype(m_data.insert({ key, value })->second)&
		{
			return m_data.insert({ key, value })->second;
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

			using __ValueRef = decltype(declval<T&>().begin()->second)&;

		public:
			template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef, __ValueRef>>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					if (!cb(pr.first, pr.second))
					{
						break;
					}
				}
			}

			template <typename CB, typename = checkCBVoid_t<CB, __KeyConstRef, __ValueRef>, typename = void>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					cb(pr.first, pr.second);
				}
			}

			template <typename CB, typename = checkCBVoid_t<CB, __ValueRef>, typename = void, typename = void>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					cb(pr.second);
				}
			}

			template <typename CB, typename = checkCBBool_t<CB, __ValueRef>, typename = void, typename = void, typename = void>
			void forEach(const CB& cb)
			{
				for (auto& pr : m_data)
				{
					if (!cb(pr.second))
					{
						break;
					}
				}
			}

			template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef, __ValueConstRef> >
			SArray<__KeyType> keys(const CB& cb) const
			{
				SArray<__KeyType> arr;
				for (auto& pr : m_data)
				{
					if (cb(pr.first, pr.second))
					{
						arr.add(pr.first);
					}
				}

				return arr;
			}

			template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef>, typename = void>
			SArray<__KeyType> keys(const CB& cb) const
			{
				return keys([&](__KeyConstRef key, __ValueConstRef) {
					return cb(key);
				});
			}

			template <typename CB, typename = checkCBBool_t<CB, __KeyConstRef, __ValueConstRef> >
			SArray<__ValueType> values(const CB& cb) const
			{
				SArray<__ValueType> arr;
				for (auto& pr : m_data)
				{
					if (cb(pr.first, pr.second))
					{
						arr.add(pr.second);
					}
				}

				return arr;
			}

			template <typename CB, typename = checkCBBool_t<CB, __ValueConstRef>, typename = void>
			SArray<__ValueType> values(const CB& cb) const
			{
				return values([&](__KeyConstRef, __ValueConstRef value) {
					return cb(value);
				});
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
