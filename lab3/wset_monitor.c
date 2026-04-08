#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <time.h>
#include <errno.h>

/* Use the syscall number defined in your syscall_64.tbl */
#define ID_page_info 462 // Custom syscall number for sys_page_info
#define PAGE_SIZE 4096 // Size of a memory page in bytes
#define MEM_SIZE (100 * 1024 * 1024) // 100 MB
#define Workload_Access_Percentage 90 // Percentage of pages to access in the workload simulation
#define MAX_ACTIVE_PAGES_TO_PRINT 3 // How many active pages to print details for

struct page_info {
    unsigned long physical_addr;
    int is_present;
    int is_dirty;
    int is_young;
};

int main() {
    struct page_info info; // To hold page info results from the syscall
    size_t num_pages = MEM_SIZE / PAGE_SIZE; // Total number of pages in the allocated memory
    size_t i;
    int working_set_count = 0; // Counter for pages that are part of the working set
    int pages_to_access; // Number of pages to randomly access in the workload simulation
    size_t random_index; // To hold random page index during workload simulation
    char temp; // Temporary variable for simulating page access
    printf("\n--- Working Set Monitor Started ---\n");

    // -----------  STEP 1: Allocate Memory -----------
    // Allocate 100 MB. fd == 1 because we use MAP_ANONYMOUS
    char *buffer = mmap(NULL, MEM_SIZE, PROT_READ | PROT_WRITE, 
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    
    if (buffer == MAP_FAILED) {
        perror("Error: mmap failed");
        return 1;
    }
    printf("--- [Step 1] Allocated 100 MB at Virtual Address: %p\n", buffer);

    // -----------  STEP 2: Touch Pages to Make Them Present -----------
    // Writing to each page ensures they become 'Present' in RAM.
    for (i = 0; i < num_pages; i++) {
        buffer[i * PAGE_SIZE] = 'X'; 
    }
    printf("--- [Step 2] Touched %zu pages to ensure they are Present.\n", num_pages);

    // -----------  STEP 3: Call sys_page_info -----------
    // Reset Accessed bits by calling syscall with reset_young=1 for all pages.
    for (i = 0; i < num_pages; i++) {
        if (syscall(ID_page_info, (unsigned long)(buffer + i * PAGE_SIZE), &info, 1) != 0) {
            perror("Syscall failed during reset");
            return 1;
        }
    }
    printf("--- [Step 3] Accessed bits (Young bits) have been reset to 0.\n");

    // -----------  STEP 4: Simulate Workload -----------
    // Simulate Workload (Randomly accessing 20% of the pages)
    printf("--- [Step 4] Simulating workload: Accessing %d%% of pages randomly...\n", Workload_Access_Percentage);
    srand(time(NULL));
    
    pages_to_access = num_pages * Workload_Access_Percentage / 100; 
    
    printf("\n--- Total Pages: %zu, Pages to Access: %d\n", num_pages, pages_to_access);

    for (i = 0; i < pages_to_access; i++) {
        random_index = rand() % num_pages;
        // Performing a read access to trigger the hardware Accessed/Young bit
        temp = buffer[random_index * PAGE_SIZE]; 
    }

    // -----------  STEP 5: Count Working Set -----------
    // Scan memory again and count Working Set
    // Calling syscall with reset_young=0 to check which pages were accessed.
    printf("--- [Step 5] Counting working set by checking Young bits...\n");
    printf("\n--- Will print up to %d active pages\n", MAX_ACTIVE_PAGES_TO_PRINT);
    for (i = 0; i < num_pages; i++) {
        if (syscall(ID_page_info, (unsigned long)(buffer + i * PAGE_SIZE), &info, 0) == 0) {
            if (info.is_young > 0) {
                   if (working_set_count < MAX_ACTIVE_PAGES_TO_PRINT) {
                        printf("\nDetected Active Page with Virtual Address: %p\n", (void*)(buffer + i * PAGE_SIZE));
                        printf("  -> Physical Address: 0x%lx\n", info.physical_addr);
                        printf("  -> Flags: Present=%d, Dirty=%d, Young=%d\n", info.is_present, info.is_dirty, info.is_young);
                } 
                working_set_count++;
            }
        }
        else {
            perror("Syscall failed during working set count");
            return 1;
        }
    }

    // Print Final Results
    printf("\n--- RESULTS ---\n");
    printf("Total Memory Monitored: %d MB\n", MEM_SIZE / (1024 * 1024));
    printf("Active Pages Detected (is_young == 1): %d\n", working_set_count);
    printf("Estimated Working Set Size: in %lld Bytes / in %lf KB\n\n", (long long)working_set_count * PAGE_SIZE, ((double)working_set_count * PAGE_SIZE) / 1024);
    
    // Cleanup
    munmap(buffer, MEM_SIZE);
    return 0;
}