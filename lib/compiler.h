/*
 * Copyright (c) 2008, 2009, 2010, 2011, 2012, 2013 Nicira, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef COMPILER_H
#define COMPILER_H 1

#ifndef __has_feature
  #define __has_feature(x) 0
#endif

#if __GNUC__ && !__CHECKER__
#define NO_RETURN __attribute__((__noreturn__))
#define OVS_UNUSED __attribute__((__unused__))
#define PRINTF_FORMAT(FMT, ARG1) __attribute__((__format__(printf, FMT, ARG1)))
#define STRFTIME_FORMAT(FMT) __attribute__((__format__(__strftime__, FMT, 0)))
#define MALLOC_LIKE __attribute__((__malloc__))
#define ALWAYS_INLINE __attribute__((always_inline))
#define WARN_UNUSED_RESULT __attribute__((__warn_unused_result__))
#define SENTINEL(N) __attribute__((sentinel(N)))
#define OVS_LIKELY(CONDITION) __builtin_expect(!!(CONDITION), 1)
#define OVS_UNLIKELY(CONDITION) __builtin_expect(!!(CONDITION), 0)
#else
#define NO_RETURN
#define OVS_UNUSED
#define PRINTF_FORMAT(FMT, ARG1)
#define STRFTIME_FORMAT(FMT)
#define MALLOC_LIKE
#define ALWAYS_INLINE
#define WARN_UNUSED_RESULT
#define SENTINEL(N)
#define OVS_LIKELY(CONDITION) (!!(CONDITION))
#define OVS_UNLIKELY(CONDITION) (!!(CONDITION))
#endif

#if __has_feature(c_thread_safety_attributes)
/* "clang" annotations for thread safety check.
 *
 * OVS_LOCKABLE indicates that the struct contains mutex element
 * which can be locked by functions like ovs_mutex_lock().
 *
 * Below, the word MUTEX stands for the name of an object with an OVS_LOCKABLE
 * struct type.  It can also be a comma-separated list of multiple structs,
 * e.g. to require a function to hold multiple locks while invoked.
 *
 *
 * On a variable:
 *
 *    - OVS_GUARDED indicates that the variable may only be accessed some mutex
 *      is held.
 *
 *    - OVS_GUARDED_BY(MUTEX) indicates that the variable may only be accessed
 *      while the specific MUTEX is held.
 *
 *
 * On a function, the following attributes apply to mutexes:
 *
 *    - OVS_ACQUIRES(MUTEX) indicate that the function must be called without
 *      holding MUTEX and that it returns holding MUTEX.
 *
 *    - OVS_RELEASES(MUTEX) indicates that the function may only be called with
 *      MUTEX held and that it returns with MUTEX released.  It can be used for
 *      all types of MUTEX.
 *
 *    - OVS_TRY_LOCK(RETVAL, MUTEX) indicate that the function will try to
 *      acquire MUTEX.  RETVAL is an integer or boolean value specifying the
 *      return value of a successful lock acquisition.
 *
 *    - OVS_REQUIRES(MUTEX) indicate that the function may only be called with
 *      MUTEX held and that the function does not release MUTEX.
 *
 *    - OVS_LOCKS_EXCLUDED(MUTEX) indicates that the function may only be
 *      called when MUTEX is not held.
 *
 * The following variants, with the same syntax, apply to reader-writer locks:
 *
 *    mutex                rwlock, for reading  rwlock, for writing
 *    -------------------  -------------------  -------------------
 *    OVS_ACQUIRES         OVS_ACQ_RDLOCK       OVS_ACQ_WRLOCK
 *    OVS_RELEASES         OVS_RELEASES         OVS_RELEASES
 *    OVS_TRY_LOCK         OVS_TRY_RDLOCK       OVS_TRY_WRLOCK
 *    OVS_REQUIRES         OVS_REQ_RDLOCK       OVS_REQ_WRLOCK
 *    OVS_LOCKS_EXCLUDED   OVS_LOCKS_EXCLUDED   OVS_LOCKS_EXCLUDED
 */
#define OVS_LOCKABLE __attribute__((lockable))
#define OVS_REQ_RDLOCK(...) __attribute__((shared_locks_required(__VA_ARGS__)))
#define OVS_ACQ_RDLOCK(...) __attribute__((shared_lock_function(__VA_ARGS__)))
#define OVS_REQ_WRLOCK(...) \
    __attribute__((exclusive_locks_required(__VA_ARGS__)))
#define OVS_ACQ_WRLOCK(...) \
    __attribute__((exclusive_lock_function(__VA_ARGS__)))
#define OVS_REQUIRES(...) \
    __attribute__((exclusive_locks_required(__VA_ARGS__)))
#define OVS_ACQUIRES(...) \
    __attribute__((exclusive_lock_function(__VA_ARGS__)))
#define OVS_TRY_WRLOCK(RETVAL, ...)                              \
    __attribute__((exclusive_trylock_function(RETVAL, __VA_ARGS__)))
#define OVS_TRY_RDLOCK(RETVAL, ...)                          \
    __attribute__((shared_trylock_function(RETVAL, __VA_ARGS__)))
#define OVS_TRY_LOCK(RETVAL, ...)                                \
    __attribute__((exclusive_trylock_function(RETVAL, __VA_ARGS__)))
#define OVS_GUARDED __attribute__((guarded_var))
#define OVS_GUARDED_BY(...) __attribute__((guarded_by(__VA_ARGS__)))
#define OVS_RELEASES(...) __attribute__((unlock_function(__VA_ARGS__)))
#define OVS_LOCKS_EXCLUDED(...) __attribute__((locks_excluded(__VA_ARGS__)))
#elif __CHECKER__
/* "sparse" annotations for mutexes and mutex-like constructs.
 *
 * Change the thread-safety check annotations to use "context" attribute.
 *
 * OVS_MACRO_LOCK and OVS_MACRO_RELEASE are suitable for use within macros,
 * where there is no function prototype to annotate. */
#define OVS_LOCKABLE
#define OVS_REQ_RDLOCK(...) __attribute__((context(MUTEX, 1, 1)))
#define OVS_ACQ_RDLOCK(...) __attribute__((context(MUTEX, 0, 1)))
#define OVS_REQ_WRLOCK(...) __attribute__((context(MUTEX, 1, 1)))
#define OVS_ACQ_WRLOCK(...) __attribute__((context(MUTEX, 0, 1)))
#define OVS_REQUIRES(...)   __attribute__((context(MUTEX, 1, 1)))
#define OVS_ACQUIRES(...)   __attribute__((context(MUTEX, 0, 1)))
#define OVS_TRY_WRLOCK(RETVAL, ...)
#define OVS_TRY_RDLOCK(RETVAL, ...)
#define OVS_TRY_LOCK(REVAL, ...)
#define OVS_GUARDED
#define OVS_GUARDED_BY(...)
#define OVS_RELEASES(...)   __attribute__((context(MUTEX, 1, 0)))
#define OVS_MACRO_LOCK(...) __context__(MUTEX, 0, 1)
#define OVS_MACRO_RELEASE(...) __context__(MUTEX, 1, 0)
#else
#define OVS_LOCKABLE
#define OVS_REQ_RDLOCK(...)
#define OVS_ACQ_RDLOCK(...)
#define OVS_REQ_WRLOCK(...)
#define OVS_ACQ_WRLOCK(...)
#define OVS_REQUIRES(...)
#define OVS_ACQUIRES(...)
#define OVS_TRY_WRLOCK(...)
#define OVS_TRY_RDLOCK(...)
#define OVS_TRY_LOCK(...)
#define OVS_GUARDED
#define OVS_GUARDED_BY(...)
#define OVS_RELEASES(...)
#define OVS_MACRO_LOCK(...)
#define OVS_MACRO_RELEASE(...)
#endif

/* ISO C says that a C implementation may choose any integer type for an enum
 * that is sufficient to hold all of its values.  Common ABIs (such as the
 * System V ABI used on i386 GNU/Linux) always use a full-sized "int", even
 * when a smaller type would suffice.
 *
 * In GNU C, "enum __attribute__((packed)) name { ... }" defines 'name' as an
 * enum compatible with a type that is no bigger than necessary.  This is the
 * intended use of OVS_PACKED_ENUM.
 *
 * OVS_PACKED_ENUM is intended for use only as a space optimization, since it
 * only works with GCC.  That means that it must not be used in wire protocols
 * or otherwise exposed outside of a single process. */
#if __GNUC__ && !__CHECKER__
#define OVS_PACKED_ENUM __attribute__((__packed__))
#else
#define OVS_PACKED_ENUM
#endif

#ifndef _MSC_VER
#define OVS_PACKED(DECL) DECL __attribute__((__packed__))
#else
#define OVS_PACKED(DECL) __pragma(pack(push, 1)) DECL __pragma(pack(pop))
#endif

#endif /* compiler.h */
