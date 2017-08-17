#ifndef M_LOG_H
#define M_LOG_H


////////////////////////////////////
typedef unsigned char uint8;
typedef unsigned char bool;
////////////////////////////////////


// Log level start
typedef enum{
    LOG_NONE,
    LOG_F,  /* FATAL */
    LOG_A,  /* ALARM */
    LOG_E,  /* ERROR */
    LOG_W,  /* WARNING */
    LOG_I,  /* INFO */
    LOG_D   /* DEBUG */
} log_level_enum;
// Log level end

// Color define start
#define NONE                 "\e[0m"
#define BLACK                "\e[0;30m"
#define L_BLACK              "\e[1;30m"
#define RED                  "\e[0;31m"
#define L_RED                "\e[1;31m"
#define GREEN                "\e[0;32m"
#define L_GREEN              "\e[1;32m"
#define BROWN                "\e[0;33m"
#define YELLOW               "\e[1;33m"
#define BLUE                 "\e[0;34m"
#define L_BLUE               "\e[1;34m"
#define PURPLE               "\e[0;35m"
#define L_PURPLE             "\e[1;35m"
#define CYAN                 "\e[0;36m"
#define L_CYAN               "\e[1;36m"
#define GRAY                 "\e[0;37m"
#define WHITE                "\e[1;37m"

#define BOLD                 "\e[1m"
#define UNDERLINE            "\e[4m"
#define BLINK                "\e[5m"
#define REVERSE              "\e[7m"
#define HIDE                 "\e[8m"
#define CLEAR                "\e[2J"
#define CLRLINE              "\r\e[K" //or "\e[1K\r"
// Color define end.

#define PRESENT_BIT8(x)        (((uint8)((uint8)1<<(x))))
#define BIT_ON8(m, b)            (((m) & PRESENT_BIT8(b)) != 0)
#define SET_BIT8(m, b)            ((m) |= PRESENT_BIT8(b))
#define CLEAR_BIT8(m, b)        ((m) &= ~PRESENT_BIT8(b))

void log_errno(const int);
void log_print(const char*);

void log_init
(
    int level,
    bool only_this_level,
    const char *log_path
);

void log_section(void);
void log_clear(void);


#define log(level,fmt...) \
        log_private(level,__FILE__,__LINE__,__FUNCTION__,##fmt)

#define log_f(fmt...) \
        log(LOG_F,##fmt)

#define log_a(fmt...) \
        log(LOG_A,##fmt)

#define log_e(fmt...) \
        log(LOG_E,##fmt)

#define log_w(fmt...) \
        log(LOG_W,##fmt)

#define log_i(fmt...) \
        log(LOG_I,##fmt)

#define log_d(fmt...) \
        log(LOG_D,##fmt)

#endif /* M_LOG_H */

