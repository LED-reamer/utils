#pragma once

int example_func(int param1, int param2);

// linkerflag -rdynamic to be able to read data
int example_data[] = {123, 456, 789};
