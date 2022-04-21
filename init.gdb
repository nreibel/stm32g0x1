set pagination off
set confirm off

# connect to STM32 target
target extended-remote :4242

# break in main
break main
run
