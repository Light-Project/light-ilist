/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * Copyright(c) 2021 Sanpe <sanpeqf@gmail.com>
 */

#include "ilist.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * ARRAY_SIZE - get the number of elements in array.
 * @arr: array to be sized.
 */
#define ARRAY_SIZE(arr) ( \
    sizeof(arr) / sizeof((arr)[0]) \
)

#define TEST_LOOP 10

struct ilist_test_pdata {
    struct ilist_node nodes[TEST_LOOP];
};

static int ilist_selftest(void *pdata)
{
    struct ilist_test_pdata *ldata = pdata;
    unsigned int count, count2;

    ILIST_HEAD(test_head);

    for (count = 0; count < TEST_LOOP; ++count) {
        printf("ilist 'ilist_add' test%u single\n", count);
        ilist_node_init(&ldata->nodes[count], count);
        ilist_add(&test_head, &ldata->nodes[count]);
    }

    for (count = 0; count < TEST_LOOP; ++count)
        ilist_del(&test_head, &ldata->nodes[count]);

    for (count = 0; count < TEST_LOOP / 2; ++count) {
        for (count2 = 0; count2 < 2; ++count2) {
            printf("ilist 'ilist_add' test%u multi%u\n", count * 2 + count2, count2);
            ilist_node_init(&ldata->nodes[count * 2 + count2], count2);
            ilist_add(&test_head, &ldata->nodes[count * 2 + count2]);
        }
    }

    for (count = 0; count < TEST_LOOP; ++count)
        ilist_del(&test_head, &ldata->nodes[count]);

    return 0;
}

int main(void)
{
    struct ilist_test_pdata *idata;
    int retval;

    idata = malloc(sizeof(struct ilist_test_pdata));
    if (!idata)
        return -1;

    retval = ilist_selftest(idata);
    free(idata);

    return retval;
}
