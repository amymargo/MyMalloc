# Custom Memory Allocator in C (MyMalloc & MyFree)
### A Lightweight Heap Manager With Block Headers, Splitting, Coalescing, Error Detection, and Comprehensive Testing

This project implements a custom dynamic memory allocator in C, replacing the standard `malloc()` and `free()` functions with `mymalloc()` and `myfree()`.  
The allocator manages a fixed-size heap, performs splitting and coalescing of blocks, ensures 8-byte alignment, and detects common memory-management errors.  
It also includes correctness, error-handling, and performance testing using `genmemtest.c` and `memgrind.c`.

This repository demonstrates low-level systems programming, pointer arithmetic, block metadata management, error detection, and performance benchmarking.

---

## Overview

This allocator:

- Manages a 4096-byte heap in static memory  
- Stores metadata in block headers (size and allocation status)  
- Supports:
  - Splitting of large free blocks
  - Coalescing adjacent free blocks
  - 8-byte alignment
- Implements:
  - `mymalloc(size, file, line)`
  - `myfree(ptr, file, line)`
- Detects and reports:
  - Double frees
  - Freeing non-malloc'd addresses
  - Freeing pointers that do not point to block payloads
  - Out-of-bounds frees
  - Header corruption

The allocator reports all errors using descriptive messages including the source file and line number.

---

## File Structure

```
README.md        # Project documentation
mymalloc.c       # Implementation of mymalloc() and myfree()
mymalloc.h       # Interface and allocator macros
memgrind.c       # Benchmarking and stress-testing suite
genmemtest.c     # General-purpose correctness tests
makefile         # Build rules
```

---

# Allocator Design

## Heap Layout
The heap is implemented as a global static array:

```
static char myblock[4096];
```

Blocks within the heap contain:

- A **header**  
- A **payload**  
- Optional padding for 8‑byte alignment

### **Header Format**
Each block begins with:

```
[ size | status ]
```

Where:
- `size` is the total size of the block (header + payload)
- `status` indicates whether the block is allocated or free

This layout allows linear traversal of the heap.

---

## Block Splitting

When a request for `n` bytes arrives:

1. The allocator scans the heap linearly  
2. Finds a free block large enough  
3. **Splits** it into:
   - An allocated block  
   - A smaller free block  

Splitting avoids unnecessary large allocations and reduces fragmentation.

---

## Coalescing

Coalescing is performed as part of `myfree()`:

- When a block is freed, the allocator checks adjacent blocks.
- If an adjacent block is free, the two blocks are merged.
- Coalescing occurs in both directions (previous and next).
- This improves memory reuse and reduces fragmentation.

---

## Allocation Strategy

### First-Fit Search

The allocator scans the heap linearly and selects the first suitable free block.

### 8-Byte Alignment

All payloads returned to the user respect 8-byte alignment.  
If headers or block sizes are not aligned to 8 bytes, the allocator reports misalignment.

### Metadata Integrity Checks

Before freeing:
- The pointer must fall within the heap bounds.
- The pointer must reference the start of a payload.
- The block must be marked as allocated.
- Header fields must be valid.

Incorrect usage triggers a detailed error message.

---

# Error Detection

The allocator detects and reports:

- Double frees  
- Freeing an unallocated block  
- Freeing a pointer in the middle of an allocated block  
- Freeing addresses outside the heap  
- Corrupted header fields  
- Insufficient remaining heap space for a requested allocation  

All errors are reported as:

```
ERROR: <description>
```

followed by the file name and line number of the offending call.

---

# Test Plan

`genmemtest.c` and `memgrind.c` provide extensive testing for correctness, error detection, and performance.

## Correctness Testing (genmemtest.c)

### testAlign()
- Verifies that all returned pointers are 8-byte aligned.
- Flags misalignment if headers or payloads are not correctly aligned.

### testFree()
- Ensures freeing is only successful when:
  - The pointer refers to the start of an allocated block.
  - The block can be safely coalesced or reused.
- Detects invalid frees.

### testMalloc()
- Ensures the allocator:
  - Reserves unused memory
  - Returns non-overlapping pointers
  - Preserves allocated blocks until they are freed

### testCoa()
- Confirms proper coalescing:
  - Allocate small chunks
  - Free them
  - Allocate one large chunk equal to the whole heap
- If this fails, coalescing is incorrect.

---

## Detectable Error Testing (genmemtest.c)

### doubleFree()
Flags an error when freeing the same pointer twice.

### malloc with insufficient memory
Triggers error when a request exceeds available free space.

### freeNonMallocAddress()
Flags an error when freeing:
- Stack addresses
- Global addresses
- Arbitrary integers

### freeOnMiddleAddress()
Flags an error when freeing:
- A pointer inside a payload instead of the block start

---

## Performance Testing (memgrind.c)

`memgrind.c` measures the execution time of multiple allocation patterns.

Tests include:

1. Allocate and free a 1-byte object 120 times.
2. Allocate 120 1-byte objects, store pointers in an array, then free them.
3. Alternate allocation and freeing using a rotating pointer array.
4. Allocate and free increasingly larger chunks (in increments of 100 bytes) up to 4088 bytes.
5. Doubling allocations while halving allocation size:
   - 1×4088 bytes
   - 2×2044 bytes
   - 4×1022 bytes
   - 8×511 bytes
   - 16×255 bytes
   - 32×127 bytes
   - 64×63 bytes

Execution time of these tests evaluates allocator performance.

---

# Design Notes

- All definitions (heap size, header size, alignment) are centralized in `mymalloc.c` for easy adjustment.
- Coalescing is integrated into `myfree()` to simplify the design and avoid multiple passes over the heap.
- Error reporting is done consistently through `"ERROR: ..."`, using the file and line macros to indicate the source of misuse.

---

# Usage

### Compile
```
make
```

### Run performance tests
```
./memgrind
```

### Run correctness and error tests
```
./genmemtest
```

---

# Authors

Amy Margolina, Daniel Wang

This project demonstrates system-level programming skills including manual memory management, pointer arithmetic, heap management, performance benchmarking, and defensive error handling.
