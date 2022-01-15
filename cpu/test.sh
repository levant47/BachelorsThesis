set -ex

../assembler/run.sh > program.vhd
ghdl -a --std=08 types.vhd program.vhd rom.vhd alu.vhd cpu.vhd cpu_test.vhd
ghdl -e --std=08 cpu_test
ghdl -r --std=08 cpu_test --stop-time=1ms --wave=wave.ghw
