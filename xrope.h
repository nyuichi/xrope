#ifndef XROPE_H__
#define XROPE_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* public APIs */

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


/* impl */

typedef struct {
  char *str;
  int refcnt;
  char autofree;
} xr_chunk;

#define XR_CHUNK_INCREF(c) do {                 \
    (c)->refcnt++;                              \
  } while (0)

#define XR_CHUNK_DECREF(c) do {                 \
    xr_chunk *c__ = (c);                        \
    if (! --c__->refcnt) {                      \
      if (c__->autofree)                        \
        free(c__->str);                         \
      free(c__);                                \
    }                                           \
  } while (0)

struct xrope {
  int refcnt;
  size_t weight;
  xr_chunk *chunk;
  size_t offset;
  struct xrope *left, *right;
};

static inline void
XROPE_INCREF(xrope *x) {
  x->refcnt++;
}

static inline void
XROPE_DECREF(xrope *x) {
  if (! --x->refcnt) {
    if (x->chunk) {
      XR_CHUNK_DECREF(x->chunk);
    }
    XROPE_DECREF(x->left);
    XROPE_DECREF(x->right);
    free(x);
  }
}

static inline xrope *
xr_new(const char *str, size_t len)
{
  xr_chunk *c;
  xrope *x;

  c = (xr_chunk *)malloc(sizeof(xr_chunk));
  c->refcnt = 1;
  c->str = (char *)str;
  c->autofree = 0;

  x = (xrope *)malloc(sizeof(xrope));
  x->refcnt = 1;
  x->left = NULL;
  x->right = NULL;
  x->weight = len;
  x->offset = 0;
  x->chunk = c;

  return x;
}

static inline xrope *
xr_new_volatile(const char *str, size_t len)
{
  char *buf;
  xr_chunk *c;
  xrope *x;

  buf = (char *)malloc(len);
  memcpy(buf, str, len);

  c = (xr_chunk *)malloc(sizeof(xr_chunk));
  c->refcnt = 1;
  c->str = buf;
  c->autofree = 1;

  x = (xrope *)malloc(sizeof(xrope));
  x->refcnt = 1;
  x->left = NULL;
  x->right = NULL;
  x->weight = len;
  x->offset = 0;
  x->chunk = c;

  return x;
}

static inline size_t
xr_len(xrope *x)
{
  return x->weight;
}

static inline char
xr_at(xrope *x, size_t i)
{
  if (x->weight <= i) {
    return -1;
  }
  if (x->chunk) {
    return x->chunk->str[x->offset + i];
  }
  return (i < x->left->weight)
    ? xr_at(x->left, i)
    : xr_at(x->right, i - x->left->weight);
}

static inline xrope *
xr_cat(xrope *x, xrope *y)
{
  xrope *z;

  z = (xrope *)malloc(sizeof(xrope));
  z->refcnt = 1;
  z->left = x;
  z->right = y;
  z->weight = x->weight + y->weight;
  z->offset = 0;
  z->chunk = NULL;

  XROPE_INCREF(x);
  XROPE_INCREF(y);

  return z;
}

static struct xrope *
xr_sub(xrope *x, size_t i, size_t j)
{
  if (x->weight < j) {          /* checked only once the first run */
    return NULL;
  }

  if (i == 0 && x->weight == j) {
    return x;
  }

  if (x->chunk) {
    xrope *y;

    y = (xrope *)malloc(sizeof(xrope));
    y->refcnt = 1;
    y->left = NULL;
    y->right = NULL;
    y->weight = j - i;
    y->offset = x->offset + i;
    y->chunk = x->chunk;

    XR_CHUNK_INCREF(x->chunk);

    return y;
  }

  if (j <= x->left->weight) {
    return xr_sub(x->left, i, j);
  }
  else if (x->left->weight <= i) {
    return xr_sub(x->right, i - x->left->weight, j - x->left->weight);
  }
  else {
    xrope *r, *l;

    l = xr_sub(x->left, i, x->left->weight);
    r = xr_sub(x->right, 0, j - x->left->weight);
    x = xr_cat(l, r);

    XROPE_DECREF(l);
    XROPE_DECREF(r);

    return x;
  }
}

static inline void
xr_fold(xrope *x, xr_chunk *c, size_t offset)
{
  if (x->chunk) {
    memcpy(c->str + offset, x->chunk->str, x->weight);
    XR_CHUNK_DECREF(x->chunk);

    x->chunk = c;
    x->offset = offset;
    XR_CHUNK_INCREF(c);
    return;
  }
  xr_fold(x->left, c, offset);
  xr_fold(x->right, c, offset + x->left->weight);

  XROPE_DECREF(x->left);
  XROPE_DECREF(x->right);
  x->left = x->right = NULL;
  x->chunk = c;
  x->offset = offset;
  XR_CHUNK_INCREF(c);
}

static inline const char *
xr_str(xrope *x)
{
  xr_chunk *c;

  c = (xr_chunk *)malloc(sizeof(xr_chunk));
  c->refcnt = 0;                /* shortcut */
  c->autofree = 1;
  c->str = (char *)malloc(x->weight + 1);
  c->str[x->weight] = '\0';

  xr_fold(x, c, 0);

  return c->str;
}

#if defined(__cplusplus)
}
#endif

#endif
