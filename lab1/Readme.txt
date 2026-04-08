======================================================================
ΕΡΓΑΣΙΑ 1: 2/3/2026
----------------------------------------------------------------------

----------------------------------------------------------------------
		1ο ΜΕΡΟΣ: Διαδρομή προς τη Ρίζα (Process Tree)
----------------------------------------------------------------------
Για τη μεταγλώττιση και εκτέλεση της στατικής βιβλιοθήκης:

Βήματα:
1. gcc -Wall -c roots.c
2. ar rcs libroots.a roots.o
3. gcc -Wall find_roots_lib.c -L. -lroots -o find_roots_lib

Εκτέλεση:
./find_roots_lib
dmesg | tail -n 10

----------------------------------------------------------------------
		2ο ΜΕΡΟΣ: Τροποποιημένος Kyber Scheduler
----------------------------------------------------------------------
Το module βρίσκεται στον φάκελο project1_module.

Βήματα:
1. Μεταγλώττιση: 
   make

2. Εισαγωγή στον Πυρήνα:
   sudo insmod project1-kyber.ko

3. Ενεργοποίηση για τη συσκευή sda:
   sudo bash -c 'echo team38994069 > /sys/block/sda/queue/scheduler'

4. Επιβεβαίωση Ενεργοποίησης:
   cat /sys/block/sda/queue/scheduler
   (Θα πρέπει να εμφανίζεται: none mq-deadline [team38994069])

5. Έλεγχος Λειτουργίας (Logging):
   echo "Testing the scheduler" > test_txt_for_part2
   dmesg | tail -n 10
   (Θα εμφανιστεί το μήνυμα: "In team38994069_kyber_dispatch_request function")

6. Επαναφορά Συστήματος:
   sudo bash -c 'echo mq-deadline > /sys/block/sda/queue/scheduler'
   sudo rmmod project1-kyber

----------------------------------------------------------------------
		3ο ΜΕΡΟΣ: Sysfs Module (find_roots)
----------------------------------------------------------------------
Το module βρίσκεται στον φάκελο sysfs_module.

Βήματα:
1. Μεταγλώττιση:
   make

2. Εισαγωγή στον Πυρήνα:
   sudo insmod sysfs_module.ko

3. Έλεγχος Λειτουργίας:
   cat /sys/kernel/team38994069/find_roots
   (Η εντολή θα επιστρέψει το PID της διεργασίας cat)

4. Επιβεβαίωση στο dmesg:
   dmesg | tail -n 10

5. Αφαίρεση Module:
   sudo rmmod sysfs_module

----------------------------------------------------------------------
			ΑΡΧΕΙΑ ΠΑΡΑΔΟΣΗΣ
----------------------------------------------------------------------
- roots.h roots.c, find_roots_lib.c (Μέρος 1)
- project1-kyber.c, Makefile (Μέρος 2)
- sysfs_module.c, Makefile (Μέρος 3)
- README.txt
======================================================================