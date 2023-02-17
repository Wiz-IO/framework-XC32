///////////////////////////////////////////////////////////////////////////////
//
//      WizIO 2023 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
///////////////////////////////////////////////////////////////////////////////

#include "interface.h"

uint32_t Elapsed(uint32_t startTime, uint32_t currentTime)
{
    if (currentTime >= startTime)
        return (currentTime - startTime);
    else
        return (((uint32_t)0xFFFFFFFFU) - currentTime) + startTime;
}

void pinMux(TREG reg, int val)
{
    SystemUnlock();
    *reg = val;
    SystemLock();
}

///////////////////////////////////////////////////////////////////////////////

void my_assert(bool _EXP_, const char *_MSG_)
{
    if (0 == _EXP_)
    {
        printf("\n[ASSERT] %s", _MSG_);
        ABORT();
    }
}

__attribute__((noreturn, naked)) void __cxa_pure_virtual()
{
    printf("\n[ASSERT] __cxa_pure_virtual File %s Line:%d", __FILE__, __LINE__);
    ABORT();
}

///////////////////////////////////////////////////////////////////////////////

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnonnull-compare"
char *strdup(const char *s)
{
    char *news = NULL;
    if (s)
    {
        news = (char *)malloc(strlen(s) + 1);
        if (news)
            strcpy(news, s);
    }
    return news;
}
#pragma GCC diagnostic pop

///////////////////////////////////////////////////////////////////////////////

typedef struct node
{
    voidFuncPtr cb;
    struct node *next;
} Node;

typedef struct list
{
    Node *head;
} List;

List *pListProcess = NULL;

static Node *createProcess(voidFuncPtr cb)
{
    Node *newNode = malloc(sizeof(Node));
    if (newNode != NULL)
    {
        newNode->cb = cb;
        newNode->next = NULL;
    }
    return newNode;
}

void addProcess(voidFuncPtr cb)
{
    if (cb)
    {
        if (NULL == pListProcess)
        {
            pListProcess = malloc(sizeof(List));
            if (pListProcess)
            {
                pListProcess->head = NULL;
            }
        }
        if (pListProcess)
        {
            ENTER_CRITICAL();
            Node *current = NULL;
            if (pListProcess->head == NULL)
            {
                pListProcess->head = createProcess(cb);
            }
            else
            {
                current = pListProcess->head;
                while (current->next != NULL)
                    current = current->next;
                current->next = createProcess(cb);
            }
            EXIT_CRITICAL();
        }
    }
}

void delProcess(voidFuncPtr cb)
{
    Node *current = pListProcess->head;
    Node *previous = current;
    while (current != NULL)
    {
        if (current->cb == cb)
        {
            ENTER_CRITICAL();
            previous->next = current->next;
            if (current == pListProcess->head)
            {
                pListProcess->head = current->next;
            }
            free(current);
            EXIT_CRITICAL();
            return;
        }
        previous = current;
        current = current->next;
    }
}

void exeProcess()
{
    Node *current = pListProcess->head;
    if (pListProcess->head)
    {
        for (; current != NULL; current = current->next)
        {
            current->cb();
            yield();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

#ifdef FREERTOS

void vAssertCalled(const char *const pcFileName, unsigned long ulLine)
{
    portDISABLE_INTERRUPTS();
    printf("\n[ASSERT] FREERTOS: %s, Line: %lu\n", pcFileName, ulLine);
    ABORT();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    portDISABLE_INTERRUPTS();
    printf("\n[ASSERT] FREERTOS Stack Overflow in Task: %s\n", (portCHAR *)pcTaskName);
    ABORT();
}

#endif

///////////////////////////////////////////////////////////////////////////////