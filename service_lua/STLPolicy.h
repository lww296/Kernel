#include "Types.h"
#ifdef _OPEN_LUA_MODULE_
#ifndef __STL_POLICY_H__
#define __STL_POLICY_H__

#include "LuaCppModule.h"

#include <luabind/config.hpp>
#include <luabind/detail/policy.hpp>
#include <vector>
#include <list>
#include <map>

using namespace std;

//lua table to stl

// vector
namespace luabind
{
	template <class T>
	struct default_converter<vector<T> >
		: native_converter_base<vector<T> >
	{
		static int compute_score(lua_State* L, int index)
		{
			return (lua_type(L, index) == LUA_TTABLE )? 0 : -1;
		}

		vector<T>  from(lua_State* L, int index)
		{
			vector<T> container; 
			luabind::object tbl(from_stack(L, index)); 

			for (luabind::iterator itr(tbl), end; itr != end; ++itr) 
			{ 
				boost::optional<T> v = object_cast_nothrow<T>(*itr); 
				if (v){ 
					container.push_back(*v); 
				} 
			} 

			return container; 
		}

		void to(lua_State* L, vector<T>  const& container)
		{
			lua_createtable(L, container.size(), 0); 

			luabind::object tbl(from_stack(L, -1)); 
			int n = 0; 

			typename std::vector<T>::const_iterator cit = container.begin();
			for ( ; cit != container.end(); ++cit) 
			{ 
				tbl[++n] = *cit; 
			}
		}
	};

	template <class T>
	struct default_converter<vector<T>  const&>
		: default_converter<vector<T> >
	{};
}

// list
namespace luabind
{
	template <class T>
	struct default_converter<list<T> >
		: native_converter_base<list<T> >
	{
		static int compute_score(lua_State* L, int index)
		{
			return (lua_type(L, index) == LUA_TTABLE )? 0 : -1;
		}

		list<T>  from(lua_State* L, int index)
		{
			list<T> container; 
			luabind::object tbl(from_stack(L, index)); 

			for (luabind::iterator itr(tbl), end; itr != end; ++itr) 
			{ 
				boost::optional<T> v = object_cast_nothrow<T>(*itr); 
				if (v){ 
					container.push_back(*v); 
				} 
			} 

			return container; 
		}

		void to(lua_State* L, list<T>  const& container)
		{
			lua_createtable(L, container.size(), 0); 

			luabind::object tbl(from_stack(L, -1)); 
			int n = 0; 

			typename std::list<T>::const_iterator itr = container.begin();
			for ( ; itr != container.end(); ++itr) 
			{ 
				tbl[++n] = *itr; 
			}
		}
	};

	template <class T>
	struct default_converter<list<T>  const&>
		: default_converter<list<T> >
	{};
}

// map
namespace luabind
{
	template <class K, class V>
	struct default_converter<map<K, V> >
		: native_converter_base<map<K, V> >
	{
		static int compute_score(lua_State* L, int index)
		{
			return (lua_type(L, index) == LUA_TTABLE )? 0 : -1;
		}

		map<K, V>  from(lua_State* L, int index)
		{
			map<K, V> container; 
			luabind::object tbl(from_stack(L, index)); 

			for (luabind::iterator itr(tbl), end; itr != end; ++itr)
			{
				typedef typename map<K, V>::key_type KeyT;
				typedef typename map<K, V>::mapped_type ValueT;
				boost::optional<KeyT> k = object_cast_nothrow<KeyT>(itr.key());

				if (k)
				{
					boost::optional<ValueT> v = object_cast_nothrow<ValueT>(*itr);
					if (v)
					{
						container.insert(std::make_pair(*k, *v));
					}
				}
			}

			return container; 
		}

		void to(lua_State* L, map<K, V>  const& container)
		{
			lua_createtable(L, container.size(), 0); 

			luabind::object tbl(from_stack(L, -1)); 
			
			typename map<K, V>::const_iterator itr = container.begin();
			for ( ; itr != container.end(); ++itr)
			{
				tbl[itr->first] = itr->second;
			}
		}
	};

	template <class K, class V>
	struct default_converter<map<K, V>  const&>
		: default_converter<map<K, V> >
	{};
}

// don't use
namespace luabind
{ 
	namespace detail {

		struct table_convert
		{
			template<class ContainerT>
			ContainerT apply(lua_State *L, by_value<ContainerT> data, int index)
			{
				luabind::object tbl(from_stack(L, index));
				ContainerT result;

				for (luabind::iterator itr(tbl), end; itr != end; ++itr)
				{
					boost::optional<typename ContainerT::value_type>
						v = object_cast_nothrow<typename ContainerT::value_type>(*itr);

					if (v)
					{
						result.push_back(*v);
					}
				}

				return result;
			}

			template<class ContainerT>
			ContainerT apply(lua_State *L, by_const_reference<ContainerT> data, int index)
			{
				return apply(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			ContainerT apply(lua_State *L, by_reference<ContainerT> data, int index)
			{
				return apply(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			static int match(lua_State* L, by_value<ContainerT>, int index)
			{
				if (lua_type(L, index) == LUA_TTABLE)
				{
					return 0;
				}

				return -1;
			}

			template<class ContainerT>
			static int match(lua_State* L, by_const_reference<ContainerT>, int index)
			{
				return match(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			static int match(lua_State* L, by_reference<ContainerT>, int index)
			{
				return match(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			void apply(lua_State *L, const ContainerT &container)
			{
				lua_createtable(L, container.size(), 0);

				luabind::object tbl(from_stack(L, -1));
				int n = 0;

				for (typename ContainerT::const_iterator itr = container.begin(); itr != container.end(); ++itr)
				{
					tbl[++n] = *itr;
				}
			}

			template<class T>
			void converter_postcall(lua_State*, T, int) {}
		};

		struct table_convert_map
		{
			template<class ContainerT>
			ContainerT apply(lua_State *L, by_value<ContainerT> data, int index)
			{
				luabind::object tbl(from_stack(L, index));
				ContainerT result;

				for (luabind::iterator itr(tbl), end; itr != end; ++itr)
				{
					typedef typename ContainerT::key_type KeyT;
					typedef typename ContainerT::mapped_type ValueT;
					boost::optional<KeyT> k = object_cast_nothrow<KeyT>(itr.key());

					if (k)
					{
						boost::optional<ValueT> v = object_cast_nothrow<ValueT>(*itr);
						if (v)
						{
							result.insert(std::make_pair(*k, *v));
						}
					}
				}

				return result;
			}

			template<class ContainerT>
			ContainerT apply(lua_State *L, by_const_reference<ContainerT> data, int index)
			{
				return apply(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			ContainerT apply(lua_State *L, by_reference<ContainerT> data, int index)
			{
				return apply(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			static int match(lua_State* L, by_value<ContainerT>, int index)
			{
				if (lua_type(L, index) == LUA_TTABLE)
				{
					return 0;
				}

				return -1;
			}

			template<class ContainerT>
			static int match(lua_State* L, by_const_reference<ContainerT>, int index)
			{
				return match(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			static int match(lua_State* L, by_reference<ContainerT>, int index)
			{
				return match(L, by_value<ContainerT>(), index);
			}

			template<class ContainerT>
			void apply(lua_State *L, const ContainerT &container)
			{
				lua_createtable(L, 0, container.size());

				luabind::object tbl(from_stack(L, -1));
				int n = 0;

				for (typename ContainerT::const_iterator itr = container.begin(); itr != container.end(); ++itr)
				{
					tbl[itr->first] = itr->second;
				}
			}


			template<class T>
			void converter_postcall(lua_State*, T, int) {}
		};

		template<int N, bool Assoc>
		struct table_policy : conversion_policy<N>
		{
			struct only_accepts_values_or_references {};

			static void precall(lua_State*, const index_map&) {}
			static void postcall(lua_State*, const index_map&) {}

			template<class T, class Direction>
			struct apply
			{
				static const bool IsPtr =
					luabind::detail::is_nonconst_pointer<T>::value ||
					luabind::detail::is_const_pointer<T>::value;

				typedef typename boost::mpl::if_c<
					IsPtr,
					only_accepts_values_or_references,
					typename boost::mpl::if_c<Assoc,
					table_convert_map, table_convert>::type
				>::type type;
			};
		};
	}
}

namespace luabind
{
	template<int N>
	detail::policy_cons<detail::table_policy<N, false>,
		detail::null_type> 
		copy_table(LUABIND_PLACEHOLDER_ARG(N)) 
	{ 
		return detail::policy_cons<detail::table_policy<N,
			false>, detail::null_type>(); 
	}

	template<int N>
	detail::policy_cons<detail::table_policy<N, true>,
		detail::null_type> 
		copy_table_map(LUABIND_PLACEHOLDER_ARG(N)) 
	{ 
		return detail::policy_cons<detail::table_policy<N,
			true>, detail::null_type>(); 
	}
}


#endif //__STL_POLICY_H__
#endif // _OPEN_LUA_MODULE_