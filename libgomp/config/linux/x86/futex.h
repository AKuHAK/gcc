/* Copyright (C) 2005-2021 Free Software Foundation, Inc.
   Contributed by Richard Henderson <rth@redhat.com>.

   This file is part of the GNU Offloading and Multi Processing Library
   (libgomp).

   Libgomp is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   Libgomp is distributed in the hope that it will be useful, but WITHOUT ANY
   WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
   FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
   more details.

   Under Section 7 of GPL version 3, you are granted additional
   permissions described in the GCC Runtime Library Exception, version
   3.1, as published by the Free Software Foundation.

   You should have received a copy of the GNU General Public License and
   a copy of the GCC Runtime Library Exception along with this program;
   see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
   <http://www.gnu.org/licenses/>.  */

/* Provide target-specific access to the futex system call.  */

#ifdef __x86_64__
# ifndef SYS_futex
#  define SYS_futex	202
# endif

static inline void
futex_wait (int *addr, int val)
{
  long res;

  register long r10 __asm__("%r10") = 0;
  __asm volatile ("syscall"
		  : "=a" (res)
		  : "0" (SYS_futex), "D" (addr), "S" (gomp_futex_wait),
		    "d" (val), "r" (r10)
		  : "r11", "rcx", "memory");
  if (__builtin_expect (res == -ENOSYS, 0))
    {
      gomp_futex_wait &= ~FUTEX_PRIVATE_FLAG;
      gomp_futex_wake &= ~FUTEX_PRIVATE_FLAG;
      __asm volatile ("syscall"
		      : "=a" (res)
		      : "0" (SYS_futex), "D" (addr), "S" (gomp_futex_wait),
			"d" (val), "r" (r10)
		      : "r11", "rcx", "memory");
    }
}

static inline void
futex_wake (int *addr, int count)
{
  long res;

  __asm volatile ("syscall"
		  : "=a" (res)
		  : "0" (SYS_futex), "D" (addr), "S" (gomp_futex_wake),
		    "d" (count)
		  : "r11", "rcx", "memory");
  if (__builtin_expect (res == -ENOSYS, 0))
    {
      gomp_futex_wait &= ~FUTEX_PRIVATE_FLAG;
      gomp_futex_wake &= ~FUTEX_PRIVATE_FLAG;
      __asm volatile ("syscall"
		      : "=a" (res)
		      : "0" (SYS_futex), "D" (addr), "S" (gomp_futex_wake),
			"d" (count)
		      : "r11", "rcx", "memory");
    }
}
#else
# ifndef SYS_futex
#  define SYS_futex	240
# endif

static inline void
futex_wait (int *addr, int val)
{
  long res;

  __asm volatile ("int $0x80"
		  : "=a" (res)
		  : "0" (SYS_futex), "b" (addr), "c" (gomp_futex_wait),
		    "d" (val), "S" (0)
		  : "memory");
  if (__builtin_expect (res == -ENOSYS, 0))
    {
      gomp_futex_wait &= ~FUTEX_PRIVATE_FLAG;
      gomp_futex_wake &= ~FUTEX_PRIVATE_FLAG;
      __asm volatile ("int $0x80"
		      : "=a" (res)
		      : "0" (SYS_futex), "b" (addr), "c" (gomp_futex_wait),
		        "d" (val), "S" (0)
		      : "memory");
    }
}

static inline void
futex_wake (int *addr, int count)
{
  long res;

  __asm volatile ("int $0x80"
		  : "=a" (res)
		  : "0" (SYS_futex), "b" (addr), "c" (gomp_futex_wake),
		    "d" (count)
		  : "memory");
  if (__builtin_expect (res == -ENOSYS, 0))
    {
      gomp_futex_wait &= ~FUTEX_PRIVATE_FLAG;
      gomp_futex_wake &= ~FUTEX_PRIVATE_FLAG;
      __asm volatile ("int $0x80"
		      : "=a" (res)
		      : "0" (SYS_futex), "b" (addr), "c" (gomp_futex_wake),
		        "d" (count)
		      : "memory");
    }
}
#endif /* __x86_64__ */

static inline void
cpu_relax (void)
{
  __builtin_ia32_pause ();
}
