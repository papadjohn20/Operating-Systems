#ifndef _LINUX_PAGE_INFO_H
#define _LINUX_PAGE_INFO_H

struct page_info {
    unsigned long physical_addr;
    int is_present;
    int is_dirty;
    int is_young;
};

#endif /* _LINUX_PAGE_INFO_H */