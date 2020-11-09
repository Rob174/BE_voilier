#include "ecoute.h"

void MyEcoute_IT_Enable(TIM_TypeDef * Timer) {
	Timer->DIER |= TIM_DIER_UIE;
}
void MyEcoute_Conf(TIM_TypeDef * Timer) {
	int CHANNEL = 1;
	//Configuration du Timer sur le channel 1 (cf p29 pr voir pin de sortie)
	LL_TIM_OC_Init(Timer,CHANNEL,&initialisation_PWM);
	if (Timer == TIM1 ) {
		// On active l'horloge du timer 1
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN ;
	}
	else if (Timer == TIM2 ) {
		// On active l'horloge des timers 2
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if (Timer == TIM3) {
		// On active l'horloge des timers 3
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if (Timer == TIM4) {
		// On active l'horloge des timers 4
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	Timer->CR1 &= ~TIM_CR1_OPM;
	
	// Active la sortie du prescaler CK_CNT
	Timer->CR1 |= TIM_CR1_CEN;
	//Sauvegarde le pointeur du timer pour les autres fonctions
	MyEcoute_Timer = Timer;
	//Allume le timer
	MyEcoute_IT_Enable(Timer);
}
void MyEcoute_Start()
{
	// Identifier quel TIM est donné en paramètre et activer celui voulu
	if(MyEcoute_Timer == TIM1)
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	else if(MyEcoute_Timer == TIM2)
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	else if(MyEcoute_Timer == TIM3)
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	else if(MyEcoute_Timer == TIM4)
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
}
void MyEcoute_Stop()
{	
	// Identifier quel TIM est donné en paramètre et désactiver celui voulu
	if(MyEcoute_Timer == TIM1)
		RCC->APB2ENR &= ~(RCC_APB2ENR_TIM1EN);
	else if(MyEcoute_Timer == TIM2)
		RCC->APB1ENR &= ~(RCC_APB1ENR_TIM2EN);
	else if(MyEcoute_Timer == TIM3)
		RCC->APB1ENR &= ~(RCC_APB1ENR_TIM3EN);
	else if(MyEcoute_Timer == TIM4)
		RCC->APB1ENR &= ~(RCC_APB1ENR_TIM4EN);
}
void MyEcoute_Changer_consigne(int consigneCourante) {
	LL_TIM_OC_SetCompareCH1(MyEcoute_Timer,consigneCourante);
}
