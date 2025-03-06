# Prepare a Linux simulator

From Epsilon's root, build a simulator and copy it there.
```bash
make PLATFORM=linux ION_SIMULATOR_EXTERNAL_APP=1 epsilon.bin
cp output/release/linux/epsilon.bin external_apps/rpn_cpp/epsilon_simulators/linux/
```
