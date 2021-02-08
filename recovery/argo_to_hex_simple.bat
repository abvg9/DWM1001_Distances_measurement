objcopy -I binary bootloader_s132.bin -O ihex bl.hex --change-addresses 0x1f000
objcopy -I binary dwm-core_fw1.bin -O ihex fw1.hex --change-addresses 0x22000
objcopy -I binary ../examples/dwm-simple/dwm-simple_fw2.bin -O ihex fw2.hex --change-addresses 0x44000
mergehex -m s132_nrf52_3.0.0_softdevice.hex fw1.hex fw2.hex -o argo-out0.hex
mergehex -m bl.hex argo-out0.hex -o dwm1001-flash.hex
rm bl.hex
rm fw1.hex
rm fw2.hex
rm argo-out0.hex
