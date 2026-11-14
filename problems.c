#include "problems.h"

// Each harness reads one test from standard input, calls the student's function and
// prints the result. c2_watchdog() ends the program if it runs for more than 2 seconds,
// so an infinite loop in the student's code cannot freeze the app.

static const Problem PROBLEMS[] = {
    {
        "leap-year", "Leap Year", DIFF_EASY, "Functions and logic", "Lab Q1",
        "Write isLeapYear(int year) that returns true when the year is a leap year.\n"
        "A year is a leap year if it is divisible by 400, or divisible by 4 but not by 100.\n\n"
        "Example: isLeapYear(2000) returns true. isLeapYear(1900) returns false.",
        "bool isLeapYear(int year) {\n    // Write your code here\n\n}\n",
        { "A year divisible by 400 is always a leap year.",
          "Years divisible by 100 are not leap years unless they are also divisible by 400.",
          "Combine the rules with && and ||, and make sure the function returns a value on every path." },
        "bool isLeapYear(int year) {\n    return (year % 400 == 0) || (year % 100 != 0 && year % 4 == 0);\n}\n",
        "int main() { c2_watchdog(); int y; cin >> y; cout << (isLeapYear(y) ? \"true\" : \"false\") << endl; }",
        6, { { "2000\n", "true" }, { "1900\n", "false" }, { "2024\n", "true" },
             { "2023\n", "false" }, { "1600\n", "true" }, { "2100\n", "false" } },
        20
    },
    {
        "palindrome-number", "Palindrome Number", DIFF_EASY, "Loops", "Lab Q2",
        "Write isAPalindrome(int n) that returns true when the digits of n read the same forwards and backwards.\n"
        "Negative numbers are not palindromes.\n\n"
        "Example: isAPalindrome(121) returns true. isAPalindrome(1213) returns false.",
        "bool isAPalindrome(int n) {\n    // Write your code here\n\n}\n",
        { "Reverse the number, then compare the reversed number with the original.",
          "Take the last digit with n % 10 and remove it with n / 10. Keep a copy of the original number before the loop changes it.",
          "reversed = reversed * 10 + lastDigit inside a while (n != 0) loop. Return false first if n is negative." },
        "bool isAPalindrome(int n) {\n    if (n < 0) return false;\n    int original = n;\n    int reversed = 0;\n    while (n != 0) {\n        reversed = reversed * 10 + n % 10;\n        n /= 10;\n    }\n    return original == reversed;\n}\n",
        "int main() { c2_watchdog(); int n; cin >> n; cout << (isAPalindrome(n) ? \"true\" : \"false\") << endl; }",
        7, { { "121\n", "true" }, { "1\n", "true" }, { "1213\n", "false" }, { "12321\n", "true" },
             { "10\n", "false" }, { "-121\n", "false" }, { "0\n", "true" } },
        20
    },
    {
        "prime-number", "Prime Number", DIFF_MEDIUM, "Loops and efficiency", "Lab Q3",
        "Write isAPrimeNumber(int n) that returns true when n is prime.\n"
        "A prime is greater than 1 and only divisible by 1 and itself.\n\n"
        "Your code must be fast: one test uses 2147483647, so checking every number up to n is too slow.",
        "bool isAPrimeNumber(int n) {\n    // Write your code here\n\n}\n",
        { "Numbers less than 2 are not prime.",
          "You only need to test divisors from 2 up to the square root of n.",
          "Loop with long long i while i * i <= n, and return false if n % i == 0. Using long long stops i * i overflowing." },
        "bool isAPrimeNumber(int n) {\n    if (n <= 1) return false;\n    for (long long i = 2; i * i <= n; i++) {\n        if (n % i == 0) return false;\n    }\n    return true;\n}\n",
        "int main() { c2_watchdog(); int n; cin >> n; cout << (isAPrimeNumber(n) ? \"true\" : \"false\") << endl; }",
        8, { { "2\n", "true" }, { "1\n", "false" }, { "4\n", "false" }, { "17\n", "true" },
             { "100\n", "false" }, { "97\n", "true" }, { "0\n", "false" }, { "2147483647\n", "true" } },
        40
    },
    {
        "binary-to-decimal", "Binary to Decimal", DIFF_EASY, "Loops", "Lab Q4",
        "Write convertBinarytoDecimal(int binaryNumber). The number is written with only 0s and 1s, for example 110, "
        "and the function returns its decimal value, 6. Do not use the shift operator.\n\n"
        "Example: convertBinarytoDecimal(110) returns 6.",
        "int convertBinarytoDecimal(int binaryNumber) {\n    // Write your code here\n\n}\n",
        { "Each digit is worth a power of 2: the last digit is 1, then 2, then 4, and so on.",
          "Take the last digit with % 10, multiply it by the current place value, then divide the number by 10.",
          "Keep a placeValue that starts at 1 and doubles every loop. Add lastDigit * placeValue to your result." },
        "int convertBinarytoDecimal(int binaryNumber) {\n    int decimal = 0;\n    int placeValue = 1;\n    while (binaryNumber != 0) {\n        int lastDigit = binaryNumber % 10;\n        decimal += lastDigit * placeValue;\n        placeValue *= 2;\n        binaryNumber /= 10;\n    }\n    return decimal;\n}\n",
        "int main() { c2_watchdog(); int b; cin >> b; cout << convertBinarytoDecimal(b) << endl; }",
        6, { { "110\n", "6" }, { "111\n", "7" }, { "0\n", "0" }, { "1\n", "1" },
             { "101010\n", "42" }, { "1111\n", "15" } },
        20
    },
    {
        "find-index", "Find an Element", DIFF_EASY, "Arrays", "Lab Q8",
        "Write findIndex(int size, int arr[], int toFind) that returns the index of the first element equal to toFind, "
        "or -1 if it is not in the array.\n\n"
        "Example: findIndex(5, {5, 3, 9, 3, 7}, 9) returns 2.",
        "int findIndex(int size, int arr[], int toFind) {\n    // Write your code here\n\n}\n",
        { "Look at each element in turn with a for loop.",
          "When arr[i] equals toFind, you can return i straight away.",
          "If the loop finishes without finding it, return -1 after the loop." },
        "int findIndex(int size, int arr[], int toFind) {\n    for (int i = 0; i < size; i++) {\n        if (arr[i] == toFind) return i;\n    }\n    return -1;\n}\n",
        "int main() { c2_watchdog(); int n, x; cin >> n; vector<int> a(n); for (auto &v : a) cin >> v; cin >> x; cout << findIndex(n, a.data(), x) << endl; }",
        5, { { "5\n5 3 9 3 7\n9\n", "2" }, { "5\n5 3 9 3 7\n4\n", "-1" }, { "5\n5 3 9 3 7\n5\n", "0" },
             { "5\n5 3 9 3 7\n7\n", "4" }, { "5\n5 3 9 3 7\n3\n", "1" } },
        20
    },
    {
        "second-largest", "Second Largest", DIFF_MEDIUM, "Arrays", "Lab Q9",
        "Write find2ndLargest(int size, int arr[]) for an array of positive integers. Return the second largest distinct value, "
        "or -1 if there is none.\n\n"
        "Example: {4, 9, 2, 9, 7} returns 7. {5, 5, 5} returns -1.",
        "int find2ndLargest(int size, int arr[]) {\n    // Write your code here\n\n}\n",
        { "Keep two variables, one for the largest and one for the second largest, both starting at -1.",
          "When you find a new largest, the old largest becomes the second largest.",
          "Otherwise, if the value is bigger than the second largest and different from the largest, it becomes the second largest." },
        "int find2ndLargest(int size, int arr[]) {\n    int largest = -1;\n    int second = -1;\n    for (int i = 0; i < size; i++) {\n        if (arr[i] > largest) {\n            second = largest;\n            largest = arr[i];\n        } else if (arr[i] > second && arr[i] != largest) {\n            second = arr[i];\n        }\n    }\n    return second;\n}\n",
        "int main() { c2_watchdog(); int n; cin >> n; vector<int> a(n); for (auto &v : a) cin >> v; cout << find2ndLargest(n, a.data()) << endl; }",
        6, { { "5\n4 9 2 9 7\n", "7" }, { "3\n5 5 5\n", "-1" }, { "1\n8\n", "-1" },
             { "2\n3 8\n", "3" }, { "4\n10 20 30 40\n", "30" }, { "3\n7 7 6\n", "6" } },
        40
    },
    {
        "frequency-count", "Frequency Count", DIFF_EASY, "Arrays", "Lab Q13",
        "Write frequencyCount(int size, int arr[], int value) that returns how many times value appears in the array.\n\n"
        "Example: frequencyCount(5, {5, 3, 9, 3, 7}, 3) returns 2.",
        "int frequencyCount(int size, int arr[], int value) {\n    // Write your code here\n\n}\n",
        { "You need a counter that starts at 0.",
          "Loop over every element and add 1 to the counter each time arr[i] equals value.",
          "Return the counter after the loop ends." },
        "int frequencyCount(int size, int arr[], int value) {\n    int count = 0;\n    for (int i = 0; i < size; i++) {\n        if (arr[i] == value) count++;\n    }\n    return count;\n}\n",
        "int main() { c2_watchdog(); int n, x; cin >> n; vector<int> a(n); for (auto &v : a) cin >> v; cin >> x; cout << frequencyCount(n, a.data(), x) << endl; }",
        5, { { "5\n5 3 9 3 7\n3\n", "2" }, { "5\n5 3 9 3 7\n5\n", "1" }, { "5\n5 3 9 3 7\n4\n", "0" },
             { "6\n1 1 1 1 1 1\n1\n", "6" }, { "1\n2\n2\n", "1" } },
        20
    },
    {
        "count-duplicates", "Count Duplicates", DIFF_MEDIUM, "Arrays", "Lab Q14",
        "Write countDuplicates(int size, int arr[]). Every element that already appeared earlier in the array counts as one duplicate.\n\n"
        "Example: {1, 2, 2, 3, 3, 3} returns 3 (the second 2, the second 3 and the third 3).",
        "int countDuplicates(int size, int arr[]) {\n    // Write your code here\n\n}\n",
        { "For each element, ask: did I see this value earlier in the array?",
          "Use an inner loop that only checks the positions before the current one (k < i).",
          "If a match is found, add 1 to the count and stop the inner loop with break." },
        "int countDuplicates(int size, int arr[]) {\n    int duplicates = 0;\n    for (int i = 0; i < size; i++) {\n        for (int k = 0; k < i; k++) {\n            if (arr[i] == arr[k]) {\n                duplicates++;\n                break;\n            }\n        }\n    }\n    return duplicates;\n}\n",
        "int main() { c2_watchdog(); int n; cin >> n; vector<int> a(n); for (auto &v : a) cin >> v; cout << countDuplicates(n, a.data()) << endl; }",
        5, { { "6\n1 2 2 3 3 3\n", "3" }, { "3\n1 2 3\n", "0" }, { "4\n5 5 5 5\n", "3" },
             { "4\n1 1 2 2\n", "2" }, { "1\n9\n", "0" } },
        40
    },
    {
        "rotate-left", "Rotate Left", DIFF_EASY, "Arrays", "Lab Q16",
        "Write rotateLeft(int size, int arr[]) that moves every element one place to the left, "
        "and moves the first element to the end. Change the array itself.\n\n"
        "Example: {1, 2, 3, 4} becomes {2, 3, 4, 1}.",
        "void rotateLeft(int size, int arr[]) {\n    // Write your code here\n\n}\n",
        { "Save the first element in a temporary variable before you overwrite it.",
          "Shift each element left: arr[i] = arr[i + 1] for i from 0 to size - 2.",
          "Finish by putting the saved element in arr[size - 1]. Do nothing if size is 1 or less." },
        "void rotateLeft(int size, int arr[]) {\n    if (size <= 1) return;\n    int first = arr[0];\n    for (int i = 0; i < size - 1; i++) {\n        arr[i] = arr[i + 1];\n    }\n    arr[size - 1] = first;\n}\n",
        "int main() { c2_watchdog(); int n; cin >> n; vector<int> a(n); for (auto &v : a) cin >> v; rotateLeft(n, a.data()); for (int i = 0; i < n; i++) { if (i) cout << \" \"; cout << a[i]; } cout << endl; }",
        4, { { "4\n1 2 3 4\n", "2 3 4 1" }, { "1\n5\n", "5" }, { "2\n1 2\n", "2 1" }, { "5\n9 8 7 6 5\n", "8 7 6 5 9" } },
        20
    },
    {
        "two-movies", "Two Movies", DIFF_MEDIUM, "Arrays", "Lab Q17",
        "You are on a flight of flightLength minutes. Write twoMovies(int flightLength, int movieLengths[], int size) that returns "
        "true if two different movies add up to exactly the flight length.\n\n"
        "Example: flight 160 with {90, 120, 100, 60} returns true (100 + 60).",
        "bool twoMovies(int flightLength, int movieLengths[], int size) {\n    // Write your code here\n\n}\n",
        { "Try every pair of movies.",
          "Use two loops. Start the inner loop at j = i + 1 so a movie is never paired with itself.",
          "Return true as soon as movieLengths[i] + movieLengths[j] == flightLength. Return false after both loops." },
        "bool twoMovies(int flightLength, int movieLengths[], int size) {\n    for (int i = 0; i < size; i++) {\n        for (int j = i + 1; j < size; j++) {\n            if (movieLengths[i] + movieLengths[j] == flightLength) return true;\n        }\n    }\n    return false;\n}\n",
        "int main() { c2_watchdog(); int f, n; cin >> f >> n; vector<int> m(n); for (auto &v : m) cin >> v; cout << (twoMovies(f, m.data(), n) ? \"true\" : \"false\") << endl; }",
        6, { { "160\n4\n90 120 100 60\n", "true" }, { "500\n4\n90 120 100 60\n", "false" }, { "180\n2\n90 90\n", "true" },
             { "100\n1\n100\n", "false" }, { "150\n1\n75\n", "false" }, { "210\n3\n100 110 50\n", "true" } },
        40
    },
    {
        "word-counter", "Word Counter", DIFF_MEDIUM, "Strings", "Lab Q18",
        "Write wordCounter(int size, char characters[]) that counts the words in the text. "
        "A word is a run of letters (A to Z or a to z). Any other character ends a word.\n\n"
        "Example: \"Hello  world, this is C++\" has 5 words.",
        "int wordCounter(int size, char characters[]) {\n    // Write your code here\n\n}\n",
        { "Walk through the characters one at a time.",
          "Keep a bool inWord that says whether the previous character was a letter.",
          "Add 1 to the count when you see a letter and inWord is false. Set inWord to false on any character that is not a letter." },
        "int wordCounter(int size, char characters[]) {\n    int words = 0;\n    bool inWord = false;\n    for (int i = 0; i < size; i++) {\n        char c = characters[i];\n        bool letter = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');\n        if (letter) {\n            if (!inWord) { words++; inWord = true; }\n        } else {\n            inWord = false;\n        }\n    }\n    return words;\n}\n",
        "int main() { c2_watchdog(); string line; getline(cin, line); cout << wordCounter((int)line.size(), &line[0]) << endl; }",
        6, { { "Hello  world, this is C++\n", "5" }, { "\n", "0" }, { "one\n", "1" },
             { "  a b  c \n", "3" }, { "C++ is fun\n", "3" }, { "a1b\n", "2" } },
        40
    },
    {
        "sum-of-digits", "Sum of Digits", DIFF_EASY, "Loops", "Practice",
        "Write sumDigits(int n) that returns the sum of the digits of a non-negative number.\n\n"
        "Example: sumDigits(123) returns 6.",
        "int sumDigits(int n) {\n    // Write your code here\n\n}\n",
        { "You can peel off the last digit of a number with n % 10.",
          "Remove that digit with n / 10 and repeat while n is not 0.",
          "Add each last digit to a total that starts at 0, and return the total." },
        "int sumDigits(int n) {\n    int total = 0;\n    while (n != 0) {\n        total += n % 10;\n        n /= 10;\n    }\n    return total;\n}\n",
        "int main() { c2_watchdog(); int n; cin >> n; cout << sumDigits(n) << endl; }",
        5, { { "123\n", "6" }, { "0\n", "0" }, { "9\n", "9" }, { "99999\n", "45" }, { "1000\n", "1" } },
        20
    },
    {
        "swap-values", "Swap with Pointers", DIFF_EASY, "Pointers", "Practice",
        "Write swapValues(int *a, int *b) that swaps the two integers the pointers point to.\n\n"
        "Example: with a = 3 and b = 5, after swapValues(&a, &b) a is 5 and b is 3.",
        "void swapValues(int *a, int *b) {\n    // Write your code here\n\n}\n",
        { "Changing the pointers themselves would not change the caller's variables. Change the values they point to.",
          "Use *a and *b to read and write the values. You need a temporary variable to hold one of them.",
          "int temp = *a; *a = *b; *b = temp;" },
        "void swapValues(int *a, int *b) {\n    int temp = *a;\n    *a = *b;\n    *b = temp;\n}\n",
        "int main() { c2_watchdog(); int a, b; cin >> a >> b; swapValues(&a, &b); cout << a << \" \" << b << endl; }",
        3, { { "3 5\n", "5 3" }, { "-1 7\n", "7 -1" }, { "0 0\n", "0 0" } },
        20
    },
    {
        "reverse-string", "Reverse a String", DIFF_EASY, "Strings", "Practice",
        "Write reverseString(string s) that returns the string written backwards.\n\n"
        "Example: reverseString(\"hello\") returns \"olleh\".",
        "string reverseString(string s) {\n    // Write your code here\n\n}\n",
        { "You can build the answer by walking through s from the end to the start.",
          "Start i at s.size() - 1 and count down to 0, adding s[i] to a new string.",
          "Or swap characters from both ends towards the middle: s[i] with s[s.size() - 1 - i]." },
        "string reverseString(string s) {\n    string result;\n    for (int i = (int)s.size() - 1; i >= 0; i--) {\n        result += s[i];\n    }\n    return result;\n}\n",
        "int main() { c2_watchdog(); string s; getline(cin, s); cout << reverseString(s) << endl; }",
        5, { { "hello\n", "olleh" }, { "a\n", "a" }, { "racecar\n", "racecar" },
             { "C++ rocks\n", "skcor ++C" }, { "\n", "" } },
        20
    }
};

#define PROBLEM_TOTAL ((int)(sizeof(PROBLEMS) / sizeof(PROBLEMS[0])))

int ProblemCount(void)
{
    return PROBLEM_TOTAL;
}

const Problem *ProblemGet(int index)
{
    if (index < 0 || index >= PROBLEM_TOTAL) return 0;
    return &PROBLEMS[index];
}

const char *DifficultyName(Difficulty d)
{
    return d == DIFF_EASY ? "Easy" : "Medium";
}