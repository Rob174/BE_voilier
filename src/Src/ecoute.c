// TOUT A FAIRE !! //

/*
 indispensable pour pouvoir adresser les registres des p�riph�riques.
 Rem : OBLIGATION d'utiliser les d�finitions utiles contenues dans ce fichier (ex : TIM_CR1_CEN, RCC_APB1ENR_TIM2EN ...)
 pour une meilleure lisibilit� du code.
 Pour les masques, utiliser �galement les d�finitions propos�e
 Rappel : pour mettre � 1  , reg = reg | Mask (ou Mask est le repr�sente le ou les bits � positionner � 1)
				  pour mettre � 0  , reg = reg&~ Mask (ou Mask est le repr�sente le ou les bits � positionner � 0)
 
*/ 
#include "stm32f103xb.h" 
#include "stm32f1xx_ll_tim.h"

void (*TIM1_handler_fct)(void);
void (*TIM2_handler_fct)(void);
void (*TIM3_handler_fct)(void);
void (*TIM4_handler_fct)(void);

/* Philosophie globale 
cf p 366 et 384
On a deux voies pour un d�compte de la p�riode globale et un d�compte du temps au niveau haut

*/
void MyEcoute_Start(TIM_TypeDef * Timer)
{
	/*
	Timer = TIM1, TIM2, TIM3 ou TIM4:
	
	TIM1EN: TIM1 timer clock enable
		Set and cleared by software.
		0: TIM1 clock disabled
		1: TIM1 clock enabled
	*/
	
	// Identifier quel TIM est donn� en param�tre et activer celui voulu
	if(Timer == TIM1)
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	else if(Timer == TIM2)
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	else if(Timer == TIM3)
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	else if(Timer == TIM4)
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
}

void MyEcoute_Stop(TIM_TypeDef * Timer)
{
	/*
	Timer = TIM1, TIM2, TIM3 ou TIM4:
	
	TIM1EN: TIM1 timer clock enable
		Set and cleared by software.
		0: TIM1 clock disabled
		1: TIM1 clock enabled
	*/
	
	// Identifier quel TIM est donn� en param�tre et d�sactiver celui voulu
	if(Timer == TIM1)
		RCC->APB2ENR &= ~(RCC_APB2ENR_TIM1EN);
	else if(Timer == TIM2)
		RCC->APB1ENR &= ~(RCC_APB1ENR_TIM2EN);
	else if(Timer == TIM3)
		RCC->APB1ENR &= ~(RCC_APB1ENR_TIM3EN);
	else if(Timer == TIM4)
		RCC->APB1ENR &= ~(RCC_APB1ENR_TIM4EN);
}
/**
	* @brief  Active l'horloge et r�gle l'ARR et le PSC du timer vis�
  * @note   Fonction � lancer avant toute autre. Le timer n'est pas encore lanc� (voir MyTimerStart)
	* @param  TIM_TypeDef Timer : indique le timer � utiliser par le chronom�tre, TIM1, TIM2, TIM3 ou TIM4
	* 				int Arr   : valeur � placer dans ARR
	*					int Psc   : valeur � placer dans PSC
  * @retval None
  */

void MyEcoute_Conf(TIM_TypeDef * Timer, void (*handler) (void), int priorite) {
	//active l'horloge pour les timers 2,3,4,5,6,7,12,13,14
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
	//Reprise de l'exemple de la page 384
	//S�lection param�tres pour TI1FP1 et TI1FP2 (1 ou 2 not� par a)
	//Configuration de TI1FP1
	//S�lectionner input CC1S de TIMx_CCMR1 (p414)
	Timer->CCMR1 |= TIM_CCMR1_CC1S_0;
	//Choix de la polarit� (high = front montant) CC1P de TIMx_CCER (p417)
	Timer->CCER |= ~TIM_CCER_CC1P; //active high
	//Configuration de TI1FP2
	//S�lectionner input CC1S de TIMx_CCMR1 (p414)
	Timer->CCMR2 |= TIM_CCMR1_CC1S_1;
	//Choix de la polarit� (high = front montant) CC1P de TIMx_CCER (p417)
	Timer->CCER |= TIM_CCER_CC1P;//active low
	//Configuration TI1FP1 et TI1FP2
	//On attend 8/72MHz ~ 0.1�s avant de comptabiliser une transition : choix du filtre (p455)
	Timer->CCMR1 |= TIM_CCMR1_IC1F_0 | TIM_CCMR1_IC1F_1;
	
	//Choisir le d�clencheur TS  de TIMx_SMCR (p406 ici 407)
	Timer->SMCR |= (TIM_SMCR_TS_0 | TIM_SMCR_TS_2);
	//Configurer le contr�leur en mode reset (recompte depuis 0 � chaque front montant) SMS de TIMx_SMCR
	Timer->SMCR |= TIM_SMCR_SMS_2;
	//Choix du prescaler pour le comptage
	//Servo : Niveau haut entre 1 et 2ms et p�riode de 20ms
	//Val max de compteur 2^16 ; en mettant 1 prescaler de 100 on aura 2ms=1440 comptages et 20ms=14400 comptages < 65536
	Timer->PSC = 100;//TOASK : --> � mettre en couche service ???
	//Ne touche pas � l'ARR car le compteur sera reset � chaque front montant d'apr�s qlqs lignes au dessus
	//Activer la capture CC1E et CC2E de TIMx_CCER
	Timer->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
	
	// Active la sortie du prescaler CK_CNT
	Timer->CR1 |= TIM_CR1_CEN;
	// Pr�paration r�cup�ration valeur
	Timer->DIER |= LL_TIM_DIER_CC1IE; //active l'interruption 1

	if (Timer == TIM1 ) {
		NVIC->ISER[0] |= 1 << 27;
		NVIC->IP[27] = priorite;
		TIM1_handler_fct = handler;
	}
	else if (Timer == TIM2 ) {
		NVIC->ISER[0] |= 1 << 28;
		NVIC->IP[28] = priorite;
		TIM2_handler_fct = handler;
	}
	else if (Timer == TIM3) {
		NVIC->ISER[0] |= 1 << 29;
		NVIC->IP[29] = priorite;
		TIM3_handler_fct = handler;
	}
	else if (Timer == TIM4) {
		NVIC->ISER[0] |= 1 << 30;
		NVIC->IP[30] = priorite;
		TIM4_handler_fct = handler;
	}
}
void MyEcoute_IT_Enable(TIM_TypeDef * Timer) {
	if(Timer == TIM1) {
		TIM1->DIER |= TIM_DIER_UIE;
	}
	else if (Timer == TIM2) {
		TIM2->DIER |= TIM_DIER_UIE;
	}
	else if (Timer == TIM3) {
		TIM3->DIER |= TIM_DIER_UIE;
	}
	else if (Timer == TIM4) {
		TIM4->DIER |= TIM_DIER_UIE;
	}
}

void TIM1_UP_Ecoute_IRQHandler(void) {
	(*TIM1_handler_fct)();
	//page 345
	TIM1->SR &= ~TIM_SR_UIF;
	
}
void TIM2_Ecoute_IRQHandler(void) {
	(*TIM2_handler_fct)();
	TIM2->SR &= ~TIM_SR_UIF;
}
void TIM3_Ecoute_IRQHandler(void) {
	(*TIM3_handler_fct)();
	TIM3->SR &= ~TIM_SR_UIF;
}
void TIM4_Ecoute_IRQHandler(void) {
	(*TIM4_handler_fct)();
	TIM4->SR &= ~TIM_SR_UIF;
}
