# xrope [![Build Status](https://travis-ci.org/wasabiz/xrope.png)](https://travis-ci.org/wasabiz/xrope)

rope data structre

## Usage

Just include "xrope.h" anywhere you want to use xrope!

## API

```c
typedef struct xrope xrope;

static inline void XROPE_INCREF(xrope *);
static inline void XROPE_DECREF(xrope *);

static inline xrope *xr_new(const char * /* static */, size_t);
static inline xrope *xr_new_volatile(const char * /* auto */, size_t);
static inline size_t xr_len(xrope *);
static inline char xr_at(xrope *, size_t);
static inline xrope *xr_cat(xrope *, xrope *);
static inline xrope *xr_sub(xrope *, size_t, size_t);
static inline const char *xr_str(xrope *);
```

## TODO

- self-balancing
- unicode support?
- custom allocator
- more test cases

## Author

Yuichi Nishiwaki (yuichi@idylls.jp)
