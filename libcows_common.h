#ifndef	LIBCOWS_COMMON_H
#define	LIBCOWS_COMMON_H

#ifndef	LIST_FOREACH_SAFE
#define LIST_FOREACH_SAFE(var, head, field, tvar)                       \
        for ((var) = LIST_FIRST((head));                                \
            (var) && ((tvar) = LIST_NEXT((var), field), 1);             \
            (var) = (tvar))
#endif

#ifndef	TAILQ_FOREACH_SAFE
#define TAILQ_FOREACH_SAFE(var, head, field, tvar)                      \
        for ((var) = TAILQ_FIRST((head));                               \
            (var) && ((tvar) = TAILQ_NEXT((var), field), 1);            \
            (var) = (tvar))
#endif

#ifndef STAILQ_FOREACH_SAFE
#define STAILQ_FOREACH_SAFE(var, head, field, tvar)                     \
        for ((var) = STAILQ_FIRST((head));                              \
            (var) && ((tvar) = STAILQ_NEXT((var), field), 1);           \
            (var) = (tvar))
#endif

#ifndef	STAILQ_LAST
#ifndef	offsetof
#define offsetof(type, member) ((size_t) &((type *)0)->member)
#endif
#define STAILQ_LAST(head, type, field)                                  \
        (STAILQ_EMPTY((head)) ?                                         \
                NULL :                                                  \
                ((struct type *)(void *)                                \
                ((char *)((head)->stqh_last) - offsetof(struct type, field))))
#endif

#define STATIC_ARRAY_SIZE(a)    (sizeof(a)/sizeof((a)[0]))

#define UNUSED  __attribute__((unused))

#endif	/* LIBCOWS_COMMON_H */
