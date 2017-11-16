#ifndef SPSC_QUEUE_HPP
#define SPSC_QUEUE_HPP


#include <cstddef>
#include <cstdint> // uint32_t
#include <stdexcept>


/* optimizations:
 * not only wait-free, but even synchronization-free: no memory barriers were used
 * size must be power of 2 because of internal divisions
 * buffer is located between indeces to avoid false data sharing between threads
 */


#if defined(__GNUC__) || defined(__GNUG__) // clang too
#  define SPSC_FORCEINLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#  define SPSC_FORCEINLINE __forceinline
#else
#  define SPSC_FORCEINLINE 
#endif

// wait-free fixed-size Single Producer Single Consumer queue
// reader thread can only call pop() and writer thread can only call push()
template <class T, T ReservedElement, std::size_t RingSize>
class spsc_queue
{
	// assignment must be atomic!
	static_assert(sizeof(T) == sizeof(void*) || sizeof(T) == sizeof(int), // todo: better check
		"this queue is for dumb elements only");

	// for performance reasons
	static_assert((RingSize & (RingSize - 1)) == 0, "RingSize should be power of 2!");

public:
	spsc_queue()
		: _writer_index(0),
		_reader_index(0)
	{
		for (T& x : _buffer)
			x = ReservedElement;
	}

	// too expensive
	spsc_queue(const spsc_queue&) = delete;
	spsc_queue(spsc_queue&&) = delete;

	SPSC_FORCEINLINE bool push(T obj)
	{
		if (obj == ReservedElement)
			throw std::logic_error("trying to push reserved element!");

		if (_buffer[_writer_index] == ReservedElement)
		{
			_buffer[_writer_index] = obj;

			++_writer_index;

			_writer_index %= RingSize;

			return true;
		}
		else
		{
			return false;
		}
	}

	SPSC_FORCEINLINE bool pop(T& x)
	{
		if (_buffer[_reader_index] != ReservedElement)
		{
			x = _buffer[_reader_index];

			_buffer[_reader_index] = ReservedElement;

			++_reader_index;

			_reader_index %= RingSize;

			return true;
		}
		else
		{
			return false;
		}
	}

private:
	unsigned int _writer_index; // must only be accesed by writer thread
	T _buffer[RingSize];
	unsigned int _reader_index; // must only be accesed by reader thread
};

#endif // SPSC_QUEUE_HPP
