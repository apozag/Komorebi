#pragma once

#define MAX_DIRLIGHTS   5
#define MAX_SPOTLIGHTS  5
#define MAX_POINTLIGHTS 5

#define PCBUFF_DIRLIGHT_SLOT	0
#define PCBUFF_POINTLIGHT_SLOT	1
#define PCBUFF_SPOTLIGHT_SLOT	2
#define PCBUFF_SHADOW_SLOT		3
#define PCBUFF_FREE_SLOT		4

#define VCBUFF_CAMERATRANSFORM_SLOT		0
#define VCBUFF_MODEL_SLOT		1
#define VCBUFF_BONES_SLOT		2
#define VCBUFF_LIGHTTRANSFORM_SLOT	3
#define VCBUFF_FREE_SLOT		4

#define SRV_SHADOWMAP_SLOT		0
#define SRV_PCF_SLOT			SRV_SHADOWMAP_SLOT + MAX_DIRLIGHTS
#define SRV_FREE_SLOT			SRV_PCF_SLOT+1