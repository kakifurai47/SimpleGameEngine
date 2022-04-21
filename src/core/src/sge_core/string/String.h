#pragma once

#include "../base/sge_base.h"
#include "Fmt.h"

namespace sge {


	
	inline
	std::ostream& operator<<(std::ostream& os, const StrView& sv) {
		os.write(sv.data(), sv.size());
		return os;
	}
} // namespace

template<>
struct fmt::formatter<sge::StrView>
{
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const sge::StrView& v, fmt::format_context& ctx)
	{
		auto it = *ctx.out();
		for (const auto& c : v) {
			it = c;
			it++;
		}
		return ctx.out();
	}
};

template<>
struct fmt::formatter<sge::StrViewW> {
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const sge::StrViewW& v, fmt::format_context& ctx) 
	{
		auto it = *ctx.out();
		for (const auto& c : v) {
			it = c;
			it++;
		}
		return ctx.out();
	}
};

template<>
struct fmt::formatter<sge::String> {
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const sge::String& v, fmt::format_context& ctx) 
	{
		sge::StrView view(v.data(), v.size());
		return fmt::format_to(ctx.out(), "{}", view);
	}
};

template<>
struct fmt::formatter<sge::StringW> {
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const sge::StringW& v, fmt::format_context& ctx)
	{
		sge::StrViewW view(v.data(), v.size());
		return fmt::format_to(ctx.out(), "{}", view);
	}
};