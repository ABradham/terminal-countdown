#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ASCII_ESC 27
#define TIMER_ABSTIME 1
#define KGRN "\x1B[32m"
#define KNRM "\x1B[0m"
#define SECS_PER_MINUTE 60
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24

void clear_screen(void) { printf("%c[2J", ASCII_ESC); }

void clear_to_end_of_line(void) { printf("%c[K", ASCII_ESC); }

void cursor_home(void) { printf("%c[H", ASCII_ESC); }

void disable_cursor_highlight(void) { printf("%c[?25l", ASCII_ESC); }

void enable_cursor_highlight(void) { printf("%c[?25h", ASCII_ESC); }

void make_text_green(void) { printf("%s", KGRN); }

void make_text_normal(void) { printf("%s", KNRM); }

void handle_exit(void) {
  make_text_normal();
  enable_cursor_highlight();
}

void handle_sigint(int signum) { exit(0); }

int main(int argc, char** argv) {
  if (argc < 2 || argc > 2) {
    printf("Format is %s YYYY-mm-dd-H:M:S!\n", argv[0]);
    return 0;
  }

  // ui setup
  disable_cursor_highlight();
  clear_screen();
  cursor_home();
  make_text_green();

  // register sigint and exit handlers
  // (these reset the terminal ui changes made above)
  atexit(handle_exit);
  signal(SIGINT, handle_sigint);

  // parse time from imput
  struct timespec ts;
  struct tm target_tm;
  time_t target_cal_time;
  if (!strptime(argv[1], "%Y-%m-%d-%H:%M:%S", &target_tm)) {
    printf("Error parsing the date string!\n");
    exit(0);
  }
  target_cal_time = mktime(&target_tm);
  if (target_cal_time == (time_t)-1) {
    printf("Error converting time from date to calendar time!\n");
    exit(0);
  }

  // main loop
  while (true) {
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
      printf("Something went wrong fetching the current time!\n");
      exit(1);
    }
    // sleep until 1 second in the future
    struct timespec sleep_time = {.tv_sec = 0,
                                  .tv_nsec = 1000000000L - ts.tv_nsec};

    // print newly fetched time
    cursor_home();
    clear_to_end_of_line();
    long secs_diff = difftime(target_cal_time, ts.tv_sec);
    printf("%ld Days, %ld Hours, %ld Minutes, %ld Seconds Until Event",
           secs_diff / (SECS_PER_MINUTE * MINUTES_PER_HOUR * HOURS_PER_DAY),
           (secs_diff / (SECS_PER_MINUTE * MINUTES_PER_HOUR)) % HOURS_PER_DAY,
           (secs_diff / SECS_PER_MINUTE) % MINUTES_PER_HOUR,
           secs_diff % SECS_PER_MINUTE);
    fflush(stdout);

    nanosleep(&sleep_time, NULL);  // sleep until next second
                                   // macos doesn't support clock_nanosleep so
                                   // we do this instead, lol
  }
  return 0;
}
