Folder Structure:
-----------------

* **avx512**
  * AVX-512 optimized implementation in C.
* **opt_32**
  * Optimized 32-bit implementation in C.
* **opt_32_armv67**
  * Assembly-optimized 32-bit implementation in ARMv6m/ARMv7m assembly.
* **opt_32_compact**
  * More compact variant of `opt_32`.
* **opt_32_stp**
  * Variant of `opt_32` that includes a leakage-resilient tag comparison.
* **opt_64**
  * Optimized 64-bit implementation in C.
* **opt_64_compact**
  * More compact variant of `opt_64`.
  * On microprocessors without rotation instruction this variant might perform better than the `opt_32` implementations
* **opt_64_stp**
  * Variant of `opt_64_compact` that includes a leakage-resilient tag comparison.
* **ref**
  * Easy-to-read implementation in C.

