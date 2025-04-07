# xv6-riscv kernel-level multithreading

Added kernel-level multithreading to the xv6-riscv operating system. This involved: 

- separating the process and thread related data into two separate structs
- changing the way the trapframe is mapped in user space so that multiple trapframes can be mapped into the same user address space
- implementing the clone() and join() syscalls
- adding lock functions (lock_init(), lock_acquire(), lock_release()) to the user library so that user programs can protect data that is shared between threads

## Important notes

The original proc struct now stores thread related data, while the process struct stores process related data.

The exit() syscall terminates the current thread, rather than the current process. This is different from posix threads, where the exit() syscall exits the current process. Additionally, the start routine of a thread must end with a call to exit() instead of a return statement.

## Implementation details

In the original xv6, the trapframe is mapped at a fixed address in user space. Since each thread has its own trapframe, when multithreading is added, a process needs to be able to have multiple trapframes, so using a fixed address for the trapframe is no longer sufficient. To solve this problem, the user virtual memory has a dedicated space for n number of trapframes, where n is the max number of threads in the xv6 operating system (defined by the macro NPROC in the code). When a thread is created, along with its trapframe, the trapframe is mapped into this dedicated space. The position of the trapframe in this space is determined by the position of the thread in the threads array (the proc array in proc.c). This ensures that each trapframe has its own unique position in user space across all processes, allowing multiple trapframes to be mapped into the user address of any given process.

After changing the position of the trapframe from a fixed address to an address based on the position of its thread in the thread array, another issue arises. When a trap occurs, the kernel pagetable needs to be installed before the interrupt/syscall/exception can be handled. This requires the address of the pagetable, which is stored in the trapframe of the process (p->trapframe->kernel_satp). Previously, when a trap occurred, the address of the trapframe could be accessed using the constant TRAPFRAME. But since the address of the trapframe is no longer fixed, there needs to be a way to pass the address of the trapframe into trampoline.S. When a process is initalised, the kernel executes usertrapret() in trap.c, which stores the address of the trapframe in the sscratch register. This way, when a trap occurs, the trap handler in trampoline.S has direct access to the trapframe, and is able to install the kernel pagetable.