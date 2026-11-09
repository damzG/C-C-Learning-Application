// Question bank and lesson scoring: raylib-free.
#ifndef QUESTIONS_H
#define QUESTIONS_H

#include <stdbool.h>

#define OPTION_COUNT 4
#define LESSON_MAX_QUESTIONS 8
#define PASS_PERCENT 60

typedef enum {
    Q_CONCEPT,   // multiple choice about an idea
    Q_OUTPUT,    // what does this code print?
    Q_BUG        // spot the bug
} QuestionKind;

typedef struct {
    int level;                       // index into the level list
    QuestionKind kind;
    const char *prompt;
    const char *code;                // lines separated by \n, or NULL
    const char *options[OPTION_COUNT];
    int correct;                     // index into options
    const char *explanation;
    const char *hint;
} Question;

const char *QuestionKindName(QuestionKind kind);

int QuestionCount(int level);
const Question *QuestionGet(int level, int index);   // index within the level, or NULL

// Smallest score that passes a lesson of this size
int LessonPassMark(int totalQuestions);
bool LessonPassed(int score, int totalQuestions);

#endif