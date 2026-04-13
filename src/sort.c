#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "benchmark.h"

#define HYBRID_MS_K 210

#define HYBRID_QS_K 300

/* Minimum run size for Timsort;
 *
 * see the minimum chunk size explanation for timsort:
 * https://en.wikipedia.org/wiki/Timsort
 *
 * TLDR: the chunk size should be a number between 32 and 64, such that the
 * size of the array divided by that number is close to a power of two.
 */
#define TS_K 32
#define TS_MAX_RUN_SIZE 1024

void _insertion_sort(int64_t* data, uint32_t start, uint32_t end) {
	int32_t i;
	uint32_t j;
	int64_t key;

	for (j = start + 1; j <= end; j++) {
		i = j - 1;
		key = data[j];

		while (i >= (int32_t)start && data[i] > key) {
			data[i + 1] = data[i];
			i--;
		}

		data[i + 1] = key;
	}
}

void insertion_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = binput->size;

	_insertion_sort(data, 0, size - 1);
}

void merge(int64_t* data, uint32_t start, uint32_t mid, uint32_t end) {
	int n1 = mid - start + 1;
	int n2 = end - mid;

	int i, j, left[n1], right[n2];

	for (i = 0; i < n1; i++) {
		left[i] = data[start + i];
	}

	for (j = 0; j < n2; j++) {
		right[j] = data[mid + 1 + j];
	}

	i = j = 0;

	for (uint32_t k = start; k <= end; k++) {
		if (i < n1) {
			if (j < n2) {
				data[k] = (left[i] <= right[j]) ? left[i++] : right[j++];
			} else {
				data[k] = left[i++];
			}
		} else {
			data[k] = right[j++];
		}
	}
}

void _merge_sort(int64_t* data, uint32_t start, uint32_t end) {
	if (start < end) {
		uint32_t mid = start + (end - start) / 2;
		_merge_sort(data, start, mid);
		_merge_sort(data, mid + 1, end);
		merge(data, start, mid, end);
	}
}

void merge_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = binput->size;

	_merge_sort(data, 0, size - 1);
}

void _hybrid_merge_sort(int64_t* data, uint32_t start, uint32_t end) {
	if (end - start > HYBRID_MS_K) {
		int mid = start + (end - start) / 2;
		_hybrid_merge_sort(data, start, mid);
		_hybrid_merge_sort(data, mid + 1, end);
		merge(data, start, mid, end);
	} else {
		_insertion_sort(data, start, end);
	}
}

void hybrid_merge_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = binput->size;

	_hybrid_merge_sort(data, 0, size - 1);
}

void swap(int64_t* arr, uint32_t i, uint32_t j) {
	int temp = arr[i];
	arr[i] = arr[j];
	arr[j] = temp;
}

uint32_t partition(int64_t* data, uint32_t start, uint32_t end) {
	int64_t pivot = data[end];
	int i = start - 1;

	for (uint32_t j = start; j < end; j++) {
		if (data[j] <= pivot) {
			swap(data, ++i, j);
		}
	}

	swap(data, ++i, end);
	return i;
}

void _quick_sort(int64_t* data, uint32_t start, uint32_t end) {
	if (start < end) {
		uint32_t mid = partition(data, start, end);
		if (mid > 0) {
			_quick_sort(data, start, mid - 1);
		}
		_quick_sort(data, mid + 1, end);
	}
}

void quick_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = binput->size;

	_quick_sort(data, 0, size - 1);
}

uint32_t median_of_three(int64_t* data, uint32_t a, uint32_t b, uint32_t c) {
	int64_t data_a = data[a];
	int64_t data_b = data[b];
	int64_t data_c = data[c];

	if (data_a > data_b) {
		if (data_b > data_c)
			return b;
		else if (data_a < data_c)
			return a;
		else
			return c;
	} else {
		if (data_a > data_c)
			return a;
		else if (data_b < data_c)
			return b;
		else
			return c;
	}
}

int mot_partition(int64_t* data, uint32_t start, uint32_t end) {
	uint32_t median =
		median_of_three(data, start, end, start + (end - start) / 2);
	swap(data, median, end);
	return partition(data, start, end);
}

void _mot_quick_sort(int64_t* data, uint32_t start, uint32_t end) {
	if (start < end) {
		uint32_t mid = partition(data, start, end);
		if (mid > 0) {
			_mot_quick_sort(data, start, mid - 1);
		}
		_mot_quick_sort(data, mid + 1, end);
	}
}

void mot_quick_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = binput->size;

	_mot_quick_sort(data, 0, size - 1);
}

void _tail_quick_sort(int64_t* data, uint32_t start, uint32_t end) {
	while (start < end) {
		int mid = mot_partition(data, start, end);
		if (mid > 0) {
			_tail_quick_sort(data, start, mid - 1);
		}
		start = mid + 1;
	}
}

void tail_quick_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = binput->size;

	_tail_quick_sort(data, 0, size - 1);
}

void _hybrid_tail_quick_sort(int64_t* data, uint32_t start, uint32_t end) {
	while (start < end) {
		uint32_t size = end - start + 1;

		if (size <= HYBRID_QS_K) {
			_insertion_sort(data, start, end);
			return;
		}

		uint32_t mid = mot_partition(data, start, end);

		if (mid - start > end - mid) {
			if (mid > 0) {
				_tail_quick_sort(data, start, mid - 1);
			}
			start = mid + 1;
		} else {
			_tail_quick_sort(data, mid + 1, end);

			if (mid == 0) {
				break;
			}
			end = mid - 1;
		}
	}
}

void hybrid_tail_quick_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = binput->size;

	_hybrid_tail_quick_sort(data, 0, size - 1);
}

/* Essentially a pair storing the initial and final index in a collection;
 * this is implemented for supporting tim sort. */
struct ts_pair {
	uint32_t start;
	uint32_t end;
};

void reverse(int64_t* data, uint32_t start, uint32_t end) {
	while (start < end) {
		swap(data, start, end);
		start++;
		end--;
	}
}

/* Find and return a new run in `data`, starting from `start`, up until `size`;
 * In particular, <= size is considered rather than < size.*/
struct ts_pair find_run(int64_t* data, uint32_t start, uint32_t size) {
	if (start >= size) {
		return (struct ts_pair){start, start};
	}

	uint32_t idx = start;

	// ascending order
	if (data[start + 1] >= data[start]) {
		while (idx < size && data[idx + 1] >= data[idx]) {
			++idx;
		}
	}
	// descending
	else {
		while (idx < size && data[idx + 1] <= data[idx]) {
			++idx;
		}
		reverse(data, start, idx);
	}

	// run size is not big enough
	if (idx - start + 1 < TS_K) {
		uint32_t target = start + TS_K - 1;
		if (target > size) {
			target = size;
		}
		_insertion_sort(data, start, target);
		return (struct ts_pair){start, target};
	} else {
		return (struct ts_pair){start, idx};
	}
}

void _tim_sort(int64_t* data, uint32_t start, uint32_t end) {
	uint32_t pos = start;
	uint32_t run_index = 0;
	struct ts_pair runs[TS_MAX_RUN_SIZE]; 

	while (pos <= end) {
		runs[run_index] = find_run(data, pos, end);
		pos = runs[run_index].end + 1;
		run_index += 1;

		// invariance-check loop
		while (run_index >= 3) {
			uint32_t X_len =
				runs[run_index - 1].end - runs[run_index - 1].start + 1;
			uint32_t Y_len =
				runs[run_index - 2].end - runs[run_index - 2].start + 1;
			uint32_t Z_len =
				runs[run_index - 3].end - runs[run_index - 3].start + 1;

			// negation of (|Z| > |Y| + |X|) && (|Y| > |X|)
			if ((Z_len <= Y_len + X_len) || (Y_len <= X_len)) {
				struct ts_pair Y = runs[run_index - 2];

				if (Z_len < X_len) {
					// merge Y and Z
					struct ts_pair Z = runs[run_index - 3];
					merge(data, Z.start, Z.end, Y.end);
					runs[run_index - 3].end = runs[run_index - 2].end;
					runs[run_index - 2] = runs[run_index];
					run_index--;
				} else {
					// merge Y and X
					struct ts_pair X = runs[run_index - 1];
					merge(data, Y.start, Y.end, X.end);
					runs[run_index - 2].end = runs[run_index - 1].end;
					run_index--;
				}
			} else {
				// exit from the invariance-check loop
				break;
			}
		}
	}

	// final merge (every Y with the subsequent X, until only Y remains)
	while (run_index > 1) {
		merge(data, runs[run_index - 2].start, runs[run_index-2].end,
			  runs[run_index-1].end);
		runs[run_index - 2].end = runs[run_index-1].end;
		run_index--;
	}
}

/* TODO: this could be further optimized by: 
 * - leveraging only one buffer for all the merge calls;
 * - implementing galloping merge;
 * - computing the "minrun" parameter as suggested from wikipedia. 
 *
 * TODO: this algorithm should be particularly fast when data already has 
 * a certain structure, with "natural runs" (small contiguous sorted chunks);
 * it is not that hard to change the generator for testing this scenario.*/
void tim_sort(struct benchmark_input* binput) {
	int64_t* data = (int64_t*)binput->data;
	uint32_t size = (uint32_t)binput->size;

	_tim_sort(data, 0, size - 1);
}

void generate_array(struct benchmark_input* binput, uint32_t size) {
	binput->size = size;
	binput->data = malloc(size * sizeof(int64_t));

	int64_t* _data = (int64_t*)binput->data;

	for (uint64_t i = 0; i < size; i++) {
		_data[i] = rand();
	}
}

algorithm_ptr select_sorting_algorithm(char* algo_name) {
	if (strcmp(algo_name, "INSERTION") == 0) {
		return &insertion_sort;
	} else if (strcmp(algo_name, "MERGE") == 0) {
		return &merge_sort;
	} else if (strcmp(algo_name, "HMERGE") == 0) {
		return &hybrid_merge_sort;
	} else if (strcmp(algo_name, "QUICK") == 0) {
		return &quick_sort;
	} else if (strcmp(algo_name, "MOTQUICK") == 0) {
		return &mot_quick_sort;
	} else if (strcmp(algo_name, "TAILQUICK") == 0) {
		return &tail_quick_sort;
	} else if (strcmp(algo_name, "HTAILQUICK") == 0) {
		return &hybrid_tail_quick_sort;
	} else if (strcmp(algo_name, "TIM") == 0) {
		return &tim_sort;
	} else {
		printf("The provided algorithm (%s) is not available.", algo_name);
		exit(-1);
	}
}

int test_sort(struct benchmark_input* binput) {
	int64_t* _data = (int64_t*)binput->data;

	for (uint64_t i = 1; i < binput->size; i++) {
		if (_data[i] < _data[i - 1]) {
			return 0;
		}
	}

	return 1;
}
