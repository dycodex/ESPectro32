/*
 * ex_ledmatrix_anim.hpp
 *
 *  Created on: Sep 15, 2017
 *      Author: andri
 */

#ifndef COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_LEDMATRIX_ANIM_HPP_
#define COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_LEDMATRIX_ANIM_HPP_


#include <esp_log.h>

#include <ESPectro32_Board.h>

#include <ESPectro32_LedMatrix_Animation.h>
ESPectro32_LedMatrix_ScrollTextAnimation ledMatrixTextAnim;

static void load_ex_ledmatrix_anim() {
	ledMatrixTextAnim.setLedMatrix(ESPectro32.LedMatrix());
	ledMatrixTextAnim.scrollText("Hello World!!", 6000);
}

#endif /* COMPONENTS_ESPECTRO32_EXAMPLES_IDF_EX_LEDMATRIX_ANIM_HPP_ */
