//
// Created by suncx on 2021/4/24.
//

#ifndef REACTOR_GNUEXT_H
#define REACTOR_GNUEXT_H

#define attr_constructor __attribute__((constructor))
#define attr_destructor __attribute__((destructor))

#define attr_deprecated __attribute__((deprecated))
#define attr_packed __attribute__((packed))

/* x 很可能为真 */
#define likely(x) __builtin_expect((x), true)

/* x 很可能为假 */
#define unlikely(x) __builtin_expect((x), false)

#endif //REACTOR_GNUEXT_H
