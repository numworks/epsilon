#include <escher/timer_manager.h>
#include <apps/apps_container.h>

void TimerManager::AddTimer(Timer * timer) {
    AppsContainer::sharedAppsContainer()->addTimer(timer);
}

void TimerManager::RemoveTimer(Timer * timer) {
    AppsContainer::sharedAppsContainer()->removeTimer(timer);
}
