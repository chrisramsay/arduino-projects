Remap MLX90614
==============

Use this script to remap an MLX90614's slave address.

Read more at https://chrisramsay.co.uk/posts/2014/09/arduino-and-multiple-mlx90614-sensors/

```
Setup...
> [ReadAddr] Reading address
  Using MLX univeral address, Data: 5A, 0, FF
> [ChangeAddr] Will change address to: 5A
> [ChangeAddr] Data erased.
> [ChangeAddr] Writing data: 5A, 0
> [ChangeAddr] Found correct CRC: 0xE1
> [ReadAddr] Reading address
  Using MLX univeral address, Data: 5A, 0, FF
> [setup] Cycle power NOW to store new address - you have 10 seconds
> [ReadAddr] Reading address
  Using MLX univeral address, Data: 5A, 0, FF
  Warning, next ReadTemp() may fail if address has not been set.
> [ReadTemp] Read temperature using MLX address: 5A, 18.64 C
**---DONE---**
> [ReadTemp] Read temperature using MLX address: 5A, 18.62 C

```
```
Setup...
> [ReadTemp] Read temperature using MLX address 1D: 16.22 C
> [ReadTemp] Read temperature using MLX address 2B: 16.36 C
> [ReadTemp] Read temperature using MLX address 1D: 16.06 C
> [ReadTemp] Read temperature using MLX address 2B: 16.26 C

```