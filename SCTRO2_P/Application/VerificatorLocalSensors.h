/*
 * VerificatorLocalSensors.h
 *
 *  Created on: 02/set/2018
 *      Author: W5
 */

#ifndef APPLICATION_VERIFICATORLOCALSENSORS_H_
#define APPLICATION_VERIFICATORLOCALSENSORS_H_

void InitVerificatorLocalParams(void);
void DisableOrganProtectionChecks(void);
void EnableOrganProtectionChecks(void);

void DisableHWVerification(void);

void VerifyArterialPressure(uint16_t ValPress);
void VerifyOxygenPressure(uint16_t Value);
void VerifyTubPressure(uint16_t Value);
void VerifyFilterPressure(uint16_t  Value);
void VerifyVenousPressure(uint16_t  Value);
void VerifyArterialPressure(uint16_t  Value);

void VerifyRxAirLevels(uint8_t AirArtLevel, uint8_t AirVenLevel);

void VerifyPlateTemp(float Value);
void VerifyArtTemp(float Value);
void VerifyVenTemp(float Value);
void VerifyFluidTemp(float Value);

#endif /* APPLICATION_VERIFICATORLOCALSENSORS_H_ */
