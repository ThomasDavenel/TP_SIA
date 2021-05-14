#ifndef _meta_type_list_H
#define _meta_type_list_H

#include <type_traits>


namespace stdext
{
	namespace internal
	{

		/// <summary>
		/// General declaration of the template struct get_type. index is the index of the type in the arguments, Args... are the type.
		/// </summary>
		template <size_t index, typename... Args>
		struct get_type
		{};


		/// <summary>
		/// Specialization of the get_type structure for index==0. This is the type we were looking for...
		/// </summary>
		template <typename T, typename... Args>
		struct get_type<0, T, Args...>
		{
			/// <summary>
			/// This is the type we were looking for...
			/// </summary>
			typedef T type;
		};


		/// <summary>
		/// Declaration of get_type used when index>0 i.e. we are still looking for the good type...
		/// </summary>
		template <size_t index, typename T, typename... Args>
		struct get_type<index, T, Args...>
		{
			/// <summary>
			/// We delegate the type definition to the other instantiation of get_type
			/// </summary>
			typedef typename get_type<index - 1, Args...>::type type;
		};
	}

	/// <summary>
	/// A type list class. 
	/// </summary>
	template <typename... Args>
	struct type_list
	{
	public:
		/// <summary>
		/// The number of types in the type list
		/// </summary>
		static constexpr size_t size() { return sizeof... (Args); }

		/// <summary>
		/// We get a type based on its index in the list.
		/// </summary>
		template <size_t index>
		using get = typename internal::get_type<index, Args...>::type;

	private:

		/// <summary>
		/// General definition of the internal_tail structure. index is the index of the first element of the sublist, Args... is the type list
		/// </summary>
		template <size_t index, typename... Args>
		struct internal_tail
		{};


		/// <summary>
		/// Specialization of internal_tail for index==0, this is the start of the sub list we were looking for.
		/// </summary>
		template <typename... Args>
		struct internal_tail<0, Args...>
		{
			typedef type_list<Args...> type;
		};


		/// <summary>
		/// General case of the definition of the tail structure. Used until index==0.
		/// </summary>
		template <size_t index, typename T, typename... Args>
		struct internal_tail<index, T, Args...>
		{
			typedef typename internal_tail<index - 1, Args...>::type type;
		};

		template <typename head, typename... Args>
		static type_list<Args...> internal_pop_front(type_list<head, Args...> const &);

	public:
		/// <summary>
		/// Extraction the tail of the list based on the index of the first element in the sub list.
		/// </summary>
		template <size_t index>
		using tail = typename internal_tail<index, Args...>::type;

		/// <summary>
		/// Instantiate a template type by providing the types in the list as template arguments
		/// </summary>
		template <template <typename...> class T>
		using instantiate = T<Args...>;

		/// <summary>
		/// Returns the list plus a new back element
		/// </summary>
		template <typename... T>
		using push_back = type_list<Args..., T...>;

		/// <summary>
		/// Returns the list plus a new front element
		/// </summary>
		template <typename... T>
		using push_front = type_list<T..., Args...>;

		/// <summary>
		/// Returns the list minus the front element
		/// </summary>
		using pop_front = decltype(internal_pop_front(::std::declval<type_list<Args...>>()));

	private:
		template <size_t length, typename... Args>
		struct internal_keep_front
		{};

		template <typename head, typename... Args>
		struct internal_keep_front<0, head, Args...>
		{
			typedef type_list<> type;
		};

		template <typename head, typename... Args>
		struct internal_keep_front<1, head, Args...>
		{
			typedef type_list<head> type;
		};

		template <size_t length, typename head, typename... Args>
		struct internal_keep_front<length, head, Args...>
		{
			typedef typename internal_keep_front<length - 1, Args...>::type::template push_front<head> type;
		};

	public:
		/// <summary>
		/// Returns the list minus the last element
		/// </summary>
		using pop_back = typename internal_keep_front<sizeof...(Args) - 1, Args...>::type;

		/// <summary>
		/// Returns the first length elements of the type list
		/// </summary>
		template <size_t length>
		using extract_first = typename internal_keep_front<length, Args...>::type;

	private:
		template <size_t length, typename... Args>
		struct internal_remove_first
		{};

		template <typename... Args>
		struct internal_remove_first<0, Args...>
		{
			typedef type_list<Args...> type;
		};

		template <size_t length, typename head, typename... Args>
		struct internal_remove_first<length, head, Args...>
		{
			typedef typename internal_remove_first<length - 1, Args...>::type type;
		};

	public:
		/// <summary>
		/// Removes the first length elements of the type list
		/// </summary>
		template <size_t length>
		using remove_first = typename internal_remove_first<length, Args...>::type;

		/// <summary>
		/// Extracts a sub list from the element start with a length of length
		/// </summary>
		template <size_t start, size_t length>
		using sub_list = typename type_list<Args...>::remove_first<start>::template extract_first<length>;

	private:

		template <template <typename> typename type_modifier, typename... Args>
		struct internal_modifier
		{
		};

		template <template <typename> typename type_modifier, typename T>
		struct internal_modifier<type_modifier, T>
		{
			typedef type_list<typename type_modifier<T>::type > type;
		};

		template <template <typename> typename type_modifier, typename T, typename... Args>
		struct internal_modifier<type_modifier, T, Args...>
		{
			typedef typename internal_modifier<type_modifier, Args...>::type super_list;
			typedef typename super_list::template push_front<typename type_modifier<T>::type> type;
		};

	public:
		/// <summary>
		/// Adds a modifier to all types in the list. This modifier (STL like, see std::add_const for instance) 
		/// must be generic of one type and define an internal type named 'type' to be applicable.
		/// </summary>
		template <template <typename> typename modifier>
		using add_modifier = typename internal_modifier<modifier, Args...>::type;
	};

	namespace internal
	{
		/// <summary>
		/// Extracts the types.
		/// </summary>
		/// <param name="">The .</param>
		/// <returns></returns>
		template <typename... Args, template <typename...> typename T>
		type_list<Args...> extract_types(const T<Args...> &)
		{
			return type_list<Args...>();
		}
	}

	/// <summary>
	/// Extract the template parameter list of an instantiated template type
	/// </summary>
	template <typename T>
	struct template_parameter_list
	{
		typedef decltype(internal::extract_types(::std::declval<T>())) type;
	};

	namespace internal
	{
		template <typename... Args1, typename... Args2>
		type_list<Args1..., Args2...> concat(type_list<Args1...> const &, type_list<Args2...> const &)
		{
			return type_list<Args1..., Args2...>();
		}
	}

	/// <summary>
	/// Tests if T is a type_list
	/// </summary>
	template <typename T>
	struct is_type_list : ::std::false_type {};

	/// <summary>
	/// Specialization of is_type_list for type_list.
	/// </summary>
	template <typename... Args>
	struct is_type_list<type_list<Args...>> : ::std::true_type {};

	/// <summary>
	/// Concatenation of two type_list (L1 and L2). The resulting type_list is accessible through the internal type 'type'
	/// </summary>
	template <class L1, class L2>
	struct concat
	{
		static_assert(is_type_list<L1>::value, "meta::concat: only type_list can be concatenated");
		static_assert(is_type_list<L2>::value, "meta::concat: only type_list can be concatenated");

		/// <summary>
		/// The concatenation of the two type_list L1 and L2
		/// </summary>
		typedef decltype(internal::concat(::std::declval<L1>(), ::std::declval<L2>())) type;
	};

	namespace internal
	{
		//template <typename ReturnType>
		//Rennes1::Ext::meta::type_list<ReturnType, void> deduce_type(ReturnType());

		template <typename ReturnType, typename... ParameterType>
		type_list<ReturnType, ParameterType...> deduce_type(ReturnType(ParameterType...));

		template <typename ClassType, typename ReturnType, typename... ParameterType>
		type_list<ReturnType, ParameterType...> deduce_type(ReturnType(ClassType::*) (ParameterType...));

		template <typename ClassType, typename ReturnType, typename... ParameterType>
		type_list<ReturnType, ParameterType...> deduce_type(ReturnType(ClassType::*) (ParameterType...) const);

		template <typename structure>
		decltype(deduce_type(&structure::operator())) deduce_type(structure);
	}

	/// <summary>
	/// Extracts the types associated with a function (return type, types of parameters)
	/// </summary>
	template <class Function>
	struct function_types
	{
		/// <summary>
		/// a type_list containing the return type and the types of the function parameters
		/// </summary>
		using types = decltype(internal::deduce_type(::std::declval<Function>()));
		/// <summary>
		/// The return type of the function
		/// </summary>
		using return_type = typename types::template get<0>;
		/// <summary>
		/// a type_list containing the parameters types.
		/// </summary>
		using parameters_type = typename types::pop_front;
	};
}
#endif