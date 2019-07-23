
#ifndef __Util_H
#define __Util_H

namespace NS_SSTL
{
	template <typename T>
	using decayType_t = typename decay<T>::type;

	template <typename T, typename U> struct decay_is_same
		: is_same<typename decay<T>::type, U>::type
	{
	};

	template <typename T> struct tagGetTypeT {
		typedef T type;
		typedef T& type_ref;
		typedef T* type_pointer;
	};

	template<typename T> struct tagGetTypeT<T&> {
		typedef typename remove_reference<T>::type type;
		typedef typename remove_reference<T>::type_ref type_ref;
		typedef typename remove_reference<T>::type_pointer type_pointer;
	};

	template<typename T> struct tagGetTypeT<T*> {
		typedef typename remove_reference<T>::type type;
		typedef typename remove_reference<T>::type_ref type_ref;
		typedef typename remove_reference<T>::type_pointer type_pointer;
	};

	template <typename T> struct tagTryCompare {
        static bool compare(const T& t1, const T& t2)
		{
            return &t1 == &t2 || _compare(&t1, &t2);
		}

		template <typename A, typename B>
        static bool _compare(const pair<A,B> *pr1, const pair<A, B> *pr2)
		{
            return tagTryCompare<A>::compare(pr1->first, pr2->first)
                && tagTryCompare<B>::compare(pr1->second, pr2->second);
		}

		template <typename U, typename RET = decltype(declval<const U&>() == declval<const U&>())>
        static RET _compare(const U *p1, const U *p2)
		{
            return *p1 == *p2;
		}

		static bool _compare(...)
		{
			return false;
		}
	};

	template <typename T> struct tagSort {
		tagSort(__CB_Sort_T<T> cb)
			: m_cb(cb)
		{
		}

		__CB_Sort_T<T> m_cb;

        bool operator() (const T&lhs, const T&rhs) const
		{
			return m_cb(lhs, rhs);
		}
	};

	template <typename T> struct tagTrySort {
		tagTrySort()
		{
		}

        bool operator() (const T&lhs, const T&rhs) const
		{
			return _compare(lhs, rhs);
		}

		template <typename U>
        static auto _compare(const U&lhs, const U&rhs) -> decltype(declval<const U>() < declval<const U>())
		{
			return lhs < rhs;
		}

		static bool _compare(...)
		{
			return false;
		}
	};

	template <typename T, typename U> struct tagTryLMove {
		static void lmove(T&t, const U&u)
		{
			_lmove(&t, &u);
		}

		template <typename X, typename Y>
		static auto _lmove(X*px, const Y*py) -> decltype(declval<X*>()->operator<<(declval<Y>()))
		{
			return *px << *py;
		}

		static bool _lmove(...)
		{
			return false;
		}

		enum { value = is_same<decltype(_lmove(declval<T*>(), declval<U*>())), T&>::value };
	};

	template <typename T, typename U = int>	struct tagLMove {
		tagLMove(T&t) : m_t(t)
		{
		}

		T& m_t;

		tagLMove& operator<<(const U& u)
		{
			tagTryLMove<T, U>::lmove(m_t, u);
			return *this;;
		}

		template <typename V>
		tagLMove& operator<<(const V&v)
		{
			tagLMove<T, V>(m_t) << v;
			return *this;
		}
	};

	using tagSSTryLMove = tagLMove<stringstream>;

	template<typename __DataType> class tagDynamicArgsExtractor
	{
	public:
		using FN_ExtractCB = const function<bool(__DataType&v)>&;

		template<typename... args>
		static bool extract(FN_ExtractCB cb, __DataType&v, args&... others)
		{
			return _extract(cb, true, v, others...);
		}

		template<typename... args>
		static bool extractReverse(FN_ExtractCB cb, __DataType&v, args&... others)
		{
			return _extract(cb, false, v, others...);
		}

		template<typename... args>
		static bool _extract(FN_ExtractCB cb, bool bForward, __DataType&v, args&... others)
		{
			if (bForward)
			{
				if (!_extract(cb, bForward, v))
				{
					return false;
				}
			}

			if (sizeof...(others))
			{
				if (!_extract(cb, bForward, others...))
				{
					return false;
				}
			}

			if (!bForward)
			{
				if (!_extract(cb, bForward, v))
				{
					return false;
				}
			}

			return true;
		}

		static bool _extract(FN_ExtractCB cb, bool bForward, __DataType&v)
		{
            (void)bForward;

			return cb(v);
		}
	};
	
	template <typename T, typename C>
	T reduce(const C& container, const function<T(const T& t1, const T& t2)>& cb)
	{
		auto itr = container.begin();
		if (itr == container.end())
		{
			return T();
		}

		auto itrPrev = itr;
		++itr;
		if (itr == container.end())
		{
			return *itrPrev;
		}

		T ret = cb(*itrPrev, *itr);
		while (true)
		{
			if (++itr == container.end())
			{
				break;
			}

			ret = cb(ret, *itr);
		}

		return ret;
	}

	template <typename C, typename CB, typename T
		= decltype(declval<CB>()(*(declval<C>().begin()), *(declval<C>().begin())))>
	T reduce(const C& container, const CB& cb)
	{
		return reduce<T, C>(container, cb);
	}

	template <typename T>
	void _qsort(T* lpData, size_t size, __CB_Sort_T<T> cbCompare)
	{
		if (size < 2)
		{
			return;
		}
		int end = (int)size - 1;

		function<void(int, int)> fnSort;
		fnSort = [&](int begin, int end) {
			if (begin >= end) {
				return;
			}

			int i = begin;
			int j = end;
			T t = lpData[begin];

			do {
				do {
					if (cbCompare(lpData[j], t)) {
						lpData[i] = lpData[j];
						i++;
						break;
					}
					j--;
				} while (i < j);

				while (i < j) {
					if (cbCompare(t, lpData[i])) {
						lpData[j] = lpData[i];
						j--;
						break;
					}
					i++;
				}
			} while (i < j);

			lpData[i] = t;

			fnSort(begin, i - 1);
			fnSort(i + 1, end);
		};
		
		fnSort(0, end);
	}

    template <typename T>
    void qsort(T* lpData, size_t size)
    {
        tagTrySort<T> sort;
        __CB_Sort_T<T> cbCompare = [&](const T& lhs, const T& rhs) {
            return sort(lhs, rhs);
        };

        _qsort(lpData, size, cbCompare);
    }

    template <typename T>
    void qsort(T* lpData, size_t size, __CB_Sort_T<T> cbCompare)
    {
        _qsort(lpData, size, cbCompare);
    }

	template <typename T>
	void qsort(vector<T>& vecData, __CB_Sort_T<T> cb)
	{
		size_t size = vecData.size();
		if (size > 1)
		{
			qsort<T>(&vecData.front(), size, cb);
		}
	}

	template<typename C, typename CB>
	void itrReverseVisit(C& container, const CB& cb)
	{
		auto itrBegin = container.begin();
		auto itr = container.end();
		while (itr != itrBegin)
		{
			--itr;
			cb(*itr);
		}
	}
}

#endif // __Util_H
