#include "shellpp.hpp"
#include "blinker.hpp"
#include "strhash.hpp"
#include "uncleos/utlist.h"
#include "uncleos/nexus.c"

#include "uncleos/os.h"

#include "inc/hw_memmap.h"

#include "kbd.h"

#define EXIT_SUCCESS 0

#define SHELL_VERBOSE

void shell::init() {

    buf = buffer<char, SHELL_BUFFER_LENGTH>();
    init_ps1();

    /* initialize the system commands */
    commands = new HashMap<uint32_t, sys_cmd, MyKeyHash>();
}

shell::shell() {

    init();
}

shell::shell(uart* u) {

    uart0 = u;
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

void shell::register_command(char* command_name, sys_cmd command) {
    uint32_t command_hash = SuperFastHash(command_name, ustrlen(command_name));
    commands->put(command_hash, command);
}

void shell::deregister_command(char* command_name) {
    uint32_t command_hash = SuperFastHash(command_name, ustrlen(command_name));
    commands->remove(command_hash);
}

sys_cmd shell::command_from_name(const char* command_name) {
    uint32_t command_hash = SuperFastHash(command_name, ustrlen(command_name));
    sys_cmd command;
    bool status = commands->get(command_hash, command);
    return status ? command : 0;
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

    sys_cmd command = command_from_name(buf.buf);

    exit_status_t exit_code =  command ? command(args) : 1;

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

