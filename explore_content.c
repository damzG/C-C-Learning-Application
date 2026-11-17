#include "explore_content.h"

#include <stddef.h>

#define ARRAY_SIZE(a) ((int)(sizeof(a) / sizeof((a)[0])))

static const ExploreTopic DSA_TOPICS[] = {
    { "Arrays",
      "Contiguous memory and O(1) index access.",
      "An array stores elements of the same type in contiguous memory.\n"
      "Indexing is O(1). Insert or delete in the middle is O(n) because\n"
      "elements must be shifted. In C use int a[n]; in C++ prefer std::vector\n"
      "when the size can change. Watch off-by-one errors: valid indexes are 0..n-1.",
      "Access O(1) | Search O(n) | Insert mid O(n)" },

    { "Linked Lists",
      "Nodes linked by pointers; cheap insert at ends.",
      "Each node holds data and a pointer to the next node (and optionally prev).\n"
      "Insert/delete at a known node is O(1), but finding that node is O(n).\n"
      "No random access: walking to index i costs O(i). Always update head/tail\n"
      "and free removed nodes to avoid leaks.",
      "Access O(n) | Insert/Delete O(1)* | Search O(n)" },

    { "Stacks",
      "LIFO: push and pop from one end.",
      "A stack is Last-In-First-Out. Use cases: undo history, call stacks,\n"
      "matching brackets, DFS recursion simulation. Operations push/pop/peek\n"
      "are O(1). Implement with an array (top index) or linked list.",
      "Push/Pop/Peek O(1)" },

    { "Queues",
      "FIFO: enqueue rear, dequeue front.",
      "A queue is First-In-First-Out. Use cases: BFS, task scheduling,\n"
      "print queues. Circular buffers avoid shifting on dequeue. Priority\n"
      "queues order by priority (heaps) instead of arrival time.",
      "Enqueue/Dequeue O(1)" },

    { "Hash Tables",
      "Average O(1) lookup with a hash function.",
      "Map keys to buckets with a hash. Collisions are handled by chaining\n"
      "(linked lists) or open addressing. Average search/insert/delete is O(1);\n"
      "worst case O(n) if everything collides. Load factor and a good hash\n"
      "matter. In C++ use std::unordered_map.",
      "Avg O(1) | Worst O(n)" },

    { "Binary Search Trees",
      "Ordered tree: left < node < right.",
      "BST search, insert and delete are O(h) where h is height. Balanced trees\n"
      "keep h near log n. Unbalanced inserts can degrade to a linked list O(n).\n"
      "Inorder traversal visits keys in sorted order.",
      "Balanced O(log n) | Skewed O(n)" },

    { "Heaps",
      "Complete tree used for priority queues.",
      "A binary heap is a complete binary tree stored in an array. Parent of i\n"
      "is (i-1)/2. Min-heap: parent <= children. Insert and extract-min are\n"
      "O(log n). Building a heap from n items is O(n).",
      "Insert/Extract O(log n) | Peek O(1)" },

    { "Graphs",
      "Nodes and edges; BFS and DFS explore them.",
      "Represent with adjacency lists (sparse) or matrices (dense).\n"
      "BFS finds shortest paths in unweighted graphs. DFS explores deep paths,\n"
      "detects cycles, and supports topological sort on DAGs. Weighted shortest\n"
      "paths need Dijkstra or Bellman-Ford.",
      "BFS/DFS O(V+E)" },

    { "Sorting Overview",
      "Compare classic sorts and when to use each.",
      "Bubble/Selection/Insertion: simple, O(n^2), fine for tiny n.\n"
      "Merge sort: stable, O(n log n), needs extra memory.\n"
      "Quick sort: average O(n log n), in-place variants, worst O(n^2).\n"
      "Heap sort: O(n log n) guaranteed, not stable.\n"
      "Counting/radix: linear for integer keys in a limited range.",
      "Comparison lower bound Omega(n log n)" },

    { "Recursion & DP",
      "Break problems into overlapping subproblems.",
      "Recursion solves a problem using smaller instances of itself. Always\n"
      "define a base case. Dynamic programming caches subproblem answers\n"
      "(memoization or tabulation) to avoid exponential rework. Classic DP:\n"
      "fibonacci, knapsack, LCS, coin change.",
      "Naive fib O(2^n) | DP fib O(n)" }
};

static const ExploreTopic CS_TOPICS[] = {
    { "CPU & Instruction Cycle",
      "Fetch, decode, execute.",
      "The CPU repeatedly fetches an instruction from memory, decodes it,\n"
      "executes it, and writes results back. Registers are tiny ultra-fast\n"
      "storage. The program counter holds the next instruction address.\n"
      "Pipelining overlaps stages for throughput.",
      NULL },

    { "Memory Hierarchy",
      "Registers, cache, RAM, disk.",
      "Faster memory is smaller and closer to the CPU. Cache misses stall\n"
      "execution. Stack memory is automatic and scoped; heap is dynamic\n"
      "(malloc/new). Virtual memory lets processes share a large address\n"
      "space mapped onto physical RAM and disk.",
      NULL },

    { "Processes & Threads",
      "Isolation vs shared-memory concurrency.",
      "A process owns its address space. Threads share that space and need\n"
      "synchronisation (mutexes, atomics). Context switches cost time.\n"
      "Race conditions appear when shared data is updated without locks.",
      NULL },

    { "OS Scheduling",
      "Who runs next on the CPU?",
      "Schedulers pick ready threads: round-robin, priority, multilevel\n"
      "feedback queues. I/O-bound work yields; CPU-bound work is time-sliced.\n"
      "Deadlocks need careful lock ordering or try-lock strategies.",
      NULL },

    { "Networking Basics",
      "Packets, IP, TCP and ports.",
      "IP routes packets between hosts. TCP adds reliable ordered streams;\n"
      "UDP is connectionless and faster but unreliable. Ports identify\n"
      "services (e.g. 80/443 for web). DNS maps names to IP addresses.",
      NULL },

    { "File Systems",
      "Files, directories and persistence.",
      "A file system organises bytes on disk into files and folders with\n"
      "metadata (size, permissions, timestamps). Buffers and caches hide\n"
      "disk latency. Always check return codes when opening or writing files.",
      NULL },

    { "Compilers & Linking",
      "Source to executable.",
      "Compile translates source to object code. The linker resolves symbols\n"
      "across translation units and libraries. Preprocessor handles #include\n"
      "and macros before compilation. Headers declare; .c/.cpp files define.",
      NULL },

    { "Number Systems",
      "Binary, hex and two's complement.",
      "Computers store bits. Hex groups 4 bits for readable dumps. Two's\n"
      "complement represents negatives so addition hardware stays simple.\n"
      "Overflow wraps; signed overflow in C/C++ is undefined behaviour.",
      NULL }
};

static const ExploreTopic SYS_TOPICS[] = {
    { "Clients & Servers",
      "Request/response foundations.",
      "Clients send requests; servers process and respond. Stateless HTTP\n"
      "scales easily; sessions need cookies or tokens. Design APIs around\n"
      "clear resources and error codes.",
      NULL },

    { "Load Balancing",
      "Spread traffic across instances.",
      "A load balancer distributes requests (round-robin, least connections,\n"
      "hashing). Health checks remove bad backends. Horizontal scale adds\n"
      "machines; vertical scale adds CPU/RAM to one machine.",
      NULL },

    { "Caching",
      "Store hot data closer to users.",
      "Caches (CDN, Redis, in-process) cut latency and database load.\n"
      "Decide TTLs and invalidation carefully. Cache stampedes and stale\n"
      "reads are common pitfalls. Cache aside vs write-through patterns.",
      NULL },

    { "Databases",
      "SQL vs NoSQL trade-offs.",
      "Relational DBs give strong consistency and joins. Document/key-value\n"
      "stores favour flexible schemas and horizontal scale. Index hot query\n"
      "columns. Prefer transactions for multi-row integrity.",
      NULL },

    { "Replication & Sharding",
      "Copy data and split by key.",
      "Replication adds read replicas and failover. Sharding partitions data\n"
      "by a shard key. Cross-shard queries and rebalancing are hard. Pick\n"
      "keys that avoid hotspots.",
      NULL },

    { "Message Queues",
      "Async work between services.",
      "Producers enqueue jobs; consumers process them. Queues smooth spikes\n"
      "and decouple services. At-least-once delivery needs idempotent handlers.\n"
      "Dead-letter queues catch poison messages.",
      NULL },

    { "CAP Intuition",
      "Consistency, Availability, Partition tolerance.",
      "During a network partition you often choose between strong consistency\n"
      "and availability. Many systems offer tunable consistency. Know your\n"
      "product's correctness needs before copying a trendy architecture.",
      NULL },

    { "Observability",
      "Logs, metrics and traces.",
      "Logs explain events, metrics show trends, traces follow a request\n"
      "across services. Alert on symptoms users feel (latency, errors),\n"
      "not only on CPU graphs.",
      NULL }
};

static const ExploreTopic AI_TOPICS[] = {
    { "What is Machine Learning?",
      "Learn patterns from data instead of hand rules.",
      "ML algorithms fit a model to examples so they generalise to new inputs.\n"
      "Supervised learning uses labelled data; unsupervised finds structure;\n"
      "reinforcement learning learns from rewards. Feature quality often\n"
      "matters more than model choice for classical problems.",
      NULL },

    { "Train / Validation / Test",
      "Split data to measure real performance.",
      "Train to fit parameters, validate to tune hyperparameters, test once\n"
      "for an unbiased estimate. Leakage (test info in training) invents\n"
      "fake accuracy. Cross-validation helps on small datasets.",
      NULL },

    { "Overfitting & Underfitting",
      "Balance bias and variance.",
      "Overfit models memorise noise and fail on new data. Underfit models\n"
      "are too simple. Regularisation, more data, simpler models, and early\n"
      "stopping fight overfitting. Learning curves diagnose both.",
      NULL },

    { "Neural Networks Intro",
      "Layers of weighted sums and nonlinearities.",
      "Neurons compute weighted sums plus bias, then apply an activation\n"
      "(ReLU, sigmoid). Deep stacks learn hierarchical features. Backprop\n"
      "and gradient descent update weights to reduce a loss.",
      NULL },

    { "Loss & Optimisers",
      "Measure error and step downhill.",
      "Loss functions quantify wrongness (MSE, cross-entropy). Optimisers\n"
      "(SGD, Adam) update weights using gradients. Learning rate is critical:\n"
      "too high diverges; too low crawls.",
      NULL },

    { "Classical Algorithms",
      "Still useful before deep learning.",
      "Linear/logistic regression, k-NN, decision trees, random forests and\n"
      "SVMs solve many tabular problems with less compute and more\n"
      "interpretability than deep nets.",
      NULL },

    { "Ethics & Safety",
      "Bias, privacy and misuse.",
      "Models amplify biases in training data. Protect personal data.\n"
      "Document intended use and failure modes. For student projects, keep\n"
      "datasets public and non-sensitive whenever possible.",
      NULL },

    { "AI Meets C/C++",
      "Where systems programming fits.",
      "Inference engines, game AI, embedded ML and high-performance kernels\n"
      "are often written in C/C++. Understanding memory and numerics helps\n"
      "you read library code and profile bottlenecks.",
      NULL }
};

static const VizInfo VIZ_LIST[] = {
    { VIZ_BUBBLE_SORT,     "Bubble Sort",      "Watch adjacent swaps bubble maxima to the end." },
    { VIZ_SELECTION_SORT,  "Selection Sort",   "Select the next minimum and place it in order." },
    { VIZ_INSERTION_SORT,  "Insertion Sort",   "Insert each value into the sorted prefix." },
    { VIZ_BINARY_SEARCH,   "Binary Search",    "Halve the search space on a sorted array." },
    { VIZ_STACK,           "Stack Ops",        "Push and pop values on a LIFO stack." },
    { VIZ_QUEUE,           "Queue Ops",        "Enqueue and dequeue on a FIFO queue." },
    { VIZ_LINKED_LIST,     "Linked List",      "Insert and walk nodes linked by pointers." },
    { VIZ_BST,             "Binary Search Tree","Insert keys and see left/right ordering." },
    { VIZ_BFS,             "BFS Traversal",    "Explore a grid level by level." },
    { VIZ_DFS,             "DFS Traversal",    "Explore a grid by diving deep first." }
};

const char *ExploreCategoryName(ExploreCategory cat)
{
    static const char *names[] = {
        "Data Structures & Algorithms",
        "CS Fundamentals",
        "System Design",
        "Artificial Intelligence"
    };
    if (cat < 0 || cat >= EXPLORE_CAT_COUNT) return "";
    return names[cat];
}

const char *ExploreCategoryBlurb(ExploreCategory cat)
{
    static const char *blurbs[] = {
        "Visualise structures and classic algorithms step by step.",
        "CPU, memory, OS, networking and how programs really run.",
        "Build intuition for scalable services and data paths.",
        "Core ML ideas every programmer should recognise."
    };
    if (cat < 0 || cat >= EXPLORE_CAT_COUNT) return "";
    return blurbs[cat];
}

static const ExploreTopic *TopicTable(ExploreCategory cat, int *count)
{
    switch (cat)
    {
    case EXPLORE_DSA: *count = ARRAY_SIZE(DSA_TOPICS); return DSA_TOPICS;
    case EXPLORE_CS:  *count = ARRAY_SIZE(CS_TOPICS);  return CS_TOPICS;
    case EXPLORE_SYS: *count = ARRAY_SIZE(SYS_TOPICS); return SYS_TOPICS;
    case EXPLORE_AI:  *count = ARRAY_SIZE(AI_TOPICS);  return AI_TOPICS;
    default:          *count = 0; return 0;
    }
}

int ExploreTopicCount(ExploreCategory cat)
{
    int n = 0;
    TopicTable(cat, &n);
    return n;
}

const ExploreTopic *ExploreTopicGet(ExploreCategory cat, int index)
{
    int n = 0;
    const ExploreTopic *t = TopicTable(cat, &n);
    if (!t || index < 0 || index >= n) return 0;
    return &t[index];
}

int VizCount(void)
{
    return ARRAY_SIZE(VIZ_LIST);
}

const VizInfo *VizGet(int index)
{
    if (index < 0 || index >= VizCount()) return 0;
    return &VIZ_LIST[index];
}
