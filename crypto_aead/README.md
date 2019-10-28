### Folder Structure:

* **isapa128av20**
  * Code for ISAP128a using Ascon-p.
* **isapa128v20**
  * Code for ISAP128 using Ascon-p.
* **isapk128av20**
  * Code for ISAP128a using Keccak-p.
* **isapk128v20**
  * Code for ISAP128 using Keccak-p.
* **isapxv20**
  * A common codebase that can be used to build all four instances of ISAP.

### Sanity Check:

Build all variants of ISAP and check KATs:

    bash sanity_check.sh

