// DSAlgo-inspired explore content: raylib-free topic banks.
#ifndef EXPLORE_CONTENT_H
#define EXPLORE_CONTENT_H

typedef enum {
    EXPLORE_DSA = 0,
    EXPLORE_CS,
    EXPLORE_SYS,
    EXPLORE_AI,
    EXPLORE_CAT_COUNT
} ExploreCategory;

typedef enum {
    VIZ_BUBBLE_SORT = 0,
    VIZ_SELECTION_SORT,
    VIZ_INSERTION_SORT,
    VIZ_BINARY_SEARCH,
    VIZ_STACK,
    VIZ_QUEUE,
    VIZ_LINKED_LIST,
    VIZ_BST,
    VIZ_BFS,
    VIZ_DFS,
    VIZ_COUNT
} VizId;

typedef struct {
    const char *title;
    const char *blurb;
    const char *body;       // multi-line study text (\n separated)
    const char *complexity; // optional Big-O note, or NULL
} ExploreTopic;

typedef struct {
    VizId id;
    const char *title;
    const char *blurb;
} VizInfo;

const char *ExploreCategoryName(ExploreCategory cat);
const char *ExploreCategoryBlurb(ExploreCategory cat);

int ExploreTopicCount(ExploreCategory cat);
const ExploreTopic *ExploreTopicGet(ExploreCategory cat, int index);

int VizCount(void);
const VizInfo *VizGet(int index);

#endif
