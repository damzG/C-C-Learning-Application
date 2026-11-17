#include "study_content.h"

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

static const CheatSheet SHEETS[] = {
    { SHEET_C, "C Language", "Syntax and essentials",
      "printf / scanf for I/O\n"
      "Types: int, long, float, double, char, void*\n"
      "Qualifiers: const, static, extern, unsigned\n"
      "Operators: + - * / %  & | ^ ~ << >>\n"
      "Compare: == != < > <= >=    Logic: && || !\n"
      "Control: if/else, switch, for, while, do-while\n"
      "Functions: returnType name(params) { ... }\n"
      "Arrays: type name[size];  name[i]\n"
      "Strings: char s[];  strlen strcpy strcat strcmp\n"
      "Pointers: *p dereference, &x address, p+1 next\n"
      "Structs: struct S { ... };  s.member  ps->member\n"
      "Memory: malloc(n)  realloc  free  sizeof\n"
      "Files: fopen fread fwrite fprintf fclose\n"
      "Headers: stdio.h stdlib.h string.h math.h\n"
      "Undefined behaviour: OOB access, use-after-free, signed overflow" },

    { SHEET_CPP, "C++ Language", "Modern C++ quick hits",
      "I/O: #include <iostream>  cin >>  cout <<\n"
      "Strings: #include <string>  std::string\n"
      "References: int& r = x;  alias, not null\n"
      "new / delete  and  new[] / delete[]\n"
      "nullptr instead of NULL\n"
      "Namespaces: namespace N { }  using N::x;\n"
      "Overloading: same name, different params\n"
      "Default args: void f(int x = 0);\n"
      "STL: vector list map set unordered_map stack queue\n"
      "vector: push_back size empty [] at begin end\n"
      "Range-for: for (auto& x : container)\n"
      "auto type deduction\n"
      "constexpr / const for immutability\n"
      "RAII: acquire resources in constructors\n"
      "Prefer containers + algorithms over raw loops when clear" },

    { SHEET_OOP, "OOP", "Objects, classes and relations",
      "Class = blueprint; object = instance\n"
      "Encapsulation: hide data, expose methods\n"
      "Access: public protected private\n"
      "Constructor initialises; destructor cleans up\n"
      "this pointer: address of current object\n"
      "Inheritance: class D : public B\n"
      "virtual functions + override for polymorphism\n"
      "Abstract base: pure virtual = 0\n"
      "Composition: has-a (prefer over deep inheritance)\n"
      "Aggregation / association for looser links\n"
      "Copy ctor & operator= for deep copies\n"
      "Rule of Three/Five: copy/move/destructor together\n"
      "Interfaces via abstract classes\n"
      "SOLID: SRP OCP LSP ISP DIP (design compass)" },

    { SHEET_COMPLEXITY, "Complexities", "Big-O cheat card",
      "O(1) constant: map/unordered_map avg, array index\n"
      "O(log n) logarithmic: binary search, balanced BST\n"
      "O(n) linear: single scan, linear search\n"
      "O(n log n) linearithmic: efficient comparison sorts\n"
      "O(n^2) quadratic: nested loops, simple sorts\n"
      "O(2^n) exponential: naive recursion subsets\n"
      "O(n!) factorial: brute-force permutations\n"
      "Space complexity counts extra memory, not input\n"
      "Best / average / worst cases can differ (quicksort)\n"
      "Amortized: vector push_back average O(1)\n"
      "Array access O(1) | insert mid O(n)\n"
      "Linked list insert O(1)* | access O(n)\n"
      "Hash avg O(1) | worst O(n)\n"
      "Heap insert/extract O(log n)\n"
      "Graph BFS/DFS O(V+E)" },

    { SHEET_PATTERNS, "Patterns", "Coding patterns that recur",
      "Two pointers: start/end move toward a goal\n"
      "Sliding window: maintain a range sum/count\n"
      "Prefix sums: range queries in O(1) after O(n)\n"
      "Binary search on answer / monotonic predicate\n"
      "Fast & slow pointers: cycles in lists\n"
      "DFS / BFS on trees and graphs\n"
      "Backtracking: build, explore, undo\n"
      "Dynamic programming: optimal substructure\n"
      "Greedy: local best with proof or counterexample\n"
      "Hashing for frequency / presence tests\n"
      "Sorting then scanning simplifies many tasks\n"
      "Stack for next-greater / matching pairs\n"
      "Union-Find for connectivity\n"
      "Topological sort for dependency order\n"
      "Bit tricks: flags, masks, parity (use carefully)" },

    { SHEET_CS_FUNDAMENTALS, "CS Fundamentals", "Visual memory & systems map",
      "CPU  -->  Fetch --> Decode --> Execute --> Write back\n"
      "Registers -> Cache -> RAM -> Disk (fast to slow)\n"
      "Process: isolated address space\n"
      "Thread: shared process memory + its own stack\n"
      "TCP: reliable, ordered stream  |  UDP: faster, datagrams\n"
      "Virtual memory: pages + page table + disk backing\n"
      "Stack: automatic  |  Heap: malloc/new  |  Static: globals\n"
      "Compiler: source -> object code -> linker -> executable\n"
      "Binary 1011 = Hex B = 11 decimal" },

    { SHEET_SYSTEM_DESIGN, "System Design", "Architecture diagram notes",
      "Client --> Load Balancer --> [App Server 1 | App Server 2]\n"
      "App servers --> Cache --> Database\n"
      "App servers --> Message Queue --> Workers\n"
      "Database --> Replicas | Partition key --> Shards\n"
      "Cache: cache-aside, TTL and invalidation\n"
      "Replication: availability  |  Sharding: horizontal scale\n"
      "Queue: at-least-once delivery + idempotent handlers\n"
      "Observability: logs + metrics + distributed traces" },

    { SHEET_AI_KNOWLEDGE, "AI Knowledge", "Visual learning loop",
      "Data --> Features --> Model --> Prediction --> Loss\n"
      "Loss --> Gradient --> Optimiser --> Updated model\n"
      "Train: fit weights | Validate: tune | Test: final estimate\n"
      "Overfitting: memorises noise  |  Underfitting: too simple\n"
      "Bias-variance: balance model flexibility and data\n"
      "Neural net: weighted sum -> activation -> next layer\n"
      "C/C++: fast kernels, embedded inference and game AI\n"
      "Ethics: privacy, bias, safety and clear intended use" }
};

static const FlashCard CARDS_C[] = {
    { "What does % do in C?", "Remainder (modulo) after integer division." },
    { "Valid indexes for int a[5]?", "0, 1, 2, 3, 4" },
    { "What is \\0 in a C string?", "The null terminator marking the end." },
    { "Stack vs heap allocation?", "Stack is automatic/scoped; heap is dynamic via malloc/new." },
    { "What does scanf(\"%d\", &x) need?", "The address of x (use & for non-pointer scalars)." },
    { "sizeof(char) in C?", "1 by definition." },
    { "What is a segmentation fault often from?", "Invalid pointer dereference or out-of-bounds access." },
    { "Header for printf?", "stdio.h" }
};

static const FlashCard CARDS_PTR[] = {
    { "What does &x mean?", "The address of variable x." },
    { "What does *p mean?", "The value at the address stored in p (dereference)." },
    { "What is nullptr?", "A pointer that points to nothing." },
    { "new int[10] must be freed with?", "delete[]" },
    { "What is a dangling pointer?", "A pointer to memory that was already freed." },
    { "p + 1 on int* moves how far?", "One int forward (sizeof(int) bytes)." },
    { "Why set pointer = nullptr after delete?", "So you do not accidentally use freed memory." },
    { "Pass by pointer lets a function...?", "Modify the caller's data (and avoid large copies)." }
};

static const FlashCard CARDS_OOP[] = {
    { "Encapsulation means?", "Bundling data with methods and hiding internals." },
    { "Constructor role?", "Initialise a new object." },
    { "What is this?", "Pointer to the current object instance." },
    { "Inheritance expresses?", "Is-a relationships and reused behaviour." },
    { "virtual enables?", "Runtime polymorphism via overrides." },
    { "Composition means?", "Has-a: objects contain other objects." },
    { "Destructor role?", "Release resources when an object dies." },
    { "Abstract class has?", "At least one pure virtual function." }
};

static const FlashCard CARDS_DSA[] = {
    { "Stack order?", "LIFO — last in, first out." },
    { "Queue order?", "FIFO — first in, first out." },
    { "BST left child is always?", "Less than the parent key." },
    { "BFS uses which structure?", "A queue." },
    { "DFS can use?", "A stack or recursion." },
    { "Hash table average lookup?", "O(1)" },
    { "Binary search needs?", "A sorted array (or monotonic space)." },
    { "Linked list access by index?", "O(n) — must walk nodes." }
};

static const FlashCard CARDS_BIGO[] = {
    { "Binary search time?", "O(log n)" },
    { "Simple nested loop over n?", "O(n^2)" },
    { "Merge sort time?", "O(n log n)" },
    { "Array index access?", "O(1)" },
    { "Linear search worst case?", "O(n)" },
    { "Heap extract-min?", "O(log n)" },
    { "BFS complexity?", "O(V + E)" },
    { "Comparison sort lower bound?", "Omega(n log n)" }
};

static const FlashCard CARDS_SYS[] = {
    { "Load balancer role?", "Distribute traffic across healthy application instances." },
    { "Cache-aside flow?", "Check cache, read DB on miss, then populate cache." },
    { "Replication trade-off?", "More availability/reads; replica consistency adds complexity." },
    { "Sharding key goal?", "Distribute data evenly and avoid hotspots." },
    { "Queue delivery?", "At-least-once needs idempotent consumers." },
    { "BFS shortest path?", "Use a queue for unweighted graphs." }
};

static const FlashCard CARDS_AI[] = {
    { "Supervised learning?", "Learn a function from labelled examples." },
    { "Validation set?", "Tune choices without using the final test set." },
    { "Overfitting?", "Good training fit but poor performance on new data." },
    { "Gradient descent?", "Update parameters downhill using the loss gradient." },
    { "ReLU?", "A nonlinear activation that passes positive values." },
    { "C/C++ AI role?", "Fast inference kernels, embedded ML and engines." }
};

static const FlashDeck DECKS[] = {
    { DECK_C_BASICS,    "C Basics",     "Syntax, types and pitfalls",           ARRAY_SIZE(CARDS_C),    CARDS_C },
    { DECK_POINTERS,    "Pointers",     "Addresses, new/delete, safety",        ARRAY_SIZE(CARDS_PTR),  CARDS_PTR },
    { DECK_OOP,         "OOP",          "Classes and object principles",        ARRAY_SIZE(CARDS_OOP),  CARDS_OOP },
    { DECK_DSA,         "DSA",          "Structures and traversals",            ARRAY_SIZE(CARDS_DSA),  CARDS_DSA },
    { DECK_COMPLEXITY,  "Complexities", "Big-O flash drills",                   ARRAY_SIZE(CARDS_BIGO), CARDS_BIGO },
    { DECK_SYSTEM_DESIGN, "System Design", "Scale and architecture recall",      ARRAY_SIZE(CARDS_SYS),  CARDS_SYS },
    { DECK_AI,           "AI Knowledge", "ML loop and safety recall",          ARRAY_SIZE(CARDS_AI),  CARDS_AI }
};

int CheatSheetCount(void)
{
    return ARRAY_SIZE(SHEETS);
}

const CheatSheet *CheatSheetGet(int index)
{
    if (index < 0 || index >= CheatSheetCount()) return 0;
    return &SHEETS[index];
}

int FlashDeckCount(void)
{
    return ARRAY_SIZE(DECKS);
}

const FlashDeck *FlashDeckGet(int index)
{
    if (index < 0 || index >= FlashDeckCount()) return 0;
    return &DECKS[index];
}
