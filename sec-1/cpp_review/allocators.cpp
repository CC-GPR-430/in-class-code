#include <cstdlib>
#include <new>
#include <limits>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

template<class T>
struct Mallocator
{
    // Class taken from https://en.cppreference.com/w/cpp/named_req/Allocator
    typedef T value_type;

    Mallocator () = default;

    template<class U>
    constexpr Mallocator (const Mallocator <U>&) noexcept {}

    [[nodiscard]] T* allocate(std::size_t n)
    {
        if (n > std::numeric_limits<std::size_t>::max() / sizeof(T))
            throw std::bad_array_new_length();


        if (auto p = static_cast<T*>(std::malloc(n * sizeof(T))))
        {
            //report(p, n);
            return p;
        }

        throw std::bad_alloc();
    }

    void deallocate(T* p, std::size_t n) noexcept
    {
        //report(p, n, 0);
        std::free(p);
    }
private:
    void report(T* p, std::size_t n, bool alloc = true) const
    {
        std::cout << (alloc ? "Alloc: " : "Dealloc: ") << sizeof(T) * n
                  << " bytes at " << std::hex << std::showbase
                  << reinterpret_cast<void*>(p) << std::dec << '\n';
    }
};

template<class T, class U>
bool operator==(const Mallocator <T>&, const Mallocator <U>&) { return true; }

template<class T, class U>
bool operator!=(const Mallocator <T>&, const Mallocator <U>&) { return false; }


static std::unordered_map<void *, std::size_t, std::hash<void *>, std::equal_to<void *>, Mallocator<std::pair<const void *, std::size_t>>> allocs;

void * operator new(std::size_t n) throw(std::bad_alloc)
{
  void *p = std::malloc(n);
  if (p == nullptr)
      throw std::bad_alloc();
  allocs[p] = n;
  std::cout << "  Alloc: " << n << " bytes at " << std::hex << std::showbase << p << std::dec << '\n';
  return p;
}
void operator delete(void * p) throw()
{
  std::size_t n = allocs.at(p);
  std::cout << "Dealloc: " << n << " bytes at " << std::hex << std::showbase << p << std::dec << '\n';
  std::free(p);
  allocs.erase(p);
}


static class Reporter
{
public:
  ~Reporter()
{
  bool hasLeak = false;
  std::cout << "==== Begin Leak Report ====\n";
  for (auto & p : allocs)
    {
      hasLeak = true;
      std::cout << "Leak: " << p.second << " bytes at " << std::hex << std::showbase << p.first << '\n';
    }
  if (!hasLeak)
    std::cout << "✨ No leaks detected ✨\n";
}
} __reporter__;