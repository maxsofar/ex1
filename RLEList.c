#include "tool/RLEList.h"
#include "stdlib.h"
#define ASCII_ZERO ('0')

struct RLEList_t{
    char character;
    int numberOfRepetitions;
    RLEList previous;
    RLEList next;
};

RLEList RLEListCreate() {
    RLEList newList = malloc(sizeof(*newList));
    if(!newList) {
        return NULL;
    }
    newList->character = '\0';
    newList->numberOfRepetitions = 0;
    newList->previous = NULL;
    newList->next = NULL;
    return newList;
}

void RLEListDestroy(RLEList list) {
    RLEList toRemove;
    while (list != NULL) {
        toRemove = list;
        list = list->next;
        free(toRemove);
    }
}

RLEListResult RLEListAppend(RLEList list, char value) {
    if (list == NULL || value == '\0') {
        return RLE_LIST_NULL_ARGUMENT;
    }

    while(list->next != NULL) {
        list = list->next;
    }

    if (list->character == value) {
        ++list->numberOfRepetitions;
        return RLE_LIST_SUCCESS;
    } else {
        RLEList newList = RLEListCreate();
        if (newList == NULL) {
            return RLE_LIST_OUT_OF_MEMORY;
        }

        list->next = newList;
        newList->previous = list;
        newList->character = value;
        ++newList->numberOfRepetitions;
        return RLE_LIST_SUCCESS;
    }
}

int RLEListSize(RLEList list) {
    if (list == NULL) {
        return -1;
    }

    int counter = 0;
    while (list != NULL) {
        counter += list->numberOfRepetitions;
        list = list->next;
    }
    return counter;
}

RLEListResult RLEListRemove(RLEList list, int index) {
    if (!list) {
        return RLE_LIST_NULL_ARGUMENT;
    }
    if(index < 0) {
        return RLE_LIST_INDEX_OUT_OF_BOUNDS;
    }
    ++index; //starts from 0
    int counter = 0;

    while (list != NULL) {
        counter += list->numberOfRepetitions;

        if (counter > index)
        {
            --list->numberOfRepetitions;
            return RLE_LIST_SUCCESS;
        }
        else if (counter == index)
        {
            if (list->previous->character != '\0') {
                list->previous->next = list->next;
                if (list->next != NULL) {
                    list->next->previous = list->previous;
                }
                free(list);
            } else {
                list->next->previous = list->previous;
                list->previous->next = list->next;
                free(list);
            }
            return RLE_LIST_SUCCESS;
        }
        list = list->next;
    }
    return RLE_LIST_INDEX_OUT_OF_BOUNDS;
}

char RLEListGet(RLEList list, int index, RLEListResult *result) {
    if (list == NULL) {
        *result = RLE_LIST_NULL_ARGUMENT;
        return 0;
    }

    if (index < 0) {
        return RLE_LIST_INDEX_OUT_OF_BOUNDS;
    }

    ++index; //starts from zero
    int counter = 0;
    while (list != NULL) {
        counter += list->numberOfRepetitions;
        if (counter >= index) {
            if (result != NULL) {
                *result = RLE_LIST_SUCCESS;
            }
            return list->character;
        }
        list = list->next;
    }
    if (result != NULL) {
        *result = RLE_LIST_INDEX_OUT_OF_BOUNDS;
    }
    return 0;
}

char* RLEListExportToString(RLEList list, RLEListResult* result) {
    if (list == NULL) {
        if (result != NULL) {
            *result = RLE_LIST_NULL_ARGUMENT;
        }
        return NULL;
    }

    int listSize = RLEListSize(list);
    char* string = malloc(sizeof(char) * listSize + 1);

    if (string == NULL) {
        return NULL;
    }

    char* stringHead = string;
    list = list->next;

    while (list != NULL) {
        *stringHead = list->character;
        ++stringHead;
        *stringHead = ASCII_ZERO + list->numberOfRepetitions;
        ++stringHead;
        *stringHead = '\n';
        ++stringHead;
        list = list->next;
    }
    *stringHead = '\0';
    if (result != NULL) {
        *result = RLE_LIST_SUCCESS;
    }
    return string;
}

RLEListResult RLEListMap(RLEList list, MapFunction map_function) {
    if (list == NULL || map_function == NULL) {
        return RLE_LIST_NULL_ARGUMENT;
    }

    list = list->next;

    while (list != NULL) {
        char updatedCharacter = map_function(list->character);
        if (updatedCharacter != list->character)
        {
            if (list->previous->character != updatedCharacter) {
                list->character = updatedCharacter;
            } else {
                list->previous->numberOfRepetitions += list->numberOfRepetitions;
                list->previous->next = list->next;
                list->next->previous = list->previous;
                RLEList toBeDeleted = list;
                list = list->previous;
                free(toBeDeleted);
            }
        }
        list = list->next;
    }
    return RLE_LIST_SUCCESS;
}