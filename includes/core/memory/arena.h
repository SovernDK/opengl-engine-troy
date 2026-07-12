#pragma once
#include <cstddef>
#include <new>
#include <memory>
#include <type_traits>

namespace mem
{
	class Arena
	{
	private:
		struct DestructNode
		{
			void (*dtor)(void*);
			DestructNode* prev;
			void* obj;
		};

		char* m_buffer;
		size_t m_capacity;
		size_t m_offset;

		DestructNode* m_tail = nullptr;
	public:
		explicit Arena(std::size_t size) :
			m_buffer(static_cast<char*>(::operator new(size))), m_capacity(size), m_offset(0) {}

		Arena(const Arena&) = delete;
		Arena(Arena&&) = delete;

		~Arena()
		{
			callDestructors();
			::operator delete(m_buffer);
		}

		template<typename T, typename... Args>
		T* create(Args&&... args)
		{
			auto [newPtr, newOffset] = allocate(m_offset, sizeof(T), alignof(T));

			if constexpr (std::is_trivially_destructible_v<T>)
			{
				T* obj = new (newPtr) T(std::forward<Args>(args)...);
				m_offset = newOffset;
				return obj;
			}
			else
			{
				auto [dnode_ptr, final_offset] = allocate(newOffset,
					sizeof(DestructNode), alignof(DestructNode));

				T* obj = new (newPtr) T(std::forward<Args>(args)...);

				auto dtor_call = [](void* p)
				{
					static_cast<T*>(p)->~T();
				};
				DestructNode* new_node = new (dnode_ptr) DestructNode{ dtor_call,
															  m_tail, obj };
				m_tail = new_node;

				m_offset = final_offset;
				return obj;
			}
		}

		void reset()
		{
			callDestructors();
			m_offset = 0;
		}

		size_t inUse() const
		{
			return m_offset;
		}

		size_t left() const
		{
			return m_capacity - m_offset;
		}

		size_t capacity() const
		{
			return m_capacity;
		}
	private:
		void callDestructors()
		{
			while (m_tail)
			{
				m_tail->dtor(m_tail->obj);
				m_tail = m_tail->prev;
			}
		}

		std::pair<void*, std::size_t> allocate(std::size_t curr_offset,
			std::size_t size, std::size_t alignment)
		{
			char* current_ptr = m_buffer + curr_offset;
			std::size_t space = m_capacity - curr_offset;
			void* aligned_ptr = current_ptr;

			if (std::align(alignment, size, aligned_ptr, space) == nullptr)
			{
				throw std::bad_alloc();
			}

			auto new_offset = static_cast<char*>(aligned_ptr) - m_buffer + size;

			return { aligned_ptr, new_offset };
		}
	};
}