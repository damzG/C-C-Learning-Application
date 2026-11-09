#include "questions.h"

#define NO_CODE 0

static const Question QUESTIONS[] = {
    // ---- Level 0: Variables and Types ----
    { 0, Q_CONCEPT, "Which type is best for storing the value 3.14?", NO_CODE,
      { "int", "char", "double", "bool" }, 2,
      "double stores numbers with a decimal point. An int would throw away the .14.",
      "Think about which type can hold a decimal point." },
    { 0, Q_OUTPUT, "What does this code print?",
      "int a = 7;\nint b = 2;\nprintf(\"%d\", a / b);",
      { "3.5", "3", "4", "2" }, 1,
      "Both values are ints, so the division is integer division and the remainder is dropped: 7 / 2 is 3.",
      "Both numbers are ints. What happens to the .5?" },
    { 0, Q_BUG, "What is wrong with this code?",
      "int age = \"23\";\nprintf(\"%d\", age);",
      { "printf cannot print an int", "A string is assigned to an int variable", "age is a reserved word", "Nothing, it compiles fine" }, 1,
      "\"23\" is text, not a number. An int needs 23 without the quotes.",
      "Look at the quotes around 23." },

    // ---- Level 1: Loops and Functions ----
    { 1, Q_OUTPUT, "What does this code print?",
      "int sum = 0;\nfor (int i = 1; i <= 4; i++)\n    sum += i;\nprintf(\"%d\", sum);",
      { "6", "4", "15", "10" }, 3,
      "The loop adds 1 + 2 + 3 + 4, which is 10.",
      "Write out the value of i on each pass of the loop." },
    { 1, Q_BUG, "This leap year function has a bug. What is it?",
      "bool isLeapYear(int year) {\n    if (year % 4 == 0)\n        return true;\n}",
      { "year % 4 is invalid syntax", "A bool function cannot return true", "There is no return when the year is not divisible by 4", "The function needs a loop" }, 2,
      "If the year is not divisible by 4 the function reaches the end without returning anything, which is undefined behaviour.",
      "What happens when the if condition is false?" },
    { 1, Q_CONCEPT, "To reverse the digits of 123, which operator gives you the last digit (3)?", NO_CODE,
      { "/ 10", "% 10", "* 10", "+ 10" }, 1,
      "123 % 10 is 3. Then 123 / 10 is 12, which drops that digit so you can repeat.",
      "You want the remainder after dividing by 10." },

    // ---- Level 2: Arrays ----
    { 2, Q_CONCEPT, "An array is declared as int arr[5]. What is the index of its last element?", NO_CODE,
      { "5", "3", "6", "4" }, 3,
      "Indexes start at 0, so a 5 element array uses 0 to 4.",
      "The first index is 0, not 1." },
    { 2, Q_OUTPUT, "What does this code print?",
      "int a[] = {5, 3, 9};\nprintf(\"%d\", a[1] + a[2]);",
      { "8", "12", "14", "17" }, 1,
      "a[1] is 3 and a[2] is 9, so the sum is 12.",
      "Index 0 is the first value, so index 1 is the second." },
    { 2, Q_BUG, "This loop is meant to shift an array one place to the right. What is the bug?",
      "for (int i = size; i > 0; i--)\n    arr[i] = arr[i - 1];",
      { "It should count up instead of down", "arr[i - 1] is invalid syntax", "It starts at size, writing past the end of the array", "It never reads arr[0]" }, 2,
      "Valid indexes are 0 to size - 1, so arr[size] is outside the array. The loop should start at size - 1.",
      "Valid indexes run from 0 to size - 1." },

    // ---- Level 3: Strings and Text ----
    { 3, Q_CONCEPT, "How does C mark the end of a character string?", NO_CODE,
      { "A newline character", "A space", "The length stored at the front", "The null character '\\0'" }, 3,
      "Strings end with the null character. Functions like printf and strlen keep reading until they find it.",
      "It is an invisible character with the value 0." },
    { 3, Q_OUTPUT, "What does this code print?",
      "char word[] = \"Hello\";\nprintf(\"%c\", word[1]);",
      { "H", "e", "l", "o" }, 1,
      "word[0] is H, so word[1] is e.",
      "Count from 0." },
    { 3, Q_CONCEPT, "A word counter uses a flag called inWord. When should it add 1 to the count?", NO_CODE,
      { "Every time it sees a letter", "Every time it sees a space", "When it sees a letter and inWord was false", "When it reaches the end of the text" }, 2,
      "A new word starts when a letter appears right after a non-letter. The flag stops the same word being counted for every letter.",
      "You only want to count at the start of each word." },

    // ---- Level 4: Pointer Basics ----
    { 4, Q_CONCEPT, "What does the & operator give you?", NO_CODE,
      { "The value stored at an address", "The size of a variable", "A copy of the variable", "The address of a variable" }, 3,
      "&x is the memory address of x. The * operator does the opposite and reads the value at an address.",
      "Pointers store addresses. How do you get one?" },
    { 4, Q_OUTPUT, "What does this code print?",
      "int x = 10;\nint *p = &x;\n*p = 25;\nprintf(\"%d\", x);",
      { "10", "25", "An address", "It will not compile" }, 1,
      "p points at x, so *p = 25 changes x itself.",
      "Writing through *p changes the variable p points to." },
    { 4, Q_CONCEPT, "What is the safest value for a pointer that has no address yet?", NO_CODE,
      { "A random number", "nullptr", "Its own address", "-1" }, 1,
      "nullptr lets you check if (p != nullptr) before using the pointer, so you never dereference garbage.",
      "Pick the value that means 'points to nothing'." },

    // ---- Level 5: Pointer Arithmetic ----
    { 5, Q_OUTPUT, "What does this code print?",
      "int n[] = {10, 20, 30};\nint *p = n;\nprintf(\"%d\", *(p + 2));",
      { "10", "20", "30", "An address" }, 2,
      "p + 2 moves two ints along the array, so *(p + 2) is n[2], which is 30.",
      "p + 2 is the same as &n[2]." },
    { 5, Q_CONCEPT, "p is an int* holding address 1000 and an int is 4 bytes. What address is p + 1?", NO_CODE,
      { "1001", "1002", "1004", "1008" }, 2,
      "Pointer arithmetic moves by whole elements, not single bytes, so p + 1 is 1000 + 4.",
      "Adding 1 moves one int, not one byte." },
    { 5, Q_BUG, "What is wrong with this code?",
      "int n[] = {1, 2, 3};\nint *p = n;\nfor (int i = 0; i < 3; i++)\n    p++;\nprintf(\"%d\", *p);",
      { "p++ is not allowed on pointers", "The loop runs four times", "n is not an array", "p has moved past the end of the array, so *p reads invalid memory" }, 3,
      "After three increments p points one past the last element. Reading it is undefined behaviour.",
      "Where does p point after the loop?" },

    // ---- Level 6: Dynamic Memory ----
    { 6, Q_CONCEPT, "Which keyword releases memory that was created with new int?", NO_CODE,
      { "remove", "clear", "delete", "release" }, 2,
      "delete frees heap memory from new. Forgetting it causes a memory leak.",
      "It is the partner of new." },
    { 6, Q_BUG, "The memory was created with new[]. What should the last line be?",
      "char *grades = new char[10];\n// ... use grades ...\ndelete grades;",
      { "delete grades; is correct", "grades = nullptr;", "free(grades);", "delete[] grades;" }, 3,
      "Memory from new[] must be released with delete[]. Setting the pointer to nullptr alone would leak the memory.",
      "Arrays need the array form of delete." },
    { 6, Q_CONCEPT, "Memory created with new lives in which area?", NO_CODE,
      { "The stack", "A CPU register", "The heap", "Read-only memory" }, 2,
      "new allocates on the heap. It stays until you delete it, unlike stack variables that vanish when the function ends.",
      "It is not the area used for normal local variables." },

    // ---- Level 7: Structs ----
    { 7, Q_CONCEPT, "Which operator accesses a member of a struct variable named student?", NO_CODE,
      { "->", "::", ".", "&" }, 2,
      "Use student.name for a struct variable. The -> operator is for pointers to structs.",
      "It is a single character." },
    { 7, Q_OUTPUT, "What does this code print?",
      "struct Point { int x; int y; };\nPoint p;\np.x = 3;\np.y = 4;\nprintf(\"%d\", p.x * p.y);",
      { "7", "12", "34", "It will not compile" }, 1,
      "3 * 4 is 12.",
      "The code multiplies the two members." },
    { 7, Q_CONCEPT, "Why does void ScoreStudent(Student &student, double g) change the caller's struct?", NO_CODE,
      { "Structs are always passed by pointer", "It takes a reference, so no copy is made", "student is a global variable", "Because the function returns void" }, 1,
      "The & makes student a reference to the original struct, so changes happen to the caller's data.",
      "Look at the & in the parameter." },

    // ---- Level 8: Classes and Objects ----
    { 8, Q_CONCEPT, "What is a constructor?", NO_CODE,
      { "A function that deletes an object", "A variable shared by every object", "A special function that runs when an object is created", "A kind of loop" }, 2,
      "A constructor has the same name as the class and sets up a new object, for example by filling in its members.",
      "It runs at the moment an object is created." },
    { 8, Q_OUTPUT, "What does this code print?",
      "class Counter { public:\n    int n = 0;\n    void add() { n++; } };\nCounter c;\nc.add(); c.add();\nprintf(\"%d\", c.n);",
      { "0", "1", "2", "It will not compile" }, 2,
      "n starts at 0 and add() is called twice.",
      "How many times is add() called?" },
    { 8, Q_CONCEPT, "In a constructor, what does this->name = name; do?", NO_CODE,
      { "Declares a new variable", "Compares the two values", "Deletes the member", "Copies the parameter into the object's member" }, 3,
      "this->name is the member. The plain name is the parameter, so the line stores the parameter in the object.",
      "this points at the object being built." },

    // ---- Level 9: Linked Lists ----
    { 9, Q_CONCEPT, "What does the next pointer of the last node in a singly linked list hold?", NO_CODE,
      { "The head", "nullptr", "Itself", "The previous node" }, 1,
      "nullptr marks the end of the list. Traversal loops stop when they reach it.",
      "How does a traversal loop know when to stop?" },
    { 9, Q_CONCEPT, "After unlinking a node inside remove(), what must you do with it?", NO_CODE,
      { "Set head to nullptr", "Print it", "Nothing, it is cleaned up automatically", "delete it" }, 3,
      "Nodes are created with new, so they must be freed with delete or the memory leaks.",
      "Every new needs a matching delete." },
    { 9, Q_OUTPUT, "What does traverse() print after these calls?",
      "list.insert(10);\nlist.insert(20);\nlist.insert(30);\nlist.remove(20);\nlist.traverse();",
      { "10, 20", "20, 30", "10, 30", "30" }, 2,
      "The node holding 20 is unlinked, so the list is 10 then 30.",
      "Which value was removed?" },

    // ---- Level 10: Templates ----
    { 10, Q_CONCEPT, "What does template <typename T> do?", NO_CODE,
      { "Creates a variable named T", "Imports a library", "Lets one function or class work with many types", "Makes code run faster at runtime" }, 2,
      "T is a placeholder type. The compiler creates a version for each type you use.",
      "T stands for 'some type'." },
    { 10, Q_OUTPUT, "What does this code print?",
      "template <typename T>\nT bigger(T a, T b) { return a > b ? a : b; }\nprintf(\"%d\", bigger(3, 8));",
      { "3", "11", "8", "It will not compile" }, 2,
      "T becomes int and the function returns the larger value, 8.",
      "The function returns whichever argument is larger." },
    { 10, Q_BUG, "Why does bigger(3, 2.5) fail to compile?",
      "template <typename T>\nT bigger(T a, T b) { return a > b ? a : b; }\nauto r = bigger(3, 2.5);",
      { "Templates cannot take numbers", "T is deduced as both int and double, which conflicts", "3 is not a valid int", "bigger must return void" }, 1,
      "The two arguments give different types for T. Writing bigger<double>(3, 2.5) fixes it.",
      "Look at the types of 3 and 2.5." },

    // ---- Level 11: STL Vectors ----
    { 11, Q_CONCEPT, "Which member function adds an element to the end of a vector?", NO_CODE,
      { "append", "add", "insert_end", "push_back" }, 3,
      "push_back adds to the end and grows the vector automatically.",
      "It is named after the action." },
    { 11, Q_OUTPUT, "What does this code print?",
      "std::vector<int> v = {4, 5, 6};\nv.push_back(7);\nprintf(\"%zu\", v.size());",
      { "3", "4", "7", "It will not compile" }, 1,
      "The vector starts with 3 elements and push_back adds a fourth.",
      "How many elements are there after push_back?" },
    { 11, Q_BUG, "What is the bug in this loop?",
      "std::vector<int> v = {1, 2, 3};\nfor (int i = 0; i <= v.size(); i++)\n    printf(\"%d \", v[i]);",
      { "v.size() is not valid", "int cannot index a vector", "The <= reads one element past the end", "push_back is missing" }, 2,
      "With 3 elements the last index is 2. The loop should use i < v.size().",
      "Compare <= with <." },

    // ---- Level 12: STL Lists ----
    { 12, Q_CONCEPT, "std::list is a doubly linked list. Which job is it good at?", NO_CODE,
      { "Reading element 500 with []", "Sorting with std::sort", "Inserting or erasing in the middle using an iterator", "Keeping elements in contiguous memory" }, 2,
      "Lists relink nodes without shifting anything. They have no [] and are not contiguous.",
      "Think about what linked nodes make easy." },
    { 12, Q_OUTPUT, "What does this code print?",
      "std::list<int> l = {1, 2, 3};\nl.push_front(0);\nprintf(\"%d\", l.front());",
      { "0", "1", "3", "It will not compile" }, 0,
      "push_front puts 0 at the start, so front() returns 0.",
      "push_front adds to the beginning." },
    { 12, Q_CONCEPT, "Why can't you write myList[2] with a std::list?", NO_CODE,
      { "Lists are read-only", "It has no random access, so you walk it with iterators", "Index 2 is reserved", "[] only works on strings" }, 1,
      "To reach the third element a list must follow links from the front, so it does not offer [].",
      "Nodes are scattered in memory." },

    // ---- Level 13: STL Maps ----
    { 13, Q_CONCEPT, "What does a std::map store?", NO_CODE,
      { "Values with no keys", "A fixed-size array", "Key and value pairs sorted by key", "Unique values in arrival order" }, 2,
      "A map links each unique key to a value and keeps the keys in sorted order.",
      "Think of a dictionary." },
    { 13, Q_OUTPUT, "What does this code print?",
      "std::map<std::string, int> age;\nage[\"Ann\"] = 30;\nage[\"Ann\"] = 31;\nprintf(\"%d\", (int)age.size());",
      { "0", "2", "3", "1" }, 3,
      "Keys are unique, so the second assignment overwrites the first. The map holds one entry.",
      "The same key is used twice." },
    { 13, Q_CONCEPT, "What does map[key] do when the key is not in the map?", NO_CODE,
      { "It throws an error", "It returns nullptr", "It crashes immediately", "It inserts the key with a default value (0 for int)" }, 3,
      "operator[] creates a new entry with a default value. Use find() or count() to check without inserting.",
      "It quietly adds something." },

    // ---- Level 14: Boss Challenge ----
    { 14, Q_BUG, "What is wrong with this code?",
      "Node* n = new Node(5);\ndelete n;\nprintf(\"%d\", n->data);",
      { "Node cannot be deleted", "n is used after it was deleted", "new Node needs two arguments", "printf cannot print data" }, 1,
      "After delete the memory is gone, so n->data is a use-after-free. Set n to nullptr after deleting.",
      "What happens to the memory on the second line?" },
    { 14, Q_OUTPUT, "What does this code print?",
      "int a[] = {1, 2, 3, 4};\nint *p = a + 1;\nprintf(\"%d\", p[2]);",
      { "2", "3", "5", "4" }, 3,
      "p starts at a[1], so p[2] is a[3], which is 4.",
      "p[2] means two elements after where p points." },
    { 14, Q_CONCEPT, "You need to look up a student's GPA quickly by name. Which container fits best?", NO_CODE,
      { "std::vector<double>", "std::list<std::string>", "std::map<std::string, double>", "A plain int array" }, 2,
      "A map links a key (the name) to a value (the GPA) and finds it quickly.",
      "You need key and value pairs." }
};

#define QUESTION_TOTAL ((int)(sizeof(QUESTIONS) / sizeof(QUESTIONS[0])))

const char *QuestionKindName(QuestionKind kind)
{
    switch (kind)
    {
        case Q_CONCEPT: return "Multiple choice";
        case Q_OUTPUT:  return "What is the output?";
        case Q_BUG:     return "Spot the bug";
    }
    return "";
}

int QuestionCount(int level)
{
    int count = 0;
    for (int i = 0; i < QUESTION_TOTAL; i++)
    {
        if (QUESTIONS[i].level == level) count++;
    }
    return count > LESSON_MAX_QUESTIONS ? LESSON_MAX_QUESTIONS : count;
}

const Question *QuestionGet(int level, int index)
{
    if (index < 0) return 0;

    int seen = 0;
    for (int i = 0; i < QUESTION_TOTAL; i++)
    {
        if (QUESTIONS[i].level != level) continue;
        if (seen == index) return &QUESTIONS[i];
        seen++;
    }
    return 0;
}

int LessonPassMark(int totalQuestions)
{
    return (totalQuestions * PASS_PERCENT + 99) / 100;
}

bool LessonPassed(int score, int totalQuestions)
{
    return totalQuestions > 0 && score >= LessonPassMark(totalQuestions);
}