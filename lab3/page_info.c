#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/page_info.h>
#include <asm/pgtable.h>

SYSCALL_DEFINE3(get_page_info, unsigned long, vaddr, struct page_info __user *, info, int, reset_young) {
    struct mm_struct *mm = current->mm;
    struct vm_area_struct *vma;
    pgd_t *pgd;
    p4d_t *p4d;
    pud_t *pud;
    pmd_t *pmd;
    pte_t *ptep;
    pte_t pte;
    spinlock_t *ptl;

    // Use a local struct to avoid copy_to_user inside the spinlock
    struct page_info local_info = {0}; 
    int ret = 0;

    if (!info) 
        return -EINVAL;

    // Step 1: Safely lock the address space for reading
    if (mmap_read_lock_killable(mm))
        return -EINTR;

    do {
        // Verify the virtual address belongs to a valid VMA
        vma = vma_lookup(mm, vaddr);
        if (!vma) {
            ret = -EFAULT;
            break;
        }

        // Step 2: Page Table Walk (PGD -> P4D -> PUD -> PMD)
        pgd = pgd_offset(mm, vaddr);
        if (pgd_none(*pgd) || pgd_bad(*pgd)) {
            ret = -EFAULT;
            break;
        }

        p4d = p4d_offset(pgd, vaddr);
        if (p4d_none(*p4d) || p4d_bad(*p4d)) {
            ret = -EFAULT;
            break;
        }

        pud = pud_offset(p4d, vaddr);
        if (pud_none(*pud) || pud_bad(*pud)) {
            ret = -EFAULT;
            break;
        }

        pmd = pmd_offset(pud, vaddr);
        if (pmd_none(*pmd) || pmd_bad(*pmd)) {
            ret = -EFAULT;
            break;
        }

        // Step 3: Lock the PTE (Split Page Table Lock)
        ptep = pte_offset_map_lock(mm, pmd, vaddr, &ptl);
        if (!ptep) {
            ret = -EFAULT;
            break;
        }

        pte = *ptep;
        local_info.is_present = pte_present(pte);

        // Step 4: Extract data and optionally reset the Accessed (Young) bit
        if (local_info.is_present) {
            // Calculate physical address using PFN and the virtual address offset
            local_info.physical_addr = (pte_pfn(pte) << PAGE_SHIFT) | (vaddr & ~PAGE_MASK);
            local_info.is_dirty = pte_dirty(pte);
            local_info.is_young = pte_young(pte);

            // Reset young bit if requested and if it is currently set
            if (reset_young && local_info.is_young) {
                ptep_test_and_clear_young(vma, vaddr, ptep);
            }
        }

        // Safely unlock the PTE BEFORE communicating with user space
        pte_unmap_unlock(ptep, ptl);

        // Step 5: Copy the local struct to user space
        if (copy_to_user(info, &local_info, sizeof(struct page_info))) {
            ret = -EFAULT;
        }
    } while (0);

    // Always release the mmap lock
    mmap_read_unlock(mm);
    return ret;
}