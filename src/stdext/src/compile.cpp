#pragma once

#include <stdext/range.h>
#include <list>
#include <vector>
#include <stdext/message_handler.h>

static constexpr bool has_begin(...)
{
	return false;
}

template <class data_structure,
	typename iterator = data_structure::iterator,
	typename method = decltype(std::declval<data_structure>().begin()),
	typename = std::enable_if<std::is_constructible<iterator, method>::value>
>
static constexpr bool has_begin(data_structure &) { return true; }

static constexpr bool has_begin_const(...) { return false; }

template <class data_structure,
	typename iterator = data_structure::const_iterator,
	typename method = decltype(std::declval<data_structure>().begin()),
	typename = std::enable_if<std::is_constructible<iterator, method>::value>
>
static constexpr bool has_begin_const(const data_structure &) { return true; }

static void test_iterator()
{
	std::vector<int> tmp;
	stdext::range<typename std::vector<int>::iterator> range(tmp.begin(), tmp.end());
	stdext::range<typename std::vector<int>::iterator> range_ = stdext::make_range(tmp);
	auto titi = stdext::make_range((const std::vector<int> &)tmp);
	range[0] = 10;
	auto it = range.begin();
	auto it2 = range.rbegin();

	//static_assert(test(tmp), "bAM");

	//class Toto 
	//{
	//public:
	//	using iterator = void*;

	//	iterator begin() { return nullptr; }
	//	iterator end() { return nullptr; }
	//};
	//Toto pif;

	//stdext::make_range(pif);

	//auto flap = stdext::make_range(pif);

	//static_assert(stdext::has_begin<Toto>(), "bam!");
	//static_assert(stdext::has_begin<std::vector<int>>(), "bam!");

	const std::list<int> tmp2;
	stdext::range<typename std::list<int>::const_iterator> range2(tmp2.begin(), tmp2.end());
	stdext::range<typename std::list<int>::const_iterator> range2_ = stdext::make_range(tmp2);
	auto it3 = range2.begin();
	auto it4 = range2.rend();
}

static void test_message_handler()
{
	stdext::message_handler msg;
	stdext::message_handler::receiver_queue<int> * r = msg.createReceiver<int>(msg.spyTarget());
	msg.broadcast(10);
	r->post(2);
	stdext::message_handler::receiver_callback<int> * callback = msg.createReceiver<int>([](int a) {}, msg.spyTarget());

	r->post(1);
	callback->post(2);
}