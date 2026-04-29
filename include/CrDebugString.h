/* get enum string from enum value for debug */

#ifndef CRERRORSTRING_H
#define CRERRORSTRING_H

#include <CrTypes.h>
#include <CrError.h>
#include <CrDeviceProperty.h>
#include <CrCommandData.h>
#include <CrOperationCode.h>
#include <CrControlCode.h>

std::string CrErrorString(SCRSDK::CrError error);
std::string CrWarningExtString(SCRSDK::CrError error, CrInt32 param1, CrInt32 param2, CrInt32 param3);

std::string CrCommandIdString(SCRSDK::CrCommandId id);
std::string CrDevicePropertyString(SCRSDK::CrDevicePropertyCode code);
std::string CrControlCodeString(SCRSDK::CrControlCode code);
std::string CrOperationCodeString(SCRSDK::CrOperationCode code);
std::string CrCameraDeviceModelString(CrInt32 id);

SCRSDK::CrCommandId CrCommandIdCode(std::string name);
SCRSDK::CrDevicePropertyCode CrDevicePropertyCode(std::string name);
SCRSDK::CrControlCode CrControlCode(std::string name);
SCRSDK::CrOperationCode CrOperationCode(std::string name);
CrInt32 CrCameraDeviceModelIdCode(std::string name);

#endif // CRERRORSTRING_H
