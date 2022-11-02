#include "ROBLEX.h"
#include "driver/mcpwm.h"


void ROBLEX::SetupMotor(uint8_t port, mcpwm_unit_t unit, uint32_t frequency) {

  int * p = ROBLEX::GetPins(port);
  int pinA = p[0];
  int pinB = p[1];

  //mcpwm_gpio_init(unidad PWM , salida A, puerto GPIO)
  mcpwm_gpio_init(unit, MCPWM0A, pinA);

  //mcpwm_gpio_init(unidad PWM , salida A, puerto GPIO)
  mcpwm_gpio_init(unit, MCPWM0B, pinB);

  mcpwm_config_t pwm_config;

  pwm_config.frequency = frequency;                          //frequência = 500Hz,
  pwm_config.cmpr_a = 0;                                //Ciclo de trabalho (duty cycle) do PWMxA = 0
  pwm_config.cmpr_b = 0;                                //Ciclo de trabalho (duty cycle) do PWMxb = 0
  pwm_config.counter_mode = MCPWM_UP_COUNTER;           //Para MCPWM assimetrico
  pwm_config.duty_mode = MCPWM_DUTY_MODE_1;             //Define ciclo de trabalho em nível alto
  //Inicia(Unidade 0, Timer 0, Config PWM)
  mcpwm_init(unit, MCPWM_TIMER_0, &pwm_config); //Define PWM0A & PWM0B com as configurações acima
}



void ROBLEX::MotorForward(mcpwm_unit_t mcpwm_num , int duty_cycle)
{
  mcpwm_set_signal_high(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_A);                    //pinA a HIGH
  mcpwm_set_duty(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_B, duty_cycle);              //pwm para el pinB
  mcpwm_set_duty_type(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_B, MCPWM_DUTY_MODE_1);
}


void ROBLEX::MotorBackward(mcpwm_unit_t mcpwm_num, int duty_cycle)
{
  mcpwm_set_signal_high(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_B);                    //pinB a HIGH
  mcpwm_set_duty(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_A, duty_cycle);              //pwm para el pinB
  mcpwm_set_duty_type(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_A, MCPWM_DUTY_MODE_1);
}


void ROBLEX::MotorStop(mcpwm_unit_t mcpwm_num)
{
  mcpwm_set_signal_high(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_A); //pinA a HIGH
  mcpwm_set_signal_high(mcpwm_num, MCPWM_TIMER_0, MCPWM_OPR_B); //pinB a HIGH
}
