#include "voord/alien.h"
#include "voord/spaceship.h"
#include "utils/display.h"
#include "utils/svc.h"

constexpr const char title[] __attribute__((used)) = "Voord";

void checkForSpaceshipAliensCollisions(Alien aliens[], int numberOfAliens, Spaceship * spaceship) {
  for (int i = 0; i < numberOfAliens; i++) {
    if (aliens[i].hits(spaceship)) {
      Ion::Display::pushRectUniform(Rect(0, 0, Display::Width, Display::Height), Red);
      while (1) {}
    }
  }
}

void checkForRocketsAliensCollisions(Spaceship * spaceship, Alien aliens[], int numberOfAliens) {
  for (int i = 0; i < spaceship->numberOfRockets(); i++) {
    for (int j = 0; j < numberOfAliens; j++) {
      spaceship->rocketAtIndex(i)->tryToKill(&aliens[j]);
    }
  }
}

void app_main() {
  Ion::Display::pushRectUniform(Rect(0, 0, Display::Width, Display::Height), Black);

  constexpr int k_maxNumberOfAliens = 10;
  Alien aliens[k_maxNumberOfAliens];

  Spaceship spaceship;

  int rocketTimer = 0;
  int alienStepTimer = 0;
  int alienMaterializationTimer = 0;
  while (1) {
    Ion::Keyboard::State keyboardState = Ion::Keyboard::scan();
    if (keyboardState.keyDown(Ion::Keyboard::Key::OK)) {
      spaceship.createRockets();
    }
    if (keyboardState.keyDown(Ion::Keyboard::Key::Up)) {
      spaceship.move(0, -Spaceship::k_step);
    }
    if (keyboardState.keyDown(Ion::Keyboard::Key::Down)) {
      spaceship.move(0, Spaceship::k_step);
    }
    if (keyboardState.keyDown(Ion::Keyboard::Key::Left)) {
      spaceship.move(-Spaceship::k_step, 0);
    }
    if (keyboardState.keyDown(Ion::Keyboard::Key::Right)) {
      spaceship.move(Spaceship::k_step, 0);
    }

    checkForSpaceshipAliensCollisions(aliens, k_maxNumberOfAliens, &spaceship);

    if (rocketTimer == Rocket::k_period) {
      rocketTimer = 0;
      for (int i = 0; i < spaceship.numberOfRockets(); i++) {
        spaceship.rocketAtIndex(i)->forward();
      }
      checkForRocketsAliensCollisions(&spaceship, aliens, k_maxNumberOfAliens);
    }
    if (alienStepTimer == Alien::k_stepPeriod) {
      alienStepTimer = 0;
      for (int i = 0; i < k_maxNumberOfAliens; i++) {
        aliens[i].step();
      }
      checkForSpaceshipAliensCollisions(aliens, k_maxNumberOfAliens, &spaceship);
      checkForRocketsAliensCollisions(&spaceship, aliens, k_maxNumberOfAliens);
    }

    Ion::Timing::msleep(20);

    // New alien
    if (alienMaterializationTimer == Alien::k_materializationPeriod) {
      alienMaterializationTimer = 0;
      for (int i = 0; i < k_maxNumberOfAliens; i++) {
        if (aliens[i].isGhost()) {
          aliens[i] = Alien(Display::CommonHorizontalMargin + (float)Ion::random()/(float)0xFFFFFFFF * (Display::Width - 2*Display::CommonHorizontalMargin));
          break;
        }
      }
    }

    // Increment timers
    rocketTimer++;
    alienStepTimer++;
    alienMaterializationTimer++;
  }
}
