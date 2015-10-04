/* -*- mode: c++; c-basic-offset: 4; -*- */
#ifndef __shellpp__
#define __shellpp__

#include <stdbool.h>

#include "uartpp.hpp"
#include "bufferpp.hpp"
#include "semaphorepp.hpp"

#include "unclelib/hashmap.hpp"

/*! \addtogroup Shell
 * @{
 */

#define SYSTEM_COMMAND_NAME_MAX_LENGTH 10
#define SYSTEM_MAX_COMMANDS    5

/*! PS1 maximum length */
#define SHELL_MAX_PS1_LENGTH 4

/*! Maximum length of shell input per command */
#define SHELL_BUFFER_LENGTH 32

#define UART_VERBOSE true

typedef int8_t exit_status_t;
typedef exit_status_t (*sys_cmd)(char*);

class shell {
private:
    uart* uart0;
    buffer<char, SHELL_BUFFER_LENGTH> buf;
    /* Wondering why there's a +1 here? Waldo has the answers */
    char ps1[SHELL_MAX_PS1_LENGTH+1];

    static void ustrcpy(char* dest, const char* source);

    void init_ps1(void);

    struct MyKeyHash {
        unsigned long operator()(const uint32_t& k) const
        {
            return k % 10;
        }
    };
    HashMap<uint32_t, sys_cmd, MyKeyHash>* commands;

    /*! Common code between constructors */
    void init(void);

public:
    shell();
    shell(uart* u);

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

    void register_command(char* command_name, sys_cmd command);

    void deregister_command(char* command_name);

    sys_cmd command_from_name(const char* command_name);

    /*! Execute this command. */
    exit_status_t execute_command();


    void shell_handler();
};

#endif

/*! End doxygen group
 * @}
 */
