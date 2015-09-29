#include "shellpp.hpp"
#include "blinker.hpp"
#include "uncleos/utlist.h"
#include "uncleos/nexus.c"

#include "uncleos/os.h"

#include "inc/hw_memmap.h"

#include "kbd.h"

#define EXIT_SUCCESS 0

#define SHELL_VERBOSE

#define shell_command_is(a)                     \
    0 == ustrncmp(a, (const char*) buf.buf, buf.length())

semaphore* shell::m_start;
semaphore* shell::m_stop;

void shell::init() {

    buf = buffer<char, SHELL_BUFFER_LENGTH>();
    init_ps1();

    /* initialize the system commands */
    unregistered_commands = 0;
    registered_commands = 0;
    system_iterator i;
    for (i=0; i<SYSTEM_MAX_COMMANDS; ++i) {
        CDL_PREPEND(unregistered_commands, &SYSTEM_COMMANDS[i]);
    }
}

shell::shell() {

    init();
}

shell::shell(uart* u) {

    uart0 = u;
    init();
}

shell::shell(uart* u, semaphore* m_start, semaphore* m_stop) {

    uart0 = u;

    m_start = m_start;
    m_stop = m_stop;

    init();
}

void shell::init_ps1() {

    /* KLUDGE: shitty way to set PS1 */
    ps1[0] = '>';
    ps1[1] = ' ';
    ps1[2] = ' ';
    ps1[3] = 0;

    print_ps1();
}

void shell::clear_buffer() {

    buf.clear();
}

void shell::set_ps1(char* new_ps1) {

    memcpy(ps1, new_ps1, ustrlen(new_ps1));
}

/*! \note has the side effect of clearing the shell buffer */
void shell::print_ps1() {

    buf.clear();
    uart0->atomic_printf("\n\n\r%s", ps1);
}

bool shell::type(char ch) {

    bool ret;
    if (buf.full()) {
        ret = false;
    } else {
        ret = true;
        buf.add((const char) ch);
    }
    if(ret) {
    uart0->atomic_printf("%c", ch);
    }
    return ret;
}

void shell::backspace() {

    bool ok;
    buf.get(ok);
    if (ok) {
        uart0->atomic_printf("\b \b");
    }
}

void shell::register_command(char* command_name, int(*command)(char* args)) {
    /* Grab the first free command, move it from the unregisted to the
     * registered pile, and populate it with this function's
     * arguments */
    system_command* sys_command = unregistered_commands;
    CDL_DELETE(unregistered_commands, sys_command);
    CDL_PREPEND(registered_commands, sys_command);

    sys_command->valid = true;
    umemset(sys_command->name, 0, SYSTEM_MAX_COMMANDS);
    _ustrcpy(sys_command->name, command_name);
    sys_command->command = command;
}

void shell::deregister_command(char* command_name) {

    /* Grab the structure of the command to deregister and invaldiate
     * the metadata. Move it from the registered to the unregistered
     * pile. */
    system_command* command = command_from_name(command_name);
    command->valid = false;
    CDL_DELETE(registered_commands, command);
    CDL_PREPEND(unregistered_commands, command);
}

system_command* shell::command_from_name(const char* command_name) {

    system_iterator i=0;
    system_command *ret;
    while(i<SYSTEM_MAX_COMMANDS &&
          0 != ustrcmp(SYSTEM_COMMANDS[i].name, command_name)) {
        ++i;
    }

    /* Graceful exit on invalid command entry */
    if (i >= SYSTEM_MAX_COMMANDS) {
        ret = 0;
    } else {
        ret = &SYSTEM_COMMANDS[i];
    }
    return ret;
}

exit_status_t shell::execute_command() {

    /* Null terminate to separate the cmd from the args */
    uint8_t len = buf.length();
    uint8_t idx = 0;
    while((idx < len) && (buf.buf[idx] != ' ')) {
        ++idx;
    }
    buf.buf[idx] = 0;

    /* Clear some space between the user input and this cmd output */
    uart0->atomic_printf("\r\n");

    char* args = &buf.buf[idx+1];

    system_command* sys_command = command_from_name(buf.buf);

    exit_status_t exit_code = sys_command ? sys_command->command(args) : 1;

#ifdef SHELL_VERBOSE
    if (exit_code != EXIT_SUCCESS) {
        uart0->atomic_printf("\n\rnonzero exit code: %d", exit_code);
    }
#endif

    /* Prepare for the next command */
    print_ps1();

    return exit_code;
}

void shell::accept(char ch) {

    switch(ch) {
    case SC_CR:
        execute_command();
        break;

    case 127:
    case SC_BACKSPACE:
        backspace();
        break;

    default:
        type(ch);
        break;
    }
}

void shell::shell_handler() {

    while(1) {
        if(uart0->uart_rx_buffer->sem->guard()) {

            bool ok;
            char recv = uart0->uart_rx_buffer->get(ok);

            if(ok) { accept(recv); }
        }
        os_surrender_context();
    }
}

