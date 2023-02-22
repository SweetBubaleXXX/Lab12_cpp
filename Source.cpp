// cp1251
#include <iostream>
#include <iomanip>
#include <Windows.h>

#define a_po_russki_mozhno setlocale(LC_ALL, "Russian"); system("chcp 1251")
#define MAX_SEMEN_NAME_LENGTH 20

using namespace std;

struct Semen {
    char name[MAX_SEMEN_NAME_LENGTH];
    int monthSowing;
    int monthSeating;
    int monthHarvesting;
};

typedef Semen* (*searchFunction)(Semen*, int, int);
typedef void (*sortingFunction)(Semen*, int);

enum Menu { EXIT, WRITE, READ, SEARCH, SORT, WRITE_TO_FILE };
enum Months { JAN = 1, FEB, MAR, APR, MAY, JUN, JUL, AUG, SEP, OCT, NOV, DEC };

int getNumberOfSiemens();

void getSiemens(Semen*, int);

void sortHarvesting(Semen*, int);

inline void quickSortWrapper(Semen*, int);

void quickSort(Semen*, int, int);

void selectionSort(Semen*, int);

int writeNamesToBuffer(char*, int, Semen*, int);

void getMenuFunctionNumber(int*);

sortingFunction getSortingFunction();

void showErrorAndExit(const char*);

void printMessage(const char*, const char*);

void writeMessageToFile(const char*, const char*);

template <typename T>
void writeFile(const char*, T*, int);

template <typename T>
T* readFile(const char*, int);

int main(int argc, char** argv) {
    a_po_russki_mozhno;

    if (argc == 1) showErrorAndExit("You should specify file path!");
    char* path = argv[1];

    int numberOfSiemens;
    Semen* siemens = nullptr;

    int bufferLength;
    char* outputBuffer;

    int functionNumber = 1;
    while (functionNumber && !cin.fail()) {
        getMenuFunctionNumber(&functionNumber);
        switch (functionNumber) {
        case EXIT:
            functionNumber = 0;
            break;
        case WRITE:
            numberOfSiemens = getNumberOfSiemens();
            siemens = new Semen[numberOfSiemens];
            getSiemens(siemens, numberOfSiemens);
            writeFile<Semen>(path, &siemens[0], numberOfSiemens);
            delete[] siemens;
            break;
        case READ:
        case SEARCH:
        case SORT:
        case WRITE_TO_FILE:
            numberOfSiemens = getNumberOfSiemens();
            siemens = readFile<Semen>(path, numberOfSiemens);
            cout << "Культуры из файла:" << endl;
            for (int i = 0; i < numberOfSiemens; i++) cout << "\"" << siemens[i].name << "\"" << endl;
            if (functionNumber == READ) break;
            else if (functionNumber == SEARCH) searchHarvesting(siemens, numberOfSiemens, JUN);
            else if (functionNumber == SORT) sortHarvesting(siemens, numberOfSiemens);
            bufferLength = numberOfSiemens * (MAX_SEMEN_NAME_LENGTH + 2) + 1;
            outputBuffer = new char[bufferLength];
            writeNamesToBuffer(outputBuffer, bufferLength, siemens, numberOfSiemens);
            if (functionNumber == SEARCH || functionNumber == SORT) printMessage(outputBuffer, "Output");
            else if (functionNumber == WRITE_TO_FILE) writeMessageToFile(outputBuffer, path);
            delete[] siemens;
            delete[] outputBuffer;
            break;
        default:
            cout << "Invalid function number!" << endl;
            break;
        }
    }
    return 0;
}

int getNumberOfSiemens() {
    int numberOfSiemens;
    cout << "Введите количество культур: ";
    cin >> numberOfSiemens;
    if (cin.fail() || numberOfSiemens <= 0) { puts("Неверное число!"); return 1; }
    return numberOfSiemens;
}

void getSiemens(Semen* siemens, int numberOfSiemens) {
    for (int n = 0; n < numberOfSiemens; n++) {
        siemens[n] = Semen();
        cout << "{\n\tНазвание культуры: ";
        scanf_s("%s", siemens[n].name, MAX_SEMEN_NAME_LENGTH);
        cout << "\tМесяц посева: ";
        scanf_s("%d", &(siemens[n].monthSowing));
        cout << "\tМесяц выссадки рассады: ";
        scanf_s("%d", &(siemens[n].monthSeating));
        cout << "\tМесяц уборки: ";
        scanf_s("%d", &(siemens[n].monthHarvesting));
        cout << "}" << endl;
    }
}

Semen* searchHarvesting(Semen* siemens, int siemensCount, int month) {
    searchFunction search = getSearchFunction();
    search(siemens, siemensCount, month);
}

Semen* linearSearch(Semen* siemens, int siemensCount, int month) {
    for (int i = 0; i < siemensCount; i++) {
        if (siemens[i].monthHarvesting == month) return &siemens[i];
    }
}

Semen* binarySearch(Semen* siemens, int siemensCount, int month) {
    quickSort(siemens, 0, siemensCount - 1);
    int left = 0;
    int right = siemensCount;
    int mid;
    while (left < right) {
        mid = (left + right) / 2;
        if (siemens[mid].monthHarvesting < month) right = mid;
        else if (siemens[mid].monthHarvesting > month) left = mid + 1;
        else return &siemens[mid];
    }
    return NULL;
}

void sortHarvesting(Semen* siemens, int siemensCount) {
    sortingFunction sort = getSortingFunction();
    sort(siemens, siemensCount);
}

inline void quickSortWrapper(Semen* siemens, int siemensCount) {
    quickSort(siemens, 0, siemensCount - 1);
}

void quickSort(Semen* siemens, int left, int right) {
    if (left >= right) return;
    int i = left, j = right;
    int mid = (left + right) / 2;
    do {
        while (i < right && siemens[i].monthHarvesting < siemens[mid].monthHarvesting) i++;
        while (j > left && siemens[j].monthHarvesting > siemens[mid].monthHarvesting) j--;
        if (i <= j) {
            swap(siemens[i], siemens[j]);
            i++; j--;
        }
    } while (i <= j);
    quickSort(siemens, left, j);
    quickSort(siemens, i, right);
}

void selectionSort(Semen* siemens, int siemensCount) {
    Semen* min_siemen;
    int i, j;
    for (i = 0; i < siemensCount - 1; i++) {
        min_siemen = &siemens[i];
        for (j = i + 1; j < siemensCount; j++) {
            if (siemens[j].monthHarvesting < min_siemen->monthHarvesting)
                min_siemen = &siemens[j];
        }
        swap(*min_siemen, siemens[i]);
    }
}

int writeNamesToBuffer(char* buffer, int bufferLength, Semen* siemens, int siemensCount) {
    int bufferPos = 0;
    for (int i = 0; i < siemensCount; i++) {
        bufferPos += sprintf_s(buffer + bufferPos, bufferLength / siemensCount, "%s, ", siemens[i].name);
    }
    return 0;
}

void getMenuFunctionNumber(int* functionNumber) {
    cout << "Menu {" << endl;
    cout << setw(15) << right << "Exit: " << EXIT << endl;
    cout << setw(15) << right << "Write: " << WRITE << endl;
    cout << setw(15) << right << "Read: " << READ << endl;
    cout << setw(15) << right << "Search: " << SEARCH << endl;
    cout << setw(15) << right << "Sort: " << SORT << endl;
    cout << setw(15) << right << "Write to file: " << WRITE_TO_FILE << endl;
    cout << "}\nEnter function number >> ";
    cin >> *functionNumber;
    cin.clear();
    cin.ignore(INT16_MAX, '\n');
}

searchFunction getSearchFunction() {
    int functionNumber;
    cout << "Sorting functions {" << endl;
    cout << setw(10) << right << "Binary: 1" << endl;
    cout << setw(10) << right << "Linear: 2" << endl;
    cout << "}\nEnter function number >> ";
    cin >> functionNumber;
    cin.clear();
    cin.ignore(INT16_MAX, '\n');
}

sortingFunction getSortingFunction() {
    int functionNumber;
    cout << "Sorting functions {" << endl;
    cout << setw(17) << right << "QuickSort: 1" << endl;
    cout << setw(17) << right << "Selection Sort: 2" << endl;
    cout << "}\nEnter function number >> ";
    cin >> functionNumber;
    cin.clear();
    cin.ignore(INT16_MAX, '\n');
    switch (functionNumber) {
    case 2:
        return selectionSort;
    default:
        return quickSortWrapper;
    }
}

void showErrorAndExit(const char* errorMessage) {
    _fcloseall();
    cout << errorMessage << endl;
    exit(1);
}

void printMessage(const char* message, const char* windowTitle) {
    MessageBoxA(NULL, message, windowTitle, MB_OK);
}

void writeMessageToFile(const char* message, const char* path) {
    FILE* filePtr;
    if (fopen_s(&filePtr, path, "w")) return showErrorAndExit("Can't create file ='(");

    fputs(message, filePtr);
    if (ferror(filePtr)) return showErrorAndExit("Error while writing file ='(");

    fclose(filePtr);
}

template <typename T>
void writeFile(const char* path, T* dataForWriting, int amount = 1) {
    FILE* filePtr;
    if (fopen_s(&filePtr, path, "wb") || filePtr == NULL) return showErrorAndExit("Can't create file ='(");

    fwrite(dataForWriting, sizeof(T), amount, filePtr);
    if (ferror(filePtr)) return showErrorAndExit("Error while writing file ='(");

    fclose(filePtr);
}

template <typename T>
T* readFile(const char* path, int amount = 1) {
    T* outputPtr = NULL;
    FILE* filePtr;
    if (fopen_s(&filePtr, path, "rb") || filePtr == NULL) {
        showErrorAndExit("Can't open file for reading ='(");
        return outputPtr;
    }

    outputPtr = new T[amount];
    int elemsRead = fread(outputPtr, sizeof(T), amount, filePtr);
    if (ferror(filePtr) || elemsRead < amount) showErrorAndExit("File is broken ='(");

    fclose(filePtr);
    return outputPtr;
}
