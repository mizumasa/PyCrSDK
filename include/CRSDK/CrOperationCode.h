#ifndef CROPERATIONCODE_H
#define CROPERATIONCODE_H

#include "CrTypes.h"
#include <memory>
#include <vector>

#if defined(_MSC_VER)
// Windows definitions
#ifdef CR_SDK_EXPORTS
#define SCRSDK_API __declspec(dllexport)
#else
#define SCRSDK_API __declspec(dllimport)
#endif
// End Windows definitions
#else
#if defined(__GNUC__)
#ifdef CR_SDK_EXPORTS
#define SCRSDK_API __attribute__ ((visibility ("default")))
#else
#define SCRSDK_API
#endif
#endif
#endif

namespace SCRSDK
{
enum CrOperationCode : CrInt16u
{
	CrOperationCode_GetLicenseInfoList = 0x924D,
};

#pragma pack(1)
class SCRSDK_API CrOperationResultData
{
public:
	CrOperationResultData()
	{
	}

	virtual ~CrOperationResultData()
	{
	}
};
#pragma pack()
#pragma pack(1)
class SCRSDK_API CrLicenseInfo
{
public:
	CrLicenseInfo();
	CrLicenseInfo(const CrLicenseInfo& ref);
	CrLicenseInfo(CrLicenseInfo&& ref) noexcept;

	~CrLicenseInfo();

	CrLicenseInfo& operator = (const CrLicenseInfo& ref);

	// License Info Remaining Hours
	static constexpr CrInt32u CrLicenseRemainingHours_Infinity = 0xFFFFFFFF;

	CrInt8u GetLicenseIdLength() const;
	const CrInt8u* GetLicenseId() const;
	CrInt32u GetRemainingHours() const;

private:
	CrInt32u remainingHours;
	CrInt8u  licenseIdLength;
	CrInt8u* licenseId;
};
#pragma pack()
#pragma pack(1)
class SCRSDK_API CrLicenseInfoList : public CrOperationResultData
{
public:
	CrLicenseInfoList();
	CrLicenseInfoList(CrLicenseInfo* list, CrInt8u listNum);
	CrLicenseInfoList(const CrLicenseInfoList& ref);
	~CrLicenseInfoList();
	CrLicenseInfoList& operator = (const CrLicenseInfoList& ref);
	
	CrLicenseInfo* GetLicenseInfo() const;
	const CrInt8u GetListNum() const;

private:
	CrLicenseInfo* list;
	CrInt8u  listNum;
};
#pragma pack()

}
#endif // CROPERATIONCODE_H
