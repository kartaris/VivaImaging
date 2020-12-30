#pragma once

#pragma pack(push, 8)

typedef struct {
    int    fault_occurred;
    int    fault_code;
    wchar_t * fault_string;
}ERROR_ST;

typedef struct _returnvo { 
 	int  returnCode;
	const wchar_t *  returnValue;
	const wchar_t * Class;
 
}RETURNVO_ST;

typedef struct _xextcolumnvalue { 
 	bool  update;
	const wchar_t *  key;
	const wchar_t *  value;
	const wchar_t *  valuetype;
	const wchar_t * Class;
 
}XEXTCOLUMNVALUE_ST;

typedef struct _xsecuritypolicy { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	int  appliedUserCount;
	const wchar_t *  name;
	bool  flagSystemDefault;
	bool  flagDefault;
	const wchar_t *  createdAt;
	const wchar_t *  applPermission;
	const wchar_t *  explorerPermission;
	const wchar_t *  secuDDPermission;
	const wchar_t *  ecmDDPermission;
	const wchar_t *  clipBoardPermission;
	const wchar_t *  capturePermission;
	const wchar_t *  printPermission;
	const wchar_t *  outlookPermission;
	const wchar_t *  centralDisk;
	const wchar_t *  sortOrder;
	const wchar_t * Class;
 
}XSECURITYPOLICY_ST;

typedef struct _xurllink { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	struct _xfile *  imageFile;
	const wchar_t *  creatorName;
	const wchar_t *  name;
	const wchar_t *  createdAt;
	const wchar_t *  creatorOID;
	const wchar_t *  deActivatedAt;
	const wchar_t *  linkType;
	const wchar_t *  urlTarget;
	const wchar_t *  browserAttributes;
	const wchar_t *  linkUrl;
	const wchar_t *  linkParameters;
	bool  flagUse;
	bool  flagEncrypt;
	const wchar_t *  method;
	const wchar_t *  description;
	const wchar_t *  sortOrder;
	const wchar_t * Class;
 
}XURLLINK_ST;

typedef struct _xfilehistory { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  creatorName;
	const wchar_t *  eventMessage;
	const wchar_t *  fileOID;
	const wchar_t *  genericFileOID;
	const wchar_t *  versionOID;
	const wchar_t *  creatorOID;
	const wchar_t *  createdAt;
	const wchar_t *  description;
	const wchar_t *  eventType;
	const wchar_t * Class;
 
}XFILEHISTORY_ST;

typedef struct _xextattribute { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	struct _xextcolumn *  extColumns;
	int extColumnsCNT;
	int  extColumnCount;
	const wchar_t *  creatorName;
	const wchar_t *  name;
	const wchar_t *  baseTableName;
	const wchar_t *  extTableName;
	const wchar_t *  creatorOID;
	const wchar_t *  createdAt;
	const wchar_t *  description;
	bool  flagNotUsed;
	bool  flagSysDefined;
	const wchar_t * Class;
 
}XEXTATTRIBUTE_ST;

typedef struct _xsyncdocumentlist { 
 	int  documentCount;
	int  totalCount;
	struct _xsyncdocument *  documents;
	int documentsCNT;
	const wchar_t * Class;
 
}XSYNCDOCUMENTLIST_ST;

typedef struct _xsyncdocument { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  folderOID;
	const wchar_t *  folderFullPath;
	const wchar_t *  creatorOID;
	const wchar_t *  name;
	const wchar_t *  fileOID;
	const wchar_t *  fileSize;
	const wchar_t *  storageFileID;
	int  lastModifiedAtLow;
	int  lastModifiedAtHigh;
	const wchar_t *  lastModifierOID;
	const wchar_t *  lastModifierName;
	const wchar_t *  creatorName;
	const wchar_t *  docStatus;
	const wchar_t *  permissions;
	const wchar_t * Class;
 
}XSYNCDOCUMENT_ST;

typedef struct _clientzoneinfo { 
 	const wchar_t *  clientLanguage;
	const wchar_t *  clientTimeZoneID;
	const wchar_t * Class;
 
}CLIENTZONEINFO_ST;

typedef struct _xmlrpcuser { 
 	const wchar_t *  userOID;
	const wchar_t *  userName;
	const wchar_t *  account;
	const wchar_t *  groupFullPathName;
	const wchar_t *  groupCode;
	const wchar_t *  groupPosition;
	const wchar_t *  groupOID;
	const wchar_t *  email;
	const wchar_t *  userType;
	const wchar_t * Class;
 
}XMLRPCUSER_ST;

typedef struct _xextcolumn { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	struct _xcodelist *  codeList;
	const wchar_t *  extAttributeOID;
	const wchar_t *  name;
	const wchar_t *  colType;
	const wchar_t *  inputType;
	const wchar_t *  dbColName;
	int  dbSize;
	int  dbSubSize;
	const wchar_t *  codeListOID;
	const wchar_t *  viewFormat;
	const wchar_t *  viewLineType;
	int  viewOrder;
	const wchar_t *  minInputValue;
	const wchar_t *  maxInputValue;
	const wchar_t *  defaultValue;
	bool  flagSearch;
	bool  flagRequired;
	bool  flagVisible;
	bool  flagLineFeed;
	bool  flagComma;
	bool  flagBetween;
	bool  flagVirtual;
	bool  flagFixedValue;
	const wchar_t *  currencyFormat;
	const wchar_t * Class;
 
}XEXTCOLUMN_ST;

typedef struct _xuserinfo { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t **  fullPathIndexNames;
	int fullPathIndexNamesCNT;
	const wchar_t *  userOID;
	const wchar_t *  groupOID;
	const wchar_t *  account;
	const wchar_t *  userName;
	const wchar_t *  userType;
	const wchar_t *  groupCode;
	const wchar_t *  groupFullPathName;
	const wchar_t *  groupPosition;
	const wchar_t *  email;
	const wchar_t *  createdAt;
	bool  flagDefault;
	bool  flagAccountLock;
	bool  flagChangePasswordNextLogin;
	const wchar_t * Class;
 
}XUSERINFO_ST;

typedef struct _folderitem { 
 	const wchar_t *  oID;
	const wchar_t *  parentOID;
	int  lastModifiedAtHigh;
	int  lastModifiedAtLow;
	int  permissionProperties;
	int  aclProperties;
	const wchar_t *  objName;
	const wchar_t *  creatorInfo;
	const wchar_t * Class;
 
}FOLDERITEM_ST;

typedef struct _driveobject { 
 	const wchar_t *  oID;
	const wchar_t *  parentOID;
	int  lastModifiedAtHigh;
	int  lastModifiedAtLow;
	int  permissionProperties;
	int  aclProperties;
	const wchar_t *  objName;
	const wchar_t *  creatorInfo;
	int  objType;
	int  fileSizeHigh;
	int  fileSizeLow;
	const wchar_t *  totalSize;
	int  totalCount;
	int  localLastModifiedAtHigh;
	int  localLastModifiedAtLow;
	int  objCreatedAtHigh;
	int  objCreatedAtLow;
	bool  flagOpen;
	bool  flagHaveChild;
	int  returnCode;
	struct _folderitem *  fullPathItems;
	int fullPathItemsCNT;
	const wchar_t *  storageFileID;
	const wchar_t *  docStatus;
	const wchar_t *  versionStatus;
	const wchar_t *  versionCode;
	const wchar_t *  processStatus;
	const wchar_t *  tags;
	const wchar_t *  sourceOID;
	const wchar_t *  drmType;
	struct _driveobject *  childObjects;
	int childObjectsCNT;
	const wchar_t *  folderType;
	bool  flagGroupRootFolder;
	bool  existFavorite;
	bool  enableChangeManagerGroup;
	bool  othersPrivateFolder;
	const wchar_t *  checkOutInfo;
	int  checkOutAtHigh;
	int  checkOutAtLow;
	const wchar_t * Class;
 
}DRIVEOBJECT_ST;

typedef struct _loginparam { 
 	const wchar_t *  userOID;
	const wchar_t *  groupOID;
	const wchar_t *  account;
	const wchar_t *  password;
	const wchar_t *  clientIP;
	const wchar_t *  sessionID;
	bool  fromAgent;
	bool  useSSO;
	bool  useMultiLogin;
	bool  autoLogin;
	struct _clientzoneinfo *  clientZoneInfo;
	const wchar_t * Class;
 
}LOGINPARAM_ST;

typedef struct _xcodelist { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	struct _xcodevalue *  codeValueList;
	int codeValueListCNT;
	const wchar_t *  name;
	const wchar_t * Class;
 
}XCODELIST_ST;

typedef struct _xmlrpcdoctype { 
 	const wchar_t *  docTypeOID;
	bool  flagRequiredTag;
	const wchar_t *  maintainDurationList;
	int  maintainDuration;
	const wchar_t * Class;
 
}XMLRPCDOCTYPE_ST;

typedef struct _xconvertfile { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  targetOID;
	const wchar_t *  targetObjectType;
	const wchar_t *  fileOID;
	const wchar_t *  storageFileID;
	const wchar_t *  convertFileType;
	const wchar_t *  convertStatus;
	const wchar_t *  convertedAt;
	const wchar_t *  fileName;
	const wchar_t * Class;
 
}XCONVERTFILE_ST;

typedef struct _xextattributevalue { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  extTableName;
	struct _xextcolumnvalue *  extColumnValueList;
	int extColumnValueListCNT;
	const wchar_t * Class;
 
}XEXTATTRIBUTEVALUE_ST;

typedef struct _xmlrpcconfiguration { 
 	const wchar_t **  configurationKeys;
	int configurationKeysCNT;
	const wchar_t **  configurationDefaults;
	int configurationDefaultsCNT;
	const wchar_t * Class;
 
}XMLRPCCONFIGURATION_ST;

typedef struct _xsyncfolder { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  parentOID;
	const wchar_t *  fullPathIndex;
	const wchar_t *  folderFullPath;
	const wchar_t *  creatorOID;
	const wchar_t *  name;
	int  lastModifiedAtLow;
	int  lastModifiedAtHigh;
	const wchar_t *  creatorName;
	const wchar_t *  folderStatus;
	const wchar_t *  permissions;
	const wchar_t * Class;
 
}XSYNCFOLDER_ST;

typedef struct _xconfiguration { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  targetOID;
	const wchar_t *  targetObjectType;
	const wchar_t *  configGroup;
	const wchar_t *  configKey;
	const wchar_t *  configValue;
	const wchar_t *  description;
	const wchar_t * Class;
 
}XCONFIGURATION_ST;

typedef struct _xtag { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	int  weight;
	int  registerCount;
	const wchar_t *  targetOID;
	const wchar_t *  targetObjectType;
	const wchar_t *  tagName;
	const wchar_t *  createdAt;
	const wchar_t * Class;
 
}XTAG_ST;

typedef struct _xcodevalue { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  codeListOID;
	const wchar_t *  name;
	const wchar_t *  codeValue;
	const wchar_t *  subCodeListOID;
	const wchar_t *  sortOrder;
	const wchar_t * Class;
 
}XCODEVALUE_ST;

typedef struct _xapplicationpolicy { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t **  excludeFilePathes;
	int excludeFilePathesCNT;
	const wchar_t *  applicationName;
	const wchar_t *  manageType;
	const wchar_t *  createdAt;
	bool  flagProcessName;
	const wchar_t *  processName;
	bool  flagPropertyInfo;
	const wchar_t *  propertyInfo;
	bool  flagCheckSum;
	const wchar_t *  checkSum;
	const wchar_t *  allowFileExt;
	const wchar_t *  excludeFilePath;
	bool  flagUse;
	const wchar_t *  lastModifiedAt;
	bool  flagChildProcess;
	bool  flagViewDrive;
	const wchar_t *  sortOrder;
	int  portAllowType;
	const wchar_t *  allowPort;
	bool  flagCopyLocalDDPolicyToSecuDD;
	const wchar_t *  portBlackList;
	bool  existedPortUseRequest;
	const wchar_t * Class;
 
}XAPPLICATIONPOLICY_ST;

typedef struct _ecmobject { 
 	const wchar_t *  objName;
	const wchar_t *  objPath;
	const wchar_t * Class;
 
}ECMOBJECT_ST;

typedef struct _xmlrpclogincontext { 
 	const wchar_t *  sessionKey;
	int  loginStatus;
	const wchar_t *  errorMsg;
	const wchar_t *  multiUserSuggest;
	const wchar_t *  loginUserInfo;
	int  maxLoginCount;
	int  failLoginCount;
	const wchar_t *  documentSaveType;
	const wchar_t *  useAutoCADHooking;
	int  messageTimingDuration;
	const wchar_t *  lastLoginIP;
	const wchar_t *  lastLoginDate;
	const wchar_t *  language;
	const wchar_t * Class;
 
}XMLRPCLOGINCONTEXT_ST;

typedef struct _driveobjectlist { 
 	int  returnCode;
	struct _driveobject *  driveObjects;
	int driveObjectsCNT;
	int  tableCount;
	int  totalCount;
	const wchar_t *  totalSize;
	const wchar_t * Class;
 
}DRIVEOBJECTLIST_ST;

typedef struct _xfile { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	struct _xconvertfile *  convertFiles;
	int convertFilesCNT;
	const wchar_t *  creatorName;
	const wchar_t *  lastModifierName;
	bool  myCheckOutFile;
	const wchar_t *  targetVersion;
	bool  updateEnable;
	const wchar_t *  targetName;
	const wchar_t *  viewURL;
	int  viewCount;
	const wchar_t *  newFileOID;
	bool  flagSaveNewVersion;
	const wchar_t *  thumbnailStatus;
	struct _xfilehistory *  fileHistory;
	const wchar_t *  targetOID;
	const wchar_t *  targetObjectType;
	const wchar_t *  genericFileOID;
	const wchar_t *  storageFileID;
	const wchar_t *  fileName;
	double   fileSize;
	const wchar_t *  fileType;
	const wchar_t *  description;
	int  fileStatus;
	const wchar_t *  creatorOID;
	const wchar_t *  lastModifierOID;
	const wchar_t *  localLastModifiedAt;
	const wchar_t *  lastModifiedAt;
	const wchar_t *  checkOutStatus;
	const wchar_t *  sortOrder;
	bool  flagDefaultVersion;
	const wchar_t *  fileLevel;
	const wchar_t *  columnName;
	const wchar_t *  drmStatus;
	const wchar_t *  checkOutUserOID;
	const wchar_t *  checkOutUserName;
	const wchar_t *  checkOutAt;
	const wchar_t *  folderOID;
	const wchar_t *  documentName;
	const wchar_t *  folderFullPathName;
	struct _xtag *  tags;
	int tagsCNT;
	const wchar_t *  highlightedFileName;
	const wchar_t *  highlightedCreatorName;
	const wchar_t *  highlightedTagList;
	const wchar_t *  highlightedFolderFullPathName;
	const wchar_t *  attach;
	double   beforeSize;
	const wchar_t * Class;
 
}XFILE_ST;

typedef struct _xmlrpcdocument { 
 	const wchar_t *  folderOID;
	const wchar_t *  localFolderPath;
	const wchar_t *  creatorOID;
	const wchar_t *  name;
	const wchar_t **  categoryFolderOIDs;
	int categoryFolderOIDsCNT;
	const wchar_t *  docTypeOID;
	const wchar_t *  tags;
	const wchar_t **  storagefileNames;
	int storagefileNamesCNT;
	const wchar_t **  storagefileSizes;
	int storagefileSizesCNT;
	const wchar_t **  storagefileOIDs;
	int storagefileOIDsCNT;
	int  maintainDuration;
	const wchar_t *  description;
	const wchar_t * Class;
 
}XMLRPCDOCUMENT_ST;

typedef struct _xamountpolicy { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  name;
	const wchar_t *  extension;
	const wchar_t *  sortOrder;
	const wchar_t * Class;
 
}XAMOUNTPOLICY_ST;

typedef struct _xgroup { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  aclOID;
	int  permission;
	struct _xace *  aceList;
	int aceListCNT;
	const wchar_t *  creatorOID;
	const wchar_t *  aclLevelOID;
	bool  flagInheritACL;
	const wchar_t *  managerGroupOID;
	const wchar_t *  parentOID;
	const wchar_t *  fullPathIndex;
	int  subLastIndex;
	int  childCount;
	const wchar_t *  fullPathName;
	struct _xuserinfo *  userInfos;
	int userInfosCNT;
	const wchar_t **  managerList;
	int managerListCNT;
	struct _xuserinfo *  managerPGroupUserInfos;
	int managerPGroupUserInfosCNT;
	const wchar_t *  managerPGroupFullPathName;
	const wchar_t *  lastUsedAt;
	const wchar_t *  documentSaveType;
	bool  inheritDocumentSaveType;
	const wchar_t *  useAutoCADHooking;
	bool  inheritUseAutoCADHooking;
	const wchar_t *  managerPGroupUserAccounts;
	const wchar_t *  docViewApproveUserAccounts;
	const wchar_t *  docExApproveUserAccounts;
	const wchar_t *  pcExApproveUserAccounts;
	struct _xgroup *  orderedChildren;
	int orderedChildrenCNT;
	const wchar_t *  managerOID;
	const wchar_t *  managerPGroupOID;
	const wchar_t *  extAttributeOID;
	const wchar_t *  name;
	const wchar_t *  groupCode;
	const wchar_t *  description;
	const wchar_t *  sortOrder;
	const wchar_t *  createdAt;
	const wchar_t *  lastModifiedAt;
	const wchar_t *  deleteStatus;
	const wchar_t *  workStatus;
	struct _xextattributevalue *  extAttributeValue;
	struct _xextattribute *  extAttribute;
	const wchar_t * Class;
 
}XGROUP_ST;

typedef struct _xsyncuserinfo { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	const wchar_t *  userOID;
	const wchar_t *  name;
	const wchar_t *  account;
	const wchar_t *  groupFullPathName;
	const wchar_t *  groupCode;
	const wchar_t *  groupPosition;
	const wchar_t *  groupOID;
	const wchar_t *  email;
	const wchar_t *  userType;
	const wchar_t * Class;
 
}XSYNCUSERINFO_ST;

typedef struct _returnintvo { 
 	int  returnCode;
	int  returnValue;
	const wchar_t * Class;
 
}RETURNINTVO_ST;

typedef struct _xuserpolicy { 
 	const wchar_t *  oID;
	const wchar_t **  opCodes;
	int opCodesCNT;
	int  searchObjectTotalCount;
	struct _xsecuritypolicy *  securityPolicy;
	const wchar_t *  userName;
	const wchar_t *  securityDDVolLetter;
	const wchar_t *  securityDDVolName;
	const wchar_t *  ecmDDVolLetter;
	const wchar_t *  ecmDDVolName;
	int  documentIncludeType;
	int  fileViewTypeInGeneralExplorer;
	int  fileViewTypeInOnlyExplorer;
	const wchar_t *  userOID;
	const wchar_t *  securityPolicyOID;
	int  pageRowNum;
	int  explorerRowNum;
	int  secuDDSizeMB;
	const wchar_t *  applPolicyModifiedAt;
	bool  flagDefaultSecurity;
	bool  flagSecuDDSize;
	const wchar_t * Class;
 
}XUSERPOLICY_ST;

typedef struct _xmlrpcwatermarkconfig { 
 	int  watermarkUseInfo;
	int  watermarkPosition;
	const wchar_t *  font;
	int  fontSize;
	const wchar_t *  companyName;
	const wchar_t *  phoneNumber;
	const wchar_t *  faxNumber;
	const wchar_t *  homepageAddress;
	const wchar_t *  userName;
	const wchar_t *  userPosition;
	const wchar_t *  userEmail;
	const wchar_t *  userGroupName;
	const wchar_t *  imageFileStorageID;
	const wchar_t *  imageFileUrl;
	const wchar_t * Class;
 
}XMLRPCWATERMARKCONFIG_ST;

typedef struct _xsyncfolderlist { 
 	int  folderCount;
	struct _xsyncfolder *  folders;
	int foldersCNT;
	const wchar_t * Class;
 
}XSYNCFOLDERLIST_ST;

typedef struct _xace { 
 	const wchar_t *  objectName;
	const wchar_t *  ownerFullPath;
	const wchar_t *  aclOID;
	const wchar_t *  targetObjectType;
	const wchar_t *  aclOwnerOID;
	const wchar_t *  accessGrade;
	int  permission;
	bool  flagUserDefined;
	const wchar_t *  sortOrder;
	int  weight;
	const wchar_t * Class;
 
}XACE_ST;

#pragma pack(pop)
