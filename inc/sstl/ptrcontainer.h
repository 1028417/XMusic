
#pragma once

namespace NS_SSTL
{
	template <template<typename...> class __BaseType, class __PtrType>
	class ptrcontainerT : public __BaseType<__PtrType>
	{
	private:
		using __Super = __BaseType<__PtrType>;

	protected:
		using __Type = typename remove_reference<decltype(*(__PtrType)NULL)>::type;
		using __RefType = __Type&;
		
		using __ConstRef = const __Type&;
		using __ConstPtr = const __Type*;

		using __InitList = InitList_T<__PtrType>;

	public:
		ptrcontainerT() = default;

		template<class _Iter, typename = checkIter_t<_Iter>>
		explicit ptrcontainerT(_Iter _First, _Iter _Last)
		{
			for (auto itr = _First; itr != _Last; ++itr)
			{
				_add(-1, *itr);
			}
		}

		explicit ptrcontainerT(__Super&& container)
		{
			__Super::swap(container);
		}

		ptrcontainerT(ptrcontainerT&& container)
		{
			__Super::swap(container);
		}

		ptrcontainerT(const ptrcontainerT& container)
			: __Super(container.begin(), container.end())
		{
		}

		explicit ptrcontainerT(__InitList initLst)
		{
			_addContainer(-1, initLst);
		}

		template <typename T, typename = checkContainer_t<T>>
		explicit ptrcontainerT(const T& container)
		{
			_addContainer(-1, container);
		}

		template <typename T, typename = checkContainer_t<T>>
		explicit ptrcontainerT(T& container)
		{
			_addContainer(-1, container);
		}
		explicit ptrcontainerT(__PtrType ptr)
		{
			_add(-1, ptr);
		}

		explicit ptrcontainerT(__RefType ref)
		{
			_add(-1, ref);
		}

		template <typename T>
		explicit ptrcontainerT(T* ptr)
		{
			_add(-1, ptr);
		}

		ptrcontainerT& operator = (const ptrcontainerT& container)
		{
			__Super::assign(container.begin(), container.end());
			return *this;
		}

	private:
		bool _add(int nPos, __PtrType ptr)
		{
			if (NULL == ptr)
			{
				return false;
			}

			return _add(nPos, *ptr);
		}

		bool _add(int nPos, __RefType ref)
		{
			if (nPos < 0 || nPos >= (int)__Super::size())
			{
				__Super::push_back(&ref);
			}
			else
			{
				__Super::insert(__Super::begin() + nPos, &ref);
			}

			return true;
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline bool _add(int nPos, T* ptr, ...)
		{
			return _add(nPos, dynamic_cast<__PtrType>(ptr));
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline bool _add(int nPos, T& ref, ...)
		{
			return _add(nPos, &ref);
		}

		template <typename T>
		size_t _addContainer(int nPos, T& container)
		{
			size_t uRet = 0;

			if (nPos < 0 || nPos >= (int)__Super::size())
			{
				for (auto&data : container)
				{
					if (_add(-1, data))
					{
						uRet++;
					}
				}
			}
			else
			{
				auto itrBegin = container.begin();
				auto itr = container.end();
				while (itr != itrBegin)
				{
					--itr;

					if (_add(nPos, *itr))
					{
						uRet++;
					}
				}
			}

			return uRet;
		}

		bool _set(size_t pos, __PtrType ptr)
		{
			if (NULL == ptr)
			{
				return false;
			}

			if (pos >= __Super::size())
			{
				return false;
			}

			__Super::at(pos) = ptr;

			return true;
		}

		inline bool _set(size_t pos, __RefType ref)
		{
			return _set(pos, &ref);
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline bool _set(size_t pos, T* ptr, ...)
		{
			return _set(pos, dynamic_cast<__PtrType>(ptr));
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline bool _set(size_t pos, T& ref, ...)
		{
			return _set(pos, &ref);
		}                                                                     

		size_t _del(__ConstPtr ptr, bool bDelOnlyOne=false)
		{
			if (NULL == ptr)
			{
				return 0;
			}

			size_t uRet = 0;

			for (auto itr = __Super::begin(); itr != __Super::end(); )
			{
				if (*itr == ptr)
				{
					uRet++;

					itr = __Super::erase(itr);

					if (bDelOnlyOne)
					{
						break;
					}
				}
				else
				{
					++itr;
				}
			}

			return uRet;
		}

		inline size_t _del(__ConstRef ref, bool bDelOnlyOne = false)
		{
			return _del(&ref, bDelOnlyOne);
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline size_t _del(T* ptr, ...)
		{
			return _del((__ConstPtr)dynamic_cast<__ConstPtr>(ptr));
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline size_t _del(T& ref, ...)
		{
			return _del(&ref);
		}

		template <typename T>
		size_t _delContainer(T& container)
		{
			size_t uRet = 0;

			for (auto&data : container)
			{
				uRet += _del(data);
			}

			return uRet;
		}

		int _indexOf(__ConstPtr ptr) const
		{
			if (NULL == ptr)
			{
				return -1;
			}

			auto itr = __Super::begin();
			for (int pos = 0; itr != __Super::end(); ++itr, pos++)
			{
				if (*itr == ptr)
				{
					return pos;
				}
			}

			return -1;
		}

		inline int _indexOf(__ConstRef ref) const
		{
			return _indexOf(&ref);
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline int _indexOf(T* ptr, ...) const
		{
			return _indexOf((__ConstPtr)dynamic_cast<__ConstPtr>(ptr));
		}

		template <typename T, typename = checkClass_t<T, __Type>>
		inline int _indexOf(T& ref, ...) const
		{
			return _indexOf(&ref);
		}

	public:
		bool add(__PtrType ptr, int nPos = -1)
		{
			return _add(nPos, ptr);
		}

		bool add(__RefType ref, int nPos = -1)
		{
			return _add(nPos, ref);
		}

		template<typename T>
		bool add(T* ptr, int nPos = -1)
		{
			return _add(nPos, ptr);
		}

		template<typename T, typename = checkNotContainer_t<T>>
		bool add(T& ref, int nPos = -1)
		{
			return _add(nPos, ref);
		}

		size_t add(__InitList initLst, int nPos = -1)
		{
			return _addContainer(nPos, initLst);
		}

		template <typename T, typename = checkContainer_t<T>>
		size_t add(const T& container, int nPos = -1)
		{
			return _addContainer(nPos, container);
		}

		template <typename T, typename = checkContainer_t<T>>
		size_t add(T& container, int nPos = -1)
		{
			return _addContainer(nPos, container);
		}

		bool set(size_t pos, __PtrType ptr)
		{
			return _set(pos, ptr);
		}

		bool set(size_t pos, __RefType ref)
		{
			return _set(pos, ref);
		}

		template<typename T>
		bool set(size_t pos, T* ptr)
		{
			return _set(pos, ptr);
		}

		template<typename T>
		bool set(size_t pos, T& ref)
		{
			return _set(pos, ref);
		}
		
		bool del_one(__ConstPtr ptr)
		{
			return 1 == _del(ptr, true);
		}

		bool del_one(__ConstRef ref)
		{
			return 1 == _del(ref, true);
		}

		size_t del(__ConstPtr ptr, bool bDelOnlyOne = true)
		{
			return _del(ptr, bDelOnlyOne);
		}

		size_t del(__ConstRef ref, bool bDelOnlyOne = true)
		{
			return _del(ref, bDelOnlyOne);
		}

		template <typename T>
		size_t del(T* ptr, bool bDelOnlyOne = true)
		{
			return _del(ptr, bDelOnlyOne);
		}

		size_t del(__InitList initLst)
		{
			return _delContainer(initLst);
		}

		template <typename T, typename = checkContainer_t<T>>
		size_t del(const T& container)
		{
			return _delContainer(container);
		}

		template <typename T, typename = checkContainer_t<T>>
		size_t del(T& container)
		{
			return _delContainer(container);
		}

		int indexOf(__ConstPtr ptr) const
		{
			return _indexOf(ptr);
		}

		int indexOf(__ConstRef ref) const
		{
			return _indexOf(ref);
		}

		template <typename T>
		int indexOf(T* ptr) const
		{
			return _indexOf(ptr);
		}

		template <typename T>
		int indexOf(T& ref) const
		{
			return _indexOf(ref);
		}
	};
};
