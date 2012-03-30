See https://github.com/DFE/HidaV/wiki/Safe-Update-and-boot-fallbacks for concept documentation.

$ bootconfig help
  bootconfig - show and set HidaV boot configuration
  Usage:
  bootconfig                      Show current boot configuration
  bootconfig info                 Show detailed / raw boot config information
  bootconfig set-kernel <mtd>     Generate new boot configuration which sets <mtd>
                                   to contain the latest kernel image. Clear all flags.
                                   Write results to boot config partition.
  bootconfig set-rootfs <mtd>     Generate new boot configuration which sets <mtd>
                                   to contain the latest root FS image. Clear all flags.
                                   Write results to boot config partition.

$ bootconfig
  epoch: 56677
  written: Mon Jan  2 01:07:41 UTC 2012
  image name      MTD   Flags
  kernel:          3     B O
  root fs:         5     B O

$ bootconfig set-kernel mtd2
  Setting current kernel to mtd2 (kernel partition #0).
  Writing to NAND...
  epoch: 56678
  written: Wed Mar 28 14:33:53 UTC 2012
  image name      MTD   Flags
  kernel:          2     - -
  root fs:         4     B O

$ bootconfig set-rootfs mtd4
  Setting current root FS to mtd4 (rootfs partition #0).
  Writing to NAND...
  epoch: 56679
  written: Wed Mar 28 14:34:15 UTC 2012
  image name      MTD   Flags
  kernel:          2     - -
  root fs:         4     - -

$ bootconfig info
  There are 8 boot config blocks.
   ---
  Boot block #1
  epoch: 56679
  written: Wed Mar 28 14:34:15 UTC 2012
  image name      MTD   Flags
  kernel          2     - -
  root fs         4     - -
   ---
  Boot block #2
  epoch: 56676
  written: Mon Jan  2 00:05:10 UTC 2012
  image name      MTD   Flags
  kernel           2    - -
  rootfs           5    B O
   ---
...
   ---
  #4 NO MAGIC FOUND
   ---
...
   ---
  #6 BAD BLOCK
   ---
...
   ---
  Boot block #8
  epoch: 56678
  written: Wed Mar 28 14:33:53 UTC 2012
  image name      MTD   Flags
  kernel           3     B -
  rootfs           5     B O
