CC = g++
CXXFLAGS = -Wall -std=c++17 -g

all: tlb

clean:
	rm -f tlb *.o

tlb: main.o mmu.o tlb_impl.o policy_fifo.o policy_lru.o policy_rand.o
	$(CC) $(CXXFLAGS) -o tlb main.o mmu.o tlb_impl.o policy_fifo.o policy_lru.o policy_rand.o

main.o: main.cc mmu.h policy.h policy_fifo.h policy_lru.h policy_rand.h tlb.h tlb_impl.h tlb_null.h
	$(CC) $(CXXFLAGS) -c main.cc

mmu.o: mmu.cc mmu.h tlb.h def.h
	$(CC) $(CXXFLAGS) -c mmu.cc

tlb_impl.o: tlb_impl.cc tlb_impl.h tlb.h policy.h def.h
	$(CC) $(CXXFLAGS) -c tlb_impl.cc

policy_fifo.o: policy_fifo.cc policy_fifo.h policy.h def.h
	$(CC) $(CXXFLAGS) -c policy_fifo.cc

policy_lru.o: policy_lru.cc policy_lru.h policy.h def.h
	$(CC) $(CXXFLAGS) -c policy_lru.cc

policy_rand.o: policy_rand.cc policy_rand.h policy.h def.h
	$(CC) $(CXXFLAGS) -c policy_rand.cc
