#include <gtest/gtest.h>

extern "C"
{
#include "mru_list.h"
}

using namespace ::testing;

typedef struct test_s{
    int intData;
    float floatData;
    char charData;
} test_s;

int test_sCopy(void* src, void* dst)
{
    memcpy(dst, src, sizeof(test_s));
    return 0;
}

TEST(InitTest, Init_test)
{
    unsigned int size = 10;
    mruList_s *lst = initList(size);
    ASSERT_EQ(0, lst->size);
    ASSERT_EQ(NULL, lst->start);
    ASSERT_EQ(NULL, lst->end);
    freeList(lst);
}

TEST(PutGetTest, Put_Get_Test)
{
    unsigned int size = 10;
    test_s data1, data2;
    data1.intData = 3;
    data1.floatData = 3.15;
    data1.charData = 'a';
    data2.intData = 4;
    data2.floatData = 5.36;
    data2.charData = 'b';
    mruList_s *lst = initList(size);
    put(lst, (void*)&data1);
    put(lst, (void*)&data2);

    test_s *recv = (test_s*)get(lst, 0);
    ASSERT_EQ(&data2, recv);
    recv = (test_s*)get(lst, 1);
    ASSERT_EQ(&data1, recv);
    recv = (test_s*)get(lst, 0);
    ASSERT_EQ(&data1, recv);
    ASSERT_EQ(2, lst->size);
    freeList(lst);
}

TEST(PutGetLastTest, Put_Get_Last_Test)
{
    unsigned int size = 10;
    test_s data1, data2;
    data1.intData = 3;
    data1.floatData = 3.15;
    data1.charData = 'a';
    data2.intData = 4;
    data2.floatData = 5.36;
    data2.charData = 'b';
    mruList_s *lst = initList(size);
    put(lst, (void*)&data1);
    put(lst, (void*)&data2);

    test_s *recv = (test_s*)getLast(lst);
    ASSERT_EQ(&data1, recv);
    recv = (test_s*)getLast(lst);
    ASSERT_EQ(&data2, recv);
    recv = (test_s*)getLast(lst);
    ASSERT_EQ(&data1, recv);
    ASSERT_EQ(2, lst->size);
    freeList(lst);
}

TEST(PutGetFirstTest, Put_Get_First_Test)
{
    unsigned int size = 10;
    test_s data1, data2;
    data1.intData = 3;
    data1.floatData = 3.15;
    data1.charData = 'a';
    data2.intData = 4;
    data2.floatData = 5.36;
    data2.charData = 'b';
    mruList_s *lst = initList(size);
    put(lst, (void*)&data1);
    put(lst, (void*)&data2);

    test_s *recv = (test_s*)getFirst(lst);
    ASSERT_EQ(&data2, recv);
    recv = (test_s*)getFirst(lst);
    ASSERT_EQ(&data2, recv);
    recv = (test_s*)getLast(lst);
    ASSERT_EQ(&data1, recv);
    recv = (test_s*)getFirst(lst);
    ASSERT_EQ(&data1, recv);
    ASSERT_EQ(2, lst->size);
    freeList(lst);
}

TEST(IsInListTest, Is_in_List_Test)
{
    unsigned int size = 10;
    test_s data1, data2, data3;
    data1.intData = 3;
    data1.floatData = 3.15;
    data1.charData = 'a';
    data2.intData = 4;
    data2.floatData = 5.36;
    data2.charData = 'b';
    mruList_s *lst = initList(size);
    put(lst, (void*)&data1);
    put(lst, (void*)&data2);

    int result;
    result = isInList(lst, &data1);
    ASSERT_EQ(2, result);
    result = isInList(lst, &data2);
    ASSERT_EQ(1, result);
    result = isInList(lst, &data3);
    ASSERT_EQ(0, result);
    freeList(lst);
}

TEST(CompareTest, Compare_Test)
{
    unsigned int size = 10;
    test_s data1, data2;
    data1.intData = 3;
    data1.floatData = 3.15;
    data1.charData = 'a';
    data2.intData = 4;
    data2.floatData = 5.36;
    data2.charData = 'b';
    mruList_s *lst = initList(size);
    put(lst, (void*)&data1);
    put(lst, (void*)&data2);

    int result;
    result = lst->functions.compare(&data1, &data1);
    ASSERT_EQ(1, result);
    result = lst->functions.compare(&data2, &data1);
    ASSERT_EQ(0, result);
    result = lst->functions.compare(&data2, &data2);
    ASSERT_EQ(1, result);
    freeList(lst);
}

TEST(CopyTest, Copy_Test)
{
    unsigned int size = 10;
    test_s data1, data2;
    data1.intData = 3;
    data1.floatData = 3.15;
    data1.charData = 'a';
    data2.intData = 4;
    data2.floatData = 5.36;
    data2.charData = 'b';
    mruList_s *lst = initList(size);
    put(lst, (void*)&data1);
    put(lst, (void*)&data2);
    lst->functions.copy = test_sCopy;

    test_s recv;
    int result;
    result = lst->functions.copy(&data1, &recv);
    ASSERT_EQ(0, result);
    ASSERT_EQ(data1.intData, recv.intData);
    ASSERT_EQ(data1.floatData, recv.floatData);
    ASSERT_EQ(data1.charData, recv.charData);
    result = lst->functions.copy(&data2, &recv);
    ASSERT_EQ(0, result);
    ASSERT_EQ(data2.intData, recv.intData);
    ASSERT_EQ(data2.floatData, recv.floatData);
    ASSERT_EQ(data2.charData, recv.charData);
    result = lst->functions.copy(&data2, &data1);
    ASSERT_EQ(0, result);
    ASSERT_EQ(data2.intData, data1.intData);
    ASSERT_EQ(data2.floatData, data1.floatData);
    ASSERT_EQ(data2.charData, data1.charData);
    freeList(lst);
}

TEST(LimitSizeTest, Limit_Size_Test)
{
    unsigned int size = 4;
    int result;
    test_s data1, data2, data3, data4, data5;
    data1.intData = 3;
    data1.floatData = 3.15;
    data1.charData = 'a';
    data2.intData = 4;
    data2.floatData = 5.36;
    data2.charData = 'b';
    data3.intData = 5;
    data3.floatData = 3.56;
    data3.charData = 'c';
    data4.intData = 6;
    data4.floatData = 7.19;
    data4.charData = 'd';
    data5.intData = 7;
    data5.floatData = 9.04;
    data5.charData = 'e';
    mruList_s *lst = initList(size);
    put(lst, (void*)&data1);
    put(lst, (void*)&data2);
    put(lst, (void*)&data3);
    put(lst, (void*)&data4);
    ASSERT_EQ(4, lst->size);
    result = isInList(lst, &data1);
    ASSERT_EQ(4, result);
    result = isInList(lst, &data2);
    ASSERT_EQ(3, result);
    result = isInList(lst, &data3);
    ASSERT_EQ(2, result);
    result = isInList(lst, &data4);
    ASSERT_EQ(1, result);
    result = isInList(lst, &data5);
    ASSERT_EQ(0, result);

    put(lst, (void*)&data5);
    ASSERT_EQ(4, lst->size);
    result = isInList(lst, &data1);
    ASSERT_EQ(0, result);
    result = isInList(lst, &data2);
    ASSERT_EQ(4, result);
    result = isInList(lst, &data3);
    ASSERT_EQ(3, result);
    result = isInList(lst, &data4);
    ASSERT_EQ(2, result);
    result = isInList(lst, &data5);
    ASSERT_EQ(1, result);
    freeList(lst);
}
