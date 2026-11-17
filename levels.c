#include "levels.h"

static const char *SECTION_NAMES[] = {
    "Programming Basics",
    "Pointers and Memory",
    "Structs and Classes",
    "Data Structures and STL",
    "Final Challenge"
};

static const Level LEVELS[] = {
    { 0, "Variables and Types", "Declare variables, learn int, double, char and bool, and print output with printf and cout.", 50 },
    { 0, "Loops and Functions", "Write for and while loops and split code into reusable functions. Practise with leap year and palindrome checks.", 50 },
    { 0, "Arrays", "Store lists of values, loop over them, and solve lab problems such as find, insert, delete and reverse.", 60 },
    { 0, "Strings and Text", "Work with character arrays and count the words in a block of text.", 60 },
    { 1, "Pointer Basics", "Addresses, the & and * operators, and why nullptr matters.", 70 },
    { 1, "Pointer Arithmetic", "Walk an array with ptr + i and see how a pointer moves through memory.", 70 },
    { 1, "Dynamic Memory", "Use new and delete, spot memory leaks, and learn the difference between the stack and the heap.", 80 },
    { 2, "Structs", "Group related data into a struct and pass it by reference to a function.", 70 },
    { 2, "Classes and Objects", "Build classes with constructors, members, getters and setters, and use the this pointer.", 80 },
    { 2, "Linked Lists", "Create nodes, insert at the end, traverse the list and remove a value without losing the rest.", 100 },
    { 3, "Templates", "Write one function or class that works with many types.", 90 },
    { 3, "STL Vectors", "Use std::vector as a dynamic array: push_back, size, iterators and loops.", 90 },
    { 3, "STL Lists", "Use std::list, learn when it beats a vector, and traverse it with iterators.", 90 },
    { 3, "STL Maps", "Store key and value pairs with std::map and look values up by key.", 100 },
    { 3, "CS50 Memory Lab", "Inspired by CS50: use pointers, bytes and heap allocation safely.", 100 },
    { 3, "W3Schools Functions Lab", "Practise pass-by-value, pass-by-reference and function overloads.", 90 },
    { 3, "LeetCode Two Pointers", "Solve sorted-array and string problems using opposite-direction pointers.", 110 },
    { 3, "LeetCode Sliding Window", "Maintain a moving range to solve substring and subarray problems.", 110 },
    { 3, "LeetCode Binary Search", "Search sorted space and binary-search an answer predicate.", 110 },
    { 3, "Recursion & Backtracking", "Use a base case, explore choices and undo state safely.", 120 },
    { 3, "Project Euler Maths", "Solve integer and divisibility problems with efficient C++ loops.", 120 },
    { 3, "Embedded Memory Lab", "Practise fixed-width integers, bitfields, volatile values and register masks.", 130 },
    { 4, "Concurrency Basics", "Explore threads, mutexes, atomics and race-condition prevention.", 130 },
    { 4, "Smart Pointers", "Use unique_ptr and shared_ptr instead of manual ownership when possible.", 120 },
    { 4, "Design Patterns", "Apply RAII, factory, observer and strategy in small C++ examples.", 130 },
    { 4, "C Make & CMake", "Build multi-file C/C++ projects with headers, libraries and tests.", 120 },
    { 4, "Debugging Quest", "Find leaks, crashes, undefined behaviour and off-by-one bugs.", 130 },
    { 4, "Boss Challenge", "Combine pointers, classes and the STL in one final mixed challenge.", 150 }
};

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

int LevelCount(void)
{
    return ARRAY_SIZE(LEVELS);
}

const Level *LevelGet(int index)
{
    if (index < 0 || index >= LevelCount()) return 0;
    return &LEVELS[index];
}

int SectionCount(void)
{
    return ARRAY_SIZE(SECTION_NAMES);
}

const char *SectionName(int section)
{
    if (section < 0 || section >= SectionCount()) return "";
    return SECTION_NAMES[section];
}