#pragma once

#define GAMEEASY_MAJOR_VERSION		2
#define GAMEEASY_MINOR_VERSION		3
#define GAMEEASY_PATCH_VERSION		0
#define GAMEEASY_BUILD_VERSION		0

#define CONCAT2(A, B)  A##B
#define CONCAT(A, B)   CONCAT2(A, B)

#define EZF_MAJOR_VERSION				CONCAT(GAMEEASY_MAJOR_VERSION, 0)
#define EZF_MINOR_VERSION				GAMEEASY_MINOR_VERSION
#define EZF_PATCH_VERSION				0
#define EZF_INTERNAL_VERSION			9

#define EZ_STR2(A)	 #A
#define EZ_STR(B) EZ_STR2(B)


#define GAMEEASY_SDK_VERSION \
	(GAMEEASY_MAJOR_VERSION << 24) | (GAMEEASY_MINOR_VERSION << 16) | \
	(GAMEEASY_PATCH_VERSION << 8) | (GAMEEASY_BUILD_VERSION << 0) | 


#define GAMEEASY_EZF_VERSION \
	(EZF_MAJOR_VERSION << 24) | (EZF_MINOR_VERSION << 16) | \
	(EZF_PATCH_VERSION << 8) | (EZF_INTERNAL_VERSION << 0)



#define GAMEEASY_SDK_VERSION_STRING \
	EZ_STR(GAMEEASY_MAJOR_VERSION) "." EZ_STR(GAMEEASY_MINOR_VERSION) "." \
	EZ_STR(GAMEEASY_PATCH_VERSION) "." EZ_STR(GAMEEASY_BUILD_VERSION)

#define GAMEEASY_EZF_VERSION_STRING \
	EZ_STR(EZF_MAJOR_VERSION) "." EZ_STR(EZF_MINOR_VERSION) "." \
	EZ_STR(EZF_PATCH_VERSION) "." EZ_STR(EZF_INTERNAL_VERSION)