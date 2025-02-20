#include <cat/bit>
#include <cat/memory>

// `tree-loop-distribute-patterns` is an optimization that replaces this code
// with a call to `memcpy`. As this function is called within `memcpy`, that
// produces an infinite loop.
[[gnu::optimize("-fno-tree-loop-distribute-patterns"),
  clang::no_builtin("memcpy")]]
void
cat::copy_memory_small(void const* p_source, void* p_destination, uword bytes) {
   unsigned char const* p_source_handle =
      static_cast<unsigned char const*>(p_source);
   unsigned char* p_destination_handle =
      static_cast<unsigned char*>(p_destination);

   for (uword::raw_type i = 0u; i < bytes; ++i) {
      p_destination_handle[i] = p_source_handle[i];
   }
}
