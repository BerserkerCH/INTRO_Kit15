<<<<<<< HEAD
/**
 * \file
 * \brief Quadrature Calibration Module.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This is the interface to the quadrature calibration module.
 */

#ifndef QUADCALIB_H_
#define QUADCALIB_H_

#include "Platform.h"
#if PL_CONFIG_HAS_QUAD_CALIBRATION

#if PL_CONFIG_HAS_SHELL
#include "CLS1.h"

uint8_t QUADCALIB_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
#endif

#endif /* PL_CONFIG_HAS_QUAD_CALIBRATION */

#endif /* QUADCALIB_H_ */
=======
/**
 * \file
 * \brief Quadrature Calibration Module.
 * \author Erich Styger, erich.styger@hslu.ch
 *
 * This is the interface to the quadrature calibration module.
 */

#ifndef QUADCALIB_H_
#define QUADCALIB_H_

#include "Platform.h"
#if PL_CONFIG_HAS_QUAD_CALIBRATION

#if PL_CONFIG_HAS_SHELL
#include "CLS1.h"

uint8_t QUADCALIB_ParseCommand(const unsigned char *cmd, bool *handled, const CLS1_StdIOType *io);
#endif

#endif /* PL_CONFIG_HAS_QUAD_CALIBRATION */

#endif /* QUADCALIB_H_ */
>>>>>>> 0017e4b2604695de8388f95d077525bce50dad25
