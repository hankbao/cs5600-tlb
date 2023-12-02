# TLB Simulator for CS5600

```zsh
$ ./tlb -h
Usage: tlb [OPTIONS]...

Supported options:
-s, --size=PAGESIZE
	size of a page in bytes, must be a power of 2
-t, --tlb=TLBSIZE
	size of the TLB L1
-c, --cost=TLBCOST
	cost of lookup in the TLB L1, default to 5 nano seconds
-l, --tlb2=TLBSIZE2
	size of the TLB L2
-d, --cost2=TLBCOST2
	cost of lookup in the TLB L2, default to 20 nano seconds
-e, --costpt=PTBCOST
	cost of lookup in the Page Table, default to 100 nano seconds
-p, --policy=TLBPOLICY
	replacement policy for TLB L1 (FIFO, LRU, RAND)
-q, --policy2=TLBPOLICY2
	replacement policy for TLB L2 (FIFO, LRU, RAND)
-a, --access=ADDRLIST
	a set of comma-separated addresses to access
-f, --prefetch=PREFETCHLIST
	a set of comma-separated addresses to prefetch
-h, --help
	print usage message and exit
```
