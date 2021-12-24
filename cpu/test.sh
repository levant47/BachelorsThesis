set -ex

ghdl -a types.vhd program.vhd rom.vhd alu.vhd cpu.vhd cpu_test.vhd
ghdl -e cpu_test
ghdl -r cpu_test --stop-time=2us --wave=wave.ghw
