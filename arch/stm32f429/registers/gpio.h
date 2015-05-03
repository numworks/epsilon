typedef enum {
  GPIO_MODE_INPUT = 0,
  GPIO_MODE_OUTPUT = 1,
  GPIO_MODE_ALTERNATE_FUNCTION = 2,
  GPIO_MODE_ANALOG = 3
} GPIO_MODE_t;

typedef struct {
  GPIO_MODE_t MODER0:2;
  unsigned int MODER1:2;
  unsigned int MODER2:2;
  unsigned int MODER3:2;
  unsigned int MODER4:2;
  unsigned int MODER5:2;
  unsigned int MODER6:2;
  unsigned int MODER7:2;
  unsigned int MODER8:2;
  unsigned int MODER9:2;
  unsigned int MODER10:2;
  unsigned int MODER11:2;
  unsigned int MODER12:2;
  unsigned int MODER13:2;
  unsigned int MODER14:2;
  unsigned int MODER15:2;
} GPIO_MODER_t;

typedef enum {
  GPIOA = 0,
  GPIOB = 1,
  GPIOC = 2,
  GPIOD = 3,
  GPIOE = 4,
  GPIOF = 5,
  GPIOG = 6,
  GPIOH = 7,
  GPIOI = 8,
  GPIOJ = 9,
  GPIOK = 10
} GPIO_t;

GPIO_MODER_t * GPIO_MODER(GPIO_t gpio);

extern GPIO_MODER_t * GPIOA_MODER;
extern GPIO_MODER_t * GPIOB_MODER;
extern GPIO_MODER_t * GPIOC_MODER;
extern GPIO_MODER_t * GPIOD_MODER;
extern GPIO_MODER_t * GPIOE_MODER;
extern GPIO_MODER_t * GPIOF_MODER;
extern GPIO_MODER_t * GPIOG_MODER;
extern GPIO_MODER_t * GPIOH_MODER;
extern GPIO_MODER_t * GPIOI_MODER;
extern GPIO_MODER_t * GPIOJ_MODER;
extern GPIO_MODER_t * GPIOK_MODER;
