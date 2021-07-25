//
// Created by suncx on 2021/4/24.
//

#ifndef REACTOR_EXT_H
#define REACTOR_EXT_H

#define attr_constructor __attribute__((constructor))
#define attr_destructor __attribute__((destructor))

#define attr_deprecated __attribute__((deprecated))
#define attr_packed __attribute__((packed))

/* condition x is very likely true. */
#define likely(x) __builtin_expect((x), true)

/* condition x is very likely false. */
#define unlikely(x) __builtin_expect((x), false)

#endif //REACTOR_EXT_H
