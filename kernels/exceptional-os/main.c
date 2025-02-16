#include <am.h>
#include <klib-macros.h>

#define STACK_SIZE (4096 * 8)
typedef union {
  uint8_t stack[STACK_SIZE];
  struct {
    Context *cp;
  };
} PCB;
static PCB pcb[1], pcb_boot, *current = &pcb_boot;

static void thread_1(void *arg) {
  putstr("thread_1: Hello!\n");
  volatile unsigned int t;
  unsigned char buf[8] = {0};
  putstr("thread_1: Before illegal instruction...\n");
  asm volatile(".word 0x00000000");
  putstr("thread_1: After illegal instruction\n");
  putstr("thread_1: Before misaligned read...\n");
  asm volatile("lw %0, 1(%1)" : "=r"(t) : "r"(buf));
  putstr("thread_1: Before misaligned read\n");
  putstr("thread_1: Before misaligned write...\n");
  t = 0xdeadbeef;
  asm volatile("sw %0, 1(%1)" : : "r"(t), "r"(buf));
  putstr("thread_1: Before misaligned write\n");
  halt(0);
}

static Context *schedule(Event ev, Context *prev) {
  uintptr_t mcause;
  asm volatile("csrr %0, mcause" : "=r"(mcause));
  switch (ev.event) {
    case EVENT_YIELD: putstr("EVENT_YIELD\n"); break;
    case EVENT_SYSCALL: putstr("EVENT_SYSCALL\n"); break;
    case EVENT_ERROR: {
      putstr("EVENT_ERROR: ");
      switch (mcause & 0x7fffffff) {
        case 0: putstr("Instruction address misaligned"); break;
        case 1: putstr("Instruction access fault"); break;
        case 2: putstr("Illegal instruction"); break;
        case 3: putstr("Breakpoint"); break;
        case 4: putstr("Load address misaligned"); break;
        case 5: putstr("Load access fault"); break;
        case 6: putstr("Store/AMO address misaligned"); break;
        case 7: putstr("Store/AMO access fault"); break;
        case 8: putstr("Environment call from U-mode"); break;
        case 9: putstr("Environment call from S-mode"); break;
        case 11: putstr("Environment call from M-mode"); break;
        case 12: putstr("Instruction page fault"); break;
        case 13: putstr("Load page fault"); break;
        case 15: putstr("Store/AMO page fault"); break;
        default: putstr("Unknown"); break;
      }
      putch('\n');
      break;
    }
    default: putstr("Unknown event\n"); break;
  }
  current->cp = prev;
  current = &pcb[0];
  return current->cp;
}

int main() {
  cte_init(schedule);
  pcb[0].cp = kcontext((Area){pcb[0].stack, &pcb[0] + 1}, thread_1, NULL);
  yield();
  panic("Should not reach here!");
}
