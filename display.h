#define SEG_A 25
#define SEG_B 26
#define SEG_C 27
#define SEG_D 14
#define SEG_E 12
#define SEG_F 13
#define SEG_G 32
#define SEG_DP 33 // opcional
#define DISPLAY_1 16 // display para temperatura ajustada
#define DISPLAY_2 17 // display para temperatura medida

const uint8_t DIGITOS[10] = 
{
    0b0111111, // 0
    0b0000110, // 1
    0b1011011, // 2
    0b1001111, // 3
    0b1100110, // 4
    0b1101101, // 5
    0b1111101, // 6
    0b0000111, // 7
    0b1111111, // 8
    0b1101111  // 9
};
void init_7segment_pins(void) 
{
    gpio_pad_select_gpio(SEG_A); gpio_set_direction(SEG_A, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEG_B); gpio_set_direction(SEG_B, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEG_C); gpio_set_direction(SEG_C, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEG_D); gpio_set_direction(SEG_D, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEG_E); gpio_set_direction(SEG_E, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEG_F); gpio_set_direction(SEG_F, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEG_G); gpio_set_direction(SEG_G, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(SEG_DP); gpio_set_direction(SEG_DP, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(DISPLAY_1); gpio_set_direction(DISPLAY_1, GPIO_MODE_OUTPUT);
    gpio_pad_select_gpio(DISPLAY_2); gpio_set_direction(DISPLAY_2, GPIO_MODE_OUTPUT);
}

void set_display(uint8_t display, bool active) 
{
    gpio_set_level(display, active ? 1 : 0);
}

void show_number_on_display(uint8_t number) 
{
    if (number > 9) return; // limite para números de 0 a 9
    uint8_t segments = DIGITOS[number];
    gpio_set_level(SEG_A, segments & 0b0000001);
    gpio_set_level(SEG_B, segments & 0b0000010);
    gpio_set_level(SEG_C, segments & 0b0000100);
    gpio_set_level(SEG_D, segments & 0b0001000);
    gpio_set_level(SEG_E, segments & 0b0010000);
    gpio_set_level(SEG_F, segments & 0b0100000);
    gpio_set_level(SEG_G, segments & 0b1000000);
    gpio_set_level(SEG_DP, 0); // decimal apagado
}

