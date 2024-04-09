rm /home/jannik/.local/share/Steam/steamapps/common/Euro\ Truck\ Simulator\ 2/bin/linux_x64/plugins/telemetry.so &&
g++ ./telemetry.cpp -c &&
make &&
mv telemetry.so /home/jannik/.local/share/Steam/steamapps/common/Euro\ Truck\ Simulator\ 2/bin/linux_x64/plugins/telemetry.so