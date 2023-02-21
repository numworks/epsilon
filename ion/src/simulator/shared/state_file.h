#ifndef ION_SIMULATOR_STATE_FILE_H
#define ION_SIMULATOR_STATE_FILE_H

namespace Ion {
namespace Simulator {
namespace StateFile {

void load(const char* filename);
void save(const char* filename);

}  // namespace StateFile
}  // namespace Simulator
}  // namespace Ion

#endif
