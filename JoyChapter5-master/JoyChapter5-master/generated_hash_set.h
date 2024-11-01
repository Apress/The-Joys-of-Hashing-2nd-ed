
#ifndef CHAINED_HASH_H
#define CHAINED_HASH_H

#include <stdbool.h>
#include <stdio.h>

#include "generated_list.h"

#define BIN(HASH_NAME) struct HASH_NAME##_bin_list
#define HTABLE(HASH_NAME) struct HASH_NAME##_hash_table
#define LIST_FN(HASH_NAME, FUNC_NAME) HASH_NAME##_bin##_##FUNC_NAME
#define HASH_FN(HASH_NAME, FUNC_NAME) HASH_NAME##_##FUNC_NAME

#define MIN_SIZE 8

#define GEN_HASH_STRUCTS(HASH_NAME, KEY_TYPE, KEY_CMP, KEY_DESTRUCTOR)         \
  GEN_LIST(HASH_NAME##_bin, KEY_TYPE, KEY_CMP, KEY_DESTRUCTOR)                 \
  HTABLE(HASH_NAME)                                                            \
  {                                                                            \
    BIN(HASH_NAME) * bins;                                                     \
    unsigned int size;                                                         \
    unsigned int used;                                                         \
  };

#define GEN_GET_KEY_BIN(HASH_NAME)                                             \
  BIN(HASH_NAME) * HASH_FN(HASH_NAME, get_key_bin)(HTABLE(HASH_NAME) * table,  \
                                                   unsigned int hash_key)      \
  {                                                                            \
    unsigned int mask = table->size - 1;                                       \
    unsigned int index = hash_key & mask;                                      \
    return &table->bins[index];                                                \
  }

#define GEN_NEW_TABLE(HASH_NAME)                                               \
  HTABLE(HASH_NAME) * HASH_FN(HASH_NAME, new_table)()                          \
  {                                                                            \
    HTABLE(HASH_NAME) *table = malloc(sizeof *table);                          \
    BIN(HASH_NAME) *bins = malloc(MIN_SIZE * sizeof *bins);                    \
    *table = (HTABLE(HASH_NAME)){.bins = bins, .size = MIN_SIZE, .used = 0};   \
    for (BIN(HASH_NAME) *bin = table->bins; bin < table->bins + table->size;   \
         bin++) {                                                              \
      bin->head = NULL;                                                        \
    }                                                                          \
    return table;                                                              \
  }

#define GEN_FREE_TABLE(HASH_NAME)                                              \
  void HASH_FN(HASH_NAME, free_table)(HTABLE(HASH_NAME) * table)               \
  {                                                                            \
    for (BIN(HASH_NAME) *bin = table->bins; bin < table->bins + table->size;   \
         bin++) {                                                              \
      LIST_FN(HASH_NAME, free_list)(bin);                                      \
    }                                                                          \
    free(table->bins);                                                         \
    free(table);                                                               \
  }

#define GEN_INSERT_KEY(HASH_NAME, KEY_TYPE, HASH)                              \
  void HASH_FN(HASH_NAME, insert_key)(HTABLE(HASH_NAME) * table, KEY_TYPE key) \
  {                                                                            \
    BIN(HASH_NAME) *bin = HASH_FN(HASH_NAME, get_key_bin)(table, HASH(key));   \
    if (!LIST_FN(HASH_NAME, contains_key)(bin, key)) {                         \
      LIST_FN(HASH_NAME, add_key)(bin, key);                                   \
      table->used++;                                                           \
      if (table->size == table->used) {                                        \
        HASH_FN(HASH_NAME, resize)(table, 2 * table->size);                    \
      }                                                                        \
    }                                                                          \
  }

#define GEN_CONTAINS_KEY(HASH_NAME, KEY_TYPE, HASH)                            \
  bool HASH_FN(HASH_NAME, contains_key)(HTABLE(HASH_NAME) * table,             \
                                        KEY_TYPE key)                          \
  {                                                                            \
    BIN(HASH_NAME) *bin = HASH_FN(HASH_NAME, get_key_bin)(table, HASH(key));   \
    return LIST_FN(HASH_NAME, contains_key)(bin, key);                         \
  }

#define GEN_DELETE_KEY(HASH_NAME, KEY_TYPE, HASH)                              \
  void HASH_FN(HASH_NAME, delete_key)(HTABLE(HASH_NAME) * table, KEY_TYPE key) \
  {                                                                            \
    BIN(HASH_NAME) *bin = HASH_FN(HASH_NAME, get_key_bin)(table, HASH(key));   \
    if (LIST_FN(HASH_NAME, contains_key)(bin, key)) {                          \
      LIST_FN(HASH_NAME, delete_key)(bin, key);                                \
      table->used--;                                                           \
      if (table->size > MIN_SIZE && table->used < table->size / 4) {           \
        HASH_FN(HASH_NAME, resize)(table, table->size / 2);                    \
      }                                                                        \
    }                                                                          \
  }

#define MOVE_LINK(FROM, TO)                                                    \
  do {                                                                         \
    typeof(**FROM) *link = *FROM;                                              \
    *FROM = link->next;                                                        \
    link->next = *TO;                                                          \
    *TO = link;                                                                \
  } while (0)

#define GEN_RESIZE(HASH_NAME, HASH)                                            \
  void HASH_FN(HASH_NAME, resize)(HTABLE(HASH_NAME) * table,                   \
                                  unsigned int new_size)                       \
  {                                                                            \
    BIN(HASH_NAME) *old_bins = table->bins, *old_from = old_bins,              \
                   *old_to = old_from + table->size;                           \
                                                                               \
    table->bins = malloc(new_size * sizeof *table->bins);                      \
    table->size = new_size;                                                    \
    for (BIN(HASH_NAME) *bin = table->bins; bin < table->bins + table->size;   \
         bin++) {                                                              \
      bin->head = NULL;                                                        \
    }                                                                          \
                                                                               \
    for (BIN(HASH_NAME) *bin = old_from; bin < old_to; bin++) {                \
      for (ITR(bin) itr = ITR_BEG(bin); !ITR_END(itr);) {                      \
        unsigned int hash_key = HASH(ITR_DEREF(itr)->key);                     \
        MOVE_LINK(itr,                                                         \
                  ITR_BEG(HASH_FN(HASH_NAME, get_key_bin)(table, hash_key)));  \
      }                                                                        \
    }                                                                          \
                                                                               \
    free(old_bins);                                                            \
  }

#define GEN_HASH_TABLE(HASH_NAME, KEY_TYPE, KEY_CMP, HASH, KEY_DESTRUCTOR)     \
  GEN_HASH_STRUCTS(HASH_NAME, KEY_TYPE, KEY_CMP, KEY_DESTRUCTOR)               \
  GEN_GET_KEY_BIN(HASH_NAME)                                                   \
  GEN_NEW_TABLE(HASH_NAME)                                                     \
  GEN_FREE_TABLE(HASH_NAME)                                                    \
  GEN_RESIZE(HASH_NAME, HASH)                                                  \
  GEN_INSERT_KEY(HASH_NAME, KEY_TYPE, HASH)                                    \
  GEN_CONTAINS_KEY(HASH_NAME, KEY_TYPE, HASH)                                  \
  GEN_DELETE_KEY(HASH_NAME, KEY_TYPE, HASH)

#endif
