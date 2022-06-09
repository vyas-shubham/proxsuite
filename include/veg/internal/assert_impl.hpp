#ifndef VEG_ASSERT_HPP_VQDAJ2IBS
#define VEG_ASSERT_HPP_VQDAJ2IBS

#include "veg/internal/typedefs.hpp"
#include "veg/util/defer.hpp"
#include "veg/internal/dbg.hpp"
#include "veg/internal/prologue.hpp"
#include <cassert>

#define VEG_ASSERT(...) assert((__VA_ARGS__))

#define VEG_ASSERT_ALL_OF(...) assert(::veg::_detail::all_of({__VA_ARGS__}))

#define VEG_UNIMPLEMENTED()                                                    \
	VEG_ASSERT(false);                                                           \
	HEDLEY_UNREACHABLE()

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_ASSERT_HPP_VQDAJ2IBS */
