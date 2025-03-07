# Prepare a web simulator

From Epsilon's root, build a simulator and copy it there.
```bash
make PLATFORM=web WEB_EXTERNAL_APPS=1 epsilon.html
cp output/release/web/epsilon.html external_apps/epsilon_simulators/web/
```
