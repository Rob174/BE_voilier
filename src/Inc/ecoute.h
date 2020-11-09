#include "stm32f103xb.h" 
#include "stm32f1xx_ll_tim.h"

LL_TIM_OC_InitTypeDef initialisation_PWM = {
		/*OCMode : */ LL_TIM_OCMODE_PWM1,//In upcounting, channel y is active as long as TIMx_CNT<TIMx_CCRy else inactive. In downcounting, channel y is inactive as long as TIMx_CNT>TIMx_CCRy else active.
		/*OCState : */ LL_TIM_OCSTATE_ENABLE, //OCx signal is output on the corresponding output pin
		/*OCNState : */ LL_TIM_OCSTATE_DISABLE,//complementary OCx signal is NOT output on the corresponding output pin (on n'en a pas besoin)
		/*CompareValue : */ 0,//Specifies the Compare value to be loaded into the Capture Compare Register: moteur arrêté au début
		/*OCPolarity : */ LL_TIM_OCPOLARITY_HIGH ,//Specifies the output polarity : commence par un niveau haut
		/*OCNPolarity : */ LL_TIM_OCPOLARITY_HIGH, //Sans importance car désactivé
		/*OCIdleState : */ LL_TIM_OCIDLESTATE_LOW,//Specifies the TIM Output Compare pin state during Idle state : ne veut pas que le servo-moteur tourne quand pas d'instruction
		/*OCNIdleState : */  LL_TIM_OCIDLESTATE_LOW//Sans importance car désactivé
	};
TIM_TypeDef *MyEcoute_Timer;
void MyEcoute_Conf(TIM_TypeDef * Timer);
void MyEcoute_IT_Enable(TIM_TypeDef * Timer);
void MyEcoute_Start();
void MyEcoute_Stop();
void MyEcoute_Changer_consigne(int consigneCourante);