# Prepare a web simulator

From Epsilon's root, build a simulator and copy it there.
```bash
make PLATFORM=web ION_SIMULATOR_EXTERNAL_APP=1 epsilon.html
cp output/release/web/epsilon.html external_apps/rpn_cpp/epsilon_simulators/web/
```
