/* -*- mode: c++; c-basic-offset: 4; -*- */
#ifndef __shellpp__
#define __shellpp__

#include <stdbool.h>

#include "uartpp.hpp"
#include "bufferpp.hpp"
#include "semaphorepp.hpp"

/*! \addtogroup Shell
 * @{
 */

#define SYSTEM_COMMAND_NAME_MAX_LENGTH 10
#define SYSTEM_MAX_COMMANDS    5

/*! PS1 maximum length */
#define SHELL_MAX_PS1_LENGTH 4

/*! Maximum length of shell input per command */
#define SHELL_BUFFER_LENGTH (uint32_t)32

#define UART_VERBOSE true

typedef uint8_t exit_status_t;
typedef exit_status_t (*sys_cmd)(const char*);

struct system_command;
typedef struct system_command {
    bool valid;
    char name[SYSTEM_COMMAND_NAME_MAX_LENGTH];
    int(*command)(char* args);
    system_command* next;
    system_command* prev;
} system_command;

/*! Iterator optimized for size that is guaranteed to be able to
 * iterate over \SYSTEM_COMMANDS. */
typedef uint8_t system_iterator;

class shell {
private:
    uart* uart0;
    buffer<char, SHELL_BUFFER_LENGTH> buf;
    /* Wondering why there's a +1 here? Waldo has the answers */
    char ps1[SHELL_MAX_PS1_LENGTH+1];

    /* Circular doubly-linked list containing all registered commands. */
    system_command SYSTEM_COMMANDS[SYSTEM_MAX_COMMANDS];
    /* Circular doubly-linked list containing all unregistered commands. */
    system_command* unregistered_commands;
    /* Circular doubly-linked list containing all registered commands. */
    system_command* registered_commands;

    static exit_status_t help_info(const char* args);
    static exit_status_t doctor(const char* args);
    static exit_status_t witch(const char* args);
    static exit_status_t jester(const char* args);

    static exit_status_t motor_start(const char* args);
    static exit_status_t motor_stop(const char* args);

    static void ustrcpy(char* dest, const char* source);

    void init_ps1(void);

    static semaphore* m_start;
    static semaphore* m_stop;

    /*! Common code between constructors */
    void init(void);

public:
    shell();
    shell(uart* u);
    shell(uart* u, semaphore* m_start, semaphore* m_stop);

    /*! Clear the shell buffer. */
    void clear_buffer();

    /*! Set the PS1. */
    void set_ps1(char* new_ps1);

    /*! Print the PS1. */
    void print_ps1();

    /*! Add a char to the shell buffer. */
    bool type(char ch);

    /*! Remove a char from the shell buffer. */
    void backspace(void);

    /*! Accept a char, shell will call \type or \backspace appropriately. */
    void accept(char ch);

    void register_command(char* command_name, int(*command)(char* args));

    void deregister_command(char* command_name);

    system_command* command_from_name(const char* command_name);

    /*! Execute this command. */
    exit_status_t execute_command();


    void shell_handler();
};

#endif

/*! End doxygen group
 * @}
 */
