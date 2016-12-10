Remap MLX90614
==============

Use this script to remap an MLX90614's slave address.

Read more at http://www.chrisramsay.co.uk/posts/2014/09/arduino-and-multiple-mlx90614-sensors/

```
Setup...
> [ReadAddr] Reading address
  Using MLX univeral address, Data: 5A, 0, FF
> [ChangeAddr] Will change address to: 5E
> [ChangeAddr] Data erased.
> [ChangeAddr] Writing data: 5E, 0
> [ChangeAddr] Found correct CRC: 0xB5
> [ReadAddr] Reading address
  Using MLX univeral address, Data: 5E, 0, BB
> [setup] Cycle power NOW - you have 10 seconds, 
  else address will be unchanged and [ReadTemp] will fail.
> [ReadTemp] Read temperature using MLX univeral address, 17.42 C
> [ReadTemp] Read temperature using MLX address: 5E, 17.46 C
**---DONE---**
> [ReadTemp] Read temperature using MLX address: 5E, 17.30 C
```
