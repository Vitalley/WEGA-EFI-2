#include <xc.h>
#include <stdlib.h>
#include <stdint.h>


uint8_t linear_interp(uint8_t x1, uint8_t x2, uint8_t y1, uint8_t y2, uint8_t x);
void search_table(uint8_t *tbl, uint8_t tbl_length, uint8_t item, struct search_table_t *r);

// argument to search_table function
struct search_table_t {
  uint8_t lbound;
  uint8_t ubound;
  uint8_t index;
};

typedef size_t byte;