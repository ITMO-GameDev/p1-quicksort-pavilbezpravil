#pragma once

#include <algorithm>

template <typename T, typename Comp>
void insertionSort(const T first, const T last, Comp comp) {
    for (auto iterSortedPart = first; iterSortedPart < last; ++iterSortedPart) {
        for (auto iter = iterSortedPart; iter > first; --iter) {
            auto iterPrev = iter - 1;
            if (comp(*iter, *(iterPrev))) {
                std::swap(*iter, *(iterPrev));
            } else {
                break;
            }
        }
    }
}

template <typename T, typename Comp>
T mypartition(T first, T last, T pivot, Comp comp) {
    auto pivotVal = *pivot;
    std::swap(*pivot, *--last);
    pivot = last;
    while (first < last) {
        if (comp(*first, pivotVal)) {
            ++first;
        } else {
            std::swap(*first, *--last);
        }
    }
    std::swap(*pivot, *first);
    return first;
}

template <typename T, typename Comp>
void mysort(const T first, const T last, Comp comp) {
    auto n = std::distance(first, last);
    if (n < 2) {
        return;
    }
    T pivot = mypartition(first, last, first + n / 2, comp);

    auto n1 = std::distance(first, pivot);
    auto n2 = std::distance(pivot + 1, last);
    if (n1 > n2) {
        mysort(pivot + 1, last, comp);
        mysort(first, pivot, comp);
    } else {
        mysort(first, pivot, comp);
        mysort(pivot + 1, last, comp);
    }
}
