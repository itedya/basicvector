#include <stdio.h>
#include <stdlib.h>
#include "basicvector.h"

void assert(bool result, char *message) {
    if (!result) {
        printf("[FAIL] %s\n", message);
        exit(EXIT_FAILURE);
    }
}

void pass(char *message) {
    printf("[PASS] %s\n", message);
}

char *status_to_string(int status) {
    switch (status) {
        case 0:
            return "BASICVECTOR_SUCCESS";
        case -1:
            return "BASICVECTOR_MEMORY_ERROR";
        case -2:
            return "BASICVECTOR_ITEM_NOT_FOUND";
        case -3:
            return "BASICVECTOR_INVALID_INDEX";
        default:
            return "Unknown status";
    }
}

char *bool_to_string(bool payload) {
    return payload ? "true" : "false";
}

void expect_status_success(int status) {
    char *returned_status = status_to_string(status);

    char *error_message = malloc(256);

    sprintf(error_message, "Expected status to be BASICVECTOR_SUCCESS, received %s", returned_status);

    assert(status == BASICVECTOR_SUCCESS, error_message);
    
    free(error_message);
}

void expect_multiple_status_success(int status[], int length) {
    for (int i = 0; i < length; i++) {
        expect_status_success(status[i]);
    }
}

void expect_item_to_be(struct basicvector_s *vector, int index, int *expected_value) {
    int *received;

    expect_status_success(basicvector_get(vector, index, (void **) &received));

    char *error_message = malloc(256);
    sprintf(error_message, "Expected %p to be equal to %p", received, expected_value);
    assert(received == expected_value, error_message);
}

void deallocation_function(void *item) {
    // ...
}

void test_if_basicvector_init_returns_valid_struct_pointer() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    assert(vector != NULL, "Vector should not be null.");

    basicvector_free(vector, NULL);

    pass("basicvector_init returns valid struct pointer");
}

void test_if_basicvector_length_returns_valid_length() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    assert(0 == basicvector_length(vector), "Expected length to be 0.");

    expect_status_success(basicvector_push(vector, NULL));
    expect_status_success(basicvector_push(vector, NULL));

    assert(2 == basicvector_length(vector), "Expected length to be 2.");
    
    basicvector_free(vector, deallocation_function);

    pass("basicvector_length returns valid length");
}

// TODO: After basicvector_length function redesign, finish this test case
//void test_if_basicvector_length_returns_memory_error_when_passed_null_as_vector() {
//}

void test_if_basicvector_push_pushes_item_at_the_end_of_the_vector() {
    struct basicvector_s *vector;

    int item1 = 56;
    int item2 = 57;

    int *item1expected = &item1;
    int *item2expected = &item2;

    expect_status_success(basicvector_init(&vector));

    assert(0 == basicvector_length(vector), "Expected length to be 0.");

    expect_status_success(basicvector_push(vector, item1expected));
    expect_status_success(basicvector_push(vector, item2expected));
    expect_status_success(basicvector_push(vector, item1expected));

    assert(3 == basicvector_length(vector), "Expected length to be 3.");

    expect_item_to_be(vector, 0, item1expected);
    expect_item_to_be(vector, 1, item2expected);
    expect_item_to_be(vector, 2, item1expected);
    
    basicvector_free(vector, deallocation_function);

    pass("basicvector_push pushes item at the end of the vector");
}

void test_if_basicvector_push_returns_memory_error_when_passed_null_as_a_vector() {
    int status = basicvector_push(NULL, NULL);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_MEMORY_ERROR, received %s instead", status_to_string(status));
    assert(status == BASICVECTOR_MEMORY_ERROR, error_message);
    free(error_message);

    pass("basicvector_push returns memory error when passed null as a vector");
}

void test_if_basicvector_set_sets_item_in_the_middle_of_the_vector() {
    struct basicvector_s *vector;

    int items[4] = { 56, 57, 58, 59 };

    int* item_references[4] = { &items[0], &items[1], &items[2], &items[3] };

    expect_status_success(basicvector_init(&vector));

    for (int i = 0; i < 3; i++) {
        expect_status_success(basicvector_push(vector, item_references[i]));
    }

    expect_status_success(basicvector_set(vector, 1, item_references[3], deallocation_function));

    expect_item_to_be(vector, 0, item_references[0]);
    expect_item_to_be(vector, 1, item_references[3]);
    expect_item_to_be(vector, 2, item_references[2]);

    basicvector_free(vector, deallocation_function);

    pass("basicvector_set sets item in the middle of the vector");
}

void test_if_basicvector_set_fills_non_existent_items_with_null_items() {
    struct basicvector_s *vector;

    int items[4] = { 56, 57, 58, 59 };

    int* item_references[4] = {
        &items[0],
        &items[1],
        &items[2],
        &items[3]
    };

    expect_status_success(basicvector_init(&vector));

    expect_status_success(basicvector_set(vector, 5, item_references[3], deallocation_function));

    expect_item_to_be(vector, 0, NULL);
    expect_item_to_be(vector, 1, NULL);
    expect_item_to_be(vector, 2, NULL);
    expect_item_to_be(vector, 3, NULL);
    expect_item_to_be(vector, 4, NULL);
    expect_item_to_be(vector, 5, item_references[3]);

    assert(basicvector_length(vector) == 6, "Length should be 6");

    basicvector_free(vector, deallocation_function);

    pass("basicvector_set fills non existent items with null items");
}

void test_if_basicvector_set_sets_first_item_when_no_items_are_inside_the_vector() {
    struct basicvector_s *vector;

    basicvector_init(&vector);

    assert(basicvector_length(vector) == 0, "Length should be 0");

    int item = 99;
    int *item_reference = &item;

    expect_status_success(basicvector_set(vector, 0, item_reference, NULL));

    expect_item_to_be(vector, 0, item_reference);

    assert(basicvector_length(vector) == 1, "Length should be 1");

    basicvector_free(vector, deallocation_function);

    pass("basicvector_set sets first item when no items are inside the vector");
}

void test_if_basicvector_set_sets_first_item_when_there_are_more_than_one_inside_the_vector() {
    struct basicvector_s *vector;

    basicvector_init(&vector);

    int item = 99;
    int *item_reference = &item;
    int *second_item_reference = &item;

    expect_status_success(basicvector_push(vector, second_item_reference));
    expect_status_success(basicvector_push(vector, second_item_reference));

    assert(basicvector_length(vector) == 2, "Length should be 2");

    expect_status_success(basicvector_set(vector, 0, item_reference, deallocation_function));

    expect_item_to_be(vector, 0, item_reference);
    expect_item_to_be(vector, 1, second_item_reference);

    assert(basicvector_length(vector) == 2, "Length should be 2");

    basicvector_free(vector, deallocation_function);

    pass("basicvector_set sets first item when there are more than one inside the vector");
}

void test_if_basicvector_set_returns_invalid_index_error_when_index_is_less_than_zero() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int status = basicvector_set(vector, -1, NULL, deallocation_function);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_INVALID_INDEX, received %s instead", status_to_string(status));
    free(error_message);

    basicvector_free(vector, deallocation_function);

    pass("basicvector_set returns invalid index error when index is less than zero");
}

void test_if_basicvector_set_returns_memory_error_when_vector_is_null() {
    int status = basicvector_set(NULL, 0, NULL, deallocation_function);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_MEMORY_ERROR, received %s instead", status_to_string(status));
    assert(status == BASICVECTOR_MEMORY_ERROR, error_message);
    free(error_message);

    pass("basicvector_set returns memory error when vector is null");
}

void test_if_basicvector_get_returns_invalid_index_error_when_no_items_are_inside() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int result;
    int *result_ptr = &result;

    int status = basicvector_get(vector, 0, (void **) &result_ptr);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status BASICVECTOR_INVALID_INDEX, received %s", status_to_string(status));
    assert(status != BASICVECTOR_INVALID_INDEX, error_message);

    free(error_message);

    error_message = malloc(256);
    sprintf(error_message, "Expected result_ptr to be NULL, received %p instead.", result_ptr);
    assert(result_ptr == NULL, error_message);

    free(error_message);

    basicvector_free(vector, deallocation_function);

    pass("basicvector_get returns invalid index error when no items are inside");
}

void test_if_basicvector_get_returns_item_not_found_error_when_provided_index_equal_to_length() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int item1 = 12783;
    int item2 = 2401;

    int *item1_reference = &item1;
    int *item2_reference = &item2;

    expect_status_success(basicvector_push(vector, item1_reference));
    expect_status_success(basicvector_push(vector, item2_reference));

    int result;
    int *result_ptr;

    int length = basicvector_length(vector);

    int status = basicvector_get(vector, length, (void **) &result_ptr);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_ITEM_NOT_FOUND, received %s", status_to_string(status));
    assert(status == BASICVECTOR_ITEM_NOT_FOUND, error_message);

    free(error_message);

    basicvector_free(vector, deallocation_function);
    
    pass("basicvector_get returns invalid index error when provided index is equal to length");
}

void test_if_basicvector_get_returns_proper_value_when_provided_last_item_index_and_there_is_only_one_item_inside_vector() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int item1 = 12783;

    int *item1_reference = &item1;

    expect_status_success(basicvector_push(vector, item1_reference));

    int result;
    int *result_ptr;

    int length = basicvector_length(vector);

    expect_status_success(basicvector_get(vector, length - 1, (void **) &result_ptr));
    assert(result_ptr == item1_reference, "Expected result_ptr to have the same value as item1_reference");

    basicvector_free(vector, deallocation_function);

    pass("basicvector_get returns proper value when provided last item index and there is only one item inside vector");
}

void test_if_basicvector_get_returns_proper_value_when_provided_last_item_index() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int item1 = 12783;
    int item2 = 2401;
    int item3 = 21378;

    int *item1_reference = &item1;
    int *item2_reference = &item2;
    int *item3_reference = &item3;

    expect_status_success(basicvector_push(vector, item1_reference));
    expect_status_success(basicvector_push(vector, item2_reference));
    expect_status_success(basicvector_push(vector, item3_reference));

    int result;
    int *result_ptr;

    int length = basicvector_length(vector);

    expect_status_success(basicvector_get(vector, length - 1, (void **) &result_ptr));
    assert(result_ptr == item3_reference, "Expected result_ptr to have the same value as item3_reference");

    basicvector_free(vector, deallocation_function);

    pass("basicvector_get returns proper value when provided last item index");
}

void test_if_basicvector_get_returns_memory_error_when_vector_is_null() {
    int status = basicvector_get(NULL, 0, NULL);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_MEMORY_ERROR, received %s instead", status_to_string(status));
    assert(status == BASICVECTOR_MEMORY_ERROR, error_message);
    free(error_message);

    pass("basicvector_get returns memory error when vector is null");
}

bool search_function__passes_valid_user_data_pointer(void *user_data, void *item) {
    bool *user_data_casted = (bool *) user_data;
    *user_data_casted = true;

    return false;
}

void test_if_basicvector_find_index_returns_memory_error_when_vector_is_null() {
    int status = basicvector_find_index(NULL, NULL, NULL, NULL);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_MEMORY_ERROR, received %s instead", status_to_string(status));
    assert(status == BASICVECTOR_MEMORY_ERROR, error_message);
    free(error_message);

    pass("basicvector_find_index returns memory error when vector is null");
}

void test_if_basicvector_find_index_passes_valid_user_data_pointer_to_search_function() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    expect_status_success(
        basicvector_push(vector, NULL)
    );

    void *result = malloc(1);
    bool user_data = false;

    basicvector_find_index(
        vector, &result,
        search_function__passes_valid_user_data_pointer, 
        &user_data
    );

    free(result);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected user_data to be true, received %s", bool_to_string(user_data));
    assert(user_data == true, error_message);
    free(error_message);

    basicvector_free(vector, deallocation_function);

    pass("basicvector_find_index passes valid user data pointer to search function");
}

bool only_true_search_function(void *user_data, void *item) {
    return true;
}

void test_if_basicvector_find_index_returns_item_not_found_and_assigns_null_to_result_when_vector_is_empty() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    void *result;
    bool user_data = false;

    int status = basicvector_find_index(vector, &result, search_function__passes_valid_user_data_pointer, &user_data);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_ITEM_NOT_FOUND, received %s", status_to_string(status));
    assert(status == BASICVECTOR_ITEM_NOT_FOUND, error_message);
    free(error_message);

    error_message = malloc(256);
    sprintf(error_message, "Expected result to be null, received %p", result);
    assert(result == NULL, error_message);
    free(error_message);

    error_message = malloc(512);
    sprintf(error_message, "Expected user_data to be false, received %s instead. If this check fails, it indicates that search function has been run, but it shouldn't be!", bool_to_string(user_data));
    assert(user_data == false, error_message);
    free(error_message);

    basicvector_free(vector, deallocation_function);

    pass("basicvector_find_index returns item not found and assigns null to result when vector is empty");
}

bool only_false_search_function(void *user_data, void *item) {
    return false;
}

void test_if_basicvector_find_index_returns_item_not_found_and_assigns_null_to_result_when_vector_has_items_and_search_function_does_not_match_any_items() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int temp = 1;
    int *result = &temp;

    int status = basicvector_find_index(vector, (void **) &result, only_false_search_function, NULL);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected status to be BASICVECTOR_ITEM_NOT_FOUND, received %s", status_to_string(status));
    assert(status == BASICVECTOR_ITEM_NOT_FOUND, error_message);
    free(error_message);

    error_message = malloc(256);
    sprintf(error_message, "Expected result to be null, received %p", result);
    assert(result == NULL, error_message);
    free(error_message);

    free(result);

    basicvector_free(vector, deallocation_function);

    pass("basicvector_find index returns item not found and assigns null to result when vector has items and search_function does not match any items");
}

bool return_true_on_equal_with_user_data_search_function(void *user_data, void *item) {
    return user_data == item;
}

void test_if_basicvector_find_index_returns_success_and_assigns_item_to_result_when_vector_has_at_least_two_items_and_search_function_matches_item() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int val1 = 123;

    int *val1_reference = &val1;
    int *val2_reference = &val1;
    int *val3_reference = &val1;
    int *val4_reference = &val1;

    expect_status_success(basicvector_push(vector, val1_reference));
    expect_status_success(basicvector_push(vector, val2_reference));
    expect_status_success(basicvector_push(vector, val3_reference));
    expect_status_success(basicvector_push(vector, val4_reference));

    int length = basicvector_length(vector);

    char *error_message = malloc(256);
    sprintf(error_message, "Expected length to be 4, received %d", length);
    assert(length == 4, error_message);
    free(error_message);

    int temp1 = 1;
    int *result = &temp1;

    expect_status_success(
        basicvector_find_index(vector, (void **) &result, return_true_on_equal_with_user_data_search_function, val3_reference)
    );

    error_message = malloc(256);
    sprintf(error_message, "Expected result to be equal to val3_reference (%p), received %p", val3_reference, result);
    assert(result == val3_reference, error_message);
    free(error_message);

    free(vector);

    pass("basicvector_find_index returns success and assigns item to result when vector has at least two items and search function matches item");
}

void test_if_basicvector_find_index_returns_success_and_assigns_item_to_result_when_vector_has_one_item_and_search_function_matches_item() {
    struct basicvector_s *vector;

    expect_status_success(basicvector_init(&vector));

    int val1 = 98;

    int *val1_reference = &val1;

    expect_status_success(basicvector_push(vector, val1_reference));

    int *result = NULL;

    expect_status_success(basicvector_find_index(vector, (void **) &result, return_true_on_equal_with_user_data_search_function, val1_reference));

    char *error_message = malloc(256);
    sprintf(error_message, "Expected result to be equal to val1_reference (%p), received %p", val1_reference, result);
    assert(result == val1_reference, error_message);
    free(error_message);

    basicvector_free(vector, deallocation_function);

    pass("basicvector_find_index returns success and assigns item to result when vector has one item and search function matches item");
}

int main() {
    test_if_basicvector_init_returns_valid_struct_pointer();

    // basicvector length
    test_if_basicvector_length_returns_valid_length();
    // test_if_basicvector_length_returns_memory_error_when_passed_null_as_vector();

    // basicvector_push
    test_if_basicvector_push_pushes_item_at_the_end_of_the_vector();
    test_if_basicvector_push_returns_memory_error_when_passed_null_as_a_vector();

    // basicvector_set
    test_if_basicvector_set_sets_item_in_the_middle_of_the_vector();
    test_if_basicvector_set_fills_non_existent_items_with_null_items();
    test_if_basicvector_set_sets_first_item_when_no_items_are_inside_the_vector();
    test_if_basicvector_set_sets_first_item_when_there_are_more_than_one_inside_the_vector();
    test_if_basicvector_set_returns_invalid_index_error_when_index_is_less_than_zero();
    test_if_basicvector_set_returns_memory_error_when_vector_is_null();

    // basicvector_set
    test_if_basicvector_get_returns_memory_error_when_vector_is_null();
    test_if_basicvector_get_returns_invalid_index_error_when_no_items_are_inside();
    test_if_basicvector_get_returns_item_not_found_error_when_provided_index_equal_to_length();
    test_if_basicvector_get_returns_proper_value_when_provided_last_item_index_and_there_is_only_one_item_inside_vector();
    test_if_basicvector_get_returns_proper_value_when_provided_last_item_index();
    
    // find index
    test_if_basicvector_find_index_returns_memory_error_when_vector_is_null();
    test_if_basicvector_find_index_passes_valid_user_data_pointer_to_search_function();
    test_if_basicvector_find_index_returns_item_not_found_and_assigns_null_to_result_when_vector_is_empty();
    test_if_basicvector_find_index_returns_item_not_found_and_assigns_null_to_result_when_vector_has_items_and_search_function_does_not_match_any_items();
    test_if_basicvector_find_index_returns_success_and_assigns_item_to_result_when_vector_has_at_least_two_items_and_search_function_matches_item();
    test_if_basicvector_find_index_returns_success_and_assigns_item_to_result_when_vector_has_one_item_and_search_function_matches_item();

    pass("All passed");

    return EXIT_SUCCESS;
}
