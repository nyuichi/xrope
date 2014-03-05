# xrope [![Build Status](https://travis-ci.org/wasabiz/xrope.png)](https://travis-ci.org/wasabiz/xrope)

rope data structre

## Usage

Just include "xrope.h" anywhere you want to use xrope!

## API

```c
typedef struct xrope xrope;

/**
 * <p>
 * | name         | frees buffer? | end with NULL? | complexity | misc
 * | ----         | ----          | ----           | ----       | ---
 * | xr_new_cstr  | no            | yes            | O(1)       | xr_new(_lit)
 * | xr_new_imbed | no            | no             | O(1)       |
 * | xr_new_move  | yes           | yes            | O(1)       |
 * | xr_new_copy  | yes           | no             | O(n)       |
 * </p>
 */

#define xr_new(cstr) xr_new_cstr(cstr, strlen(cstr))
#define xr_new_lit(cstr) xr_new_cstr(cstr, sizeof(cstr) - 1)
static inline xrope *xr_new_cstr(const char *, size_t);
static inline xrope *xr_new_imbed(const char *, size_t);
static inline xrope *xr_new_move(const char *, size_t);
static inline xrope *xr_new_copy(const char *, size_t);


static inline void XROPE_INCREF(xrope *);
static inline void XROPE_DECREF(xrope *);

static inline size_t xr_len(xrope *);
static inline char xr_at(xrope *, size_t);
static inline xrope *xr_cat(xrope *, xrope *);
static inline xrope *xr_sub(xrope *, size_t, size_t);
static inline const char *xr_cstr(xrope *); /* returns NULL-terminated string */
```

## TODO

- self-balancing
- unicode support?
- custom allocator
- more test cases

## Author

Yuichi Nishiwaki (yuichi@idylls.jp)
