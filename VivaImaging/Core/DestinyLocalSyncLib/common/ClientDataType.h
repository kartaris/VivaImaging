// Destiny Msgpack RPC Server generated source.
//
#pragma once

typedef struct {
int    fault_occurred;
int    fault_code;
wchar_t * fault_string;
}ERROR_ST;


template<typename T>
struct BASE
{
BASE() : size{ sizeof(T) } {}
const unsigned int size;
};

namespace DestinyMsgpack_LSIF
{
using Int = int;
using Bool = bool;
using Float = float;
using Double = double;
using Byte = unsigned char;
//using Bytes = msgpack::type::raw_ref;
using Bytes = Byte*;
using Long = long long;

struct XEXTCOLUMNVALUE_;
struct XDOCTYPE_;
struct XSECURITYPOLICY_;
struct XURLLINK_;
struct XFILEHISTORY_;
struct XEXTATTRIBUTE_;
struct CLIENTZONEINFO_;
struct XUSERINFO_;
struct FOLDERITEM_;
struct XMLRPCSEARCHPARAMS_;
struct ASYNCCOPYFOLDERSTATUS_;
struct XCODELIST_;
struct XCONVERTFILE_;
struct XEXTATTRIBUTEVALUE_;
struct XSYNCFOLDER_;
struct XCONFIGURATION_;
struct XTAG_;
struct XCODEVALUE_;
struct XAPPLICATIONPOLICY_;
struct ECMOBJECT_;
struct XAPPLICATIONEXCLUSIVENWPOLICY_;
struct XMLRPCLOGINCONTEXT_;
struct XFILE_;
struct XSYNCDOCUMENTWITHEXTATTR_;
struct XSITERULE_;
struct XMLRPCDOCUMENT_;
struct XNETWORKRULE_;
struct XAMOUNTPOLICY_;
struct XGROUP_;
struct XMEDIARULE_;
struct RPCDOCUMENT_;
struct XDLPPOLICY_;
struct RETURNINTVO_;
struct XSYNCFOLDERLIST_;
struct RETURNVO_;
struct XASYNCOPERATIONRESULT_;
struct XSYNCDOCUMENTLIST_;
struct XSYNCDOCUMENT_;
struct XMLRPCUSER_;
struct XEXTCOLUMN_;
struct DRIVEOBJECT_;
struct LOGINPARAM_;
struct XMLRPCDOCTYPE_;
struct NETWORKDRIVERESPONSE_;
struct XMLRPCCONFIGURATION_;
struct XPROCESSRULE_;
struct DOWNLOADDRIVEOBJECTLIST_;
struct RPCFILE_;
struct DRIVEOBJECTLIST_;
struct XSYNCUSERINFO_;
struct ASYNCOPERATIONSTATUS_;
struct LICENSESTATUS_;
struct XPRINTRULE_;
struct XMLRPCWATERMARKCONFIG_;
struct XUSERPOLICY_;
struct NETWORKCONTROLEXCLUSIVEPOLICY_;
struct DOWNLOADDRIVEOBJECT_;
struct XACE_;


struct XEXTCOLUMNVALUE_ : BASE<XEXTCOLUMNVALUE_> {
	bool update;
	const wchar_t* key;
	const wchar_t* value;
	const wchar_t* valuetype;
	XEXTCOLUMNVALUE_()
		: BASE<XEXTCOLUMNVALUE_>()
		, update(0)
		, key(0)
		, value(0)
		, valuetype(0)
	{
	}
	XEXTCOLUMNVALUE_(const XEXTCOLUMNVALUE_& src)
		: BASE<XEXTCOLUMNVALUE_>()
		, update(src.update)
		, key(src.key)
		, value(src.value)
		, valuetype(src.valuetype)
	{
	}
	XEXTCOLUMNVALUE_(const XEXTCOLUMNVALUE_* src)
		: BASE<XEXTCOLUMNVALUE_>()
		, update(src->update)
		, key(src->key)
		, value(src->value)
		, valuetype(src->valuetype)
	{
	}
	XEXTCOLUMNVALUE_& operator=(const XEXTCOLUMNVALUE_& src)
	{
		if (this != &src) {
			update = src.update;
			key = src.key;
			value = src.value;
			valuetype = src.valuetype;
		}
		return *this;
	}
};

struct XDOCTYPE_ : BASE<XDOCTYPE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* extAttributeName;
	XEXTATTRIBUTE_* extAttribute;
	const wchar_t** propertyDisplayInfo;
	int propertyDisplayInfoCNT;
	const wchar_t* categoryFullPathName;
	Long lastUsedAt;
	const wchar_t* extAttributeOID;
	const wchar_t* categoryOID;
	const wchar_t* name;
	const wchar_t* creatorOID;
	const wchar_t* description;
	const wchar_t* defaultAclOID;
	const wchar_t* defaultAclLevelOID;
	bool flagInheritACL;
	int maintainDuration;
	const wchar_t* imagePath;
	Long createdAt;
	Long activatedAt;
	Long deactivatedAt;
	const wchar_t* propertyDisplayOrder;
	bool flagActivated;
	bool flagExtAttribute;
	bool flagRouting;
	bool flagChangeACL;
	bool flagChangeMainPeriod;
	bool flagRequiredTag;
	bool flagSaveNewVersion;
	const wchar_t* sortOrder;
	XDOCTYPE_()
		: BASE<XDOCTYPE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, extAttributeName(0)
		, extAttribute(0)
		, propertyDisplayInfo(0)
		, propertyDisplayInfoCNT(0)
		, categoryFullPathName(0)
		, lastUsedAt(0)
		, extAttributeOID(0)
		, categoryOID(0)
		, name(0)
		, creatorOID(0)
		, description(0)
		, defaultAclOID(0)
		, defaultAclLevelOID(0)
		, flagInheritACL(0)
		, maintainDuration(0)
		, imagePath(0)
		, createdAt(0)
		, activatedAt(0)
		, deactivatedAt(0)
		, propertyDisplayOrder(0)
		, flagActivated(0)
		, flagExtAttribute(0)
		, flagRouting(0)
		, flagChangeACL(0)
		, flagChangeMainPeriod(0)
		, flagRequiredTag(0)
		, flagSaveNewVersion(0)
		, sortOrder(0)
	{
	}
	XDOCTYPE_(const XDOCTYPE_& src)
		: BASE<XDOCTYPE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, extAttributeName(src.extAttributeName)
		, extAttribute(src.extAttribute)
		, propertyDisplayInfo(src.propertyDisplayInfo)
		, propertyDisplayInfoCNT(src.propertyDisplayInfoCNT)
		, categoryFullPathName(src.categoryFullPathName)
		, lastUsedAt(src.lastUsedAt)
		, extAttributeOID(src.extAttributeOID)
		, categoryOID(src.categoryOID)
		, name(src.name)
		, creatorOID(src.creatorOID)
		, description(src.description)
		, defaultAclOID(src.defaultAclOID)
		, defaultAclLevelOID(src.defaultAclLevelOID)
		, flagInheritACL(src.flagInheritACL)
		, maintainDuration(src.maintainDuration)
		, imagePath(src.imagePath)
		, createdAt(src.createdAt)
		, activatedAt(src.activatedAt)
		, deactivatedAt(src.deactivatedAt)
		, propertyDisplayOrder(src.propertyDisplayOrder)
		, flagActivated(src.flagActivated)
		, flagExtAttribute(src.flagExtAttribute)
		, flagRouting(src.flagRouting)
		, flagChangeACL(src.flagChangeACL)
		, flagChangeMainPeriod(src.flagChangeMainPeriod)
		, flagRequiredTag(src.flagRequiredTag)
		, flagSaveNewVersion(src.flagSaveNewVersion)
		, sortOrder(src.sortOrder)
	{
	}
	XDOCTYPE_(const XDOCTYPE_* src)
		: BASE<XDOCTYPE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, extAttributeName(src->extAttributeName)
		, extAttribute(src->extAttribute)
		, propertyDisplayInfo(src->propertyDisplayInfo)
		, propertyDisplayInfoCNT(src->propertyDisplayInfoCNT)
		, categoryFullPathName(src->categoryFullPathName)
		, lastUsedAt(src->lastUsedAt)
		, extAttributeOID(src->extAttributeOID)
		, categoryOID(src->categoryOID)
		, name(src->name)
		, creatorOID(src->creatorOID)
		, description(src->description)
		, defaultAclOID(src->defaultAclOID)
		, defaultAclLevelOID(src->defaultAclLevelOID)
		, flagInheritACL(src->flagInheritACL)
		, maintainDuration(src->maintainDuration)
		, imagePath(src->imagePath)
		, createdAt(src->createdAt)
		, activatedAt(src->activatedAt)
		, deactivatedAt(src->deactivatedAt)
		, propertyDisplayOrder(src->propertyDisplayOrder)
		, flagActivated(src->flagActivated)
		, flagExtAttribute(src->flagExtAttribute)
		, flagRouting(src->flagRouting)
		, flagChangeACL(src->flagChangeACL)
		, flagChangeMainPeriod(src->flagChangeMainPeriod)
		, flagRequiredTag(src->flagRequiredTag)
		, flagSaveNewVersion(src->flagSaveNewVersion)
		, sortOrder(src->sortOrder)
	{
	}
	XDOCTYPE_& operator=(const XDOCTYPE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			extAttributeName = src.extAttributeName;
			extAttribute = src.extAttribute;
			propertyDisplayInfo = src.propertyDisplayInfo;
			propertyDisplayInfoCNT = src.propertyDisplayInfoCNT;
			categoryFullPathName = src.categoryFullPathName;
			lastUsedAt = src.lastUsedAt;
			extAttributeOID = src.extAttributeOID;
			categoryOID = src.categoryOID;
			name = src.name;
			creatorOID = src.creatorOID;
			description = src.description;
			defaultAclOID = src.defaultAclOID;
			defaultAclLevelOID = src.defaultAclLevelOID;
			flagInheritACL = src.flagInheritACL;
			maintainDuration = src.maintainDuration;
			imagePath = src.imagePath;
			createdAt = src.createdAt;
			activatedAt = src.activatedAt;
			deactivatedAt = src.deactivatedAt;
			propertyDisplayOrder = src.propertyDisplayOrder;
			flagActivated = src.flagActivated;
			flagExtAttribute = src.flagExtAttribute;
			flagRouting = src.flagRouting;
			flagChangeACL = src.flagChangeACL;
			flagChangeMainPeriod = src.flagChangeMainPeriod;
			flagRequiredTag = src.flagRequiredTag;
			flagSaveNewVersion = src.flagSaveNewVersion;
			sortOrder = src.sortOrder;
		}
		return *this;
	}
};

struct XSECURITYPOLICY_ : BASE<XSECURITYPOLICY_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	int appliedUserCount;
	const wchar_t* name;
	bool flagSystemDefault;
	bool flagDefault;
	Long createdAt;
	const wchar_t* applPermission;
	const wchar_t* explorerPermission;
	const wchar_t* secuDDPermission;
	const wchar_t* ecmDDPermission;
	const wchar_t* clipBoardPermission;
	const wchar_t* capturePermission;
	const wchar_t* printPermission;
	const wchar_t* outlookPermission;
	const wchar_t* centralDisk;
	const wchar_t* sortOrder;
	const wchar_t* applPermissionExternal;
	const wchar_t* explorerPermissionExternal;
	const wchar_t* secuDDPermissionExternal;
	const wchar_t* ecmDDPermissionExternal;
	const wchar_t* clipBoardPermissionExternal;
	const wchar_t* capturePermissionExternal;
	const wchar_t* printPermissionExternal;
	const wchar_t* outlookPermissionExternal;
	const wchar_t* mobileFileType;
	const wchar_t* mobileWatermark;
	const wchar_t* outlookFileAttachType;
	const wchar_t* outlookExternalFileAttachType;
	const wchar_t* uninstallPermission;
	const wchar_t* dLPPolicyOID;
	const wchar_t* pcFileCollectType;
	const wchar_t* pcFileCollectTypeExtra;
	XDLPPOLICY_* dlpPolicy;
	XSECURITYPOLICY_()
		: BASE<XSECURITYPOLICY_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, appliedUserCount(0)
		, name(0)
		, flagSystemDefault(0)
		, flagDefault(0)
		, createdAt(0)
		, applPermission(0)
		, explorerPermission(0)
		, secuDDPermission(0)
		, ecmDDPermission(0)
		, clipBoardPermission(0)
		, capturePermission(0)
		, printPermission(0)
		, outlookPermission(0)
		, centralDisk(0)
		, sortOrder(0)
		, applPermissionExternal(0)
		, explorerPermissionExternal(0)
		, secuDDPermissionExternal(0)
		, ecmDDPermissionExternal(0)
		, clipBoardPermissionExternal(0)
		, capturePermissionExternal(0)
		, printPermissionExternal(0)
		, outlookPermissionExternal(0)
		, mobileFileType(0)
		, mobileWatermark(0)
		, outlookFileAttachType(0)
		, outlookExternalFileAttachType(0)
		, uninstallPermission(0)
		, dLPPolicyOID(0)
		, pcFileCollectType(0)
		, pcFileCollectTypeExtra(0)
		, dlpPolicy(0)
	{
	}
	XSECURITYPOLICY_(const XSECURITYPOLICY_& src)
		: BASE<XSECURITYPOLICY_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, appliedUserCount(src.appliedUserCount)
		, name(src.name)
		, flagSystemDefault(src.flagSystemDefault)
		, flagDefault(src.flagDefault)
		, createdAt(src.createdAt)
		, applPermission(src.applPermission)
		, explorerPermission(src.explorerPermission)
		, secuDDPermission(src.secuDDPermission)
		, ecmDDPermission(src.ecmDDPermission)
		, clipBoardPermission(src.clipBoardPermission)
		, capturePermission(src.capturePermission)
		, printPermission(src.printPermission)
		, outlookPermission(src.outlookPermission)
		, centralDisk(src.centralDisk)
		, sortOrder(src.sortOrder)
		, applPermissionExternal(src.applPermissionExternal)
		, explorerPermissionExternal(src.explorerPermissionExternal)
		, secuDDPermissionExternal(src.secuDDPermissionExternal)
		, ecmDDPermissionExternal(src.ecmDDPermissionExternal)
		, clipBoardPermissionExternal(src.clipBoardPermissionExternal)
		, capturePermissionExternal(src.capturePermissionExternal)
		, printPermissionExternal(src.printPermissionExternal)
		, outlookPermissionExternal(src.outlookPermissionExternal)
		, mobileFileType(src.mobileFileType)
		, mobileWatermark(src.mobileWatermark)
		, outlookFileAttachType(src.outlookFileAttachType)
		, outlookExternalFileAttachType(src.outlookExternalFileAttachType)
		, uninstallPermission(src.uninstallPermission)
		, dLPPolicyOID(src.dLPPolicyOID)
		, pcFileCollectType(src.pcFileCollectType)
		, pcFileCollectTypeExtra(src.pcFileCollectTypeExtra)
		, dlpPolicy(src.dlpPolicy)
	{
	}
	XSECURITYPOLICY_(const XSECURITYPOLICY_* src)
		: BASE<XSECURITYPOLICY_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, appliedUserCount(src->appliedUserCount)
		, name(src->name)
		, flagSystemDefault(src->flagSystemDefault)
		, flagDefault(src->flagDefault)
		, createdAt(src->createdAt)
		, applPermission(src->applPermission)
		, explorerPermission(src->explorerPermission)
		, secuDDPermission(src->secuDDPermission)
		, ecmDDPermission(src->ecmDDPermission)
		, clipBoardPermission(src->clipBoardPermission)
		, capturePermission(src->capturePermission)
		, printPermission(src->printPermission)
		, outlookPermission(src->outlookPermission)
		, centralDisk(src->centralDisk)
		, sortOrder(src->sortOrder)
		, applPermissionExternal(src->applPermissionExternal)
		, explorerPermissionExternal(src->explorerPermissionExternal)
		, secuDDPermissionExternal(src->secuDDPermissionExternal)
		, ecmDDPermissionExternal(src->ecmDDPermissionExternal)
		, clipBoardPermissionExternal(src->clipBoardPermissionExternal)
		, capturePermissionExternal(src->capturePermissionExternal)
		, printPermissionExternal(src->printPermissionExternal)
		, outlookPermissionExternal(src->outlookPermissionExternal)
		, mobileFileType(src->mobileFileType)
		, mobileWatermark(src->mobileWatermark)
		, outlookFileAttachType(src->outlookFileAttachType)
		, outlookExternalFileAttachType(src->outlookExternalFileAttachType)
		, uninstallPermission(src->uninstallPermission)
		, dLPPolicyOID(src->dLPPolicyOID)
		, pcFileCollectType(src->pcFileCollectType)
		, pcFileCollectTypeExtra(src->pcFileCollectTypeExtra)
		, dlpPolicy(src->dlpPolicy)
	{
	}
	XSECURITYPOLICY_& operator=(const XSECURITYPOLICY_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			appliedUserCount = src.appliedUserCount;
			name = src.name;
			flagSystemDefault = src.flagSystemDefault;
			flagDefault = src.flagDefault;
			createdAt = src.createdAt;
			applPermission = src.applPermission;
			explorerPermission = src.explorerPermission;
			secuDDPermission = src.secuDDPermission;
			ecmDDPermission = src.ecmDDPermission;
			clipBoardPermission = src.clipBoardPermission;
			capturePermission = src.capturePermission;
			printPermission = src.printPermission;
			outlookPermission = src.outlookPermission;
			centralDisk = src.centralDisk;
			sortOrder = src.sortOrder;
			applPermissionExternal = src.applPermissionExternal;
			explorerPermissionExternal = src.explorerPermissionExternal;
			secuDDPermissionExternal = src.secuDDPermissionExternal;
			ecmDDPermissionExternal = src.ecmDDPermissionExternal;
			clipBoardPermissionExternal = src.clipBoardPermissionExternal;
			capturePermissionExternal = src.capturePermissionExternal;
			printPermissionExternal = src.printPermissionExternal;
			outlookPermissionExternal = src.outlookPermissionExternal;
			mobileFileType = src.mobileFileType;
			mobileWatermark = src.mobileWatermark;
			outlookFileAttachType = src.outlookFileAttachType;
			outlookExternalFileAttachType = src.outlookExternalFileAttachType;
			uninstallPermission = src.uninstallPermission;
			dLPPolicyOID = src.dLPPolicyOID;
			pcFileCollectType = src.pcFileCollectType;
			pcFileCollectTypeExtra = src.pcFileCollectTypeExtra;
			dlpPolicy = src.dlpPolicy;
		}
		return *this;
	}
};

struct XURLLINK_ : BASE<XURLLINK_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	XFILE_* imageFile;
	const wchar_t* creatorName;
	const wchar_t* name;
	Long createdAt;
	const wchar_t* creatorOID;
	Long deActivatedAt;
	const wchar_t* linkType;
	const wchar_t* urlTarget;
	const wchar_t* browserAttributes;
	const wchar_t* linkUrl;
	const wchar_t* linkParameters;
	bool flagUse;
	bool flagEncrypt;
	const wchar_t* method;
	const wchar_t* description;
	const wchar_t* sortOrder;
	XURLLINK_()
		: BASE<XURLLINK_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, imageFile(0)
		, creatorName(0)
		, name(0)
		, createdAt(0)
		, creatorOID(0)
		, deActivatedAt(0)
		, linkType(0)
		, urlTarget(0)
		, browserAttributes(0)
		, linkUrl(0)
		, linkParameters(0)
		, flagUse(0)
		, flagEncrypt(0)
		, method(0)
		, description(0)
		, sortOrder(0)
	{
	}
	XURLLINK_(const XURLLINK_& src)
		: BASE<XURLLINK_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, imageFile(src.imageFile)
		, creatorName(src.creatorName)
		, name(src.name)
		, createdAt(src.createdAt)
		, creatorOID(src.creatorOID)
		, deActivatedAt(src.deActivatedAt)
		, linkType(src.linkType)
		, urlTarget(src.urlTarget)
		, browserAttributes(src.browserAttributes)
		, linkUrl(src.linkUrl)
		, linkParameters(src.linkParameters)
		, flagUse(src.flagUse)
		, flagEncrypt(src.flagEncrypt)
		, method(src.method)
		, description(src.description)
		, sortOrder(src.sortOrder)
	{
	}
	XURLLINK_(const XURLLINK_* src)
		: BASE<XURLLINK_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, imageFile(src->imageFile)
		, creatorName(src->creatorName)
		, name(src->name)
		, createdAt(src->createdAt)
		, creatorOID(src->creatorOID)
		, deActivatedAt(src->deActivatedAt)
		, linkType(src->linkType)
		, urlTarget(src->urlTarget)
		, browserAttributes(src->browserAttributes)
		, linkUrl(src->linkUrl)
		, linkParameters(src->linkParameters)
		, flagUse(src->flagUse)
		, flagEncrypt(src->flagEncrypt)
		, method(src->method)
		, description(src->description)
		, sortOrder(src->sortOrder)
	{
	}
	XURLLINK_& operator=(const XURLLINK_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			imageFile = src.imageFile;
			creatorName = src.creatorName;
			name = src.name;
			createdAt = src.createdAt;
			creatorOID = src.creatorOID;
			deActivatedAt = src.deActivatedAt;
			linkType = src.linkType;
			urlTarget = src.urlTarget;
			browserAttributes = src.browserAttributes;
			linkUrl = src.linkUrl;
			linkParameters = src.linkParameters;
			flagUse = src.flagUse;
			flagEncrypt = src.flagEncrypt;
			method = src.method;
			description = src.description;
			sortOrder = src.sortOrder;
		}
		return *this;
	}
};

struct XFILEHISTORY_ : BASE<XFILEHISTORY_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* creatorName;
	const wchar_t* eventMessage;
	const wchar_t* fileOID;
	const wchar_t* genericFileOID;
	const wchar_t* versionOID;
	const wchar_t* creatorOID;
	Long createdAt;
	const wchar_t* description;
	const wchar_t* eventType;
	XFILEHISTORY_()
		: BASE<XFILEHISTORY_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, creatorName(0)
		, eventMessage(0)
		, fileOID(0)
		, genericFileOID(0)
		, versionOID(0)
		, creatorOID(0)
		, createdAt(0)
		, description(0)
		, eventType(0)
	{
	}
	XFILEHISTORY_(const XFILEHISTORY_& src)
		: BASE<XFILEHISTORY_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, creatorName(src.creatorName)
		, eventMessage(src.eventMessage)
		, fileOID(src.fileOID)
		, genericFileOID(src.genericFileOID)
		, versionOID(src.versionOID)
		, creatorOID(src.creatorOID)
		, createdAt(src.createdAt)
		, description(src.description)
		, eventType(src.eventType)
	{
	}
	XFILEHISTORY_(const XFILEHISTORY_* src)
		: BASE<XFILEHISTORY_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, creatorName(src->creatorName)
		, eventMessage(src->eventMessage)
		, fileOID(src->fileOID)
		, genericFileOID(src->genericFileOID)
		, versionOID(src->versionOID)
		, creatorOID(src->creatorOID)
		, createdAt(src->createdAt)
		, description(src->description)
		, eventType(src->eventType)
	{
	}
	XFILEHISTORY_& operator=(const XFILEHISTORY_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			creatorName = src.creatorName;
			eventMessage = src.eventMessage;
			fileOID = src.fileOID;
			genericFileOID = src.genericFileOID;
			versionOID = src.versionOID;
			creatorOID = src.creatorOID;
			createdAt = src.createdAt;
			description = src.description;
			eventType = src.eventType;
		}
		return *this;
	}
};

struct XEXTATTRIBUTE_ : BASE<XEXTATTRIBUTE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	XEXTCOLUMN_** extColumns;
	int extColumnsCNT;
	int extColumnCount;
	const wchar_t* creatorName;
	const wchar_t* name;
	const wchar_t* baseTableName;
	const wchar_t* extTableName;
	const wchar_t* creatorOID;
	Long createdAt;
	const wchar_t* description;
	bool flagNotUsed;
	bool flagSysDefined;
	XEXTATTRIBUTE_()
		: BASE<XEXTATTRIBUTE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, extColumns(0)
		, extColumnsCNT(0)
		, extColumnCount(0)
		, creatorName(0)
		, name(0)
		, baseTableName(0)
		, extTableName(0)
		, creatorOID(0)
		, createdAt(0)
		, description(0)
		, flagNotUsed(0)
		, flagSysDefined(0)
	{
	}
	XEXTATTRIBUTE_(const XEXTATTRIBUTE_& src)
		: BASE<XEXTATTRIBUTE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, extColumns(src.extColumns)
		, extColumnsCNT(src.extColumnsCNT)
		, extColumnCount(src.extColumnCount)
		, creatorName(src.creatorName)
		, name(src.name)
		, baseTableName(src.baseTableName)
		, extTableName(src.extTableName)
		, creatorOID(src.creatorOID)
		, createdAt(src.createdAt)
		, description(src.description)
		, flagNotUsed(src.flagNotUsed)
		, flagSysDefined(src.flagSysDefined)
	{
	}
	XEXTATTRIBUTE_(const XEXTATTRIBUTE_* src)
		: BASE<XEXTATTRIBUTE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, extColumns(src->extColumns)
		, extColumnsCNT(src->extColumnsCNT)
		, extColumnCount(src->extColumnCount)
		, creatorName(src->creatorName)
		, name(src->name)
		, baseTableName(src->baseTableName)
		, extTableName(src->extTableName)
		, creatorOID(src->creatorOID)
		, createdAt(src->createdAt)
		, description(src->description)
		, flagNotUsed(src->flagNotUsed)
		, flagSysDefined(src->flagSysDefined)
	{
	}
	XEXTATTRIBUTE_& operator=(const XEXTATTRIBUTE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			extColumns = src.extColumns;
			extColumnsCNT = src.extColumnsCNT;
			extColumnCount = src.extColumnCount;
			creatorName = src.creatorName;
			name = src.name;
			baseTableName = src.baseTableName;
			extTableName = src.extTableName;
			creatorOID = src.creatorOID;
			createdAt = src.createdAt;
			description = src.description;
			flagNotUsed = src.flagNotUsed;
			flagSysDefined = src.flagSysDefined;
		}
		return *this;
	}
};

struct CLIENTZONEINFO_ : BASE<CLIENTZONEINFO_> {
	const wchar_t* clientLanguage;
	const wchar_t* clientTimeZoneID;
	CLIENTZONEINFO_()
		: BASE<CLIENTZONEINFO_>()
		, clientLanguage(0)
		, clientTimeZoneID(0)
	{
	}
	CLIENTZONEINFO_(const CLIENTZONEINFO_& src)
		: BASE<CLIENTZONEINFO_>()
		, clientLanguage(src.clientLanguage)
		, clientTimeZoneID(src.clientTimeZoneID)
	{
	}
	CLIENTZONEINFO_(const CLIENTZONEINFO_* src)
		: BASE<CLIENTZONEINFO_>()
		, clientLanguage(src->clientLanguage)
		, clientTimeZoneID(src->clientTimeZoneID)
	{
	}
	CLIENTZONEINFO_& operator=(const CLIENTZONEINFO_& src)
	{
		if (this != &src) {
			clientLanguage = src.clientLanguage;
			clientTimeZoneID = src.clientTimeZoneID;
		}
		return *this;
	}
};

struct XUSERINFO_ : BASE<XUSERINFO_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t** fullPathIndexNames;
	int fullPathIndexNamesCNT;
	const wchar_t* userOID;
	const wchar_t* groupOID;
	const wchar_t* account;
	const wchar_t* userName;
	const wchar_t* userType;
	const wchar_t* groupCode;
	const wchar_t* groupFullPathName;
	const wchar_t* groupPosition;
	const wchar_t* email;
	Long createdAt;
	bool flagDefault;
	bool flagAccountLock;
	bool flagChangePasswordNextLogin;
	const wchar_t* availableNetworkMode;
	int allowDeviceInfoCount;
	int registedDeviceInfoCount;
	bool flagPrimary;
	bool flagPrivateFolderAllow;
	XUSERINFO_()
		: BASE<XUSERINFO_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, fullPathIndexNames(0)
		, fullPathIndexNamesCNT(0)
		, userOID(0)
		, groupOID(0)
		, account(0)
		, userName(0)
		, userType(0)
		, groupCode(0)
		, groupFullPathName(0)
		, groupPosition(0)
		, email(0)
		, createdAt(0)
		, flagDefault(0)
		, flagAccountLock(0)
		, flagChangePasswordNextLogin(0)
		, availableNetworkMode(0)
		, allowDeviceInfoCount(0)
		, registedDeviceInfoCount(0)
		, flagPrimary(0)
		, flagPrivateFolderAllow(0)
	{
	}
	XUSERINFO_(const XUSERINFO_& src)
		: BASE<XUSERINFO_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, fullPathIndexNames(src.fullPathIndexNames)
		, fullPathIndexNamesCNT(src.fullPathIndexNamesCNT)
		, userOID(src.userOID)
		, groupOID(src.groupOID)
		, account(src.account)
		, userName(src.userName)
		, userType(src.userType)
		, groupCode(src.groupCode)
		, groupFullPathName(src.groupFullPathName)
		, groupPosition(src.groupPosition)
		, email(src.email)
		, createdAt(src.createdAt)
		, flagDefault(src.flagDefault)
		, flagAccountLock(src.flagAccountLock)
		, flagChangePasswordNextLogin(src.flagChangePasswordNextLogin)
		, availableNetworkMode(src.availableNetworkMode)
		, allowDeviceInfoCount(src.allowDeviceInfoCount)
		, registedDeviceInfoCount(src.registedDeviceInfoCount)
		, flagPrimary(src.flagPrimary)
		, flagPrivateFolderAllow(src.flagPrivateFolderAllow)
	{
	}
	XUSERINFO_(const XUSERINFO_* src)
		: BASE<XUSERINFO_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, fullPathIndexNames(src->fullPathIndexNames)
		, fullPathIndexNamesCNT(src->fullPathIndexNamesCNT)
		, userOID(src->userOID)
		, groupOID(src->groupOID)
		, account(src->account)
		, userName(src->userName)
		, userType(src->userType)
		, groupCode(src->groupCode)
		, groupFullPathName(src->groupFullPathName)
		, groupPosition(src->groupPosition)
		, email(src->email)
		, createdAt(src->createdAt)
		, flagDefault(src->flagDefault)
		, flagAccountLock(src->flagAccountLock)
		, flagChangePasswordNextLogin(src->flagChangePasswordNextLogin)
		, availableNetworkMode(src->availableNetworkMode)
		, allowDeviceInfoCount(src->allowDeviceInfoCount)
		, registedDeviceInfoCount(src->registedDeviceInfoCount)
		, flagPrimary(src->flagPrimary)
		, flagPrivateFolderAllow(src->flagPrivateFolderAllow)
	{
	}
	XUSERINFO_& operator=(const XUSERINFO_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			fullPathIndexNames = src.fullPathIndexNames;
			fullPathIndexNamesCNT = src.fullPathIndexNamesCNT;
			userOID = src.userOID;
			groupOID = src.groupOID;
			account = src.account;
			userName = src.userName;
			userType = src.userType;
			groupCode = src.groupCode;
			groupFullPathName = src.groupFullPathName;
			groupPosition = src.groupPosition;
			email = src.email;
			createdAt = src.createdAt;
			flagDefault = src.flagDefault;
			flagAccountLock = src.flagAccountLock;
			flagChangePasswordNextLogin = src.flagChangePasswordNextLogin;
			availableNetworkMode = src.availableNetworkMode;
			allowDeviceInfoCount = src.allowDeviceInfoCount;
			registedDeviceInfoCount = src.registedDeviceInfoCount;
			flagPrimary = src.flagPrimary;
			flagPrivateFolderAllow = src.flagPrivateFolderAllow;
		}
		return *this;
	}
};

struct FOLDERITEM_ : BASE<FOLDERITEM_> {
	const wchar_t* oid;
	const wchar_t* parentOID;
	int lastModifiedAtHigh;
	int lastModifiedAtLow;
	int permissionProperties;
	int aclProperties;
	const wchar_t* objName;
	const wchar_t* creatorInfo;
	FOLDERITEM_()
		: BASE<FOLDERITEM_>()
		, oid(0)
		, parentOID(0)
		, lastModifiedAtHigh(0)
		, lastModifiedAtLow(0)
		, permissionProperties(0)
		, aclProperties(0)
		, objName(0)
		, creatorInfo(0)
	{
	}
	FOLDERITEM_(const FOLDERITEM_& src)
		: BASE<FOLDERITEM_>()
		, oid(src.oid)
		, parentOID(src.parentOID)
		, lastModifiedAtHigh(src.lastModifiedAtHigh)
		, lastModifiedAtLow(src.lastModifiedAtLow)
		, permissionProperties(src.permissionProperties)
		, aclProperties(src.aclProperties)
		, objName(src.objName)
		, creatorInfo(src.creatorInfo)
	{
	}
	FOLDERITEM_(const FOLDERITEM_* src)
		: BASE<FOLDERITEM_>()
		, oid(src->oid)
		, parentOID(src->parentOID)
		, lastModifiedAtHigh(src->lastModifiedAtHigh)
		, lastModifiedAtLow(src->lastModifiedAtLow)
		, permissionProperties(src->permissionProperties)
		, aclProperties(src->aclProperties)
		, objName(src->objName)
		, creatorInfo(src->creatorInfo)
	{
	}
	FOLDERITEM_& operator=(const FOLDERITEM_& src)
	{
		if (this != &src) {
			oid = src.oid;
			parentOID = src.parentOID;
			lastModifiedAtHigh = src.lastModifiedAtHigh;
			lastModifiedAtLow = src.lastModifiedAtLow;
			permissionProperties = src.permissionProperties;
			aclProperties = src.aclProperties;
			objName = src.objName;
			creatorInfo = src.creatorInfo;
		}
		return *this;
	}
};

struct XMLRPCSEARCHPARAMS_ : BASE<XMLRPCSEARCHPARAMS_> {
	const wchar_t* searchTargetOID;
	const wchar_t* name;
	const wchar_t* content;
	const wchar_t* creatorOID;
	const wchar_t* createFrom;
	const wchar_t* createTo;
	const wchar_t* modifyFrom;
	const wchar_t* modifyTo;
	bool includeGroupChildren;
	const wchar_t* managerGroupOID;
	const wchar_t* tagName;
	const wchar_t* fileType;
	const wchar_t* fileSizeFrom;
	const wchar_t* fileSizeTo;
	const wchar_t* folderOID;
	bool searchByPrefix;
	bool includeFolderChildren;
	XMLRPCSEARCHPARAMS_()
		: BASE<XMLRPCSEARCHPARAMS_>()
		, searchTargetOID(0)
		, name(0)
		, content(0)
		, creatorOID(0)
		, createFrom(0)
		, createTo(0)
		, modifyFrom(0)
		, modifyTo(0)
		, includeGroupChildren(0)
		, managerGroupOID(0)
		, tagName(0)
		, fileType(0)
		, fileSizeFrom(0)
		, fileSizeTo(0)
		, folderOID(0)
		, searchByPrefix(0)
		, includeFolderChildren(0)
	{
	}
	XMLRPCSEARCHPARAMS_(const XMLRPCSEARCHPARAMS_& src)
		: BASE<XMLRPCSEARCHPARAMS_>()
		, searchTargetOID(src.searchTargetOID)
		, name(src.name)
		, content(src.content)
		, creatorOID(src.creatorOID)
		, createFrom(src.createFrom)
		, createTo(src.createTo)
		, modifyFrom(src.modifyFrom)
		, modifyTo(src.modifyTo)
		, includeGroupChildren(src.includeGroupChildren)
		, managerGroupOID(src.managerGroupOID)
		, tagName(src.tagName)
		, fileType(src.fileType)
		, fileSizeFrom(src.fileSizeFrom)
		, fileSizeTo(src.fileSizeTo)
		, folderOID(src.folderOID)
		, searchByPrefix(src.searchByPrefix)
		, includeFolderChildren(src.includeFolderChildren)
	{
	}
	XMLRPCSEARCHPARAMS_(const XMLRPCSEARCHPARAMS_* src)
		: BASE<XMLRPCSEARCHPARAMS_>()
		, searchTargetOID(src->searchTargetOID)
		, name(src->name)
		, content(src->content)
		, creatorOID(src->creatorOID)
		, createFrom(src->createFrom)
		, createTo(src->createTo)
		, modifyFrom(src->modifyFrom)
		, modifyTo(src->modifyTo)
		, includeGroupChildren(src->includeGroupChildren)
		, managerGroupOID(src->managerGroupOID)
		, tagName(src->tagName)
		, fileType(src->fileType)
		, fileSizeFrom(src->fileSizeFrom)
		, fileSizeTo(src->fileSizeTo)
		, folderOID(src->folderOID)
		, searchByPrefix(src->searchByPrefix)
		, includeFolderChildren(src->includeFolderChildren)
	{
	}
	XMLRPCSEARCHPARAMS_& operator=(const XMLRPCSEARCHPARAMS_& src)
	{
		if (this != &src) {
			searchTargetOID = src.searchTargetOID;
			name = src.name;
			content = src.content;
			creatorOID = src.creatorOID;
			createFrom = src.createFrom;
			createTo = src.createTo;
			modifyFrom = src.modifyFrom;
			modifyTo = src.modifyTo;
			includeGroupChildren = src.includeGroupChildren;
			managerGroupOID = src.managerGroupOID;
			tagName = src.tagName;
			fileType = src.fileType;
			fileSizeFrom = src.fileSizeFrom;
			fileSizeTo = src.fileSizeTo;
			folderOID = src.folderOID;
			searchByPrefix = src.searchByPrefix;
			includeFolderChildren = src.includeFolderChildren;
		}
		return *this;
	}
};

struct ASYNCCOPYFOLDERSTATUS_ : BASE<ASYNCCOPYFOLDERSTATUS_> {
	const wchar_t* asyncOperationOID;
	int status;
	int progressRatio;
	const wchar_t* errorCode;
	const wchar_t* errorMsg;
	int totalFolderCount;
	int totalFileCount;
	int copiedFolderCount;
	int copiedFileCount;
	const wchar_t* srcFolderPath;
	const wchar_t* targetFolderPath;
	const wchar_t* copiedFolderPath;
	ASYNCCOPYFOLDERSTATUS_()
		: BASE<ASYNCCOPYFOLDERSTATUS_>()
		, asyncOperationOID(0)
		, status(0)
		, progressRatio(0)
		, errorCode(0)
		, errorMsg(0)
		, totalFolderCount(0)
		, totalFileCount(0)
		, copiedFolderCount(0)
		, copiedFileCount(0)
		, srcFolderPath(0)
		, targetFolderPath(0)
		, copiedFolderPath(0)
	{
	}
	ASYNCCOPYFOLDERSTATUS_(const ASYNCCOPYFOLDERSTATUS_& src)
		: BASE<ASYNCCOPYFOLDERSTATUS_>()
		, asyncOperationOID(src.asyncOperationOID)
		, status(src.status)
		, progressRatio(src.progressRatio)
		, errorCode(src.errorCode)
		, errorMsg(src.errorMsg)
		, totalFolderCount(src.totalFolderCount)
		, totalFileCount(src.totalFileCount)
		, copiedFolderCount(src.copiedFolderCount)
		, copiedFileCount(src.copiedFileCount)
		, srcFolderPath(src.srcFolderPath)
		, targetFolderPath(src.targetFolderPath)
		, copiedFolderPath(src.copiedFolderPath)
	{
	}
	ASYNCCOPYFOLDERSTATUS_(const ASYNCCOPYFOLDERSTATUS_* src)
		: BASE<ASYNCCOPYFOLDERSTATUS_>()
		, asyncOperationOID(src->asyncOperationOID)
		, status(src->status)
		, progressRatio(src->progressRatio)
		, errorCode(src->errorCode)
		, errorMsg(src->errorMsg)
		, totalFolderCount(src->totalFolderCount)
		, totalFileCount(src->totalFileCount)
		, copiedFolderCount(src->copiedFolderCount)
		, copiedFileCount(src->copiedFileCount)
		, srcFolderPath(src->srcFolderPath)
		, targetFolderPath(src->targetFolderPath)
		, copiedFolderPath(src->copiedFolderPath)
	{
	}
	ASYNCCOPYFOLDERSTATUS_& operator=(const ASYNCCOPYFOLDERSTATUS_& src)
	{
		if (this != &src) {
			asyncOperationOID = src.asyncOperationOID;
			status = src.status;
			progressRatio = src.progressRatio;
			errorCode = src.errorCode;
			errorMsg = src.errorMsg;
			totalFolderCount = src.totalFolderCount;
			totalFileCount = src.totalFileCount;
			copiedFolderCount = src.copiedFolderCount;
			copiedFileCount = src.copiedFileCount;
			srcFolderPath = src.srcFolderPath;
			targetFolderPath = src.targetFolderPath;
			copiedFolderPath = src.copiedFolderPath;
		}
		return *this;
	}
};

struct XCODELIST_ : BASE<XCODELIST_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	XCODEVALUE_** codeValueList;
	int codeValueListCNT;
	const wchar_t* name;
	XCODELIST_()
		: BASE<XCODELIST_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, codeValueList(0)
		, codeValueListCNT(0)
		, name(0)
	{
	}
	XCODELIST_(const XCODELIST_& src)
		: BASE<XCODELIST_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, codeValueList(src.codeValueList)
		, codeValueListCNT(src.codeValueListCNT)
		, name(src.name)
	{
	}
	XCODELIST_(const XCODELIST_* src)
		: BASE<XCODELIST_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, codeValueList(src->codeValueList)
		, codeValueListCNT(src->codeValueListCNT)
		, name(src->name)
	{
	}
	XCODELIST_& operator=(const XCODELIST_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			codeValueList = src.codeValueList;
			codeValueListCNT = src.codeValueListCNT;
			name = src.name;
		}
		return *this;
	}
};

struct XCONVERTFILE_ : BASE<XCONVERTFILE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* targetOID;
	const wchar_t* targetObjectType;
	const wchar_t* fileOID;
	const wchar_t* storageFileID;
	const wchar_t* convertFileType;
	const wchar_t* convertStatus;
	Long convertedAt;
	const wchar_t* errorMessage;
	const wchar_t* fileName;
	XCONVERTFILE_()
		: BASE<XCONVERTFILE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, targetOID(0)
		, targetObjectType(0)
		, fileOID(0)
		, storageFileID(0)
		, convertFileType(0)
		, convertStatus(0)
		, convertedAt(0)
		, errorMessage(0)
		, fileName(0)
	{
	}
	XCONVERTFILE_(const XCONVERTFILE_& src)
		: BASE<XCONVERTFILE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, targetOID(src.targetOID)
		, targetObjectType(src.targetObjectType)
		, fileOID(src.fileOID)
		, storageFileID(src.storageFileID)
		, convertFileType(src.convertFileType)
		, convertStatus(src.convertStatus)
		, convertedAt(src.convertedAt)
		, errorMessage(src.errorMessage)
		, fileName(src.fileName)
	{
	}
	XCONVERTFILE_(const XCONVERTFILE_* src)
		: BASE<XCONVERTFILE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, targetOID(src->targetOID)
		, targetObjectType(src->targetObjectType)
		, fileOID(src->fileOID)
		, storageFileID(src->storageFileID)
		, convertFileType(src->convertFileType)
		, convertStatus(src->convertStatus)
		, convertedAt(src->convertedAt)
		, errorMessage(src->errorMessage)
		, fileName(src->fileName)
	{
	}
	XCONVERTFILE_& operator=(const XCONVERTFILE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			targetOID = src.targetOID;
			targetObjectType = src.targetObjectType;
			fileOID = src.fileOID;
			storageFileID = src.storageFileID;
			convertFileType = src.convertFileType;
			convertStatus = src.convertStatus;
			convertedAt = src.convertedAt;
			errorMessage = src.errorMessage;
			fileName = src.fileName;
		}
		return *this;
	}
};

struct XEXTATTRIBUTEVALUE_ : BASE<XEXTATTRIBUTEVALUE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* extTableName;
	XEXTCOLUMNVALUE_** extColumnValueList;
	int extColumnValueListCNT;
	XEXTATTRIBUTEVALUE_()
		: BASE<XEXTATTRIBUTEVALUE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, extTableName(0)
		, extColumnValueList(0)
		, extColumnValueListCNT(0)
	{
	}
	XEXTATTRIBUTEVALUE_(const XEXTATTRIBUTEVALUE_& src)
		: BASE<XEXTATTRIBUTEVALUE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, extTableName(src.extTableName)
		, extColumnValueList(src.extColumnValueList)
		, extColumnValueListCNT(src.extColumnValueListCNT)
	{
	}
	XEXTATTRIBUTEVALUE_(const XEXTATTRIBUTEVALUE_* src)
		: BASE<XEXTATTRIBUTEVALUE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, extTableName(src->extTableName)
		, extColumnValueList(src->extColumnValueList)
		, extColumnValueListCNT(src->extColumnValueListCNT)
	{
	}
	XEXTATTRIBUTEVALUE_& operator=(const XEXTATTRIBUTEVALUE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			extTableName = src.extTableName;
			extColumnValueList = src.extColumnValueList;
			extColumnValueListCNT = src.extColumnValueListCNT;
		}
		return *this;
	}
};

struct XSYNCFOLDER_ : BASE<XSYNCFOLDER_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* parentOID;
	const wchar_t* fullPathIndex;
	const wchar_t* folderFullPath;
	const wchar_t* creatorOID;
	const wchar_t* name;
	int lastModifiedAtLow;
	int lastModifiedAtHigh;
	const wchar_t* creatorName;
	const wchar_t* folderStatus;
	const wchar_t* permissions;
	XSYNCFOLDER_()
		: BASE<XSYNCFOLDER_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, parentOID(0)
		, fullPathIndex(0)
		, folderFullPath(0)
		, creatorOID(0)
		, name(0)
		, lastModifiedAtLow(0)
		, lastModifiedAtHigh(0)
		, creatorName(0)
		, folderStatus(0)
		, permissions(0)
	{
	}
	XSYNCFOLDER_(const XSYNCFOLDER_& src)
		: BASE<XSYNCFOLDER_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, parentOID(src.parentOID)
		, fullPathIndex(src.fullPathIndex)
		, folderFullPath(src.folderFullPath)
		, creatorOID(src.creatorOID)
		, name(src.name)
		, lastModifiedAtLow(src.lastModifiedAtLow)
		, lastModifiedAtHigh(src.lastModifiedAtHigh)
		, creatorName(src.creatorName)
		, folderStatus(src.folderStatus)
		, permissions(src.permissions)
	{
	}
	XSYNCFOLDER_(const XSYNCFOLDER_* src)
		: BASE<XSYNCFOLDER_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, parentOID(src->parentOID)
		, fullPathIndex(src->fullPathIndex)
		, folderFullPath(src->folderFullPath)
		, creatorOID(src->creatorOID)
		, name(src->name)
		, lastModifiedAtLow(src->lastModifiedAtLow)
		, lastModifiedAtHigh(src->lastModifiedAtHigh)
		, creatorName(src->creatorName)
		, folderStatus(src->folderStatus)
		, permissions(src->permissions)
	{
	}
	XSYNCFOLDER_& operator=(const XSYNCFOLDER_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			parentOID = src.parentOID;
			fullPathIndex = src.fullPathIndex;
			folderFullPath = src.folderFullPath;
			creatorOID = src.creatorOID;
			name = src.name;
			lastModifiedAtLow = src.lastModifiedAtLow;
			lastModifiedAtHigh = src.lastModifiedAtHigh;
			creatorName = src.creatorName;
			folderStatus = src.folderStatus;
			permissions = src.permissions;
		}
		return *this;
	}
};

struct XCONFIGURATION_ : BASE<XCONFIGURATION_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* targetOID;
	const wchar_t* targetObjectType;
	const wchar_t* configGroup;
	const wchar_t* configKey;
	const wchar_t* configValue;
	const wchar_t* description;
	XCONFIGURATION_()
		: BASE<XCONFIGURATION_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, targetOID(0)
		, targetObjectType(0)
		, configGroup(0)
		, configKey(0)
		, configValue(0)
		, description(0)
	{
	}
	XCONFIGURATION_(const XCONFIGURATION_& src)
		: BASE<XCONFIGURATION_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, targetOID(src.targetOID)
		, targetObjectType(src.targetObjectType)
		, configGroup(src.configGroup)
		, configKey(src.configKey)
		, configValue(src.configValue)
		, description(src.description)
	{
	}
	XCONFIGURATION_(const XCONFIGURATION_* src)
		: BASE<XCONFIGURATION_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, targetOID(src->targetOID)
		, targetObjectType(src->targetObjectType)
		, configGroup(src->configGroup)
		, configKey(src->configKey)
		, configValue(src->configValue)
		, description(src->description)
	{
	}
	XCONFIGURATION_& operator=(const XCONFIGURATION_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			targetOID = src.targetOID;
			targetObjectType = src.targetObjectType;
			configGroup = src.configGroup;
			configKey = src.configKey;
			configValue = src.configValue;
			description = src.description;
		}
		return *this;
	}
};

struct XTAG_ : BASE<XTAG_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	int weight;
	int registerCount;
	const wchar_t* targetOID;
	const wchar_t* targetObjectType;
	const wchar_t* tagName;
	Long createdAt;
	XTAG_()
		: BASE<XTAG_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, weight(0)
		, registerCount(0)
		, targetOID(0)
		, targetObjectType(0)
		, tagName(0)
		, createdAt(0)
	{
	}
	XTAG_(const XTAG_& src)
		: BASE<XTAG_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, weight(src.weight)
		, registerCount(src.registerCount)
		, targetOID(src.targetOID)
		, targetObjectType(src.targetObjectType)
		, tagName(src.tagName)
		, createdAt(src.createdAt)
	{
	}
	XTAG_(const XTAG_* src)
		: BASE<XTAG_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, weight(src->weight)
		, registerCount(src->registerCount)
		, targetOID(src->targetOID)
		, targetObjectType(src->targetObjectType)
		, tagName(src->tagName)
		, createdAt(src->createdAt)
	{
	}
	XTAG_& operator=(const XTAG_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			weight = src.weight;
			registerCount = src.registerCount;
			targetOID = src.targetOID;
			targetObjectType = src.targetObjectType;
			tagName = src.tagName;
			createdAt = src.createdAt;
		}
		return *this;
	}
};

struct XCODEVALUE_ : BASE<XCODEVALUE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* codeListOID;
	const wchar_t* name;
	const wchar_t* codeValue;
	const wchar_t* subCodeListOID;
	const wchar_t* sortOrder;
	XCODEVALUE_()
		: BASE<XCODEVALUE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, codeListOID(0)
		, name(0)
		, codeValue(0)
		, subCodeListOID(0)
		, sortOrder(0)
	{
	}
	XCODEVALUE_(const XCODEVALUE_& src)
		: BASE<XCODEVALUE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, codeListOID(src.codeListOID)
		, name(src.name)
		, codeValue(src.codeValue)
		, subCodeListOID(src.subCodeListOID)
		, sortOrder(src.sortOrder)
	{
	}
	XCODEVALUE_(const XCODEVALUE_* src)
		: BASE<XCODEVALUE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, codeListOID(src->codeListOID)
		, name(src->name)
		, codeValue(src->codeValue)
		, subCodeListOID(src->subCodeListOID)
		, sortOrder(src->sortOrder)
	{
	}
	XCODEVALUE_& operator=(const XCODEVALUE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			codeListOID = src.codeListOID;
			name = src.name;
			codeValue = src.codeValue;
			subCodeListOID = src.subCodeListOID;
			sortOrder = src.sortOrder;
		}
		return *this;
	}
};

struct XAPPLICATIONPOLICY_ : BASE<XAPPLICATIONPOLICY_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t** excludeFilePathes;
	int excludeFilePathesCNT;
	const wchar_t* applicationName;
	const wchar_t* manageType;
	Long createdAt;
	bool flagProcessName;
	const wchar_t* processName;
	bool flagPropertyInfo;
	const wchar_t* propertyInfo;
	bool flagCheckSum;
	const wchar_t* checkSum;
	const wchar_t* allowFileExt;
	const wchar_t* excludeFilePath;
	bool flagUse;
	Long lastModifiedAt;
	bool flagChildProcess;
	bool flagViewDrive;
	const wchar_t* sortOrder;
	int portAllowType;
	const wchar_t* allowPort;
	bool flagCopyLocalDDPolicyToSecuDD;
	const wchar_t* portBlackList;
	bool flagOpenFileReadOnly;
	bool flagBlockSecureDD;
	bool flagSystemRegist;
	const wchar_t* fileAttachType;
	bool existedPortUseRequest;
	XAPPLICATIONPOLICY_()
		: BASE<XAPPLICATIONPOLICY_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, excludeFilePathes(0)
		, excludeFilePathesCNT(0)
		, applicationName(0)
		, manageType(0)
		, createdAt(0)
		, flagProcessName(0)
		, processName(0)
		, flagPropertyInfo(0)
		, propertyInfo(0)
		, flagCheckSum(0)
		, checkSum(0)
		, allowFileExt(0)
		, excludeFilePath(0)
		, flagUse(0)
		, lastModifiedAt(0)
		, flagChildProcess(0)
		, flagViewDrive(0)
		, sortOrder(0)
		, portAllowType(0)
		, allowPort(0)
		, flagCopyLocalDDPolicyToSecuDD(0)
		, portBlackList(0)
		, flagOpenFileReadOnly(0)
		, flagBlockSecureDD(0)
		, flagSystemRegist(0)
		, fileAttachType(0)
		, existedPortUseRequest(0)
	{
	}
	XAPPLICATIONPOLICY_(const XAPPLICATIONPOLICY_& src)
		: BASE<XAPPLICATIONPOLICY_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, excludeFilePathes(src.excludeFilePathes)
		, excludeFilePathesCNT(src.excludeFilePathesCNT)
		, applicationName(src.applicationName)
		, manageType(src.manageType)
		, createdAt(src.createdAt)
		, flagProcessName(src.flagProcessName)
		, processName(src.processName)
		, flagPropertyInfo(src.flagPropertyInfo)
		, propertyInfo(src.propertyInfo)
		, flagCheckSum(src.flagCheckSum)
		, checkSum(src.checkSum)
		, allowFileExt(src.allowFileExt)
		, excludeFilePath(src.excludeFilePath)
		, flagUse(src.flagUse)
		, lastModifiedAt(src.lastModifiedAt)
		, flagChildProcess(src.flagChildProcess)
		, flagViewDrive(src.flagViewDrive)
		, sortOrder(src.sortOrder)
		, portAllowType(src.portAllowType)
		, allowPort(src.allowPort)
		, flagCopyLocalDDPolicyToSecuDD(src.flagCopyLocalDDPolicyToSecuDD)
		, portBlackList(src.portBlackList)
		, flagOpenFileReadOnly(src.flagOpenFileReadOnly)
		, flagBlockSecureDD(src.flagBlockSecureDD)
		, flagSystemRegist(src.flagSystemRegist)
		, fileAttachType(src.fileAttachType)
		, existedPortUseRequest(src.existedPortUseRequest)
	{
	}
	XAPPLICATIONPOLICY_(const XAPPLICATIONPOLICY_* src)
		: BASE<XAPPLICATIONPOLICY_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, excludeFilePathes(src->excludeFilePathes)
		, excludeFilePathesCNT(src->excludeFilePathesCNT)
		, applicationName(src->applicationName)
		, manageType(src->manageType)
		, createdAt(src->createdAt)
		, flagProcessName(src->flagProcessName)
		, processName(src->processName)
		, flagPropertyInfo(src->flagPropertyInfo)
		, propertyInfo(src->propertyInfo)
		, flagCheckSum(src->flagCheckSum)
		, checkSum(src->checkSum)
		, allowFileExt(src->allowFileExt)
		, excludeFilePath(src->excludeFilePath)
		, flagUse(src->flagUse)
		, lastModifiedAt(src->lastModifiedAt)
		, flagChildProcess(src->flagChildProcess)
		, flagViewDrive(src->flagViewDrive)
		, sortOrder(src->sortOrder)
		, portAllowType(src->portAllowType)
		, allowPort(src->allowPort)
		, flagCopyLocalDDPolicyToSecuDD(src->flagCopyLocalDDPolicyToSecuDD)
		, portBlackList(src->portBlackList)
		, flagOpenFileReadOnly(src->flagOpenFileReadOnly)
		, flagBlockSecureDD(src->flagBlockSecureDD)
		, flagSystemRegist(src->flagSystemRegist)
		, fileAttachType(src->fileAttachType)
		, existedPortUseRequest(src->existedPortUseRequest)
	{
	}
	XAPPLICATIONPOLICY_& operator=(const XAPPLICATIONPOLICY_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			excludeFilePathes = src.excludeFilePathes;
			excludeFilePathesCNT = src.excludeFilePathesCNT;
			applicationName = src.applicationName;
			manageType = src.manageType;
			createdAt = src.createdAt;
			flagProcessName = src.flagProcessName;
			processName = src.processName;
			flagPropertyInfo = src.flagPropertyInfo;
			propertyInfo = src.propertyInfo;
			flagCheckSum = src.flagCheckSum;
			checkSum = src.checkSum;
			allowFileExt = src.allowFileExt;
			excludeFilePath = src.excludeFilePath;
			flagUse = src.flagUse;
			lastModifiedAt = src.lastModifiedAt;
			flagChildProcess = src.flagChildProcess;
			flagViewDrive = src.flagViewDrive;
			sortOrder = src.sortOrder;
			portAllowType = src.portAllowType;
			allowPort = src.allowPort;
			flagCopyLocalDDPolicyToSecuDD = src.flagCopyLocalDDPolicyToSecuDD;
			portBlackList = src.portBlackList;
			flagOpenFileReadOnly = src.flagOpenFileReadOnly;
			flagBlockSecureDD = src.flagBlockSecureDD;
			flagSystemRegist = src.flagSystemRegist;
			fileAttachType = src.fileAttachType;
			existedPortUseRequest = src.existedPortUseRequest;
		}
		return *this;
	}
};

struct ECMOBJECT_ : BASE<ECMOBJECT_> {
	const wchar_t* objName;
	const wchar_t* objPath;
	ECMOBJECT_()
		: BASE<ECMOBJECT_>()
		, objName(0)
		, objPath(0)
	{
	}
	ECMOBJECT_(const ECMOBJECT_& src)
		: BASE<ECMOBJECT_>()
		, objName(src.objName)
		, objPath(src.objPath)
	{
	}
	ECMOBJECT_(const ECMOBJECT_* src)
		: BASE<ECMOBJECT_>()
		, objName(src->objName)
		, objPath(src->objPath)
	{
	}
	ECMOBJECT_& operator=(const ECMOBJECT_& src)
	{
		if (this != &src) {
			objName = src.objName;
			objPath = src.objPath;
		}
		return *this;
	}
};

struct XAPPLICATIONEXCLUSIVENWPOLICY_ : BASE<XAPPLICATIONEXCLUSIVENWPOLICY_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* applicationName;
	Long createdAt;
	bool flagProcessName;
	const wchar_t* processName;
	bool flagPropertyInfo;
	const wchar_t* propertyInfo;
	bool flagUse;
	Long lastModifiedAt;
	const wchar_t* sortOrder;
	const wchar_t* controlType;
	XAPPLICATIONEXCLUSIVENWPOLICY_()
		: BASE<XAPPLICATIONEXCLUSIVENWPOLICY_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, applicationName(0)
		, createdAt(0)
		, flagProcessName(0)
		, processName(0)
		, flagPropertyInfo(0)
		, propertyInfo(0)
		, flagUse(0)
		, lastModifiedAt(0)
		, sortOrder(0)
		, controlType(0)
	{
	}
	XAPPLICATIONEXCLUSIVENWPOLICY_(const XAPPLICATIONEXCLUSIVENWPOLICY_& src)
		: BASE<XAPPLICATIONEXCLUSIVENWPOLICY_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, applicationName(src.applicationName)
		, createdAt(src.createdAt)
		, flagProcessName(src.flagProcessName)
		, processName(src.processName)
		, flagPropertyInfo(src.flagPropertyInfo)
		, propertyInfo(src.propertyInfo)
		, flagUse(src.flagUse)
		, lastModifiedAt(src.lastModifiedAt)
		, sortOrder(src.sortOrder)
		, controlType(src.controlType)
	{
	}
	XAPPLICATIONEXCLUSIVENWPOLICY_(const XAPPLICATIONEXCLUSIVENWPOLICY_* src)
		: BASE<XAPPLICATIONEXCLUSIVENWPOLICY_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, applicationName(src->applicationName)
		, createdAt(src->createdAt)
		, flagProcessName(src->flagProcessName)
		, processName(src->processName)
		, flagPropertyInfo(src->flagPropertyInfo)
		, propertyInfo(src->propertyInfo)
		, flagUse(src->flagUse)
		, lastModifiedAt(src->lastModifiedAt)
		, sortOrder(src->sortOrder)
		, controlType(src->controlType)
	{
	}
	XAPPLICATIONEXCLUSIVENWPOLICY_& operator=(const XAPPLICATIONEXCLUSIVENWPOLICY_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			applicationName = src.applicationName;
			createdAt = src.createdAt;
			flagProcessName = src.flagProcessName;
			processName = src.processName;
			flagPropertyInfo = src.flagPropertyInfo;
			propertyInfo = src.propertyInfo;
			flagUse = src.flagUse;
			lastModifiedAt = src.lastModifiedAt;
			sortOrder = src.sortOrder;
			controlType = src.controlType;
		}
		return *this;
	}
};

struct XMLRPCLOGINCONTEXT_ : BASE<XMLRPCLOGINCONTEXT_> {
	const wchar_t* sessionKey;
	int loginStatus;
	const wchar_t* errorMsg;
	const wchar_t* multiUserSuggest;
	const wchar_t* loginUserInfo;
	int maxLoginCount;
	int failLoginCount;
	const wchar_t* documentSaveType;
	const wchar_t* useAutoCADHooking;
	int messageTimingDuration;
	const wchar_t* lastLoginIP;
	const wchar_t* lastLoginDate;
	const wchar_t* language;
	bool manager;
	XMLRPCLOGINCONTEXT_()
		: BASE<XMLRPCLOGINCONTEXT_>()
		, sessionKey(0)
		, loginStatus(0)
		, errorMsg(0)
		, multiUserSuggest(0)
		, loginUserInfo(0)
		, maxLoginCount(0)
		, failLoginCount(0)
		, documentSaveType(0)
		, useAutoCADHooking(0)
		, messageTimingDuration(0)
		, lastLoginIP(0)
		, lastLoginDate(0)
		, language(0)
		, manager(0)
	{
	}
	XMLRPCLOGINCONTEXT_(const XMLRPCLOGINCONTEXT_& src)
		: BASE<XMLRPCLOGINCONTEXT_>()
		, sessionKey(src.sessionKey)
		, loginStatus(src.loginStatus)
		, errorMsg(src.errorMsg)
		, multiUserSuggest(src.multiUserSuggest)
		, loginUserInfo(src.loginUserInfo)
		, maxLoginCount(src.maxLoginCount)
		, failLoginCount(src.failLoginCount)
		, documentSaveType(src.documentSaveType)
		, useAutoCADHooking(src.useAutoCADHooking)
		, messageTimingDuration(src.messageTimingDuration)
		, lastLoginIP(src.lastLoginIP)
		, lastLoginDate(src.lastLoginDate)
		, language(src.language)
		, manager(src.manager)
	{
	}
	XMLRPCLOGINCONTEXT_(const XMLRPCLOGINCONTEXT_* src)
		: BASE<XMLRPCLOGINCONTEXT_>()
		, sessionKey(src->sessionKey)
		, loginStatus(src->loginStatus)
		, errorMsg(src->errorMsg)
		, multiUserSuggest(src->multiUserSuggest)
		, loginUserInfo(src->loginUserInfo)
		, maxLoginCount(src->maxLoginCount)
		, failLoginCount(src->failLoginCount)
		, documentSaveType(src->documentSaveType)
		, useAutoCADHooking(src->useAutoCADHooking)
		, messageTimingDuration(src->messageTimingDuration)
		, lastLoginIP(src->lastLoginIP)
		, lastLoginDate(src->lastLoginDate)
		, language(src->language)
		, manager(src->manager)
	{
	}
	XMLRPCLOGINCONTEXT_& operator=(const XMLRPCLOGINCONTEXT_& src)
	{
		if (this != &src) {
			sessionKey = src.sessionKey;
			loginStatus = src.loginStatus;
			errorMsg = src.errorMsg;
			multiUserSuggest = src.multiUserSuggest;
			loginUserInfo = src.loginUserInfo;
			maxLoginCount = src.maxLoginCount;
			failLoginCount = src.failLoginCount;
			documentSaveType = src.documentSaveType;
			useAutoCADHooking = src.useAutoCADHooking;
			messageTimingDuration = src.messageTimingDuration;
			lastLoginIP = src.lastLoginIP;
			lastLoginDate = src.lastLoginDate;
			language = src.language;
			manager = src.manager;
		}
		return *this;
	}
};

struct XFILE_ : BASE<XFILE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	XCONVERTFILE_** convertFiles;
	int convertFilesCNT;
	const wchar_t* creatorName;
	const wchar_t* lastModifierName;
	bool myCheckOutFile;
	const wchar_t* targetVersion;
	bool updateEnable;
	const wchar_t* targetName;
	const wchar_t* viewURL;
	int viewCount;
	const wchar_t* newFileOID;
	bool flagSaveNewVersion;
	const wchar_t* thumbnailStatus;
	XFILEHISTORY_* fileHistory;
	const wchar_t* targetOID;
	const wchar_t* targetObjectType;
	const wchar_t* genericFileOID;
	const wchar_t* storageFileID;
	const wchar_t* fileName;
	double fileSize;
	const wchar_t* fileType;
	const wchar_t* description;
	int fileStatus;
	const wchar_t* creatorOID;
	const wchar_t* lastModifierOID;
	Long localLastModifiedAt;
	Long lastModifiedAt;
	const wchar_t* checkOutStatus;
	const wchar_t* sortOrder;
	bool flagDefaultVersion;
	const wchar_t* fileLevel;
	const wchar_t* columnName;
	Long localCreatedAt;
	const wchar_t* drmStatus;
	bool flagCheckedFileSize;
	const wchar_t* checkOutUserOID;
	const wchar_t* checkOutUserName;
	Long checkOutAt;
	const wchar_t* folderOID;
	const wchar_t* documentName;
	const wchar_t* folderFullPathName;
	XTAG_** tags;
	int tagsCNT;
	const wchar_t* highlightedFileName;
	const wchar_t* highlightedCreatorName;
	const wchar_t* highlightedTagList;
	const wchar_t* highlightedFolderFullPathName;
	const wchar_t* attach;
	double beforeSize;
	const wchar_t* folderType;
	const wchar_t* moveFileOID;
	XFILE_()
		: BASE<XFILE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, convertFiles(0)
		, convertFilesCNT(0)
		, creatorName(0)
		, lastModifierName(0)
		, myCheckOutFile(0)
		, targetVersion(0)
		, updateEnable(0)
		, targetName(0)
		, viewURL(0)
		, viewCount(0)
		, newFileOID(0)
		, flagSaveNewVersion(0)
		, thumbnailStatus(0)
		, fileHistory(0)
		, targetOID(0)
		, targetObjectType(0)
		, genericFileOID(0)
		, storageFileID(0)
		, fileName(0)
		, fileSize(0)
		, fileType(0)
		, description(0)
		, fileStatus(0)
		, creatorOID(0)
		, lastModifierOID(0)
		, localLastModifiedAt(0)
		, lastModifiedAt(0)
		, checkOutStatus(0)
		, sortOrder(0)
		, flagDefaultVersion(0)
		, fileLevel(0)
		, columnName(0)
		, localCreatedAt(0)
		, drmStatus(0)
		, flagCheckedFileSize(0)
		, checkOutUserOID(0)
		, checkOutUserName(0)
		, checkOutAt(0)
		, folderOID(0)
		, documentName(0)
		, folderFullPathName(0)
		, tags(0)
		, tagsCNT(0)
		, highlightedFileName(0)
		, highlightedCreatorName(0)
		, highlightedTagList(0)
		, highlightedFolderFullPathName(0)
		, attach(0)
		, beforeSize(0)
		, folderType(0)
		, moveFileOID(0)
	{
	}
	XFILE_(const XFILE_& src)
		: BASE<XFILE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, convertFiles(src.convertFiles)
		, convertFilesCNT(src.convertFilesCNT)
		, creatorName(src.creatorName)
		, lastModifierName(src.lastModifierName)
		, myCheckOutFile(src.myCheckOutFile)
		, targetVersion(src.targetVersion)
		, updateEnable(src.updateEnable)
		, targetName(src.targetName)
		, viewURL(src.viewURL)
		, viewCount(src.viewCount)
		, newFileOID(src.newFileOID)
		, flagSaveNewVersion(src.flagSaveNewVersion)
		, thumbnailStatus(src.thumbnailStatus)
		, fileHistory(src.fileHistory)
		, targetOID(src.targetOID)
		, targetObjectType(src.targetObjectType)
		, genericFileOID(src.genericFileOID)
		, storageFileID(src.storageFileID)
		, fileName(src.fileName)
		, fileSize(src.fileSize)
		, fileType(src.fileType)
		, description(src.description)
		, fileStatus(src.fileStatus)
		, creatorOID(src.creatorOID)
		, lastModifierOID(src.lastModifierOID)
		, localLastModifiedAt(src.localLastModifiedAt)
		, lastModifiedAt(src.lastModifiedAt)
		, checkOutStatus(src.checkOutStatus)
		, sortOrder(src.sortOrder)
		, flagDefaultVersion(src.flagDefaultVersion)
		, fileLevel(src.fileLevel)
		, columnName(src.columnName)
		, localCreatedAt(src.localCreatedAt)
		, drmStatus(src.drmStatus)
		, flagCheckedFileSize(src.flagCheckedFileSize)
		, checkOutUserOID(src.checkOutUserOID)
		, checkOutUserName(src.checkOutUserName)
		, checkOutAt(src.checkOutAt)
		, folderOID(src.folderOID)
		, documentName(src.documentName)
		, folderFullPathName(src.folderFullPathName)
		, tags(src.tags)
		, tagsCNT(src.tagsCNT)
		, highlightedFileName(src.highlightedFileName)
		, highlightedCreatorName(src.highlightedCreatorName)
		, highlightedTagList(src.highlightedTagList)
		, highlightedFolderFullPathName(src.highlightedFolderFullPathName)
		, attach(src.attach)
		, beforeSize(src.beforeSize)
		, folderType(src.folderType)
		, moveFileOID(src.moveFileOID)
	{
	}
	XFILE_(const XFILE_* src)
		: BASE<XFILE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, convertFiles(src->convertFiles)
		, convertFilesCNT(src->convertFilesCNT)
		, creatorName(src->creatorName)
		, lastModifierName(src->lastModifierName)
		, myCheckOutFile(src->myCheckOutFile)
		, targetVersion(src->targetVersion)
		, updateEnable(src->updateEnable)
		, targetName(src->targetName)
		, viewURL(src->viewURL)
		, viewCount(src->viewCount)
		, newFileOID(src->newFileOID)
		, flagSaveNewVersion(src->flagSaveNewVersion)
		, thumbnailStatus(src->thumbnailStatus)
		, fileHistory(src->fileHistory)
		, targetOID(src->targetOID)
		, targetObjectType(src->targetObjectType)
		, genericFileOID(src->genericFileOID)
		, storageFileID(src->storageFileID)
		, fileName(src->fileName)
		, fileSize(src->fileSize)
		, fileType(src->fileType)
		, description(src->description)
		, fileStatus(src->fileStatus)
		, creatorOID(src->creatorOID)
		, lastModifierOID(src->lastModifierOID)
		, localLastModifiedAt(src->localLastModifiedAt)
		, lastModifiedAt(src->lastModifiedAt)
		, checkOutStatus(src->checkOutStatus)
		, sortOrder(src->sortOrder)
		, flagDefaultVersion(src->flagDefaultVersion)
		, fileLevel(src->fileLevel)
		, columnName(src->columnName)
		, localCreatedAt(src->localCreatedAt)
		, drmStatus(src->drmStatus)
		, flagCheckedFileSize(src->flagCheckedFileSize)
		, checkOutUserOID(src->checkOutUserOID)
		, checkOutUserName(src->checkOutUserName)
		, checkOutAt(src->checkOutAt)
		, folderOID(src->folderOID)
		, documentName(src->documentName)
		, folderFullPathName(src->folderFullPathName)
		, tags(src->tags)
		, tagsCNT(src->tagsCNT)
		, highlightedFileName(src->highlightedFileName)
		, highlightedCreatorName(src->highlightedCreatorName)
		, highlightedTagList(src->highlightedTagList)
		, highlightedFolderFullPathName(src->highlightedFolderFullPathName)
		, attach(src->attach)
		, beforeSize(src->beforeSize)
		, folderType(src->folderType)
		, moveFileOID(src->moveFileOID)
	{
	}
	XFILE_& operator=(const XFILE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			convertFiles = src.convertFiles;
			convertFilesCNT = src.convertFilesCNT;
			creatorName = src.creatorName;
			lastModifierName = src.lastModifierName;
			myCheckOutFile = src.myCheckOutFile;
			targetVersion = src.targetVersion;
			updateEnable = src.updateEnable;
			targetName = src.targetName;
			viewURL = src.viewURL;
			viewCount = src.viewCount;
			newFileOID = src.newFileOID;
			flagSaveNewVersion = src.flagSaveNewVersion;
			thumbnailStatus = src.thumbnailStatus;
			fileHistory = src.fileHistory;
			targetOID = src.targetOID;
			targetObjectType = src.targetObjectType;
			genericFileOID = src.genericFileOID;
			storageFileID = src.storageFileID;
			fileName = src.fileName;
			fileSize = src.fileSize;
			fileType = src.fileType;
			description = src.description;
			fileStatus = src.fileStatus;
			creatorOID = src.creatorOID;
			lastModifierOID = src.lastModifierOID;
			localLastModifiedAt = src.localLastModifiedAt;
			lastModifiedAt = src.lastModifiedAt;
			checkOutStatus = src.checkOutStatus;
			sortOrder = src.sortOrder;
			flagDefaultVersion = src.flagDefaultVersion;
			fileLevel = src.fileLevel;
			columnName = src.columnName;
			localCreatedAt = src.localCreatedAt;
			drmStatus = src.drmStatus;
			flagCheckedFileSize = src.flagCheckedFileSize;
			checkOutUserOID = src.checkOutUserOID;
			checkOutUserName = src.checkOutUserName;
			checkOutAt = src.checkOutAt;
			folderOID = src.folderOID;
			documentName = src.documentName;
			folderFullPathName = src.folderFullPathName;
			tags = src.tags;
			tagsCNT = src.tagsCNT;
			highlightedFileName = src.highlightedFileName;
			highlightedCreatorName = src.highlightedCreatorName;
			highlightedTagList = src.highlightedTagList;
			highlightedFolderFullPathName = src.highlightedFolderFullPathName;
			attach = src.attach;
			beforeSize = src.beforeSize;
			folderType = src.folderType;
			moveFileOID = src.moveFileOID;
		}
		return *this;
	}
};

struct XSYNCDOCUMENTWITHEXTATTR_ : BASE<XSYNCDOCUMENTWITHEXTATTR_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* folderOID;
	const wchar_t* folderFullPath;
	const wchar_t* creatorOID;
	const wchar_t* name;
	const wchar_t* fileOID;
	const wchar_t* fileSize;
	const wchar_t* storageFileID;
	int lastModifiedAtLow;
	int lastModifiedAtHigh;
	const wchar_t* lastModifierOID;
	const wchar_t* lastModifierName;
	const wchar_t* creatorName;
	const wchar_t* docStatus;
	const wchar_t* permissions;
	Long lastModifiedAt;
	Long localCreatedAt;
	const wchar_t* docTypeOID;
	XEXTCOLUMNVALUE_** extColumnValues;
	int extColumnValuesCNT;
	XSYNCDOCUMENTWITHEXTATTR_()
		: BASE<XSYNCDOCUMENTWITHEXTATTR_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, folderOID(0)
		, folderFullPath(0)
		, creatorOID(0)
		, name(0)
		, fileOID(0)
		, fileSize(0)
		, storageFileID(0)
		, lastModifiedAtLow(0)
		, lastModifiedAtHigh(0)
		, lastModifierOID(0)
		, lastModifierName(0)
		, creatorName(0)
		, docStatus(0)
		, permissions(0)
		, lastModifiedAt(0)
		, localCreatedAt(0)
		, docTypeOID(0)
		, extColumnValues(0)
		, extColumnValuesCNT(0)
	{
	}
	XSYNCDOCUMENTWITHEXTATTR_(const XSYNCDOCUMENTWITHEXTATTR_& src)
		: BASE<XSYNCDOCUMENTWITHEXTATTR_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, folderOID(src.folderOID)
		, folderFullPath(src.folderFullPath)
		, creatorOID(src.creatorOID)
		, name(src.name)
		, fileOID(src.fileOID)
		, fileSize(src.fileSize)
		, storageFileID(src.storageFileID)
		, lastModifiedAtLow(src.lastModifiedAtLow)
		, lastModifiedAtHigh(src.lastModifiedAtHigh)
		, lastModifierOID(src.lastModifierOID)
		, lastModifierName(src.lastModifierName)
		, creatorName(src.creatorName)
		, docStatus(src.docStatus)
		, permissions(src.permissions)
		, lastModifiedAt(src.lastModifiedAt)
		, localCreatedAt(src.localCreatedAt)
		, docTypeOID(src.docTypeOID)
		, extColumnValues(src.extColumnValues)
		, extColumnValuesCNT(src.extColumnValuesCNT)
	{
	}
	XSYNCDOCUMENTWITHEXTATTR_(const XSYNCDOCUMENTWITHEXTATTR_* src)
		: BASE<XSYNCDOCUMENTWITHEXTATTR_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, folderOID(src->folderOID)
		, folderFullPath(src->folderFullPath)
		, creatorOID(src->creatorOID)
		, name(src->name)
		, fileOID(src->fileOID)
		, fileSize(src->fileSize)
		, storageFileID(src->storageFileID)
		, lastModifiedAtLow(src->lastModifiedAtLow)
		, lastModifiedAtHigh(src->lastModifiedAtHigh)
		, lastModifierOID(src->lastModifierOID)
		, lastModifierName(src->lastModifierName)
		, creatorName(src->creatorName)
		, docStatus(src->docStatus)
		, permissions(src->permissions)
		, lastModifiedAt(src->lastModifiedAt)
		, localCreatedAt(src->localCreatedAt)
		, docTypeOID(src->docTypeOID)
		, extColumnValues(src->extColumnValues)
		, extColumnValuesCNT(src->extColumnValuesCNT)
	{
	}
	XSYNCDOCUMENTWITHEXTATTR_& operator=(const XSYNCDOCUMENTWITHEXTATTR_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			folderOID = src.folderOID;
			folderFullPath = src.folderFullPath;
			creatorOID = src.creatorOID;
			name = src.name;
			fileOID = src.fileOID;
			fileSize = src.fileSize;
			storageFileID = src.storageFileID;
			lastModifiedAtLow = src.lastModifiedAtLow;
			lastModifiedAtHigh = src.lastModifiedAtHigh;
			lastModifierOID = src.lastModifierOID;
			lastModifierName = src.lastModifierName;
			creatorName = src.creatorName;
			docStatus = src.docStatus;
			permissions = src.permissions;
			lastModifiedAt = src.lastModifiedAt;
			localCreatedAt = src.localCreatedAt;
			docTypeOID = src.docTypeOID;
			extColumnValues = src.extColumnValues;
			extColumnValuesCNT = src.extColumnValuesCNT;
		}
		return *this;
	}
};

struct XSITERULE_ : BASE<XSITERULE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* policyOID;
	const wchar_t* ruleType;
	const wchar_t* sortOrder;
	const wchar_t* name;
	bool flagAll;
	const wchar_t* type;
	const wchar_t* siteDomain;
	const wchar_t* localDrivePermission;
	const wchar_t* serverDrivePermission;
	const wchar_t* securityDrivePermission;
	const wchar_t* siteFileAttachType;
	bool flagSaveLog;
	const wchar_t* checkType;
	XSITERULE_()
		: BASE<XSITERULE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, policyOID(0)
		, ruleType(0)
		, sortOrder(0)
		, name(0)
		, flagAll(0)
		, type(0)
		, siteDomain(0)
		, localDrivePermission(0)
		, serverDrivePermission(0)
		, securityDrivePermission(0)
		, siteFileAttachType(0)
		, flagSaveLog(0)
		, checkType(0)
	{
	}
	XSITERULE_(const XSITERULE_& src)
		: BASE<XSITERULE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, policyOID(src.policyOID)
		, ruleType(src.ruleType)
		, sortOrder(src.sortOrder)
		, name(src.name)
		, flagAll(src.flagAll)
		, type(src.type)
		, siteDomain(src.siteDomain)
		, localDrivePermission(src.localDrivePermission)
		, serverDrivePermission(src.serverDrivePermission)
		, securityDrivePermission(src.securityDrivePermission)
		, siteFileAttachType(src.siteFileAttachType)
		, flagSaveLog(src.flagSaveLog)
		, checkType(src.checkType)
	{
	}
	XSITERULE_(const XSITERULE_* src)
		: BASE<XSITERULE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, policyOID(src->policyOID)
		, ruleType(src->ruleType)
		, sortOrder(src->sortOrder)
		, name(src->name)
		, flagAll(src->flagAll)
		, type(src->type)
		, siteDomain(src->siteDomain)
		, localDrivePermission(src->localDrivePermission)
		, serverDrivePermission(src->serverDrivePermission)
		, securityDrivePermission(src->securityDrivePermission)
		, siteFileAttachType(src->siteFileAttachType)
		, flagSaveLog(src->flagSaveLog)
		, checkType(src->checkType)
	{
	}
	XSITERULE_& operator=(const XSITERULE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			policyOID = src.policyOID;
			ruleType = src.ruleType;
			sortOrder = src.sortOrder;
			name = src.name;
			flagAll = src.flagAll;
			type = src.type;
			siteDomain = src.siteDomain;
			localDrivePermission = src.localDrivePermission;
			serverDrivePermission = src.serverDrivePermission;
			securityDrivePermission = src.securityDrivePermission;
			siteFileAttachType = src.siteFileAttachType;
			flagSaveLog = src.flagSaveLog;
			checkType = src.checkType;
		}
		return *this;
	}
};

struct XMLRPCDOCUMENT_ : BASE<XMLRPCDOCUMENT_> {
	const wchar_t* folderOID;
	const wchar_t* localFolderPath;
	const wchar_t* creatorOID;
	const wchar_t* name;
	const wchar_t** categoryFolderOIDs;
	int categoryFolderOIDsCNT;
	const wchar_t* docTypeOID;
	const wchar_t* tags;
	const wchar_t** storagefileNames;
	int storagefileNamesCNT;
	const wchar_t** storagefileSizes;
	int storagefileSizesCNT;
	const wchar_t** storagefileOIDs;
	int storagefileOIDsCNT;
	const wchar_t** localCreatedAts;
	int localCreatedAtsCNT;
	const wchar_t** localLastModifiedAts;
	int localLastModifiedAtsCNT;
	int maintainDuration;
	const wchar_t* description;
	XEXTCOLUMNVALUE_** extValues;
	int extValuesCNT;
	XMLRPCDOCUMENT_()
		: BASE<XMLRPCDOCUMENT_>()
		, folderOID(0)
		, localFolderPath(0)
		, creatorOID(0)
		, name(0)
		, categoryFolderOIDs(0)
		, categoryFolderOIDsCNT(0)
		, docTypeOID(0)
		, tags(0)
		, storagefileNames(0)
		, storagefileNamesCNT(0)
		, storagefileSizes(0)
		, storagefileSizesCNT(0)
		, storagefileOIDs(0)
		, storagefileOIDsCNT(0)
		, localCreatedAts(0)
		, localCreatedAtsCNT(0)
		, localLastModifiedAts(0)
		, localLastModifiedAtsCNT(0)
		, maintainDuration(0)
		, description(0)
		, extValues(0)
		, extValuesCNT(0)
	{
	}
	XMLRPCDOCUMENT_(const XMLRPCDOCUMENT_& src)
		: BASE<XMLRPCDOCUMENT_>()
		, folderOID(src.folderOID)
		, localFolderPath(src.localFolderPath)
		, creatorOID(src.creatorOID)
		, name(src.name)
		, categoryFolderOIDs(src.categoryFolderOIDs)
		, categoryFolderOIDsCNT(src.categoryFolderOIDsCNT)
		, docTypeOID(src.docTypeOID)
		, tags(src.tags)
		, storagefileNames(src.storagefileNames)
		, storagefileNamesCNT(src.storagefileNamesCNT)
		, storagefileSizes(src.storagefileSizes)
		, storagefileSizesCNT(src.storagefileSizesCNT)
		, storagefileOIDs(src.storagefileOIDs)
		, storagefileOIDsCNT(src.storagefileOIDsCNT)
		, localCreatedAts(src.localCreatedAts)
		, localCreatedAtsCNT(src.localCreatedAtsCNT)
		, localLastModifiedAts(src.localLastModifiedAts)
		, localLastModifiedAtsCNT(src.localLastModifiedAtsCNT)
		, maintainDuration(src.maintainDuration)
		, description(src.description)
		, extValues(src.extValues)
		, extValuesCNT(src.extValuesCNT)
	{
	}
	XMLRPCDOCUMENT_(const XMLRPCDOCUMENT_* src)
		: BASE<XMLRPCDOCUMENT_>()
		, folderOID(src->folderOID)
		, localFolderPath(src->localFolderPath)
		, creatorOID(src->creatorOID)
		, name(src->name)
		, categoryFolderOIDs(src->categoryFolderOIDs)
		, categoryFolderOIDsCNT(src->categoryFolderOIDsCNT)
		, docTypeOID(src->docTypeOID)
		, tags(src->tags)
		, storagefileNames(src->storagefileNames)
		, storagefileNamesCNT(src->storagefileNamesCNT)
		, storagefileSizes(src->storagefileSizes)
		, storagefileSizesCNT(src->storagefileSizesCNT)
		, storagefileOIDs(src->storagefileOIDs)
		, storagefileOIDsCNT(src->storagefileOIDsCNT)
		, localCreatedAts(src->localCreatedAts)
		, localCreatedAtsCNT(src->localCreatedAtsCNT)
		, localLastModifiedAts(src->localLastModifiedAts)
		, localLastModifiedAtsCNT(src->localLastModifiedAtsCNT)
		, maintainDuration(src->maintainDuration)
		, description(src->description)
		, extValues(src->extValues)
		, extValuesCNT(src->extValuesCNT)
	{
	}
	XMLRPCDOCUMENT_& operator=(const XMLRPCDOCUMENT_& src)
	{
		if (this != &src) {
			folderOID = src.folderOID;
			localFolderPath = src.localFolderPath;
			creatorOID = src.creatorOID;
			name = src.name;
			categoryFolderOIDs = src.categoryFolderOIDs;
			categoryFolderOIDsCNT = src.categoryFolderOIDsCNT;
			docTypeOID = src.docTypeOID;
			tags = src.tags;
			storagefileNames = src.storagefileNames;
			storagefileNamesCNT = src.storagefileNamesCNT;
			storagefileSizes = src.storagefileSizes;
			storagefileSizesCNT = src.storagefileSizesCNT;
			storagefileOIDs = src.storagefileOIDs;
			storagefileOIDsCNT = src.storagefileOIDsCNT;
			localCreatedAts = src.localCreatedAts;
			localCreatedAtsCNT = src.localCreatedAtsCNT;
			localLastModifiedAts = src.localLastModifiedAts;
			localLastModifiedAtsCNT = src.localLastModifiedAtsCNT;
			maintainDuration = src.maintainDuration;
			description = src.description;
			extValues = src.extValues;
			extValuesCNT = src.extValuesCNT;
		}
		return *this;
	}
};

struct XNETWORKRULE_ : BASE<XNETWORKRULE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* policyOID;
	const wchar_t* ruleType;
	const wchar_t* sortOrder;
	const wchar_t* applicationName;
	bool flagAll;
	const wchar_t* processName;
	const wchar_t* processInfo;
	bool flagProcessName;
	bool flagProcessInfo;
	const wchar_t* sourceIP;
	const wchar_t* targetIP;
	const wchar_t* targetPort;
	const wchar_t* permission;
	bool flagSaveLog;
	XNETWORKRULE_()
		: BASE<XNETWORKRULE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, policyOID(0)
		, ruleType(0)
		, sortOrder(0)
		, applicationName(0)
		, flagAll(0)
		, processName(0)
		, processInfo(0)
		, flagProcessName(0)
		, flagProcessInfo(0)
		, sourceIP(0)
		, targetIP(0)
		, targetPort(0)
		, permission(0)
		, flagSaveLog(0)
	{
	}
	XNETWORKRULE_(const XNETWORKRULE_& src)
		: BASE<XNETWORKRULE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, policyOID(src.policyOID)
		, ruleType(src.ruleType)
		, sortOrder(src.sortOrder)
		, applicationName(src.applicationName)
		, flagAll(src.flagAll)
		, processName(src.processName)
		, processInfo(src.processInfo)
		, flagProcessName(src.flagProcessName)
		, flagProcessInfo(src.flagProcessInfo)
		, sourceIP(src.sourceIP)
		, targetIP(src.targetIP)
		, targetPort(src.targetPort)
		, permission(src.permission)
		, flagSaveLog(src.flagSaveLog)
	{
	}
	XNETWORKRULE_(const XNETWORKRULE_* src)
		: BASE<XNETWORKRULE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, policyOID(src->policyOID)
		, ruleType(src->ruleType)
		, sortOrder(src->sortOrder)
		, applicationName(src->applicationName)
		, flagAll(src->flagAll)
		, processName(src->processName)
		, processInfo(src->processInfo)
		, flagProcessName(src->flagProcessName)
		, flagProcessInfo(src->flagProcessInfo)
		, sourceIP(src->sourceIP)
		, targetIP(src->targetIP)
		, targetPort(src->targetPort)
		, permission(src->permission)
		, flagSaveLog(src->flagSaveLog)
	{
	}
	XNETWORKRULE_& operator=(const XNETWORKRULE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			policyOID = src.policyOID;
			ruleType = src.ruleType;
			sortOrder = src.sortOrder;
			applicationName = src.applicationName;
			flagAll = src.flagAll;
			processName = src.processName;
			processInfo = src.processInfo;
			flagProcessName = src.flagProcessName;
			flagProcessInfo = src.flagProcessInfo;
			sourceIP = src.sourceIP;
			targetIP = src.targetIP;
			targetPort = src.targetPort;
			permission = src.permission;
			flagSaveLog = src.flagSaveLog;
		}
		return *this;
	}
};

struct XAMOUNTPOLICY_ : BASE<XAMOUNTPOLICY_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* name;
	const wchar_t* extension;
	const wchar_t* sortOrder;
	XAMOUNTPOLICY_()
		: BASE<XAMOUNTPOLICY_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, name(0)
		, extension(0)
		, sortOrder(0)
	{
	}
	XAMOUNTPOLICY_(const XAMOUNTPOLICY_& src)
		: BASE<XAMOUNTPOLICY_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, name(src.name)
		, extension(src.extension)
		, sortOrder(src.sortOrder)
	{
	}
	XAMOUNTPOLICY_(const XAMOUNTPOLICY_* src)
		: BASE<XAMOUNTPOLICY_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, name(src->name)
		, extension(src->extension)
		, sortOrder(src->sortOrder)
	{
	}
	XAMOUNTPOLICY_& operator=(const XAMOUNTPOLICY_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			name = src.name;
			extension = src.extension;
			sortOrder = src.sortOrder;
		}
		return *this;
	}
};

struct XGROUP_ : BASE<XGROUP_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* aclOID;
	int permission;
	XACE_** aceList;
	int aceListCNT;
	const wchar_t* creatorOID;
	const wchar_t* aclLevelOID;
	bool flagInheritACL;
	const wchar_t* managerGroupOID;
	const wchar_t* parentOID;
	const wchar_t* fullPathIndex;
	int subLastIndex;
	int childCount;
	const wchar_t* fullPathName;
	XUSERINFO_** userInfos;
	int userInfosCNT;
	const wchar_t** managerList;
	int managerListCNT;
	XUSERINFO_** managerPGroupUserInfos;
	int managerPGroupUserInfosCNT;
	const wchar_t* managerPGroupFullPathName;
	Long lastUsedAt;
	const wchar_t* documentSaveType;
	bool inheritDocumentSaveType;
	const wchar_t* useAutoCADHooking;
	bool inheritUseAutoCADHooking;
	const wchar_t* managerPGroupUserAccounts;
	const wchar_t* docViewApproveUserAccounts;
	const wchar_t* docExApproveUserAccounts;
	const wchar_t* pcExApproveUserAccounts;
	XGROUP_** orderedChildren;
	int orderedChildrenCNT;
	const wchar_t* managerOID;
	const wchar_t* managerPGroupOID;
	const wchar_t* extAttributeOID;
	const wchar_t* name;
	const wchar_t* groupCode;
	const wchar_t* description;
	const wchar_t* sortOrder;
	Long createdAt;
	Long lastModifiedAt;
	const wchar_t* deleteStatus;
	const wchar_t* workStatus;
	XEXTATTRIBUTEVALUE_* extAttributeValue;
	XEXTATTRIBUTE_* extAttribute;
	XGROUP_()
		: BASE<XGROUP_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, aclOID(0)
		, permission(0)
		, aceList(0)
		, aceListCNT(0)
		, creatorOID(0)
		, aclLevelOID(0)
		, flagInheritACL(0)
		, managerGroupOID(0)
		, parentOID(0)
		, fullPathIndex(0)
		, subLastIndex(0)
		, childCount(0)
		, fullPathName(0)
		, userInfos(0)
		, userInfosCNT(0)
		, managerList(0)
		, managerListCNT(0)
		, managerPGroupUserInfos(0)
		, managerPGroupUserInfosCNT(0)
		, managerPGroupFullPathName(0)
		, lastUsedAt(0)
		, documentSaveType(0)
		, inheritDocumentSaveType(0)
		, useAutoCADHooking(0)
		, inheritUseAutoCADHooking(0)
		, managerPGroupUserAccounts(0)
		, docViewApproveUserAccounts(0)
		, docExApproveUserAccounts(0)
		, pcExApproveUserAccounts(0)
		, orderedChildren(0)
		, orderedChildrenCNT(0)
		, managerOID(0)
		, managerPGroupOID(0)
		, extAttributeOID(0)
		, name(0)
		, groupCode(0)
		, description(0)
		, sortOrder(0)
		, createdAt(0)
		, lastModifiedAt(0)
		, deleteStatus(0)
		, workStatus(0)
		, extAttributeValue(0)
		, extAttribute(0)
	{
	}
	XGROUP_(const XGROUP_& src)
		: BASE<XGROUP_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, aclOID(src.aclOID)
		, permission(src.permission)
		, aceList(src.aceList)
		, aceListCNT(src.aceListCNT)
		, creatorOID(src.creatorOID)
		, aclLevelOID(src.aclLevelOID)
		, flagInheritACL(src.flagInheritACL)
		, managerGroupOID(src.managerGroupOID)
		, parentOID(src.parentOID)
		, fullPathIndex(src.fullPathIndex)
		, subLastIndex(src.subLastIndex)
		, childCount(src.childCount)
		, fullPathName(src.fullPathName)
		, userInfos(src.userInfos)
		, userInfosCNT(src.userInfosCNT)
		, managerList(src.managerList)
		, managerListCNT(src.managerListCNT)
		, managerPGroupUserInfos(src.managerPGroupUserInfos)
		, managerPGroupUserInfosCNT(src.managerPGroupUserInfosCNT)
		, managerPGroupFullPathName(src.managerPGroupFullPathName)
		, lastUsedAt(src.lastUsedAt)
		, documentSaveType(src.documentSaveType)
		, inheritDocumentSaveType(src.inheritDocumentSaveType)
		, useAutoCADHooking(src.useAutoCADHooking)
		, inheritUseAutoCADHooking(src.inheritUseAutoCADHooking)
		, managerPGroupUserAccounts(src.managerPGroupUserAccounts)
		, docViewApproveUserAccounts(src.docViewApproveUserAccounts)
		, docExApproveUserAccounts(src.docExApproveUserAccounts)
		, pcExApproveUserAccounts(src.pcExApproveUserAccounts)
		, orderedChildren(src.orderedChildren)
		, orderedChildrenCNT(src.orderedChildrenCNT)
		, managerOID(src.managerOID)
		, managerPGroupOID(src.managerPGroupOID)
		, extAttributeOID(src.extAttributeOID)
		, name(src.name)
		, groupCode(src.groupCode)
		, description(src.description)
		, sortOrder(src.sortOrder)
		, createdAt(src.createdAt)
		, lastModifiedAt(src.lastModifiedAt)
		, deleteStatus(src.deleteStatus)
		, workStatus(src.workStatus)
		, extAttributeValue(src.extAttributeValue)
		, extAttribute(src.extAttribute)
	{
	}
	XGROUP_(const XGROUP_* src)
		: BASE<XGROUP_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, aclOID(src->aclOID)
		, permission(src->permission)
		, aceList(src->aceList)
		, aceListCNT(src->aceListCNT)
		, creatorOID(src->creatorOID)
		, aclLevelOID(src->aclLevelOID)
		, flagInheritACL(src->flagInheritACL)
		, managerGroupOID(src->managerGroupOID)
		, parentOID(src->parentOID)
		, fullPathIndex(src->fullPathIndex)
		, subLastIndex(src->subLastIndex)
		, childCount(src->childCount)
		, fullPathName(src->fullPathName)
		, userInfos(src->userInfos)
		, userInfosCNT(src->userInfosCNT)
		, managerList(src->managerList)
		, managerListCNT(src->managerListCNT)
		, managerPGroupUserInfos(src->managerPGroupUserInfos)
		, managerPGroupUserInfosCNT(src->managerPGroupUserInfosCNT)
		, managerPGroupFullPathName(src->managerPGroupFullPathName)
		, lastUsedAt(src->lastUsedAt)
		, documentSaveType(src->documentSaveType)
		, inheritDocumentSaveType(src->inheritDocumentSaveType)
		, useAutoCADHooking(src->useAutoCADHooking)
		, inheritUseAutoCADHooking(src->inheritUseAutoCADHooking)
		, managerPGroupUserAccounts(src->managerPGroupUserAccounts)
		, docViewApproveUserAccounts(src->docViewApproveUserAccounts)
		, docExApproveUserAccounts(src->docExApproveUserAccounts)
		, pcExApproveUserAccounts(src->pcExApproveUserAccounts)
		, orderedChildren(src->orderedChildren)
		, orderedChildrenCNT(src->orderedChildrenCNT)
		, managerOID(src->managerOID)
		, managerPGroupOID(src->managerPGroupOID)
		, extAttributeOID(src->extAttributeOID)
		, name(src->name)
		, groupCode(src->groupCode)
		, description(src->description)
		, sortOrder(src->sortOrder)
		, createdAt(src->createdAt)
		, lastModifiedAt(src->lastModifiedAt)
		, deleteStatus(src->deleteStatus)
		, workStatus(src->workStatus)
		, extAttributeValue(src->extAttributeValue)
		, extAttribute(src->extAttribute)
	{
	}
	XGROUP_& operator=(const XGROUP_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			aclOID = src.aclOID;
			permission = src.permission;
			aceList = src.aceList;
			aceListCNT = src.aceListCNT;
			creatorOID = src.creatorOID;
			aclLevelOID = src.aclLevelOID;
			flagInheritACL = src.flagInheritACL;
			managerGroupOID = src.managerGroupOID;
			parentOID = src.parentOID;
			fullPathIndex = src.fullPathIndex;
			subLastIndex = src.subLastIndex;
			childCount = src.childCount;
			fullPathName = src.fullPathName;
			userInfos = src.userInfos;
			userInfosCNT = src.userInfosCNT;
			managerList = src.managerList;
			managerListCNT = src.managerListCNT;
			managerPGroupUserInfos = src.managerPGroupUserInfos;
			managerPGroupUserInfosCNT = src.managerPGroupUserInfosCNT;
			managerPGroupFullPathName = src.managerPGroupFullPathName;
			lastUsedAt = src.lastUsedAt;
			documentSaveType = src.documentSaveType;
			inheritDocumentSaveType = src.inheritDocumentSaveType;
			useAutoCADHooking = src.useAutoCADHooking;
			inheritUseAutoCADHooking = src.inheritUseAutoCADHooking;
			managerPGroupUserAccounts = src.managerPGroupUserAccounts;
			docViewApproveUserAccounts = src.docViewApproveUserAccounts;
			docExApproveUserAccounts = src.docExApproveUserAccounts;
			pcExApproveUserAccounts = src.pcExApproveUserAccounts;
			orderedChildren = src.orderedChildren;
			orderedChildrenCNT = src.orderedChildrenCNT;
			managerOID = src.managerOID;
			managerPGroupOID = src.managerPGroupOID;
			extAttributeOID = src.extAttributeOID;
			name = src.name;
			groupCode = src.groupCode;
			description = src.description;
			sortOrder = src.sortOrder;
			createdAt = src.createdAt;
			lastModifiedAt = src.lastModifiedAt;
			deleteStatus = src.deleteStatus;
			workStatus = src.workStatus;
			extAttributeValue = src.extAttributeValue;
			extAttribute = src.extAttribute;
		}
		return *this;
	}
};

struct XMEDIARULE_ : BASE<XMEDIARULE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* policyOID;
	const wchar_t* ruleType;
	const wchar_t* sortOrder;
	const wchar_t* mediaType;
	const wchar_t* permission;
	bool flagSaveLog;
	XMEDIARULE_()
		: BASE<XMEDIARULE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, policyOID(0)
		, ruleType(0)
		, sortOrder(0)
		, mediaType(0)
		, permission(0)
		, flagSaveLog(0)
	{
	}
	XMEDIARULE_(const XMEDIARULE_& src)
		: BASE<XMEDIARULE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, policyOID(src.policyOID)
		, ruleType(src.ruleType)
		, sortOrder(src.sortOrder)
		, mediaType(src.mediaType)
		, permission(src.permission)
		, flagSaveLog(src.flagSaveLog)
	{
	}
	XMEDIARULE_(const XMEDIARULE_* src)
		: BASE<XMEDIARULE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, policyOID(src->policyOID)
		, ruleType(src->ruleType)
		, sortOrder(src->sortOrder)
		, mediaType(src->mediaType)
		, permission(src->permission)
		, flagSaveLog(src->flagSaveLog)
	{
	}
	XMEDIARULE_& operator=(const XMEDIARULE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			policyOID = src.policyOID;
			ruleType = src.ruleType;
			sortOrder = src.sortOrder;
			mediaType = src.mediaType;
			permission = src.permission;
			flagSaveLog = src.flagSaveLog;
		}
		return *this;
	}
};

struct RPCDOCUMENT_ : BASE<RPCDOCUMENT_> {
	const wchar_t* folderOID;
	const wchar_t* name;
	RPCDOCUMENT_()
		: BASE<RPCDOCUMENT_>()
		, folderOID(0)
		, name(0)
	{
	}
	RPCDOCUMENT_(const RPCDOCUMENT_& src)
		: BASE<RPCDOCUMENT_>()
		, folderOID(src.folderOID)
		, name(src.name)
	{
	}
	RPCDOCUMENT_(const RPCDOCUMENT_* src)
		: BASE<RPCDOCUMENT_>()
		, folderOID(src->folderOID)
		, name(src->name)
	{
	}
	RPCDOCUMENT_& operator=(const RPCDOCUMENT_& src)
	{
		if (this != &src) {
			folderOID = src.folderOID;
			name = src.name;
		}
		return *this;
	}
};

struct XDLPPOLICY_ : BASE<XDLPPOLICY_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	XMEDIARULE_** mediaRules;
	int mediaRulesCNT;
	XSITERULE_** siteRules;
	int siteRulesCNT;
	XNETWORKRULE_** networkRules;
	int networkRulesCNT;
	XPROCESSRULE_** processRules;
	int processRulesCNT;
	XPRINTRULE_** printRules;
	int printRulesCNT;
	const wchar_t* name;
	bool flagDefault;
	Long createdAt;
	bool flagMediaControl;
	bool flagSiteControl;
	bool flagNetworkControl;
	bool flagProcessControl;
	bool flagPrintControl;
	const wchar_t* logSaveType;
	const wchar_t* sortOrder;
	XDLPPOLICY_()
		: BASE<XDLPPOLICY_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, mediaRules(0)
		, mediaRulesCNT(0)
		, siteRules(0)
		, siteRulesCNT(0)
		, networkRules(0)
		, networkRulesCNT(0)
		, processRules(0)
		, processRulesCNT(0)
		, printRules(0)
		, printRulesCNT(0)
		, name(0)
		, flagDefault(0)
		, createdAt(0)
		, flagMediaControl(0)
		, flagSiteControl(0)
		, flagNetworkControl(0)
		, flagProcessControl(0)
		, flagPrintControl(0)
		, logSaveType(0)
		, sortOrder(0)
	{
	}
	XDLPPOLICY_(const XDLPPOLICY_& src)
		: BASE<XDLPPOLICY_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, mediaRules(src.mediaRules)
		, mediaRulesCNT(src.mediaRulesCNT)
		, siteRules(src.siteRules)
		, siteRulesCNT(src.siteRulesCNT)
		, networkRules(src.networkRules)
		, networkRulesCNT(src.networkRulesCNT)
		, processRules(src.processRules)
		, processRulesCNT(src.processRulesCNT)
		, printRules(src.printRules)
		, printRulesCNT(src.printRulesCNT)
		, name(src.name)
		, flagDefault(src.flagDefault)
		, createdAt(src.createdAt)
		, flagMediaControl(src.flagMediaControl)
		, flagSiteControl(src.flagSiteControl)
		, flagNetworkControl(src.flagNetworkControl)
		, flagProcessControl(src.flagProcessControl)
		, flagPrintControl(src.flagPrintControl)
		, logSaveType(src.logSaveType)
		, sortOrder(src.sortOrder)
	{
	}
	XDLPPOLICY_(const XDLPPOLICY_* src)
		: BASE<XDLPPOLICY_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, mediaRules(src->mediaRules)
		, mediaRulesCNT(src->mediaRulesCNT)
		, siteRules(src->siteRules)
		, siteRulesCNT(src->siteRulesCNT)
		, networkRules(src->networkRules)
		, networkRulesCNT(src->networkRulesCNT)
		, processRules(src->processRules)
		, processRulesCNT(src->processRulesCNT)
		, printRules(src->printRules)
		, printRulesCNT(src->printRulesCNT)
		, name(src->name)
		, flagDefault(src->flagDefault)
		, createdAt(src->createdAt)
		, flagMediaControl(src->flagMediaControl)
		, flagSiteControl(src->flagSiteControl)
		, flagNetworkControl(src->flagNetworkControl)
		, flagProcessControl(src->flagProcessControl)
		, flagPrintControl(src->flagPrintControl)
		, logSaveType(src->logSaveType)
		, sortOrder(src->sortOrder)
	{
	}
	XDLPPOLICY_& operator=(const XDLPPOLICY_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			mediaRules = src.mediaRules;
			mediaRulesCNT = src.mediaRulesCNT;
			siteRules = src.siteRules;
			siteRulesCNT = src.siteRulesCNT;
			networkRules = src.networkRules;
			networkRulesCNT = src.networkRulesCNT;
			processRules = src.processRules;
			processRulesCNT = src.processRulesCNT;
			printRules = src.printRules;
			printRulesCNT = src.printRulesCNT;
			name = src.name;
			flagDefault = src.flagDefault;
			createdAt = src.createdAt;
			flagMediaControl = src.flagMediaControl;
			flagSiteControl = src.flagSiteControl;
			flagNetworkControl = src.flagNetworkControl;
			flagProcessControl = src.flagProcessControl;
			flagPrintControl = src.flagPrintControl;
			logSaveType = src.logSaveType;
			sortOrder = src.sortOrder;
		}
		return *this;
	}
};

struct RETURNINTVO_ : BASE<RETURNINTVO_> {
	int returnCode;
	int returnValue;
	RETURNINTVO_()
		: BASE<RETURNINTVO_>()
		, returnCode(0)
		, returnValue(0)
	{
	}
	RETURNINTVO_(const RETURNINTVO_& src)
		: BASE<RETURNINTVO_>()
		, returnCode(src.returnCode)
		, returnValue(src.returnValue)
	{
	}
	RETURNINTVO_(const RETURNINTVO_* src)
		: BASE<RETURNINTVO_>()
		, returnCode(src->returnCode)
		, returnValue(src->returnValue)
	{
	}
	RETURNINTVO_& operator=(const RETURNINTVO_& src)
	{
		if (this != &src) {
			returnCode = src.returnCode;
			returnValue = src.returnValue;
		}
		return *this;
	}
};

struct XSYNCFOLDERLIST_ : BASE<XSYNCFOLDERLIST_> {
	int folderCount;
	XSYNCFOLDER_** folders;
	int foldersCNT;
	XSYNCFOLDERLIST_()
		: BASE<XSYNCFOLDERLIST_>()
		, folderCount(0)
		, folders(0)
		, foldersCNT(0)
	{
	}
	XSYNCFOLDERLIST_(const XSYNCFOLDERLIST_& src)
		: BASE<XSYNCFOLDERLIST_>()
		, folderCount(src.folderCount)
		, folders(src.folders)
		, foldersCNT(src.foldersCNT)
	{
	}
	XSYNCFOLDERLIST_(const XSYNCFOLDERLIST_* src)
		: BASE<XSYNCFOLDERLIST_>()
		, folderCount(src->folderCount)
		, folders(src->folders)
		, foldersCNT(src->foldersCNT)
	{
	}
	XSYNCFOLDERLIST_& operator=(const XSYNCFOLDERLIST_& src)
	{
		if (this != &src) {
			folderCount = src.folderCount;
			folders = src.folders;
			foldersCNT = src.foldersCNT;
		}
		return *this;
	}
};

struct RETURNVO_ : BASE<RETURNVO_> {
	int returnCode;
	const wchar_t* returnValue;
	RETURNVO_()
		: BASE<RETURNVO_>()
		, returnCode(0)
		, returnValue(0)
	{
	}
	RETURNVO_(const RETURNVO_& src)
		: BASE<RETURNVO_>()
		, returnCode(src.returnCode)
		, returnValue(src.returnValue)
	{
	}
	RETURNVO_(const RETURNVO_* src)
		: BASE<RETURNVO_>()
		, returnCode(src->returnCode)
		, returnValue(src->returnValue)
	{
	}
	RETURNVO_& operator=(const RETURNVO_& src)
	{
		if (this != &src) {
			returnCode = src.returnCode;
			returnValue = src.returnValue;
		}
		return *this;
	}
};

struct XASYNCOPERATIONRESULT_ : BASE<XASYNCOPERATIONRESULT_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* operationOID;
	const wchar_t* operationName;
	const wchar_t* fromOID;
	const wchar_t* fromName;
	const wchar_t* fromType;
	const wchar_t* toOID;
	const wchar_t* toName;
	const wchar_t* toType;
	const wchar_t* errorCode;
	const wchar_t* errorMsg;
	XASYNCOPERATIONRESULT_()
		: BASE<XASYNCOPERATIONRESULT_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, operationOID(0)
		, operationName(0)
		, fromOID(0)
		, fromName(0)
		, fromType(0)
		, toOID(0)
		, toName(0)
		, toType(0)
		, errorCode(0)
		, errorMsg(0)
	{
	}
	XASYNCOPERATIONRESULT_(const XASYNCOPERATIONRESULT_& src)
		: BASE<XASYNCOPERATIONRESULT_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, operationOID(src.operationOID)
		, operationName(src.operationName)
		, fromOID(src.fromOID)
		, fromName(src.fromName)
		, fromType(src.fromType)
		, toOID(src.toOID)
		, toName(src.toName)
		, toType(src.toType)
		, errorCode(src.errorCode)
		, errorMsg(src.errorMsg)
	{
	}
	XASYNCOPERATIONRESULT_(const XASYNCOPERATIONRESULT_* src)
		: BASE<XASYNCOPERATIONRESULT_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, operationOID(src->operationOID)
		, operationName(src->operationName)
		, fromOID(src->fromOID)
		, fromName(src->fromName)
		, fromType(src->fromType)
		, toOID(src->toOID)
		, toName(src->toName)
		, toType(src->toType)
		, errorCode(src->errorCode)
		, errorMsg(src->errorMsg)
	{
	}
	XASYNCOPERATIONRESULT_& operator=(const XASYNCOPERATIONRESULT_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			operationOID = src.operationOID;
			operationName = src.operationName;
			fromOID = src.fromOID;
			fromName = src.fromName;
			fromType = src.fromType;
			toOID = src.toOID;
			toName = src.toName;
			toType = src.toType;
			errorCode = src.errorCode;
			errorMsg = src.errorMsg;
		}
		return *this;
	}
};

struct XSYNCDOCUMENTLIST_ : BASE<XSYNCDOCUMENTLIST_> {
	int documentCount;
	int totalCount;
	XSYNCDOCUMENT_** documents;
	int documentsCNT;
	XSYNCDOCUMENTLIST_()
		: BASE<XSYNCDOCUMENTLIST_>()
		, documentCount(0)
		, totalCount(0)
		, documents(0)
		, documentsCNT(0)
	{
	}
	XSYNCDOCUMENTLIST_(const XSYNCDOCUMENTLIST_& src)
		: BASE<XSYNCDOCUMENTLIST_>()
		, documentCount(src.documentCount)
		, totalCount(src.totalCount)
		, documents(src.documents)
		, documentsCNT(src.documentsCNT)
	{
	}
	XSYNCDOCUMENTLIST_(const XSYNCDOCUMENTLIST_* src)
		: BASE<XSYNCDOCUMENTLIST_>()
		, documentCount(src->documentCount)
		, totalCount(src->totalCount)
		, documents(src->documents)
		, documentsCNT(src->documentsCNT)
	{
	}
	XSYNCDOCUMENTLIST_& operator=(const XSYNCDOCUMENTLIST_& src)
	{
		if (this != &src) {
			documentCount = src.documentCount;
			totalCount = src.totalCount;
			documents = src.documents;
			documentsCNT = src.documentsCNT;
		}
		return *this;
	}
};

struct XSYNCDOCUMENT_ : BASE<XSYNCDOCUMENT_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* folderOID;
	const wchar_t* folderFullPath;
	const wchar_t* creatorOID;
	const wchar_t* name;
	const wchar_t* fileOID;
	const wchar_t* fileSize;
	const wchar_t* storageFileID;
	int lastModifiedAtLow;
	int lastModifiedAtHigh;
	const wchar_t* lastModifierOID;
	const wchar_t* lastModifierName;
	const wchar_t* creatorName;
	const wchar_t* docStatus;
	const wchar_t* permissions;
	XSYNCDOCUMENT_()
		: BASE<XSYNCDOCUMENT_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, folderOID(0)
		, folderFullPath(0)
		, creatorOID(0)
		, name(0)
		, fileOID(0)
		, fileSize(0)
		, storageFileID(0)
		, lastModifiedAtLow(0)
		, lastModifiedAtHigh(0)
		, lastModifierOID(0)
		, lastModifierName(0)
		, creatorName(0)
		, docStatus(0)
		, permissions(0)
	{
	}
	XSYNCDOCUMENT_(const XSYNCDOCUMENT_& src)
		: BASE<XSYNCDOCUMENT_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, folderOID(src.folderOID)
		, folderFullPath(src.folderFullPath)
		, creatorOID(src.creatorOID)
		, name(src.name)
		, fileOID(src.fileOID)
		, fileSize(src.fileSize)
		, storageFileID(src.storageFileID)
		, lastModifiedAtLow(src.lastModifiedAtLow)
		, lastModifiedAtHigh(src.lastModifiedAtHigh)
		, lastModifierOID(src.lastModifierOID)
		, lastModifierName(src.lastModifierName)
		, creatorName(src.creatorName)
		, docStatus(src.docStatus)
		, permissions(src.permissions)
	{
	}
	XSYNCDOCUMENT_(const XSYNCDOCUMENT_* src)
		: BASE<XSYNCDOCUMENT_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, folderOID(src->folderOID)
		, folderFullPath(src->folderFullPath)
		, creatorOID(src->creatorOID)
		, name(src->name)
		, fileOID(src->fileOID)
		, fileSize(src->fileSize)
		, storageFileID(src->storageFileID)
		, lastModifiedAtLow(src->lastModifiedAtLow)
		, lastModifiedAtHigh(src->lastModifiedAtHigh)
		, lastModifierOID(src->lastModifierOID)
		, lastModifierName(src->lastModifierName)
		, creatorName(src->creatorName)
		, docStatus(src->docStatus)
		, permissions(src->permissions)
	{
	}
	XSYNCDOCUMENT_& operator=(const XSYNCDOCUMENT_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			folderOID = src.folderOID;
			folderFullPath = src.folderFullPath;
			creatorOID = src.creatorOID;
			name = src.name;
			fileOID = src.fileOID;
			fileSize = src.fileSize;
			storageFileID = src.storageFileID;
			lastModifiedAtLow = src.lastModifiedAtLow;
			lastModifiedAtHigh = src.lastModifiedAtHigh;
			lastModifierOID = src.lastModifierOID;
			lastModifierName = src.lastModifierName;
			creatorName = src.creatorName;
			docStatus = src.docStatus;
			permissions = src.permissions;
		}
		return *this;
	}
};

struct XMLRPCUSER_ : BASE<XMLRPCUSER_> {
	const wchar_t* userOID;
	const wchar_t* userName;
	const wchar_t* account;
	const wchar_t* groupFullPathName;
	const wchar_t* groupCode;
	const wchar_t* groupPosition;
	const wchar_t* groupOID;
	const wchar_t* email;
	const wchar_t* userType;
	XMLRPCUSER_()
		: BASE<XMLRPCUSER_>()
		, userOID(0)
		, userName(0)
		, account(0)
		, groupFullPathName(0)
		, groupCode(0)
		, groupPosition(0)
		, groupOID(0)
		, email(0)
		, userType(0)
	{
	}
	XMLRPCUSER_(const XMLRPCUSER_& src)
		: BASE<XMLRPCUSER_>()
		, userOID(src.userOID)
		, userName(src.userName)
		, account(src.account)
		, groupFullPathName(src.groupFullPathName)
		, groupCode(src.groupCode)
		, groupPosition(src.groupPosition)
		, groupOID(src.groupOID)
		, email(src.email)
		, userType(src.userType)
	{
	}
	XMLRPCUSER_(const XMLRPCUSER_* src)
		: BASE<XMLRPCUSER_>()
		, userOID(src->userOID)
		, userName(src->userName)
		, account(src->account)
		, groupFullPathName(src->groupFullPathName)
		, groupCode(src->groupCode)
		, groupPosition(src->groupPosition)
		, groupOID(src->groupOID)
		, email(src->email)
		, userType(src->userType)
	{
	}
	XMLRPCUSER_& operator=(const XMLRPCUSER_& src)
	{
		if (this != &src) {
			userOID = src.userOID;
			userName = src.userName;
			account = src.account;
			groupFullPathName = src.groupFullPathName;
			groupCode = src.groupCode;
			groupPosition = src.groupPosition;
			groupOID = src.groupOID;
			email = src.email;
			userType = src.userType;
		}
		return *this;
	}
};

struct XEXTCOLUMN_ : BASE<XEXTCOLUMN_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	XCODELIST_* codeList;
	const wchar_t* extAttributeOID;
	const wchar_t* name;
	const wchar_t* colType;
	const wchar_t* inputType;
	const wchar_t* dbColName;
	int dbSize;
	int dbSubSize;
	const wchar_t* codeListOID;
	const wchar_t* viewFormat;
	const wchar_t* viewLineType;
	int viewOrder;
	const wchar_t* minInputValue;
	const wchar_t* maxInputValue;
	const wchar_t* defaultValue;
	bool flagSearch;
	bool flagRequired;
	bool flagVisible;
	bool flagLineFeed;
	bool flagComma;
	bool flagBetween;
	bool flagVirtual;
	bool flagFixedValue;
	const wchar_t* currencyFormat;
	XEXTCOLUMN_()
		: BASE<XEXTCOLUMN_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, codeList(0)
		, extAttributeOID(0)
		, name(0)
		, colType(0)
		, inputType(0)
		, dbColName(0)
		, dbSize(0)
		, dbSubSize(0)
		, codeListOID(0)
		, viewFormat(0)
		, viewLineType(0)
		, viewOrder(0)
		, minInputValue(0)
		, maxInputValue(0)
		, defaultValue(0)
		, flagSearch(0)
		, flagRequired(0)
		, flagVisible(0)
		, flagLineFeed(0)
		, flagComma(0)
		, flagBetween(0)
		, flagVirtual(0)
		, flagFixedValue(0)
		, currencyFormat(0)
	{
	}
	XEXTCOLUMN_(const XEXTCOLUMN_& src)
		: BASE<XEXTCOLUMN_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, codeList(src.codeList)
		, extAttributeOID(src.extAttributeOID)
		, name(src.name)
		, colType(src.colType)
		, inputType(src.inputType)
		, dbColName(src.dbColName)
		, dbSize(src.dbSize)
		, dbSubSize(src.dbSubSize)
		, codeListOID(src.codeListOID)
		, viewFormat(src.viewFormat)
		, viewLineType(src.viewLineType)
		, viewOrder(src.viewOrder)
		, minInputValue(src.minInputValue)
		, maxInputValue(src.maxInputValue)
		, defaultValue(src.defaultValue)
		, flagSearch(src.flagSearch)
		, flagRequired(src.flagRequired)
		, flagVisible(src.flagVisible)
		, flagLineFeed(src.flagLineFeed)
		, flagComma(src.flagComma)
		, flagBetween(src.flagBetween)
		, flagVirtual(src.flagVirtual)
		, flagFixedValue(src.flagFixedValue)
		, currencyFormat(src.currencyFormat)
	{
	}
	XEXTCOLUMN_(const XEXTCOLUMN_* src)
		: BASE<XEXTCOLUMN_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, codeList(src->codeList)
		, extAttributeOID(src->extAttributeOID)
		, name(src->name)
		, colType(src->colType)
		, inputType(src->inputType)
		, dbColName(src->dbColName)
		, dbSize(src->dbSize)
		, dbSubSize(src->dbSubSize)
		, codeListOID(src->codeListOID)
		, viewFormat(src->viewFormat)
		, viewLineType(src->viewLineType)
		, viewOrder(src->viewOrder)
		, minInputValue(src->minInputValue)
		, maxInputValue(src->maxInputValue)
		, defaultValue(src->defaultValue)
		, flagSearch(src->flagSearch)
		, flagRequired(src->flagRequired)
		, flagVisible(src->flagVisible)
		, flagLineFeed(src->flagLineFeed)
		, flagComma(src->flagComma)
		, flagBetween(src->flagBetween)
		, flagVirtual(src->flagVirtual)
		, flagFixedValue(src->flagFixedValue)
		, currencyFormat(src->currencyFormat)
	{
	}
	XEXTCOLUMN_& operator=(const XEXTCOLUMN_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			codeList = src.codeList;
			extAttributeOID = src.extAttributeOID;
			name = src.name;
			colType = src.colType;
			inputType = src.inputType;
			dbColName = src.dbColName;
			dbSize = src.dbSize;
			dbSubSize = src.dbSubSize;
			codeListOID = src.codeListOID;
			viewFormat = src.viewFormat;
			viewLineType = src.viewLineType;
			viewOrder = src.viewOrder;
			minInputValue = src.minInputValue;
			maxInputValue = src.maxInputValue;
			defaultValue = src.defaultValue;
			flagSearch = src.flagSearch;
			flagRequired = src.flagRequired;
			flagVisible = src.flagVisible;
			flagLineFeed = src.flagLineFeed;
			flagComma = src.flagComma;
			flagBetween = src.flagBetween;
			flagVirtual = src.flagVirtual;
			flagFixedValue = src.flagFixedValue;
			currencyFormat = src.currencyFormat;
		}
		return *this;
	}
};

struct DRIVEOBJECT_ : BASE<DRIVEOBJECT_> {
	const wchar_t* oid;
	const wchar_t* parentOID;
	int lastModifiedAtHigh;
	int lastModifiedAtLow;
	int permissionProperties;
	int aclProperties;
	const wchar_t* objName;
	const wchar_t* creatorInfo;
	int objType;
	int fileSizeHigh;
	int fileSizeLow;
	const wchar_t* totalSize;
	int totalCount;
	int localLastModifiedAtHigh;
	int localLastModifiedAtLow;
	int objCreatedAtHigh;
	int objCreatedAtLow;
	bool flagOpen;
	bool flagHaveChild;
	int returnCode;
	FOLDERITEM_** fullPathItems;
	int fullPathItemsCNT;
	const wchar_t* storageFileID;
	const wchar_t* docStatus;
	const wchar_t* versionStatus;
	const wchar_t* versionCode;
	const wchar_t* processStatus;
	const wchar_t* tags;
	const wchar_t* sourceOID;
	const wchar_t* drmType;
	DRIVEOBJECT_** childObjects;
	int childObjectsCNT;
	const wchar_t* folderType;
	bool flagGroupRootFolder;
	bool existFavorite;
	bool enableChangeManagerGroup;
	bool othersPrivateFolder;
	const wchar_t* checkOutInfo;
	int checkOutAtHigh;
	int checkOutAtLow;
	int localCreatedAtHigh;
	int localCreatedAtLow;
	double searchResultScore;
	DRIVEOBJECT_()
		: BASE<DRIVEOBJECT_>()
		, oid(0)
		, parentOID(0)
		, lastModifiedAtHigh(0)
		, lastModifiedAtLow(0)
		, permissionProperties(0)
		, aclProperties(0)
		, objName(0)
		, creatorInfo(0)
		, objType(0)
		, fileSizeHigh(0)
		, fileSizeLow(0)
		, totalSize(0)
		, totalCount(0)
		, localLastModifiedAtHigh(0)
		, localLastModifiedAtLow(0)
		, objCreatedAtHigh(0)
		, objCreatedAtLow(0)
		, flagOpen(0)
		, flagHaveChild(0)
		, returnCode(0)
		, fullPathItems(0)
		, fullPathItemsCNT(0)
		, storageFileID(0)
		, docStatus(0)
		, versionStatus(0)
		, versionCode(0)
		, processStatus(0)
		, tags(0)
		, sourceOID(0)
		, drmType(0)
		, childObjects(0)
		, childObjectsCNT(0)
		, folderType(0)
		, flagGroupRootFolder(0)
		, existFavorite(0)
		, enableChangeManagerGroup(0)
		, othersPrivateFolder(0)
		, checkOutInfo(0)
		, checkOutAtHigh(0)
		, checkOutAtLow(0)
		, localCreatedAtHigh(0)
		, localCreatedAtLow(0)
		, searchResultScore(0)
	{
	}
	DRIVEOBJECT_(const DRIVEOBJECT_& src)
		: BASE<DRIVEOBJECT_>()
		, oid(src.oid)
		, parentOID(src.parentOID)
		, lastModifiedAtHigh(src.lastModifiedAtHigh)
		, lastModifiedAtLow(src.lastModifiedAtLow)
		, permissionProperties(src.permissionProperties)
		, aclProperties(src.aclProperties)
		, objName(src.objName)
		, creatorInfo(src.creatorInfo)
		, objType(src.objType)
		, fileSizeHigh(src.fileSizeHigh)
		, fileSizeLow(src.fileSizeLow)
		, totalSize(src.totalSize)
		, totalCount(src.totalCount)
		, localLastModifiedAtHigh(src.localLastModifiedAtHigh)
		, localLastModifiedAtLow(src.localLastModifiedAtLow)
		, objCreatedAtHigh(src.objCreatedAtHigh)
		, objCreatedAtLow(src.objCreatedAtLow)
		, flagOpen(src.flagOpen)
		, flagHaveChild(src.flagHaveChild)
		, returnCode(src.returnCode)
		, fullPathItems(src.fullPathItems)
		, fullPathItemsCNT(src.fullPathItemsCNT)
		, storageFileID(src.storageFileID)
		, docStatus(src.docStatus)
		, versionStatus(src.versionStatus)
		, versionCode(src.versionCode)
		, processStatus(src.processStatus)
		, tags(src.tags)
		, sourceOID(src.sourceOID)
		, drmType(src.drmType)
		, childObjects(src.childObjects)
		, childObjectsCNT(src.childObjectsCNT)
		, folderType(src.folderType)
		, flagGroupRootFolder(src.flagGroupRootFolder)
		, existFavorite(src.existFavorite)
		, enableChangeManagerGroup(src.enableChangeManagerGroup)
		, othersPrivateFolder(src.othersPrivateFolder)
		, checkOutInfo(src.checkOutInfo)
		, checkOutAtHigh(src.checkOutAtHigh)
		, checkOutAtLow(src.checkOutAtLow)
		, localCreatedAtHigh(src.localCreatedAtHigh)
		, localCreatedAtLow(src.localCreatedAtLow)
		, searchResultScore(src.searchResultScore)
	{
	}
	DRIVEOBJECT_(const DRIVEOBJECT_* src)
		: BASE<DRIVEOBJECT_>()
		, oid(src->oid)
		, parentOID(src->parentOID)
		, lastModifiedAtHigh(src->lastModifiedAtHigh)
		, lastModifiedAtLow(src->lastModifiedAtLow)
		, permissionProperties(src->permissionProperties)
		, aclProperties(src->aclProperties)
		, objName(src->objName)
		, creatorInfo(src->creatorInfo)
		, objType(src->objType)
		, fileSizeHigh(src->fileSizeHigh)
		, fileSizeLow(src->fileSizeLow)
		, totalSize(src->totalSize)
		, totalCount(src->totalCount)
		, localLastModifiedAtHigh(src->localLastModifiedAtHigh)
		, localLastModifiedAtLow(src->localLastModifiedAtLow)
		, objCreatedAtHigh(src->objCreatedAtHigh)
		, objCreatedAtLow(src->objCreatedAtLow)
		, flagOpen(src->flagOpen)
		, flagHaveChild(src->flagHaveChild)
		, returnCode(src->returnCode)
		, fullPathItems(src->fullPathItems)
		, fullPathItemsCNT(src->fullPathItemsCNT)
		, storageFileID(src->storageFileID)
		, docStatus(src->docStatus)
		, versionStatus(src->versionStatus)
		, versionCode(src->versionCode)
		, processStatus(src->processStatus)
		, tags(src->tags)
		, sourceOID(src->sourceOID)
		, drmType(src->drmType)
		, childObjects(src->childObjects)
		, childObjectsCNT(src->childObjectsCNT)
		, folderType(src->folderType)
		, flagGroupRootFolder(src->flagGroupRootFolder)
		, existFavorite(src->existFavorite)
		, enableChangeManagerGroup(src->enableChangeManagerGroup)
		, othersPrivateFolder(src->othersPrivateFolder)
		, checkOutInfo(src->checkOutInfo)
		, checkOutAtHigh(src->checkOutAtHigh)
		, checkOutAtLow(src->checkOutAtLow)
		, localCreatedAtHigh(src->localCreatedAtHigh)
		, localCreatedAtLow(src->localCreatedAtLow)
		, searchResultScore(src->searchResultScore)
	{
	}
	DRIVEOBJECT_& operator=(const DRIVEOBJECT_& src)
	{
		if (this != &src) {
			oid = src.oid;
			parentOID = src.parentOID;
			lastModifiedAtHigh = src.lastModifiedAtHigh;
			lastModifiedAtLow = src.lastModifiedAtLow;
			permissionProperties = src.permissionProperties;
			aclProperties = src.aclProperties;
			objName = src.objName;
			creatorInfo = src.creatorInfo;
			objType = src.objType;
			fileSizeHigh = src.fileSizeHigh;
			fileSizeLow = src.fileSizeLow;
			totalSize = src.totalSize;
			totalCount = src.totalCount;
			localLastModifiedAtHigh = src.localLastModifiedAtHigh;
			localLastModifiedAtLow = src.localLastModifiedAtLow;
			objCreatedAtHigh = src.objCreatedAtHigh;
			objCreatedAtLow = src.objCreatedAtLow;
			flagOpen = src.flagOpen;
			flagHaveChild = src.flagHaveChild;
			returnCode = src.returnCode;
			fullPathItems = src.fullPathItems;
			fullPathItemsCNT = src.fullPathItemsCNT;
			storageFileID = src.storageFileID;
			docStatus = src.docStatus;
			versionStatus = src.versionStatus;
			versionCode = src.versionCode;
			processStatus = src.processStatus;
			tags = src.tags;
			sourceOID = src.sourceOID;
			drmType = src.drmType;
			childObjects = src.childObjects;
			childObjectsCNT = src.childObjectsCNT;
			folderType = src.folderType;
			flagGroupRootFolder = src.flagGroupRootFolder;
			existFavorite = src.existFavorite;
			enableChangeManagerGroup = src.enableChangeManagerGroup;
			othersPrivateFolder = src.othersPrivateFolder;
			checkOutInfo = src.checkOutInfo;
			checkOutAtHigh = src.checkOutAtHigh;
			checkOutAtLow = src.checkOutAtLow;
			localCreatedAtHigh = src.localCreatedAtHigh;
			localCreatedAtLow = src.localCreatedAtLow;
			searchResultScore = src.searchResultScore;
		}
		return *this;
	}
};

struct LOGINPARAM_ : BASE<LOGINPARAM_> {
	const wchar_t* userOID;
	const wchar_t* groupOID;
	const wchar_t* account;
	const wchar_t* password;
	const wchar_t* clientIP;
	const wchar_t* sessionID;
	bool fromAgent;
	bool useSSO;
	bool useMultiLogin;
	bool autoLogin;
	bool windowLogin;
	const wchar_t* domain;
	CLIENTZONEINFO_* clientZoneInfo;
	const wchar_t* networkMode;
	const wchar_t* systemId;
	const wchar_t* macAddress;
	const wchar_t* osInfo;
	const wchar_t* deviceUID;
	const wchar_t* deviceName;
	bool masterKey;
	bool checkDeviceCertification;
	const wchar_t* deviceAppkey;
	LOGINPARAM_()
		: BASE<LOGINPARAM_>()
		, userOID(0)
		, groupOID(0)
		, account(0)
		, password(0)
		, clientIP(0)
		, sessionID(0)
		, fromAgent(0)
		, useSSO(0)
		, useMultiLogin(0)
		, autoLogin(0)
		, windowLogin(0)
		, domain(0)
		, clientZoneInfo(0)
		, networkMode(0)
		, systemId(0)
		, macAddress(0)
		, osInfo(0)
		, deviceUID(0)
		, deviceName(0)
		, masterKey(0)
		, checkDeviceCertification(0)
		, deviceAppkey(0)
	{
	}
	LOGINPARAM_(const LOGINPARAM_& src)
		: BASE<LOGINPARAM_>()
		, userOID(src.userOID)
		, groupOID(src.groupOID)
		, account(src.account)
		, password(src.password)
		, clientIP(src.clientIP)
		, sessionID(src.sessionID)
		, fromAgent(src.fromAgent)
		, useSSO(src.useSSO)
		, useMultiLogin(src.useMultiLogin)
		, autoLogin(src.autoLogin)
		, windowLogin(src.windowLogin)
		, domain(src.domain)
		, clientZoneInfo(src.clientZoneInfo)
		, networkMode(src.networkMode)
		, systemId(src.systemId)
		, macAddress(src.macAddress)
		, osInfo(src.osInfo)
		, deviceUID(src.deviceUID)
		, deviceName(src.deviceName)
		, masterKey(src.masterKey)
		, checkDeviceCertification(src.checkDeviceCertification)
		, deviceAppkey(src.deviceAppkey)
	{
	}
	LOGINPARAM_(const LOGINPARAM_* src)
		: BASE<LOGINPARAM_>()
		, userOID(src->userOID)
		, groupOID(src->groupOID)
		, account(src->account)
		, password(src->password)
		, clientIP(src->clientIP)
		, sessionID(src->sessionID)
		, fromAgent(src->fromAgent)
		, useSSO(src->useSSO)
		, useMultiLogin(src->useMultiLogin)
		, autoLogin(src->autoLogin)
		, windowLogin(src->windowLogin)
		, domain(src->domain)
		, clientZoneInfo(src->clientZoneInfo)
		, networkMode(src->networkMode)
		, systemId(src->systemId)
		, macAddress(src->macAddress)
		, osInfo(src->osInfo)
		, deviceUID(src->deviceUID)
		, deviceName(src->deviceName)
		, masterKey(src->masterKey)
		, checkDeviceCertification(src->checkDeviceCertification)
		, deviceAppkey(src->deviceAppkey)
	{
	}
	LOGINPARAM_& operator=(const LOGINPARAM_& src)
	{
		if (this != &src) {
			userOID = src.userOID;
			groupOID = src.groupOID;
			account = src.account;
			password = src.password;
			clientIP = src.clientIP;
			sessionID = src.sessionID;
			fromAgent = src.fromAgent;
			useSSO = src.useSSO;
			useMultiLogin = src.useMultiLogin;
			autoLogin = src.autoLogin;
			windowLogin = src.windowLogin;
			domain = src.domain;
			clientZoneInfo = src.clientZoneInfo;
			networkMode = src.networkMode;
			systemId = src.systemId;
			macAddress = src.macAddress;
			osInfo = src.osInfo;
			deviceUID = src.deviceUID;
			deviceName = src.deviceName;
			masterKey = src.masterKey;
			checkDeviceCertification = src.checkDeviceCertification;
			deviceAppkey = src.deviceAppkey;
		}
		return *this;
	}
};

struct XMLRPCDOCTYPE_ : BASE<XMLRPCDOCTYPE_> {
	const wchar_t* docTypeOID;
	bool flagRequiredTag;
	const wchar_t* maintainDurationList;
	int maintainDuration;
	XMLRPCDOCTYPE_()
		: BASE<XMLRPCDOCTYPE_>()
		, docTypeOID(0)
		, flagRequiredTag(0)
		, maintainDurationList(0)
		, maintainDuration(0)
	{
	}
	XMLRPCDOCTYPE_(const XMLRPCDOCTYPE_& src)
		: BASE<XMLRPCDOCTYPE_>()
		, docTypeOID(src.docTypeOID)
		, flagRequiredTag(src.flagRequiredTag)
		, maintainDurationList(src.maintainDurationList)
		, maintainDuration(src.maintainDuration)
	{
	}
	XMLRPCDOCTYPE_(const XMLRPCDOCTYPE_* src)
		: BASE<XMLRPCDOCTYPE_>()
		, docTypeOID(src->docTypeOID)
		, flagRequiredTag(src->flagRequiredTag)
		, maintainDurationList(src->maintainDurationList)
		, maintainDuration(src->maintainDuration)
	{
	}
	XMLRPCDOCTYPE_& operator=(const XMLRPCDOCTYPE_& src)
	{
		if (this != &src) {
			docTypeOID = src.docTypeOID;
			flagRequiredTag = src.flagRequiredTag;
			maintainDurationList = src.maintainDurationList;
			maintainDuration = src.maintainDuration;
		}
		return *this;
	}
};

struct NETWORKDRIVERESPONSE_ : BASE<NETWORKDRIVERESPONSE_> {
	const wchar_t* fileOID;
	const wchar_t* token;
	const char* contents;
	int contentsCNT;
	int length;
	DRIVEOBJECT_* driveObject;
	NETWORKDRIVERESPONSE_()
		: BASE<NETWORKDRIVERESPONSE_>()
		, fileOID(0)
		, token(0)
		, contents(0)
		, contentsCNT(0)
		, length(0)
		, driveObject(0)
	{
	}
	NETWORKDRIVERESPONSE_(const NETWORKDRIVERESPONSE_& src)
		: BASE<NETWORKDRIVERESPONSE_>()
		, fileOID(src.fileOID)
		, token(src.token)
		, contents(src.contents)
		, contentsCNT(src.contentsCNT)
		, length(src.length)
		, driveObject(src.driveObject)
	{
	}
	NETWORKDRIVERESPONSE_(const NETWORKDRIVERESPONSE_* src)
		: BASE<NETWORKDRIVERESPONSE_>()
		, fileOID(src->fileOID)
		, token(src->token)
		, contents(src->contents)
		, contentsCNT(src->contentsCNT)
		, length(src->length)
		, driveObject(src->driveObject)
	{
	}
	NETWORKDRIVERESPONSE_& operator=(const NETWORKDRIVERESPONSE_& src)
	{
		if (this != &src) {
			fileOID = src.fileOID;
			token = src.token;
			contents = src.contents;
			contentsCNT = src.contentsCNT;
			length = src.length;
			driveObject = src.driveObject;
		}
		return *this;
	}
};

struct XMLRPCCONFIGURATION_ : BASE<XMLRPCCONFIGURATION_> {
	const wchar_t** configurationKeys;
	int configurationKeysCNT;
	const wchar_t** configurationDefaults;
	int configurationDefaultsCNT;
	XMLRPCCONFIGURATION_()
		: BASE<XMLRPCCONFIGURATION_>()
		, configurationKeys(0)
		, configurationKeysCNT(0)
		, configurationDefaults(0)
		, configurationDefaultsCNT(0)
	{
	}
	XMLRPCCONFIGURATION_(const XMLRPCCONFIGURATION_& src)
		: BASE<XMLRPCCONFIGURATION_>()
		, configurationKeys(src.configurationKeys)
		, configurationKeysCNT(src.configurationKeysCNT)
		, configurationDefaults(src.configurationDefaults)
		, configurationDefaultsCNT(src.configurationDefaultsCNT)
	{
	}
	XMLRPCCONFIGURATION_(const XMLRPCCONFIGURATION_* src)
		: BASE<XMLRPCCONFIGURATION_>()
		, configurationKeys(src->configurationKeys)
		, configurationKeysCNT(src->configurationKeysCNT)
		, configurationDefaults(src->configurationDefaults)
		, configurationDefaultsCNT(src->configurationDefaultsCNT)
	{
	}
	XMLRPCCONFIGURATION_& operator=(const XMLRPCCONFIGURATION_& src)
	{
		if (this != &src) {
			configurationKeys = src.configurationKeys;
			configurationKeysCNT = src.configurationKeysCNT;
			configurationDefaults = src.configurationDefaults;
			configurationDefaultsCNT = src.configurationDefaultsCNT;
		}
		return *this;
	}
};

struct XPROCESSRULE_ : BASE<XPROCESSRULE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* policyOID;
	const wchar_t* ruleType;
	const wchar_t* sortOrder;
	const wchar_t* applicationName;
	const wchar_t* processName;
	const wchar_t* processInfo;
	bool flagProcessName;
	bool flagProcessInfo;
	bool flagSaveLog;
	XPROCESSRULE_()
		: BASE<XPROCESSRULE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, policyOID(0)
		, ruleType(0)
		, sortOrder(0)
		, applicationName(0)
		, processName(0)
		, processInfo(0)
		, flagProcessName(0)
		, flagProcessInfo(0)
		, flagSaveLog(0)
	{
	}
	XPROCESSRULE_(const XPROCESSRULE_& src)
		: BASE<XPROCESSRULE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, policyOID(src.policyOID)
		, ruleType(src.ruleType)
		, sortOrder(src.sortOrder)
		, applicationName(src.applicationName)
		, processName(src.processName)
		, processInfo(src.processInfo)
		, flagProcessName(src.flagProcessName)
		, flagProcessInfo(src.flagProcessInfo)
		, flagSaveLog(src.flagSaveLog)
	{
	}
	XPROCESSRULE_(const XPROCESSRULE_* src)
		: BASE<XPROCESSRULE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, policyOID(src->policyOID)
		, ruleType(src->ruleType)
		, sortOrder(src->sortOrder)
		, applicationName(src->applicationName)
		, processName(src->processName)
		, processInfo(src->processInfo)
		, flagProcessName(src->flagProcessName)
		, flagProcessInfo(src->flagProcessInfo)
		, flagSaveLog(src->flagSaveLog)
	{
	}
	XPROCESSRULE_& operator=(const XPROCESSRULE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			policyOID = src.policyOID;
			ruleType = src.ruleType;
			sortOrder = src.sortOrder;
			applicationName = src.applicationName;
			processName = src.processName;
			processInfo = src.processInfo;
			flagProcessName = src.flagProcessName;
			flagProcessInfo = src.flagProcessInfo;
			flagSaveLog = src.flagSaveLog;
		}
		return *this;
	}
};

struct DOWNLOADDRIVEOBJECTLIST_ : BASE<DOWNLOADDRIVEOBJECTLIST_> {
	int returnCode;
	int tableCount;
	int totalCount;
	const wchar_t* totalSize;
	DOWNLOADDRIVEOBJECT_** driveObjects;
	int driveObjectsCNT;
	DOWNLOADDRIVEOBJECTLIST_()
		: BASE<DOWNLOADDRIVEOBJECTLIST_>()
		, returnCode(0)
		, tableCount(0)
		, totalCount(0)
		, totalSize(0)
		, driveObjects(0)
		, driveObjectsCNT(0)
	{
	}
	DOWNLOADDRIVEOBJECTLIST_(const DOWNLOADDRIVEOBJECTLIST_& src)
		: BASE<DOWNLOADDRIVEOBJECTLIST_>()
		, returnCode(src.returnCode)
		, tableCount(src.tableCount)
		, totalCount(src.totalCount)
		, totalSize(src.totalSize)
		, driveObjects(src.driveObjects)
		, driveObjectsCNT(src.driveObjectsCNT)
	{
	}
	DOWNLOADDRIVEOBJECTLIST_(const DOWNLOADDRIVEOBJECTLIST_* src)
		: BASE<DOWNLOADDRIVEOBJECTLIST_>()
		, returnCode(src->returnCode)
		, tableCount(src->tableCount)
		, totalCount(src->totalCount)
		, totalSize(src->totalSize)
		, driveObjects(src->driveObjects)
		, driveObjectsCNT(src->driveObjectsCNT)
	{
	}
	DOWNLOADDRIVEOBJECTLIST_& operator=(const DOWNLOADDRIVEOBJECTLIST_& src)
	{
		if (this != &src) {
			returnCode = src.returnCode;
			tableCount = src.tableCount;
			totalCount = src.totalCount;
			totalSize = src.totalSize;
			driveObjects = src.driveObjects;
			driveObjectsCNT = src.driveObjectsCNT;
		}
		return *this;
	}
};

struct RPCFILE_ : BASE<RPCFILE_> {
	const wchar_t* targetOID;
	const wchar_t* fileName;
	double fileSize;
	const wchar_t* storageFileID;
	const wchar_t* localCreatedAt;
	const wchar_t* localLastModifiedAt;
	RPCFILE_()
		: BASE<RPCFILE_>()
		, targetOID(0)
		, fileName(0)
		, fileSize(0)
		, storageFileID(0)
		, localCreatedAt(0)
		, localLastModifiedAt(0)
	{
	}
	RPCFILE_(const RPCFILE_& src)
		: BASE<RPCFILE_>()
		, targetOID(src.targetOID)
		, fileName(src.fileName)
		, fileSize(src.fileSize)
		, storageFileID(src.storageFileID)
		, localCreatedAt(src.localCreatedAt)
		, localLastModifiedAt(src.localLastModifiedAt)
	{
	}
	RPCFILE_(const RPCFILE_* src)
		: BASE<RPCFILE_>()
		, targetOID(src->targetOID)
		, fileName(src->fileName)
		, fileSize(src->fileSize)
		, storageFileID(src->storageFileID)
		, localCreatedAt(src->localCreatedAt)
		, localLastModifiedAt(src->localLastModifiedAt)
	{
	}
	RPCFILE_& operator=(const RPCFILE_& src)
	{
		if (this != &src) {
			targetOID = src.targetOID;
			fileName = src.fileName;
			fileSize = src.fileSize;
			storageFileID = src.storageFileID;
			localCreatedAt = src.localCreatedAt;
			localLastModifiedAt = src.localLastModifiedAt;
		}
		return *this;
	}
};

struct DRIVEOBJECTLIST_ : BASE<DRIVEOBJECTLIST_> {
	int returnCode;
	int tableCount;
	int totalCount;
	const wchar_t* totalSize;
	DRIVEOBJECT_** driveObjects;
	int driveObjectsCNT;
	int folderTableCount;
	DRIVEOBJECTLIST_()
		: BASE<DRIVEOBJECTLIST_>()
		, returnCode(0)
		, tableCount(0)
		, totalCount(0)
		, totalSize(0)
		, driveObjects(0)
		, driveObjectsCNT(0)
		, folderTableCount(0)
	{
	}
	DRIVEOBJECTLIST_(const DRIVEOBJECTLIST_& src)
		: BASE<DRIVEOBJECTLIST_>()
		, returnCode(src.returnCode)
		, tableCount(src.tableCount)
		, totalCount(src.totalCount)
		, totalSize(src.totalSize)
		, driveObjects(src.driveObjects)
		, driveObjectsCNT(src.driveObjectsCNT)
		, folderTableCount(src.folderTableCount)
	{
	}
	DRIVEOBJECTLIST_(const DRIVEOBJECTLIST_* src)
		: BASE<DRIVEOBJECTLIST_>()
		, returnCode(src->returnCode)
		, tableCount(src->tableCount)
		, totalCount(src->totalCount)
		, totalSize(src->totalSize)
		, driveObjects(src->driveObjects)
		, driveObjectsCNT(src->driveObjectsCNT)
		, folderTableCount(src->folderTableCount)
	{
	}
	DRIVEOBJECTLIST_& operator=(const DRIVEOBJECTLIST_& src)
	{
		if (this != &src) {
			returnCode = src.returnCode;
			tableCount = src.tableCount;
			totalCount = src.totalCount;
			totalSize = src.totalSize;
			driveObjects = src.driveObjects;
			driveObjectsCNT = src.driveObjectsCNT;
			folderTableCount = src.folderTableCount;
		}
		return *this;
	}
};

struct XSYNCUSERINFO_ : BASE<XSYNCUSERINFO_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* userOID;
	const wchar_t* name;
	const wchar_t* account;
	const wchar_t* groupFullPathName;
	const wchar_t* groupCode;
	const wchar_t* groupPosition;
	const wchar_t* groupOID;
	const wchar_t* email;
	const wchar_t* userType;
	XSYNCUSERINFO_()
		: BASE<XSYNCUSERINFO_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, userOID(0)
		, name(0)
		, account(0)
		, groupFullPathName(0)
		, groupCode(0)
		, groupPosition(0)
		, groupOID(0)
		, email(0)
		, userType(0)
	{
	}
	XSYNCUSERINFO_(const XSYNCUSERINFO_& src)
		: BASE<XSYNCUSERINFO_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, userOID(src.userOID)
		, name(src.name)
		, account(src.account)
		, groupFullPathName(src.groupFullPathName)
		, groupCode(src.groupCode)
		, groupPosition(src.groupPosition)
		, groupOID(src.groupOID)
		, email(src.email)
		, userType(src.userType)
	{
	}
	XSYNCUSERINFO_(const XSYNCUSERINFO_* src)
		: BASE<XSYNCUSERINFO_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, userOID(src->userOID)
		, name(src->name)
		, account(src->account)
		, groupFullPathName(src->groupFullPathName)
		, groupCode(src->groupCode)
		, groupPosition(src->groupPosition)
		, groupOID(src->groupOID)
		, email(src->email)
		, userType(src->userType)
	{
	}
	XSYNCUSERINFO_& operator=(const XSYNCUSERINFO_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			userOID = src.userOID;
			name = src.name;
			account = src.account;
			groupFullPathName = src.groupFullPathName;
			groupCode = src.groupCode;
			groupPosition = src.groupPosition;
			groupOID = src.groupOID;
			email = src.email;
			userType = src.userType;
		}
		return *this;
	}
};

struct ASYNCOPERATIONSTATUS_ : BASE<ASYNCOPERATIONSTATUS_> {
	const wchar_t* asyncOperationOID;
	int status;
	int progressRatio;
	const wchar_t* errorCode;
	const wchar_t* errorMsg;
	ASYNCOPERATIONSTATUS_()
		: BASE<ASYNCOPERATIONSTATUS_>()
		, asyncOperationOID(0)
		, status(0)
		, progressRatio(0)
		, errorCode(0)
		, errorMsg(0)
	{
	}
	ASYNCOPERATIONSTATUS_(const ASYNCOPERATIONSTATUS_& src)
		: BASE<ASYNCOPERATIONSTATUS_>()
		, asyncOperationOID(src.asyncOperationOID)
		, status(src.status)
		, progressRatio(src.progressRatio)
		, errorCode(src.errorCode)
		, errorMsg(src.errorMsg)
	{
	}
	ASYNCOPERATIONSTATUS_(const ASYNCOPERATIONSTATUS_* src)
		: BASE<ASYNCOPERATIONSTATUS_>()
		, asyncOperationOID(src->asyncOperationOID)
		, status(src->status)
		, progressRatio(src->progressRatio)
		, errorCode(src->errorCode)
		, errorMsg(src->errorMsg)
	{
	}
	ASYNCOPERATIONSTATUS_& operator=(const ASYNCOPERATIONSTATUS_& src)
	{
		if (this != &src) {
			asyncOperationOID = src.asyncOperationOID;
			status = src.status;
			progressRatio = src.progressRatio;
			errorCode = src.errorCode;
			errorMsg = src.errorMsg;
		}
		return *this;
	}
};

struct LICENSESTATUS_ : BASE<LICENSESTATUS_> {
	int contractedCount;
	int appliedSystemCount;
	int usedCount;
	Long expiredAt;
	const wchar_t* licenseType;
	const wchar_t* notificationType;
	int excessCount;
	const wchar_t* technicalSupporter;
	LICENSESTATUS_()
		: BASE<LICENSESTATUS_>()
		, contractedCount(0)
		, appliedSystemCount(0)
		, usedCount(0)
		, expiredAt(0)
		, licenseType(0)
		, notificationType(0)
		, excessCount(0)
		, technicalSupporter(0)
	{
	}
	LICENSESTATUS_(const LICENSESTATUS_& src)
		: BASE<LICENSESTATUS_>()
		, contractedCount(src.contractedCount)
		, appliedSystemCount(src.appliedSystemCount)
		, usedCount(src.usedCount)
		, expiredAt(src.expiredAt)
		, licenseType(src.licenseType)
		, notificationType(src.notificationType)
		, excessCount(src.excessCount)
		, technicalSupporter(src.technicalSupporter)
	{
	}
	LICENSESTATUS_(const LICENSESTATUS_* src)
		: BASE<LICENSESTATUS_>()
		, contractedCount(src->contractedCount)
		, appliedSystemCount(src->appliedSystemCount)
		, usedCount(src->usedCount)
		, expiredAt(src->expiredAt)
		, licenseType(src->licenseType)
		, notificationType(src->notificationType)
		, excessCount(src->excessCount)
		, technicalSupporter(src->technicalSupporter)
	{
	}
	LICENSESTATUS_& operator=(const LICENSESTATUS_& src)
	{
		if (this != &src) {
			contractedCount = src.contractedCount;
			appliedSystemCount = src.appliedSystemCount;
			usedCount = src.usedCount;
			expiredAt = src.expiredAt;
			licenseType = src.licenseType;
			notificationType = src.notificationType;
			excessCount = src.excessCount;
			technicalSupporter = src.technicalSupporter;
		}
		return *this;
	}
};

struct XPRINTRULE_ : BASE<XPRINTRULE_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	const wchar_t* policyOID;
	const wchar_t* ruleType;
	const wchar_t* sortOrder;
	const wchar_t* applicationName;
	bool flagAll;
	const wchar_t* processName;
	const wchar_t* processInfo;
	bool flagProcessName;
	bool flagProcessInfo;
	bool flagAllow;
	bool flagWatermark;
	bool flagSavePrintContent;
	const wchar_t* saveOption;
	bool flagSaveLog;
	XPRINTRULE_()
		: BASE<XPRINTRULE_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, policyOID(0)
		, ruleType(0)
		, sortOrder(0)
		, applicationName(0)
		, flagAll(0)
		, processName(0)
		, processInfo(0)
		, flagProcessName(0)
		, flagProcessInfo(0)
		, flagAllow(0)
		, flagWatermark(0)
		, flagSavePrintContent(0)
		, saveOption(0)
		, flagSaveLog(0)
	{
	}
	XPRINTRULE_(const XPRINTRULE_& src)
		: BASE<XPRINTRULE_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, policyOID(src.policyOID)
		, ruleType(src.ruleType)
		, sortOrder(src.sortOrder)
		, applicationName(src.applicationName)
		, flagAll(src.flagAll)
		, processName(src.processName)
		, processInfo(src.processInfo)
		, flagProcessName(src.flagProcessName)
		, flagProcessInfo(src.flagProcessInfo)
		, flagAllow(src.flagAllow)
		, flagWatermark(src.flagWatermark)
		, flagSavePrintContent(src.flagSavePrintContent)
		, saveOption(src.saveOption)
		, flagSaveLog(src.flagSaveLog)
	{
	}
	XPRINTRULE_(const XPRINTRULE_* src)
		: BASE<XPRINTRULE_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, policyOID(src->policyOID)
		, ruleType(src->ruleType)
		, sortOrder(src->sortOrder)
		, applicationName(src->applicationName)
		, flagAll(src->flagAll)
		, processName(src->processName)
		, processInfo(src->processInfo)
		, flagProcessName(src->flagProcessName)
		, flagProcessInfo(src->flagProcessInfo)
		, flagAllow(src->flagAllow)
		, flagWatermark(src->flagWatermark)
		, flagSavePrintContent(src->flagSavePrintContent)
		, saveOption(src->saveOption)
		, flagSaveLog(src->flagSaveLog)
	{
	}
	XPRINTRULE_& operator=(const XPRINTRULE_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			policyOID = src.policyOID;
			ruleType = src.ruleType;
			sortOrder = src.sortOrder;
			applicationName = src.applicationName;
			flagAll = src.flagAll;
			processName = src.processName;
			processInfo = src.processInfo;
			flagProcessName = src.flagProcessName;
			flagProcessInfo = src.flagProcessInfo;
			flagAllow = src.flagAllow;
			flagWatermark = src.flagWatermark;
			flagSavePrintContent = src.flagSavePrintContent;
			saveOption = src.saveOption;
			flagSaveLog = src.flagSaveLog;
		}
		return *this;
	}
};

struct XMLRPCWATERMARKCONFIG_ : BASE<XMLRPCWATERMARKCONFIG_> {
	int watermarkUseInfo;
	int watermarkPosition;
	const wchar_t* font;
	int fontSize;
	const wchar_t* companyName;
	const wchar_t* phoneNumber;
	const wchar_t* faxNumber;
	const wchar_t* homepageAddress;
	const wchar_t* userName;
	const wchar_t* userPosition;
	const wchar_t* userEmail;
	const wchar_t* userGroupName;
	const wchar_t* imageFileStorageID;
	const wchar_t* imageFileUrl;
	XMLRPCWATERMARKCONFIG_()
		: BASE<XMLRPCWATERMARKCONFIG_>()
		, watermarkUseInfo(0)
		, watermarkPosition(0)
		, font(0)
		, fontSize(0)
		, companyName(0)
		, phoneNumber(0)
		, faxNumber(0)
		, homepageAddress(0)
		, userName(0)
		, userPosition(0)
		, userEmail(0)
		, userGroupName(0)
		, imageFileStorageID(0)
		, imageFileUrl(0)
	{
	}
	XMLRPCWATERMARKCONFIG_(const XMLRPCWATERMARKCONFIG_& src)
		: BASE<XMLRPCWATERMARKCONFIG_>()
		, watermarkUseInfo(src.watermarkUseInfo)
		, watermarkPosition(src.watermarkPosition)
		, font(src.font)
		, fontSize(src.fontSize)
		, companyName(src.companyName)
		, phoneNumber(src.phoneNumber)
		, faxNumber(src.faxNumber)
		, homepageAddress(src.homepageAddress)
		, userName(src.userName)
		, userPosition(src.userPosition)
		, userEmail(src.userEmail)
		, userGroupName(src.userGroupName)
		, imageFileStorageID(src.imageFileStorageID)
		, imageFileUrl(src.imageFileUrl)
	{
	}
	XMLRPCWATERMARKCONFIG_(const XMLRPCWATERMARKCONFIG_* src)
		: BASE<XMLRPCWATERMARKCONFIG_>()
		, watermarkUseInfo(src->watermarkUseInfo)
		, watermarkPosition(src->watermarkPosition)
		, font(src->font)
		, fontSize(src->fontSize)
		, companyName(src->companyName)
		, phoneNumber(src->phoneNumber)
		, faxNumber(src->faxNumber)
		, homepageAddress(src->homepageAddress)
		, userName(src->userName)
		, userPosition(src->userPosition)
		, userEmail(src->userEmail)
		, userGroupName(src->userGroupName)
		, imageFileStorageID(src->imageFileStorageID)
		, imageFileUrl(src->imageFileUrl)
	{
	}
	XMLRPCWATERMARKCONFIG_& operator=(const XMLRPCWATERMARKCONFIG_& src)
	{
		if (this != &src) {
			watermarkUseInfo = src.watermarkUseInfo;
			watermarkPosition = src.watermarkPosition;
			font = src.font;
			fontSize = src.fontSize;
			companyName = src.companyName;
			phoneNumber = src.phoneNumber;
			faxNumber = src.faxNumber;
			homepageAddress = src.homepageAddress;
			userName = src.userName;
			userPosition = src.userPosition;
			userEmail = src.userEmail;
			userGroupName = src.userGroupName;
			imageFileStorageID = src.imageFileStorageID;
			imageFileUrl = src.imageFileUrl;
		}
		return *this;
	}
};

struct XUSERPOLICY_ : BASE<XUSERPOLICY_> {
	const wchar_t* oid;
	const wchar_t** opCodes;
	int opCodesCNT;
	int searchObjectTotalCount;
	XSECURITYPOLICY_* securityPolicy;
	const wchar_t* userName;
	const wchar_t* securityDDVolLetter;
	const wchar_t* securityDDVolName;
	const wchar_t* ecmDDVolLetter;
	const wchar_t* ecmDDVolName;
	int documentIncludeType;
	int fileViewTypeInGeneralExplorer;
	int fileViewTypeInOnlyExplorer;
	const wchar_t* userOID;
	const wchar_t* securityPolicyOID;
	int pageRowNum;
	int explorerRowNum;
	int secuDDSizeMB;
	Long applPolicyModifiedAt;
	bool flagDefaultSecurity;
	bool flagSecuDDSize;
	const wchar_t* availableNetworkMode;
	bool flagRootDeptDocManager;
	const wchar_t* groupOID;
	const wchar_t* groupName;
	const wchar_t* groupFullPathName;
	const wchar_t* availableInternalIPs;
	const wchar_t* securityPolicyName;
	XUSERPOLICY_()
		: BASE<XUSERPOLICY_>()
		, oid(0)
		, opCodes(0)
		, opCodesCNT(0)
		, searchObjectTotalCount(0)
		, securityPolicy(0)
		, userName(0)
		, securityDDVolLetter(0)
		, securityDDVolName(0)
		, ecmDDVolLetter(0)
		, ecmDDVolName(0)
		, documentIncludeType(0)
		, fileViewTypeInGeneralExplorer(0)
		, fileViewTypeInOnlyExplorer(0)
		, userOID(0)
		, securityPolicyOID(0)
		, pageRowNum(0)
		, explorerRowNum(0)
		, secuDDSizeMB(0)
		, applPolicyModifiedAt(0)
		, flagDefaultSecurity(0)
		, flagSecuDDSize(0)
		, availableNetworkMode(0)
		, flagRootDeptDocManager(0)
		, groupOID(0)
		, groupName(0)
		, groupFullPathName(0)
		, availableInternalIPs(0)
		, securityPolicyName(0)
	{
	}
	XUSERPOLICY_(const XUSERPOLICY_& src)
		: BASE<XUSERPOLICY_>()
		, oid(src.oid)
		, opCodes(src.opCodes)
		, opCodesCNT(src.opCodesCNT)
		, searchObjectTotalCount(src.searchObjectTotalCount)
		, securityPolicy(src.securityPolicy)
		, userName(src.userName)
		, securityDDVolLetter(src.securityDDVolLetter)
		, securityDDVolName(src.securityDDVolName)
		, ecmDDVolLetter(src.ecmDDVolLetter)
		, ecmDDVolName(src.ecmDDVolName)
		, documentIncludeType(src.documentIncludeType)
		, fileViewTypeInGeneralExplorer(src.fileViewTypeInGeneralExplorer)
		, fileViewTypeInOnlyExplorer(src.fileViewTypeInOnlyExplorer)
		, userOID(src.userOID)
		, securityPolicyOID(src.securityPolicyOID)
		, pageRowNum(src.pageRowNum)
		, explorerRowNum(src.explorerRowNum)
		, secuDDSizeMB(src.secuDDSizeMB)
		, applPolicyModifiedAt(src.applPolicyModifiedAt)
		, flagDefaultSecurity(src.flagDefaultSecurity)
		, flagSecuDDSize(src.flagSecuDDSize)
		, availableNetworkMode(src.availableNetworkMode)
		, flagRootDeptDocManager(src.flagRootDeptDocManager)
		, groupOID(src.groupOID)
		, groupName(src.groupName)
		, groupFullPathName(src.groupFullPathName)
		, availableInternalIPs(src.availableInternalIPs)
		, securityPolicyName(src.securityPolicyName)
	{
	}
	XUSERPOLICY_(const XUSERPOLICY_* src)
		: BASE<XUSERPOLICY_>()
		, oid(src->oid)
		, opCodes(src->opCodes)
		, opCodesCNT(src->opCodesCNT)
		, searchObjectTotalCount(src->searchObjectTotalCount)
		, securityPolicy(src->securityPolicy)
		, userName(src->userName)
		, securityDDVolLetter(src->securityDDVolLetter)
		, securityDDVolName(src->securityDDVolName)
		, ecmDDVolLetter(src->ecmDDVolLetter)
		, ecmDDVolName(src->ecmDDVolName)
		, documentIncludeType(src->documentIncludeType)
		, fileViewTypeInGeneralExplorer(src->fileViewTypeInGeneralExplorer)
		, fileViewTypeInOnlyExplorer(src->fileViewTypeInOnlyExplorer)
		, userOID(src->userOID)
		, securityPolicyOID(src->securityPolicyOID)
		, pageRowNum(src->pageRowNum)
		, explorerRowNum(src->explorerRowNum)
		, secuDDSizeMB(src->secuDDSizeMB)
		, applPolicyModifiedAt(src->applPolicyModifiedAt)
		, flagDefaultSecurity(src->flagDefaultSecurity)
		, flagSecuDDSize(src->flagSecuDDSize)
		, availableNetworkMode(src->availableNetworkMode)
		, flagRootDeptDocManager(src->flagRootDeptDocManager)
		, groupOID(src->groupOID)
		, groupName(src->groupName)
		, groupFullPathName(src->groupFullPathName)
		, availableInternalIPs(src->availableInternalIPs)
		, securityPolicyName(src->securityPolicyName)
	{
	}
	XUSERPOLICY_& operator=(const XUSERPOLICY_& src)
	{
		if (this != &src) {
			oid = src.oid;
			opCodes = src.opCodes;
			opCodesCNT = src.opCodesCNT;
			searchObjectTotalCount = src.searchObjectTotalCount;
			securityPolicy = src.securityPolicy;
			userName = src.userName;
			securityDDVolLetter = src.securityDDVolLetter;
			securityDDVolName = src.securityDDVolName;
			ecmDDVolLetter = src.ecmDDVolLetter;
			ecmDDVolName = src.ecmDDVolName;
			documentIncludeType = src.documentIncludeType;
			fileViewTypeInGeneralExplorer = src.fileViewTypeInGeneralExplorer;
			fileViewTypeInOnlyExplorer = src.fileViewTypeInOnlyExplorer;
			userOID = src.userOID;
			securityPolicyOID = src.securityPolicyOID;
			pageRowNum = src.pageRowNum;
			explorerRowNum = src.explorerRowNum;
			secuDDSizeMB = src.secuDDSizeMB;
			applPolicyModifiedAt = src.applPolicyModifiedAt;
			flagDefaultSecurity = src.flagDefaultSecurity;
			flagSecuDDSize = src.flagSecuDDSize;
			availableNetworkMode = src.availableNetworkMode;
			flagRootDeptDocManager = src.flagRootDeptDocManager;
			groupOID = src.groupOID;
			groupName = src.groupName;
			groupFullPathName = src.groupFullPathName;
			availableInternalIPs = src.availableInternalIPs;
			securityPolicyName = src.securityPolicyName;
		}
		return *this;
	}
};

struct NETWORKCONTROLEXCLUSIVEPOLICY_ : BASE<NETWORKCONTROLEXCLUSIVEPOLICY_> {
	const wchar_t* exclusiveIPs;
	XAPPLICATIONEXCLUSIVENWPOLICY_** applicationExclusiveNWPolicies;
	int applicationExclusiveNWPoliciesCNT;
	XAPPLICATIONEXCLUSIVENWPOLICY_** applicationInternalBlockPolicies;
	int applicationInternalBlockPoliciesCNT;
	NETWORKCONTROLEXCLUSIVEPOLICY_()
		: BASE<NETWORKCONTROLEXCLUSIVEPOLICY_>()
		, exclusiveIPs(0)
		, applicationExclusiveNWPolicies(0)
		, applicationExclusiveNWPoliciesCNT(0)
		, applicationInternalBlockPolicies(0)
		, applicationInternalBlockPoliciesCNT(0)
	{
	}
	NETWORKCONTROLEXCLUSIVEPOLICY_(const NETWORKCONTROLEXCLUSIVEPOLICY_& src)
		: BASE<NETWORKCONTROLEXCLUSIVEPOLICY_>()
		, exclusiveIPs(src.exclusiveIPs)
		, applicationExclusiveNWPolicies(src.applicationExclusiveNWPolicies)
		, applicationExclusiveNWPoliciesCNT(src.applicationExclusiveNWPoliciesCNT)
		, applicationInternalBlockPolicies(src.applicationInternalBlockPolicies)
		, applicationInternalBlockPoliciesCNT(src.applicationInternalBlockPoliciesCNT)
	{
	}
	NETWORKCONTROLEXCLUSIVEPOLICY_(const NETWORKCONTROLEXCLUSIVEPOLICY_* src)
		: BASE<NETWORKCONTROLEXCLUSIVEPOLICY_>()
		, exclusiveIPs(src->exclusiveIPs)
		, applicationExclusiveNWPolicies(src->applicationExclusiveNWPolicies)
		, applicationExclusiveNWPoliciesCNT(src->applicationExclusiveNWPoliciesCNT)
		, applicationInternalBlockPolicies(src->applicationInternalBlockPolicies)
		, applicationInternalBlockPoliciesCNT(src->applicationInternalBlockPoliciesCNT)
	{
	}
	NETWORKCONTROLEXCLUSIVEPOLICY_& operator=(const NETWORKCONTROLEXCLUSIVEPOLICY_& src)
	{
		if (this != &src) {
			exclusiveIPs = src.exclusiveIPs;
			applicationExclusiveNWPolicies = src.applicationExclusiveNWPolicies;
			applicationExclusiveNWPoliciesCNT = src.applicationExclusiveNWPoliciesCNT;
			applicationInternalBlockPolicies = src.applicationInternalBlockPolicies;
			applicationInternalBlockPoliciesCNT = src.applicationInternalBlockPoliciesCNT;
		}
		return *this;
	}
};

struct DOWNLOADDRIVEOBJECT_ : BASE<DOWNLOADDRIVEOBJECT_> {
	const wchar_t* oid;
	const wchar_t* parentOID;
	int lastModifiedAtHigh;
	int lastModifiedAtLow;
	int permissionProperties;
	int aclProperties;
	const wchar_t* objName;
	const wchar_t* creatorInfo;
	int objType;
	int fileSizeHigh;
	int fileSizeLow;
	int localLastModifiedAtHigh;
	int localLastModifiedAtLow;
	int localCreatedAtHigh;
	int localCreatedAtLow;
	const wchar_t* storageFileID;
	DOWNLOADDRIVEOBJECT_** childObjects;
	int childObjectsCNT;
	DOWNLOADDRIVEOBJECT_()
		: BASE<DOWNLOADDRIVEOBJECT_>()
		, oid(0)
		, parentOID(0)
		, lastModifiedAtHigh(0)
		, lastModifiedAtLow(0)
		, permissionProperties(0)
		, aclProperties(0)
		, objName(0)
		, creatorInfo(0)
		, objType(0)
		, fileSizeHigh(0)
		, fileSizeLow(0)
		, localLastModifiedAtHigh(0)
		, localLastModifiedAtLow(0)
		, localCreatedAtHigh(0)
		, localCreatedAtLow(0)
		, storageFileID(0)
		, childObjects(0)
		, childObjectsCNT(0)
	{
	}
	DOWNLOADDRIVEOBJECT_(const DOWNLOADDRIVEOBJECT_& src)
		: BASE<DOWNLOADDRIVEOBJECT_>()
		, oid(src.oid)
		, parentOID(src.parentOID)
		, lastModifiedAtHigh(src.lastModifiedAtHigh)
		, lastModifiedAtLow(src.lastModifiedAtLow)
		, permissionProperties(src.permissionProperties)
		, aclProperties(src.aclProperties)
		, objName(src.objName)
		, creatorInfo(src.creatorInfo)
		, objType(src.objType)
		, fileSizeHigh(src.fileSizeHigh)
		, fileSizeLow(src.fileSizeLow)
		, localLastModifiedAtHigh(src.localLastModifiedAtHigh)
		, localLastModifiedAtLow(src.localLastModifiedAtLow)
		, localCreatedAtHigh(src.localCreatedAtHigh)
		, localCreatedAtLow(src.localCreatedAtLow)
		, storageFileID(src.storageFileID)
		, childObjects(src.childObjects)
		, childObjectsCNT(src.childObjectsCNT)
	{
	}
	DOWNLOADDRIVEOBJECT_(const DOWNLOADDRIVEOBJECT_* src)
		: BASE<DOWNLOADDRIVEOBJECT_>()
		, oid(src->oid)
		, parentOID(src->parentOID)
		, lastModifiedAtHigh(src->lastModifiedAtHigh)
		, lastModifiedAtLow(src->lastModifiedAtLow)
		, permissionProperties(src->permissionProperties)
		, aclProperties(src->aclProperties)
		, objName(src->objName)
		, creatorInfo(src->creatorInfo)
		, objType(src->objType)
		, fileSizeHigh(src->fileSizeHigh)
		, fileSizeLow(src->fileSizeLow)
		, localLastModifiedAtHigh(src->localLastModifiedAtHigh)
		, localLastModifiedAtLow(src->localLastModifiedAtLow)
		, localCreatedAtHigh(src->localCreatedAtHigh)
		, localCreatedAtLow(src->localCreatedAtLow)
		, storageFileID(src->storageFileID)
		, childObjects(src->childObjects)
		, childObjectsCNT(src->childObjectsCNT)
	{
	}
	DOWNLOADDRIVEOBJECT_& operator=(const DOWNLOADDRIVEOBJECT_& src)
	{
		if (this != &src) {
			oid = src.oid;
			parentOID = src.parentOID;
			lastModifiedAtHigh = src.lastModifiedAtHigh;
			lastModifiedAtLow = src.lastModifiedAtLow;
			permissionProperties = src.permissionProperties;
			aclProperties = src.aclProperties;
			objName = src.objName;
			creatorInfo = src.creatorInfo;
			objType = src.objType;
			fileSizeHigh = src.fileSizeHigh;
			fileSizeLow = src.fileSizeLow;
			localLastModifiedAtHigh = src.localLastModifiedAtHigh;
			localLastModifiedAtLow = src.localLastModifiedAtLow;
			localCreatedAtHigh = src.localCreatedAtHigh;
			localCreatedAtLow = src.localCreatedAtLow;
			storageFileID = src.storageFileID;
			childObjects = src.childObjects;
			childObjectsCNT = src.childObjectsCNT;
		}
		return *this;
	}
};

struct XACE_ : BASE<XACE_> {
	const wchar_t* objectName;
	const wchar_t* ownerFullPath;
	const wchar_t* aclOID;
	const wchar_t* targetObjectType;
	const wchar_t* aclOwnerOID;
	const wchar_t* accessGrade;
	int permission;
	bool flagUserDefined;
	const wchar_t* sortOrder;
	int weight;
	XACE_()
		: BASE<XACE_>()
		, objectName(0)
		, ownerFullPath(0)
		, aclOID(0)
		, targetObjectType(0)
		, aclOwnerOID(0)
		, accessGrade(0)
		, permission(0)
		, flagUserDefined(0)
		, sortOrder(0)
		, weight(0)
	{
	}
	XACE_(const XACE_& src)
		: BASE<XACE_>()
		, objectName(src.objectName)
		, ownerFullPath(src.ownerFullPath)
		, aclOID(src.aclOID)
		, targetObjectType(src.targetObjectType)
		, aclOwnerOID(src.aclOwnerOID)
		, accessGrade(src.accessGrade)
		, permission(src.permission)
		, flagUserDefined(src.flagUserDefined)
		, sortOrder(src.sortOrder)
		, weight(src.weight)
	{
	}
	XACE_(const XACE_* src)
		: BASE<XACE_>()
		, objectName(src->objectName)
		, ownerFullPath(src->ownerFullPath)
		, aclOID(src->aclOID)
		, targetObjectType(src->targetObjectType)
		, aclOwnerOID(src->aclOwnerOID)
		, accessGrade(src->accessGrade)
		, permission(src->permission)
		, flagUserDefined(src->flagUserDefined)
		, sortOrder(src->sortOrder)
		, weight(src->weight)
	{
	}
	XACE_& operator=(const XACE_& src)
	{
		if (this != &src) {
			objectName = src.objectName;
			ownerFullPath = src.ownerFullPath;
			aclOID = src.aclOID;
			targetObjectType = src.targetObjectType;
			aclOwnerOID = src.aclOwnerOID;
			accessGrade = src.accessGrade;
			permission = src.permission;
			flagUserDefined = src.flagUserDefined;
			sortOrder = src.sortOrder;
			weight = src.weight;
		}
		return *this;
	}
};
}; // end of namespace

typedef DestinyMsgpack_LSIF::XEXTCOLUMNVALUE_ XEXTCOLUMNVALUE_ST;
typedef DestinyMsgpack_LSIF::XSECURITYPOLICY_ XSECURITYPOLICY_ST;
typedef DestinyMsgpack_LSIF::XURLLINK_ XURLLINK_ST;
typedef DestinyMsgpack_LSIF::XFILEHISTORY_ XFILEHISTORY_ST;
typedef DestinyMsgpack_LSIF::XEXTATTRIBUTE_ XEXTATTRIBUTE_ST;
typedef DestinyMsgpack_LSIF::CLIENTZONEINFO_ CLIENTZONEINFO_ST;
typedef DestinyMsgpack_LSIF::XUSERINFO_ XUSERINFO_ST;
typedef DestinyMsgpack_LSIF::FOLDERITEM_ FOLDERITEM_ST;
typedef DestinyMsgpack_LSIF::XMLRPCSEARCHPARAMS_ XMLRPCSEARCHPARAMS_ST;
typedef DestinyMsgpack_LSIF::XCODELIST_ XCODELIST_ST;
typedef DestinyMsgpack_LSIF::XCONVERTFILE_ XCONVERTFILE_ST;
typedef DestinyMsgpack_LSIF::XEXTATTRIBUTEVALUE_ XEXTATTRIBUTEVALUE_ST;
typedef DestinyMsgpack_LSIF::XSYNCFOLDER_ XSYNCFOLDER_ST;
typedef DestinyMsgpack_LSIF::XCONFIGURATION_ XCONFIGURATION_ST;
typedef DestinyMsgpack_LSIF::XTAG_ XTAG_ST;
typedef DestinyMsgpack_LSIF::XCODEVALUE_ XCODEVALUE_ST;
typedef DestinyMsgpack_LSIF::XAPPLICATIONPOLICY_ XAPPLICATIONPOLICY_ST;
typedef DestinyMsgpack_LSIF::ECMOBJECT_ ECMOBJECT_ST;
typedef DestinyMsgpack_LSIF::XMLRPCLOGINCONTEXT_ XMLRPCLOGINCONTEXT_ST;
typedef DestinyMsgpack_LSIF::XAPPLICATIONEXCLUSIVENWPOLICY_ XAPPLICATIONEXCLUSIVENWPOLICY_ST;
typedef DestinyMsgpack_LSIF::XFILE_ XFILE_ST;
typedef DestinyMsgpack_LSIF::XSITERULE_ XSITERULE_ST;
typedef DestinyMsgpack_LSIF::XMLRPCDOCUMENT_ XMLRPCDOCUMENT_ST;
typedef DestinyMsgpack_LSIF::XNETWORKRULE_ XNETWORKRULE_ST;
typedef DestinyMsgpack_LSIF::XAMOUNTPOLICY_ XAMOUNTPOLICY_ST;
typedef DestinyMsgpack_LSIF::XGROUP_ XGROUP_ST;
typedef DestinyMsgpack_LSIF::XMEDIARULE_ XMEDIARULE_ST;
typedef DestinyMsgpack_LSIF::RPCDOCUMENT_ RPCDOCUMENT_ST;
typedef DestinyMsgpack_LSIF::XDLPPOLICY_ XDLPPOLICY_ST;
typedef DestinyMsgpack_LSIF::RETURNINTVO_ RETURNINTVO_ST;
typedef DestinyMsgpack_LSIF::XSYNCFOLDERLIST_ XSYNCFOLDERLIST_ST;
typedef DestinyMsgpack_LSIF::RETURNVO_ RETURNVO_ST;
typedef DestinyMsgpack_LSIF::XSYNCDOCUMENTLIST_ XSYNCDOCUMENTLIST_ST;
typedef DestinyMsgpack_LSIF::XSYNCDOCUMENT_ XSYNCDOCUMENT_ST;
typedef DestinyMsgpack_LSIF::XMLRPCUSER_ XMLRPCUSER_ST;
typedef DestinyMsgpack_LSIF::XEXTCOLUMN_ XEXTCOLUMN_ST;
typedef DestinyMsgpack_LSIF::DRIVEOBJECT_ DRIVEOBJECT_ST;
typedef DestinyMsgpack_LSIF::LOGINPARAM_ LOGINPARAM_ST;
typedef DestinyMsgpack_LSIF::XMLRPCDOCTYPE_ XMLRPCDOCTYPE_ST;
typedef DestinyMsgpack_LSIF::XMLRPCCONFIGURATION_ XMLRPCCONFIGURATION_ST;
typedef DestinyMsgpack_LSIF::XPROCESSRULE_ XPROCESSRULE_ST;
typedef DestinyMsgpack_LSIF::DOWNLOADDRIVEOBJECTLIST_ DOWNLOADDRIVEOBJECTLIST_ST;
typedef DestinyMsgpack_LSIF::RPCFILE_ RPCFILE_ST;
typedef DestinyMsgpack_LSIF::DRIVEOBJECTLIST_ DRIVEOBJECTLIST_ST;
typedef DestinyMsgpack_LSIF::XSYNCUSERINFO_ XSYNCUSERINFO_ST;
typedef DestinyMsgpack_LSIF::LICENSESTATUS_ LICENSESTATUS_ST;
typedef DestinyMsgpack_LSIF::XPRINTRULE_ XPRINTRULE_ST;
typedef DestinyMsgpack_LSIF::XUSERPOLICY_ XUSERPOLICY_ST;
typedef DestinyMsgpack_LSIF::XMLRPCWATERMARKCONFIG_ XMLRPCWATERMARKCONFIG_ST;
typedef DestinyMsgpack_LSIF::XACE_ XACE_ST;
typedef DestinyMsgpack_LSIF::DOWNLOADDRIVEOBJECT_ DOWNLOADDRIVEOBJECT_ST;
typedef DestinyMsgpack_LSIF::NETWORKCONTROLEXCLUSIVEPOLICY_ NETWORKCONTROLEXCLUSIVEPOLICY_ST;
typedef DestinyMsgpack_LSIF::XDOCTYPE_ XDOCTYPE_ST;
typedef DestinyMsgpack_LSIF::XSYNCDOCUMENTWITHEXTATTR_ XSYNCDOCUMENTWITHEXTATTR_ST;
